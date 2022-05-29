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
             const uint32_t& fiber_id);
    const char* GetFileName() const { return file_name_; }
    const uint64_t& GetTime() const { return time_; }
    const uint32_t& GetElapse() const { return elapse_; }
    const uint32_t& GetLine() const { return line_; }
    const uint32_t& GetThreadId() const { return thread_id_; }
    const std::string& GetThreadName() const { return thread_name_; }
    const uint32_t& GetFiberId() const { return fiber_id_; }
    const std::string GetContent() const { return content_.str(); }
    std::stringstream& GetStringStream() { return content_; }

private:
    const char* file_name_; // file name
    uint64_t time_; // timestamp
    uint32_t elapse_; // elapsed time from program run
    uint32_t line_; // line number
    uint32_t thread_id_; // thread id
    std::string thread_name_; // thread name
    uint32_t fiber_id_; // fiber id
    std::stringstream content_; // content

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


;
class Logger : public std::enable_shared_from_this<Logger> {
public:
    typedef std::shared_ptr<Logger> SharedPtr;
    Logger() {}
    Logger(const std::string& logger_name, LogLevel::Level level);
    // set the event level as DEBUG and log the event
    void Debug(LogEvent::SharedPtr event);
    // set the event level as INFO and log the event
    void Info(LogEvent::SharedPtr event);
    // set the event level as WARNING and log the event
    void Warning(LogEvent::SharedPtr event);
    // set the event level as ERROR and log the event
    void Error(LogEvent::SharedPtr event);
    // set the event level as FATAL and log the event
    void Fatal(LogEvent::SharedPtr event);
    // add a log appender to the logger
    void AddAppender(LogAppender::SharedPtr appender);
    // delete a log appender to the logger
    void DeleteAppender(LogAppender::SharedPtr appender);
    // clear all log appenders
    void ClearAppenders() { log_appenders_.clear(); }
    // get the logger name
    const std::string& GetName() { return logger_name_; }
private:
    // default name
    const std::string logger_name_ = "root"; 
    // set the event level and log it
    void Log(LogLevel::Level level, LogEvent::SharedPtr event);
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
    bool ReopenFile();
private:
    const std::string file_name_;
    std::ofstream file_stream_;
    void Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::SharedPtr evnet) override;
};

}

#endif