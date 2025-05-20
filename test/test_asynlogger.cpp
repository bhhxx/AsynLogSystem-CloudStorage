#include "../src/AsynLogger.hpp"
#include "../src/LogFlush.hpp"

#include <iostream>
#include <vector>
#include <memory>
asynlog::Util::JsonData* conf_data = asynlog::Util::JsonData::GetJsonData();

int main() {
    // // class AsynLogger
    // std::vector<asynlog::LogFlush::ptr> flushes;
    // flushes.push_back(std::make_shared<asynlog::StdOutFlush>());
    // flushes.push_back(std::make_shared<asynlog::StdOutFlush>());
    // asynlog::AsynLogger logger("test", asynlog::AsynType::ASYNC_SAFE, flushes);
    // logger.Info(__FILE__, __LINE__, "Hello %s, number = %d", "World", 42);
    // logger.Debug(__FILE__, __LINE__, "Hello %s, number = %d", "World", 42);
    // logger.Error(__FILE__, __LINE__, "Hello %s, number = %d", "World", 42);
    // logger.Fatal(__FILE__, __LINE__, "Hello %s, number = %d", "World", 42);
    // logger.Warn(__FILE__, __LINE__, "Hello %s, number = %d", "World", 42);
    // sleep(10); // wait the child thread flush all, otherwise the logger destroy

    // // class LoggerBuilder
    // asynlog::LoggerBuilder builder;
    // builder.BuildLoggerName("test");
    // builder.BuildLoggerType(asynlog::AsynType::ASYNC_SAFE);
    // builder.BuildLoggerFlush<asynlog::StdOutFlush>();
    // asynlog::AsynLogger::ptr logger = builder.Build();
    // logger->Info(__FILE__, __LINE__, "Hello %s, number = %d", "World", 42);
    // logger->Debug(__FILE__, __LINE__, "Hello %s, number = %d", "World", 42);
    // logger->Error(__FILE__, __LINE__, "Hello %s, number = %d", "World", 42);
    // logger->Fatal(__FILE__, __LINE__, "Hello %s, number = %d", "World", 42);
    // logger->Warn(__FILE__, __LINE__, "Hello %s, number = %d", "World", 42);
    // sleep(10); // wait the child thread flush all, otherwise the logger destroy
    return 0;
}