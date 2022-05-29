#include "../src/logger.h"

using namespace mysylar;
int main() {
    Logger::SharedPtr logger(new Logger());
    StdoutLogAppender::SharedPtr stdout_log_appender(new StdoutLogAppender());
    logger->AddAppender(stdout_log_appender);
    LogEvent::SharedPtr event(new LogEvent(__FILE__, time(0), 0, __LINE__, 1, "logtest thread", 2));
    Formatter::SharedPtr formatter(new Formatter("[%p]%d%T%f:%l%T%n"));
    stdout_log_appender->SetFormatter(formatter);
    logger->Debug(event);
    logger->Fatal(event);
    formatter.reset(new Formatter());
    stdout_log_appender->SetFormatter(formatter);
    logger->Debug(event);
    logger->Fatal(event);
    return 0;
}
