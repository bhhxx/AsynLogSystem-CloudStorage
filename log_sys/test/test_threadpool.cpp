#include <iostream>
#include "../src/ThreadPool.hpp"
using namespace std;

int main() {
    ThreadPool pool(4); // Create a thread pool with 4 threads

    // Enqueue some tasks and return futures
    auto result1 = pool.enqueue([](int a, int b) { return a + b; }, 1, 2);
    auto result2 = pool.enqueue([](int a, int b) { return a * b; }, 3, 4);

    // Get the results from the futures
    cout << "Result of addition: " << result1.get() << endl; // Should print 3
    cout << "Result of multiplication: " << result2.get() << endl; // Should print 12

    return 0;
}