#include <iostream>
#include <string>
#include "../src/LogFlush.hpp"
asynlog::Util::JsonData* g_conf_data = asynlog::Util::JsonData::GetJsonData();
int main() {
    auto flush = asynlog::LogFlushFactory::CreateLog<asynlog::StdOutFlush>();
    flush->Flush("test", 4);

    auto flush2 = asynlog::LogFlushFactory::CreateLog<asynlog::FileFlush>("jzq/jzq.txt");
    flush2->Flush("test", 4);
    
    auto flush3 = asynlog::LogFlushFactory::CreateLog<asynlog::RollFileFlush>("jzq/", 1000);
    std::string str(10000, 'a');
    flush3->Flush(str.c_str(), str.size());
}
