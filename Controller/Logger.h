#pragma once
#include <string>
#include <vector>

class Logger {
public:
    static void log(const std::string& text);
    static const std::vector<std::string>& lines();
};
