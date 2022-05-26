#pragma once

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

namespace mysylar {

class Logger;
class LogAppender;
class LogEvent;
class Formatter;

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
  static const std::string& ToString(Level level); 
};

class Logger : public std::enable_shared_from_this<Logger> {
public:
    typedef std::shared_ptr<Logger> SharedPtr;
    Logger(const std::string& logger_name, LogLevel::Level level);
    void Log(LogLevel::Level level, LogEvent::SharedPtr event);
    void Debug(LogEvent::SharedPtr event);
    void Info(LogEvent::SharedPtr event);
    void Warning(LogEvent::SharedPtr event);
    void Error(LogEvent::SharedPtr event);
    void Fatal(LogEvent::SharedPtr event);
    void AddAppender(LogAppender::SharedPtr appender);
    void DeleteAppender(LogAppender::SharedPtr appender);
    void ClearAppender() { log_appenders_.clear(); }
    const std::string& GetName() { return logger_name_; }
private:
    const std::string logger_name_; 
    LogLevel::Level level_;
    std::list<LogAppender::SharedPtr> log_appenders_;

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

private:
    const char* file_name_; // file name
    uint64_t time_; // timestamp
    uint32_t elapse_; // elapsed time from program run
    uint32_t line_; // line number
    uint32_t thread_id_; // thread id
    std::string thread_name_; // thread name
    uint32_t fiber_id_; // fiber id

};

class Formatter {
public:
    typedef std::shared_ptr<Formatter> SharedPtr;
    Formatter(std::string& pattern);
    void Format(
        std::ostream& os, Logger::SharedPtr logger, 
        LogLevel::Level level, LogEvent::SharedPtr event);
    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> SharedPtr;
        virtual ~FormatItem();
        virtual void Format(
            std::ostream& os, Logger::SharedPtr logger, 
            LogLevel::Level level, LogEvent::SharedPtr event) = 0;
    };
private:
    std::string pattern_;
    std::vector<FormatItem::SharedPtr> format_items_;
    void PatternParse();

};

class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> SharedPtr;
    LogAppender();
    virtual ~LogAppender();
    virtual void Log(Logger::SharedPtr logger, LogLevel::Level level, LogEvent::SharedPtr evnet) = 0;
    void SetFormatter(Formatter::SharedPtr formatter) { formatter_ = formatter; }
    Formatter::SharedPtr GetFormatter() { return formatter_; }
    void SetLevel(LogLevel::Level level) { level_ = level; }
    LogLevel::Level GetLevel() { return level_; }
protected:
    LogLevel::Level level_ = LogLevel::Level::UNKNOWN;
    Formatter::SharedPtr formatter_;
};

class StdoutLogAppender : public LogAppender {
public:
    void Log(Logger::SharedPtr logger, LogLevel::Level level, LogEvent::SharedPtr event) override;

};

class FileLogAppender : public LogAppender {
public:
    FileLogAppender(const std::string file_name);
    bool ReopenFile();
    void Log(Logger::SharedPtr logger, LogLevel::Level level, LogEvent::SharedPtr evnet) override;
private:
    const std::string file_name_;
    std::ofstream file_stream_;


}
;

};