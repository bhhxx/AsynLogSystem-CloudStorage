#include <iostream> // for cout 
#include <string> // for string
#include "../src/Level.hpp" // for level
#include "../src/Message.hpp" // for LogMessage

int main() {
    // Create a LogMessage object
    asynlog::LogMessage log(asynlog::LogLevel::value::INFO, __FILE__, __LINE__, "log", "This is a test log message");

    // Format the log message
    std::string formatted_log = log.format();

    // Print the formatted log message
    std::cout << formatted_log;

    return 0;
}
