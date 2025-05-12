#include "../src/Level.hpp"
#include <iostream>
using namespace std;
using namespace asynlog;
int main() {
    LogLevel::value level = LogLevel::value::DEBUG;
    cout << LogLevel::ToString(level) << endl;
}