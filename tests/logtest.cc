#include "../src/logger.hpp"
#include "../src/singleton.hpp"

using namespace mysylar;
int main() {
    // 1. log directly
    LDEBUG("root") << "log directly using root logger";
    LRINFO << "log directly using root logger";
    LFATAL("") << "log directly using root logger";

    // 2. log by new logger
    Logger::SharedPtr test_logger(new Logger("test_logger", LogLevel::Level::DEBUG)); // new logger
    StdoutLogAppender::SharedPtr stdout_log_appender(new StdoutLogAppender()); // new stdout log appender
    FileLogAppender::SharedPtr file_log_appender(new FileLogAppender("./log.txt")); // new file log appender
    test_logger->AddAppender(stdout_log_appender); // attach log appender to logger
    test_logger->AddAppender(file_log_appender); // attach log appender to logger
    file_log_appender->SetLevel(LogLevel::Level::ERROR);
    LoggerManager::GetInstance().AddLogger(test_logger); // attach logger to logger manager
    LDEBUG("test_logger") << "log using new logger";
    LFATAL("test_logger") << "log using new logger";

    //3. log by new log event
    LogEvent::SharedPtr event(new LogEvent(__FILE__, time(0), 0, __LINE__, 1, "logtest thread", 2, test_logger, LogLevel::Level::ERROR)); // new event
    event->GetStringStream() << "log using event";
    Formatter::SharedPtr formatter(new Formatter("[%p]%d%T%f:%l%T%n")); //new format
    stdout_log_appender->SetFormatter(formatter); // set format
    LoggerManager::GetInstance().GetLogger("test_logger")->Log(LogLevel::DEBUG, event);
    //test_logger->Debug(event);
    //test_logger->Fatal(event);
    return 0;
}
