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
    std::stringstream content_;

};



class Formatter {
public:
    typedef std::shared_ptr<Formatter> SharedPtr;
    Formatter() { PatternParse(); }
    Formatter(const std::string& pattern);
    void Format(
        std::ostream& os, std::shared_ptr<Logger> logger, 
        LogLevel::Level level, LogEvent::SharedPtr event);
    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> SharedPtr;
        //FormatItem(const std::string& str = "") {}
        virtual ~FormatItem() {}
        virtual void Format(
            std::ostream& os, std::shared_ptr<Logger> logger, 
            LogLevel::Level level, LogEvent::SharedPtr event) = 0;
    };
private:
    std::string pattern_ = std::string("[%p]%d{%Y-%m-%d %H:%M:%S}%T(tid)%t%T(tname)%N%T(fid)%F%T[%c]%T%f:%l%T%m%n");
    std::vector<FormatItem::SharedPtr> format_items_;
    void PatternParse();

};



class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> SharedPtr;
    virtual ~LogAppender() {}
    virtual void Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::SharedPtr evnet) = 0;
    void SetFormatter(Formatter::SharedPtr formatter) { formatter_ = formatter; }
    Formatter::SharedPtr GetFormatter() { return formatter_; }
    void SetLevel(LogLevel::Level level) { level_ = level; }
    LogLevel::Level GetLevel() { return level_; }
protected:
    LogLevel::Level level_ = LogLevel::Level::DEBUG;
    Formatter::SharedPtr formatter_;
};


;
class Logger : public std::enable_shared_from_this<Logger> {
public:
    typedef std::shared_ptr<Logger> SharedPtr;
    Logger() {}
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
    const std::string logger_name_ = "root"; 
    Formatter::SharedPtr formatter_ = std::make_shared<Formatter>();
    LogLevel::Level level_ = LogLevel::Level::DEBUG;
    std::list<LogAppender::SharedPtr> log_appenders_;

};

class StdoutLogAppender : public LogAppender {
public:
    void Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::SharedPtr event) override;

};

class FileLogAppender : public LogAppender {
public:
    FileLogAppender(const std::string file_name);
    bool ReopenFile();
    void Log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::SharedPtr evnet) override;
private:
    const std::string file_name_;
    std::ofstream file_stream_;
};

}

#endif