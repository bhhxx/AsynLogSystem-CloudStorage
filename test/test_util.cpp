#include "../src/Util.hpp"
#include <iostream>
#include <fstream> // for istream
using namespace std;
using namespace asynlog;
using namespace Util;
int main() {
    // test Date::Now()
    cout << Date::Now() << endl;

    // test File::Exists()
    cout << File::Exists("../src/Util.hpp") << endl;
    cout << File::Exists("../src/Util.h") << endl;

    // test File::Path()    
    cout << File::Path("/") << endl;

    // test File::CreateDirectory()
    File::CreateDirectory("./");
    
    // test File::FileSize()
    cout << File::FileSize("../src/Util.hpp") << endl;

    // test File::GetContent()
    string content;
    File::GetContent(&content, "../src/Util.hpp");
    cout << content << endl;

    // test JsonUtil::UnSerialize()
    string json_str = R"({"name": "asynlog", "version": "1.0.0"})";
    Json::Value val;
    if (JsonUtil::UnSerialize(json_str, &val)) {
        cout << val["name"].asString() << endl;
        cout << val["version"].asString() << endl;
    }

    // test JsonUtil::Serialize()
    Json::Value val2;
    val2["name"] = "asynlog";
    val2["version"] = "1.0.0";
    string json_str2;
    if (JsonUtil::Serialize(val2, &json_str2)) {
        cout << json_str2 << endl;
    }
    ofstream ofs("test.json");
    if (!ofs.is_open()) {
        cout << "Open file failed" << endl;
        return 0;
    }
    ofs << json_str2;

    // test JsonData::GetJsonData()
    JsonData* json_data = JsonData::GetJsonData();
    cout << json_data->buffer_size << endl;
    cout << json_data->threshold << endl;
    return 0;
}
