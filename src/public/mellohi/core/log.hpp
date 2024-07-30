#pragma once
#pragma once

#include <print>
#include <sstream>

#include "mellohi/core/types.hpp"

namespace mellohi
{
    template <typename... Args>
    void log_helper(const str color, const str tag, const char* file_name, int line,
        const str message, Args&&... args)
    {
        auto to_string = [](const auto& value) -> string
        {
            std::ostringstream oss;
            oss << value;
            return oss.str();
        };

        const string formatted_message = std::vformat(
            message,
            std::make_format_args(to_string(std::forward<Args>(args))...)
        );

        constexpr auto color_reset = "\033[0m";
        std::println("{}[{}] {}({}): {}{}", color, tag, file_name, line, formatted_message, color_reset);
    }

    template <typename... Args>
    void log_info(const char* file_name, int line, const str message, Args&&... args)
    {
        constexpr auto color_reset = "\033[0m";
        log_helper(color_reset, "INFO", file_name, line, message, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void log_warn(const char* file_name, int line, const str message, Args&&... args)
    {
        constexpr auto color_yellow = "\033[33m";
        log_helper(color_yellow, "WARN", file_name, line, message, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void log_error(const char* file_name, int line, const str message, Args&&... args)
    {
        constexpr auto color_red = "\033[31m";
        log_helper(color_red, "ERROR", file_name, line, message, std::forward<Args>(args)...);
    }
}

#define MH_INFO(message, ...) mellohi::log_info(__FILE_NAME__, __LINE__, message, ##__VA_ARGS__)
#define MH_WARN(message, ...) mellohi::log_warn(__FILE_NAME__, __LINE__, message, ##__VA_ARGS__)
#define MH_ERROR(message, ...) mellohi::log_error(__FILE_NAME__, __LINE__, message, ##__VA_ARGS__)

#define MH_ASSERT(condition, message, ...) \
    if (!(condition)) { \
    MH_ERROR(message, ##__VA_ARGS__); \
    std::abort(); \
    }
