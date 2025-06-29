#pragma once

#include <ctime>
#include <iostream>
#include <sstream>

enum class LogLevel { INFO, SUCCESS, ERROR, DEBUG };

class Logger {
public:
  template <typename... Args> static void log(LogLevel level, Args &&...args) {
    std::ostringstream oss;
    (oss << ... << args);

    std::string prefix, color;
    switch (level) {
    case LogLevel::INFO:
      prefix = "[INFO]  ";
      color = "\033[33m";
      break;
    case LogLevel::SUCCESS:
      prefix = "[✓]     ";
      color = "\033[32m";
      break;
    case LogLevel::ERROR:
      prefix = "[✗]     ";
      color = "\033[31m";
      break;
    case LogLevel::DEBUG:
      prefix = "[DEBUG] ";
      color = "\033[35m";
      break;
    }

    auto &out = (level == LogLevel::ERROR) ? std::cerr : std::cout;
    // out << color << prefix << oss.str() << "\033[0m\n";

    out << timestamp() << color << " | " << prefix << oss.str() << "\033[0m\n";
  }

private:
  static std::string timestamp() {
    std::time_t t = std::time(nullptr);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&t));
    return buf;
  }
};

#define LOG_INFO(...) Logger::log(LogLevel::INFO, __VA_ARGS__)
#define LOG_SUCCESS(...) Logger::log(LogLevel::SUCCESS, __VA_ARGS__)
#define LOG_ERROR(...) Logger::log(LogLevel::ERROR, __VA_ARGS__)
#define LOG_DEBUG(...) Logger::log(LogLevel::DEBUG, __VA_ARGS__)
