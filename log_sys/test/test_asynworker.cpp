#include "../src/AsynWorker.hpp"
#include <iostream>
asynlog::Util::JsonData* conf_data = asynlog::Util::JsonData::GetJsonData();
int main() {
    asynlog::functor cb = [](asynlog::Buffer& buf) {
        std::cout << "Callback called with buffer size: " << buf.ReadableSize() << std::endl;
    };
    asynlog::AsynWorker worker(cb, asynlog::AsynType::ASYNC_SAFE);
    const char* data = "Hello, World!";
    size_t len = strlen(data);
    worker.Push(data, len);
    std::this_thread::sleep_for(std::chrono::seconds(1)); // wait for the callback to be called
    return 0;
}