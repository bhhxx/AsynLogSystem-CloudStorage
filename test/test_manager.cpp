#include "../src/Manager.hpp"
asynlog::Util::JsonData* conf_data = asynlog::Util::JsonData::GetJsonData(); 
int main() {
    std::shared_ptr<asynlog::LoggerBuilder> builder = std::make_shared<asynlog::LoggerBuilder>();
    builder->BuildLoggerName("test_logger");
    builder->BuildLoggerType(asynlog::AsynType::ASYNC_SAFE);
    builder->BuildLoggerFlush<asynlog::StdOutFlush>();

    asynlog::LoggerManager::GetInstance().AddLogger(builder->Build());
    asynlog::LoggerManager::GetInstance().GetLogger("test_logger")->Info(__FILE__, __LINE__, "This is a test log message: %s", "Hello, World!");
    sleep(1); // Sleep for 1 second to allow async logging to complete
    return 0;
}