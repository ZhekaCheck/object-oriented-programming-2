#include "Logger.h"
#include <fstream>

namespace {
std::vector<std::string> history;

std::ofstream& logFile() {
    static std::ofstream out("game.log", std::ios::app);
    return out;
}
}

void Logger::log(const std::string& text) {
    history.push_back(text);
    logFile() << text << std::endl;
}

const std::vector<std::string>& Logger::lines() {
    return history;
}
