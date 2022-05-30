#include "../src/logger.hpp"

using namespace mysylar;
int main() {
    Logger::SharedPtr logger(new Logger()); // new logger
    StdoutLogAppender::SharedPtr stdout_log_appender(new StdoutLogAppender()); // new logappender
    logger->AddAppender(stdout_log_appender); // add logappender to logger
    LogEvent::SharedPtr event(new LogEvent(__FILE__, time(0), 0, __LINE__, 1, "logtest thread", 2, logger, LogLevel::Level::DEBUG)); // new event
    logger->Debug(event);
    logger->Fatal(event);
    LDEBUG(logger) << "hello";
    Formatter::SharedPtr formatter(new Formatter("[%p]%d%T%f:%l%T%n")); //new format
    stdout_log_appender->SetFormatter(formatter);
    logger->Debug(event);
    logger->Fatal(event);
    return 0;
}
