#pragma once

#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/xchar.h>

namespace cht::log {

    template<typename ... T>
    void debug(const fmt::format_string<T...> &fmt, T&& ... args) {
        #if defined(DEBUG)
            fmt::print(stdout, "[{0: ^12s}] ", LOGGER_SOURCE_NAME);
            fmt::print(fg(fmt::color::green_yellow) | fmt::emphasis::bold, "[D] ");
            fmt::print(stdout, fmt, args...);
            fmt::print("\n");
        #endif
    }

    template<typename ... T>
    void info(const fmt::format_string<T...> &fmt, T&& ... args) {
        fmt::print(stdout, "[{0: ^12s}] ", LOGGER_SOURCE_NAME);
        fmt::print(stdout, fg(fmt::color::cadet_blue) | fmt::emphasis::bold, "[i] ");
        fmt::print(stdout, fmt, args...);
        fmt::print(stdout, "\n");
    }

    template<typename ... T>
    void warn(const fmt::format_string<T...> &fmt, T&& ... args) {
        fmt::print(stdout, "[{0: ^12s}] ", LOGGER_SOURCE_NAME);
        fmt::print(fg(fmt::color::orange) | fmt::emphasis::bold, "[*] ");
        fmt::print(stdout, fmt, args...);
        fmt::print("\n");
    }

    template<typename ... T>
    void error(const fmt::format_string<T...> &fmt, T&& ... args) {
        fmt::print(stdout, "[{0: ^12s}] ", LOGGER_SOURCE_NAME);
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "[!] ");
        fmt::print(stdout, fmt, args...);
        fmt::print("\n");
    }

    template<typename ... T>
    void fatal(const fmt::format_string<T...> &fmt, T&& ... args) {
        fmt::print(stdout, "[{0: ^12s}] ", LOGGER_SOURCE_NAME);
        fmt::print(fg(fmt::color::purple) | fmt::emphasis::bold, "[#] ");
        fmt::print(stdout, fmt, args...);
        fmt::print("\n");
    }

    template<typename ... T>
    void debug(const fmt::wformat_string<T...> &fmt, T&& ... args) {
        #if defined(DEBUG)
            fmt::print(stdout, "[{0: ^12s}] ", LOGGER_SOURCE_NAME);
            fmt::print(fg(fmt::color::green_yellow) | fmt::emphasis::bold, "[D] ");
            fmt::print(stdout, fmt, args...);
            fmt::print("\n");
        #endif
    }

    template<typename ... T>
    void info(const fmt::wformat_string<T...> &fmt, T&& ... args) {
        fmt::print(stdout, "[{0: ^12s}] ", LOGGER_SOURCE_NAME);
        fmt::print(stdout, fg(fmt::color::cadet_blue) | fmt::emphasis::bold, "[i] ");
        fmt::print(stdout, fmt, args...);
        fmt::print(stdout, "\n");
    }

    template<typename ... T>
    void warn(const fmt::wformat_string<T...> &fmt, T&& ... args) {
        fmt::print(stdout, "[{0: ^12s}] ", LOGGER_SOURCE_NAME);
        fmt::print(fg(fmt::color::orange) | fmt::emphasis::bold, "[*] ");
        fmt::print(stdout, fmt, args...);
        fmt::print("\n");
    }

    template<typename ... T>
    void error(const fmt::wformat_string<T...> &fmt, T&& ... args) {
        fmt::print(stdout, "[{0: ^12s}] ", LOGGER_SOURCE_NAME);
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "[!] ");
        fmt::print(stdout, fmt, args...);
        fmt::print("\n");
    }

    template<typename ... T>
    void fatal(const fmt::wformat_string<T...> &fmt, T&& ... args) {
        fmt::print(stdout, "[{0: ^12s}] ", LOGGER_SOURCE_NAME);
        fmt::print(fg(fmt::color::purple) | fmt::emphasis::bold, "[#] ");
        fmt::print(stdout, fmt, args...);
        fmt::print("\n");
    }

}