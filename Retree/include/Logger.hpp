#pragma once
#include <string>
#include <print>
#include <format>
class Logger {
public:

	static void Error(const std::string& msg, auto&&... args) {
		std::print("\033[31m[error]\033[0m {}\n", std::vformat(msg, std::make_format_args(args...)));
	}
	static void Info(const std::string& msg, auto&&... args) {
		std::print("\033[32m[info]\033[0m {}\n", std::vformat(msg, std::make_format_args(args...)));
	}
	static void Warn(const std::string& msg, auto&&... args) {
		std::print("\033[33m[warn]\033[0m {}\n", std::vformat(msg, std::make_format_args(args...)));
	}
	static void Debug(const std::string& msg, auto&&... args) {
		std::print("\033[38;5;245m[debug]\033[0m {}\n", std::vformat(msg, std::make_format_args(args...)));
	}
	
};
