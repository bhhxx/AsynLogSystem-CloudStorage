#include "../src/AsynBuffer.hpp"
#include <iostream>
using namespace asynlog;
using namespace std;
asynlog::Util::JsonData* conf_data;
int main() {
    conf_data = asynlog::Util::JsonData::GetJsonData(); // get singleton instance
    // test buffer
    Buffer buf1, buf2;

    // test IsEmpty
    cout << "buf1 is empty so the out is 1" << endl;
    cout << buf1.IsEmpty() << endl << endl;
    
    // test Push
    buf1.Push("hello", 5);
    cout << "Push hello into buf1, so readable size is 5" << endl;
    cout << buf1.ReadableSize() << endl << endl;

    // test ReadBegin
    cout << "Read hello from buf1" << endl;
    char *data = buf1.ReadBegin(5);
    cout << string(data, 5) << endl << endl;

    // test MoveReadPos
    cout << "Move read position by 1" << endl;
    cout << "so the readable size is 4" << endl;
    buf1.MoveReadPos(1);
    cout << buf1.ReadableSize() << endl << endl;

    // test MoveWritePos
    cout << "Move write position by 1" << endl;
    cout << "so the readable size is 5" << endl;
    buf1.MoveWritePos(1);
    cout << buf1.ReadableSize() << endl << endl;

    // test Swap
    cout << "Swap buf1 and buf2" << endl;
    buf1.Swap(buf2);
    cout << "buf1 is empty so the out is 1" << endl;
    cout << buf1.IsEmpty() << endl;
    cout << "buf2 is not empty so the out is 0" << endl;
    cout << buf2.IsEmpty() << endl << endl;

    // test Reset
    cout << "Reset buf2" << endl;
    buf2.Reset();
    cout << "buf2 is empty so the out is 1" << endl;
    cout << buf2.IsEmpty() << endl << endl;
}
