#pragma once

#include <print>

namespace mellohi
{
    template <typename... Args>
    void log_helper(const std::string_view color, const std::string_view tag, const char* file_name, int line,
                    const std::string_view message, Args&&... args)
    {
        constexpr auto color_reset = "\033[0m";
        std::println("{}[{}] {}({}): {}{}", color, tag, file_name, line, std::vformat(message, std::make_format_args(args...)), color_reset);
    }

    template <typename... Args>
    void log_info(const char* file_name, int line, const std::string_view message, Args&&... args)
    {
        constexpr auto color_reset = "\033[0m";
        log_helper(color_reset, "INFO", file_name, line, message, args...);
    }

    template <typename... Args>
    void log_warn(const char* file_name, int line, const std::string_view message, Args&&... args)
    {
        constexpr auto color_yellow = "\033[33m";
        log_helper(color_yellow, "WARN", file_name, line, message, args...);
    }

    template <typename... Args>
    void log_error(const char* file_name, int line, const std::string_view message, Args&&... args)
    {
        constexpr auto color_red = "\033[31m";
        log_helper(color_red, "ERROR", file_name, line, message, args...);
    }
}

#define MH_INFO(message, ...) mellohi::log_info(__FILE_NAME__, __LINE__, message, ##__VA_ARGS__)
#define MH_WARN(message, ...) mellohi::log_warn(__FILE_NAME__, __LINE__, message, ##__VA_ARGS__)
#define MH_ERROR(message, ...) mellohi::log_error(__FILE_NAME__, __LINE__, message, ##__VA_ARGS__)

#define MH_ASSERT(condition, message, ...) \
    if (!(condition)) { \
        MH_ERROR("Assertion failed: " + std::string(message), ##__VA_ARGS__); \
        std::abort(); \
    }
