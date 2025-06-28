#include "../src/Manager.hpp"
asynlog::Util::JsonData* conf_data = asynlog::Util::JsonData::GetJsonData();
ThreadPool *tp = new ThreadPool(4);
int main() {
    std::shared_ptr<asynlog::LoggerBuilder> builder = std::make_shared<asynlog::LoggerBuilder>();
    builder->BuildLoggerName("test_logger");
    builder->BuildLoggerType(asynlog::AsynType::ASYNC_SAFE);
    builder->BuildLoggerFlush<asynlog::StdOutFlush>();

    asynlog::LoggerManager::GetInstance().AddLogger(builder->Build());
    asynlog::LoggerManager::GetInstance().GetLogger("test_logger")->Info(__FILE__, __LINE__, "This is a test log message: %s", "Hello, World!");
    sleep(1); // Sleep for 1 second to allow async logging to complete
    asynlog::LoggerManager::GetInstance().GetLogger("test_logger")->Warn(__FILE__, __LINE__, "This is a test log message: %s", "Hello, World!");
    sleep(1); // Sleep for 1 second to allow async logging to complete
    asynlog::LoggerManager::GetInstance().GetLogger("test_logger")->Info(__FILE__, __LINE__, "This is a test log message: %s", "Hello, World!");
    sleep(1); // Sleep for 1 second to allow async logging to complete
    asynlog::LoggerManager::GetInstance().GetLogger("test_logger")->Debug(__FILE__, __LINE__, "This is a test log message: %s", "Hello, World!");
    sleep(1); // Sleep for 1 second to allow async logging to complete
    asynlog::LoggerManager::GetInstance().GetLogger("test_logger")->Info(__FILE__, __LINE__, "This is a test log message: %s", "Hello, World!");
    sleep(1); // Sleep for 1 second to allow async logging to complete
    asynlog::LoggerManager::GetInstance().GetLogger("test_logger")->Error(__FILE__, __LINE__, "This is a test log message: %s", "Hello, World!");
    sleep(1); // Sleep for 1 second to allow async logging to complete
    // delete tp;
    return 0;
}