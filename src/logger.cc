#include "logger.hpp"
#include <iostream>

namespace mysylar {
LogLevel::Level LogLevel::ToLevel(const std::string& level_str) {
    auto level_str_upper = const_cast<std::string&>(level_str);
    std::transform(level_str_upper.begin(), level_str_upper.end(), level_str_upper.begin(), ::toupper); 
#define XX(L) \
    if (#L == level_str_upper) return LogLevel::Level::L;
    XX(DEBUG)
    XX(INFO)
    XX(WARNING)
    XX(ERROR)
    XX(FATAL)
#undef XX
    return LogLevel::Level::UNKNOWN;
}

const std::string LogLevel::ToString(LogLevel::Level level) {
    switch (level) {
#define XX(L) \
        case LogLevel::Level::L: return std::string(#L);
        XX(DEBUG)
        XX(INFO)
        XX(WARNING)
        XX(ERROR)
        XX(FATAL)
#undef XX
        default: return std::string("UNKNOWN");
    }
}

Logger::Logger(const std::string& logger_name, LogLevel::Level level) : logger_name_(logger_name), level_(level){

}

void Logger::AddAppender(LogAppender::SharedPtr appender) {
    if (!appender->GetFormatter()) {
        appender->SetFormatter(formatter_);
    }
    log_appenders_.push_back(appender);
} 

void Logger::DeleteAppender(LogAppender::SharedPtr appender) {
    for (auto i = log_appenders_.begin(); i != log_appenders_.end(); ++i) {
        if (*i == appender) {
            log_appenders_.erase(i);
        }
    }
}

void Logger::Log(LogEvent::SharedPtr event) {
    auto event_level = event->GetLevel();
    if (event_level >= level_) {
        for (auto i : log_appenders_) {
            i->Log(shared_from_this(), event_level, event);
        }
    }
}

void LogEvent::Format(const char* format, ...) {
    va_list valist;
    va_start(valist, format);
    char* buf = nullptr;
    int len = vasprintf(&buf, format, valist);
    if(len != -1) {
        content_ss_ << std::string(buf, len);
        free(buf);
    }
    va_end(valist);
}

LogEvent::LogEvent(
    const char* file_name, 
    const uint64_t& time, 
    const uint32_t& elapse, 
    const uint32_t& line,
    const uint32_t& thread_id, 
    const std::string& thread_name, 
    const uint32_t& fiber_id,
    std::shared_ptr<Logger> logger,
    LogLevel::Level level) :
    file_name_(file_name), time_(time), elapse_(elapse), 
    line_(line), thread_id_(thread_id),
    thread_name_(thread_name), fiber_id_(fiber_id),
    logger_(logger), level_(level) {

}

LogEventWrap::~LogEventWrap() {
    event_->GetLogger()->Log(event_); 
}

Formatter::Formatter(const std::string& pattern) : pattern_(pattern) {
    PatternParse();
}

void Formatter::Format(
    std::ostream& os, Logger::SharedPtr logger, 
    LogLevel::Level level, LogEvent::SharedPtr event) {
    for (auto& i : format_items_) {
        i->Format(os, logger, level, event);
    }

}

FileLogAppender::FileLogAppender(const std::string file_name) : file_name_(file_name) {
    if (file_stream_.is_open()) {
        file_stream_.close();
    }
    file_stream_.open(file_name_);
}

bool FileLogAppender::ReopenFile() {
    if (file_stream_.is_open()) {
        file_stream_.close();
    }
    file_stream_.open(file_name_, std::ios_base::app);
    return file_stream_.is_open();
}

void FileLogAppender::Log(Logger::SharedPtr logger, LogLevel::Level level, LogEvent::SharedPtr event) {
    ReopenFile();
    if (level >= level_) {
        formatter_->Format(file_stream_, logger, level, event);
    }
}


void StdoutLogAppender::Log(Logger::SharedPtr logger, LogLevel::Level level, LogEvent::SharedPtr event) {
    if (level >= level_) {
        switch (level) {
        case LogLevel::Level::DEBUG:
            std::cout << "\033[1;32m"; break;
        case LogLevel::Level::INFO:
            std::cout << "\033[1;36m"; break;
        case LogLevel::Level::WARNING:
            std::cout << "\033[1;33m"; break;
        case LogLevel::Level::ERROR:
            std::cout << "\033[1;31m"; break;
        case LogLevel::Level::FATAL:
            std::cout << "\033[1;41m"; break;
        default:
            break;
        }
        formatter_->Format(std::cout, logger, level, event);
        std::cout << "\033[0m";
    }
}


class ContentFormatItem : public Formatter::FormatItem {
public:
    // In order to use map, `str` never used
    ContentFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger::SharedPtr logger, LogLevel::Level level, LogEvent::SharedPtr event) override {
        os << event->GetContent();
    }
};

class FileNameFormatItem : public Formatter::FormatItem {
public:
    // In order to use map, `str` never used
    FileNameFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger::SharedPtr logger, LogLevel::Level level, LogEvent::SharedPtr event) override {
        os << event->GetFileName();
    }
};

class TimeFormatItem : public Formatter::FormatItem {
public:
    TimeFormatItem(const std::string& time_format) : time_format_(time_format) {
        if (time_format.empty()) {
            time_format_ = "%Y-%m-%d %H:%M:%S";
        }
    }
    void Format(std::ostream& os, Logger::SharedPtr logger, LogLevel::Level level, LogEvent::SharedPtr event) override {
        struct tm tm_struct;
        time_t time = event->GetTime();
        localtime_r(&time, &tm_struct);
        char buf[64];
        strftime(buf, sizeof(buf), time_format_.c_str(), &tm_struct);
        os << buf;
    }
private:
    std::string time_format_;
};

class ElapseFormatItem : public Formatter::FormatItem {
public:
    // In order to use map, `str` never used
    ElapseFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger::SharedPtr logger, LogLevel::Level level, LogEvent::SharedPtr event) override {
        os << event->GetFileName();
    }
};

class LineFormatItem : public Formatter::FormatItem {
public:
    // In order to use map, `str` never used
    LineFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger::SharedPtr logger, LogLevel::Level level, LogEvent::SharedPtr event) override {
        os << event->GetLine();
    }
};

class ThreadIdFormatItem : public Formatter::FormatItem {
public:
    // In order to use map, `str` never used
    ThreadIdFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger::SharedPtr logger, LogLevel::Level level, LogEvent::SharedPtr event) override {
        os << event->GetThreadId();
    }
};

class ThreadNameFormatItem : public Formatter::FormatItem {
public:
    // In order to use map, `str` never used
    ThreadNameFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger::SharedPtr logger, LogLevel::Level level, LogEvent::SharedPtr event) override {
        os << event->GetThreadName();
    }
};

class FiberIdFormatItem : public Formatter::FormatItem {
public:
    // In order to use map, `str` never used
    FiberIdFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger::SharedPtr logger, LogLevel::Level level, LogEvent::SharedPtr event) override {
        os << event->GetFiberId();
    }
};

class StringFormatItem : public Formatter::FormatItem {
public:
    StringFormatItem(const std::string& string_content = "") : string_content_(string_content) {}
    void Format(std::ostream& os, Logger::SharedPtr logger, LogLevel::Level level, LogEvent::SharedPtr event) override {
        os << string_content_;
    }
private:
    std::string string_content_;
};

class LevelFormatItem : public Formatter::FormatItem {
public:
    // In order to use map, `str` never used
    LevelFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger::SharedPtr logger, LogLevel::Level level, LogEvent::SharedPtr event) override {
        os << LogLevel::ToString(level);
    }
};

class LoggerFormatItem : public Formatter::FormatItem {
public:
    // In order to use map, `str` never used
    LoggerFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger::SharedPtr logger, LogLevel::Level level, LogEvent::SharedPtr event) override {
        os << logger->GetName();
    }
};

class NewLineFormatItem : public Formatter::FormatItem {
public:
    // In order to use map, `str` never used
    NewLineFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger::SharedPtr logger, LogLevel::Level level, LogEvent::SharedPtr event) override {
        os << std::endl; 
    }
};

class TabFormatItem : public Formatter::FormatItem {
public:
    // In order to use map, `str` never used
    TabFormatItem(const std::string& str = "") {}
    void Format(std::ostream& os, Logger::SharedPtr logger, LogLevel::Level level, LogEvent::SharedPtr event) override {
        os << "  "; 
    }
};

void Formatter::PatternParse() {

    //std::cout << "-----------start parse-----------" << std::endl;
    // item: FormatItem, the character after %
    // fmt: the parameter of FormatItem's Constructor, which is the character in {} 
    // type: 0 for StringItem, 1 for others
    std::vector<std::tuple<std::string /*item*/, std::string /*format*/, int /*type*/> > pattern_parsed;
    std::string content_str; // not the formatted string, just content
    for (auto str_left = pattern_.begin(); str_left != pattern_.end(); ) {
        // char, have not met escape
        if (*str_left != '%') {
            content_str += *str_left;
            ++str_left;
            continue;
        }
        // met escape
        if ((++str_left != pattern_.end()) && *str_left == '%') { // if %%
            content_str += *str_left; 
            continue;
        }
        auto str_right = str_left;
        std::string item_str; // the string after % before {
        std::string item_format_str; // the string in {}
        int parse_state = 0;    //0 means parsing item, 1 means parsing fmt
        while (str_left != pattern_.end()) {
            // parse item
            if (parse_state == 0) {
                if (!isalpha(*str_right) && *str_right != '{' && *str_right != '}') { // current item has been parsed, get to next
                    item_str = std::string(str_left, str_right);
                    str_left = str_right;
                    break;
                }
                if (*str_right == '{') {
                    item_str = std::string(str_left, str_right);
                    str_left = ++str_right;
                    parse_state = 1; // start parsing fmt
                }
            }
            // parse fmt 
            if (parse_state == 1 && *str_right == '}') {
                item_format_str = std::string(str_left, str_right);
                str_left = str_right + 1;
                parse_state = 0; // end parsing fmt
                break;
            }
            ++str_right;
            // last item
            if (str_right == pattern_.end() && item_str.empty()) {
                item_str = std::string(str_left, str_right);
                str_left = str_right;
            }
        }
        if (parse_state == 1 || item_str.empty()) { // error item

        }
        if (!content_str.empty()) {
            pattern_parsed.push_back(std::make_tuple(content_str, "", 0));
            content_str.clear();
        }
        pattern_parsed.push_back(std::make_tuple(item_str, item_format_str, 1));
        item_str.clear();
        item_format_str.clear();
    }
    if (!content_str.empty()) {
        pattern_parsed.push_back(std::make_tuple(content_str, "", 0));
    }
    // static map from char to item constructor
    static std::map<std::string,   
        std::function<FormatItem::SharedPtr(const std::string&)> > char_to_item_map = {
#define XX(STR, CLASS) \
    { #STR, [](const std::string& str){ return FormatItem::SharedPtr(new CLASS(str)); } }

    XX(m, ContentFormatItem),
    XX(p, LevelFormatItem),
    XX(r, ElapseFormatItem),
    XX(c, LoggerFormatItem), // logger name
    XX(t, ThreadIdFormatItem),
    XX(n, NewLineFormatItem),
    XX(d, TimeFormatItem),
    XX(f, FileNameFormatItem),
    XX(l, LineFormatItem),
    XX(T, TabFormatItem),
    XX(F, FiberIdFormatItem),
    XX(N, ThreadNameFormatItem),
#undef XX
    };
    // add to format_items
    for (auto& i : pattern_parsed) {
        if (std::get<2>(i) == 0) {
            format_items_.push_back(FormatItem::SharedPtr(new StringFormatItem(std::get<0>(i))));
        } else {
            auto it = char_to_item_map.find(std::get<0>(i));
            if (it == char_to_item_map.end()) {
                format_items_.push_back(
                    FormatItem::SharedPtr(new StringFormatItem(
                    "<<error_format %" + std::get<0>(i) + ">>")));
            } else {
                format_items_.push_back(it->second(std::get<1>(i)));
            }
        }
        //std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
    }
    //std::cout << "-----------end parse-----------" << std::endl;
}


bool LoggerManager::AddLogger(std::shared_ptr<Logger> logger) {
    auto logger_name = logger->GetName();
    if (loggers_.find(logger_name) == loggers_.end()) {
        loggers_.insert(std::pair(logger_name, logger));
        return true;
    }
    return false;
}
bool LoggerManager::DeleteLogger(std::shared_ptr<Logger> logger) {
    auto logger_name = logger->GetName();
    if (loggers_.find(logger_name) != loggers_.end()) {
        loggers_.erase(logger_name);
        return true;
    }
    return false;
}
bool LoggerManager::DeleteLogger(const std::string& logger_name) {
    if (loggers_.find(logger_name) != loggers_.end()) {
        loggers_.erase(logger_name);
        return true;
    }
    return false;
}
void LoggerManager::ClearLoggers() {
    loggers_.clear();
}
std::shared_ptr<Logger> LoggerManager::GetLogger(const std::string& logger_name) {
    auto it = loggers_.find(logger_name);
    if (it != loggers_.end()) {
        return it->second; 
    } else {
        return root_logger_;
    }
}

LoggerManager::LoggerManager() { 
    root_logger_ = std::make_shared<Logger>("root", LogLevel::Level::DEBUG);
    StdoutLogAppender::SharedPtr stdout_log_appender(new StdoutLogAppender());
    root_logger_->AddAppender(stdout_log_appender);
    AddLogger(root_logger_);
}

};