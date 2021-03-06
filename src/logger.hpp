#ifndef __LOGGER_H
#define __LOGGER_H

#include <functional>
#include <tuple>
#include <map>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <list>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include "utils.hpp"
#include "singleton.hpp"



#define LLOG(logger_name, event_level) mysylar::LogEventWrap::SharedPtr( \
    new mysylar::LogEventWrap(mysylar::LogEvent::SharedPtr( \
    new mysylar::LogEvent(__FILE__, time(NULL), 0, __LINE__, \
    mysylar::GetThreadId(), mysylar::GetThreadName(), mysylar::GetFiberId(), \
    mysylar::LoggerManager::GetInstance().GetLogger(logger_name), \
    event_level))))->GetStringStream() 
#define LDEBUG(logger_name) LLOG(logger_name, mysylar::LogLevel::Level::DEBUG)
#define LINFO(logger_name) LLOG(logger_name, mysylar::LogLevel::Level::INFO)
#define LWARNING(logger_name) LLOG(logger_name, mysylar::LogLevel::Level::WARNING)
#define LERROR(logger_name) LLOG(logger_name, mysylar::LogLevel::Level::ERROR)
#define LFATAL(logger_name) LLOG(logger_name, mysylar::LogLevel::Level::FATAL)

#define LRDEBUG LDEBUG("root")
#define LRINFO LINFO("root")
#define LRWARNING LWARNING("root")
#define LRERROR LERROR("root")
#define LRFATAL LFATAL("root")


#define FLLOG(logger_name, event_level, format, ...) mysylar::LogEventWrap::SharedPtr( \
    new mysylar::LogEventWrap(mysylar::LogEvent::SharedPtr( \
    new mysylar::LogEvent(__FILE__, time(NULL), 0, __LINE__, \
    mysylar::GetThreadId(), mysylar::GetThreadName(), mysylar::GetFiberId(), \
    mysylar::LoggerManager::GetInstance().GetLogger(logger_name), \
    event_level))))->GetEvent()->Format(format, __VA_ARGS__) 
#define FLDEBUG(logger_name, format, ...) FLLOG(logger_name, \
    mysylar::LogLevel::Level::DEBUG, format, __VA_ARGS__)
#define FLINFO(logger_name, format, ...) FLLOG(logger_name, \
    mysylar::LogLevel::Level::INFO, format, __VA_ARGS__)
#define FLWARNING(logger_name, format, ...) FLLOG(logger_name, \
    mysylar::LogLevel::Level::WARNING, format, __VA_ARGS__)
#define FLERROR(logger_name, format, ...) FLLOG(logger_name, \
    mysylar::LogLevel::Level::ERROR, format, __VA_ARGS__)
#define FLFATAL(logger_name, format, ...) FLLOG(logger_name, \
    mysylar::LogLevel::Level::FATAL, format, __VA_ARGS__)

#define FLRDEBUG(format, ...) FLDEBUG("root", format, __VA_ARGS__)
#define FLRINFO(format, ...) FLINFO("root", format, __VA_ARGS__)
#define FLRWARNING(format, ...) FLWARNING("root", fromat, __VA_ARGS__)
#define FLRERROR(format, ...) FLERROR("root", format, _VA__ARGS__)
#define FLRFATAL(format, ...) FLFATAL("root", format, _VA_ARGS__)


namespace mysylar {


class Logger;

class LogLevel {
public:
    enum Level {
        UNKNOWN = 0,
        DEBUG = 1,
        INFO = 2,
        WARNING = 3,
        ERROR = 4,
        FATAL = 5,
  };
  static Level ToLevel(const std::string& level_str); 
  static const std::string ToString(Level level); 
};

class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> SharedPtr;
    LogEvent(const char* file_name, const uint64_t& time, 
             const uint32_t& elapse, const uint32_t& line, 
             const uint32_t& thread_id, const std::string& thread_name,
             const uint32_t& fiber_id, std::shared_ptr<Logger> logger,
             LogLevel::Level level);
    const char* GetFileName() const { return file_name_; }
    const uint64_t& GetTime() const { return time_; }
    const uint32_t& GetElapse() const { return elapse_; }
    const uint32_t& GetLine() const { return line_; }
    const uint32_t& GetThreadId() const { return thread_id_; }
    const std::string& GetThreadName() const { return thread_name_; }
    const uint32_t& GetFiberId() const { return fiber_id_; }
    const std::string GetContent() const { return content_ss_.str(); }
    const LogLevel::Level GetLevel() const { return level_; }
    std::shared_ptr<Logger> GetLogger() { return logger_; }
    std::stringstream& GetStringStream() { return content_ss_; }
    void Format(const char* format, ...);
private:
    const char* file_name_; // file name
    uint64_t time_; // timestamp
    uint32_t elapse_; // elapsed time from program run
    uint32_t line_; // line number
    uint32_t thread_id_; // thread id
    std::string thread_name_; // thread name
    uint32_t fiber_id_; // fiber id
    std::stringstream content_ss_; // content
    std::shared_ptr<Logger> logger_;
    LogLevel::Level level_;

};

class LogEventWrap {
public:
    typedef std::shared_ptr<LogEventWrap> SharedPtr;
    LogEventWrap(LogEvent::SharedPtr event) : event_(event) {}
    ~LogEventWrap(); 
    LogEvent::SharedPtr GetEvent() { return event_; }
    std::stringstream& GetStringStream() { return event_->GetStringStream(); }
private:
    LogEvent::SharedPtr event_;
};



class Formatter {
public:
    typedef std::shared_ptr<Formatter> SharedPtr;
    Formatter() { PatternParse(); }
    Formatter(const std::string& pattern);
    // output the event as formatted
    void Format(
        std::ostream& os, std::shared_ptr<Logger> logger, 
        LogLevel::Level level, LogEvent::SharedPtr event);
    class FormatItem { // every format item has `Format` method to output their content
    public:
        typedef std::shared_ptr<FormatItem> SharedPtr;
        //FormatItem(const std::string& str = "") {}
        virtual ~FormatItem() {}
        virtual void Format(
            std::ostream& os, std::shared_ptr<Logger> logger, 
            LogLevel::Level level, LogEvent::SharedPtr event) = 0;
    };
private:
    std::string pattern_ = std::string("[%p]%d{%Y-%m-%d %H:%M:%S}%T(tid)%t%T(tname)%N%T(fid)%F%T[%c]%T%f:%l%T%m%n"); // the pattern of formatter
    std::vector<FormatItem::SharedPtr> format_items_; // formatted items
    void PatternParse(); // parse the pattern to format items

};



class LogAppender {
friend class Logger;
public:
    typedef std::shared_ptr<LogAppender> SharedPtr;
    virtual ~LogAppender() {}
    // set the formatter of the appender
    void SetFormatter(Formatter::SharedPtr formatter) { formatter_ = formatter; }
    // get the formatter of the appender
    Formatter::SharedPtr GetFormatter() { return formatter_; }
    // set the level of the appender
    void SetLevel(LogLevel::Level level) { level_ = level; }
    // get the level of the appender
    LogLevel::Level GetLevel() { return level_; }
protected:
    // set the event level and log it 
    virtual void Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::SharedPtr evnet) = 0;
    LogLevel::Level level_ = LogLevel::Level::DEBUG;
    Formatter::SharedPtr formatter_;
};



class Logger : public std::enable_shared_from_this<Logger> {
friend class LogEventWrap;
public:
    typedef std::shared_ptr<Logger> SharedPtr;
    Logger(const std::string& logger_name, LogLevel::Level level);
    Logger(const Logger& logger);
    // log the event
    void Log(LogEvent::SharedPtr event);
    void AddAppender(LogAppender::SharedPtr appender);
    // delete a log appender to the logger
    void DeleteAppender(LogAppender::SharedPtr appender);
    // clear all log appenders
    void ClearAppenders() { log_appenders_.clear(); }
//TODO: get log appender by name
    // get the logger name
    const std::string& GetName() { return logger_name_; }
    void SetLevel(LogLevel::Level level) { level_ = level; }
private:
    // default name
    const std::string logger_name_; 
    // default formatter
    Formatter::SharedPtr formatter_ = std::make_shared<Formatter>(); 
    // default level
    LogLevel::Level level_ = LogLevel::Level::DEBUG;
    // the list of logappenders
    std::list<LogAppender::SharedPtr> log_appenders_;

};

class StdoutLogAppender : public LogAppender {
private:
    void Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::SharedPtr event) override;
};

class FileLogAppender : public LogAppender {
public:
    FileLogAppender(const std::string file_name);
private:
    bool ReopenFile();
    const std::string file_name_;
    std::ofstream file_stream_;
    void Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::SharedPtr evnet) override;
};

class LoggerManager : public Singleton<LoggerManager> {
friend class Singleton<LoggerManager>;
public:
    bool AddLogger(std::shared_ptr<Logger> logger);
    bool DeleteLogger(std::shared_ptr<Logger> logger);
    bool DeleteLogger(const std::string& logger_name);
    void ClearLoggers();
    std::shared_ptr<Logger> GetLogger(const std::string& logger_name);
private:
    std::shared_ptr<Logger> root_logger_;
    std::map<std::string, std::shared_ptr<Logger> > loggers_;
    LoggerManager();

};

struct LogAppenderConfig {
    int type = 0;
    std::string path;
    std::string format_pattern;
    LogLevel::Level level;

    bool operator==(const LogAppenderConfig& log_appender_config) const {
        return type == log_appender_config.type
            && path == log_appender_config.path
            && format_pattern == log_appender_config.format_pattern
            && level == log_appender_config.level;
    }
};

struct LoggerConfig {
    std::string name;
    LogLevel::Level level;
    std::string format_pattern;
    std::vector<LogAppenderConfig> appenders;

    bool operator==(const LoggerConfig& logger_config) const {
        return name == logger_config.name
            && level == logger_config.level
            && format_pattern == logger_config.format_pattern
            && appenders == logger_config.appenders;
    }
    
};


}

#endif