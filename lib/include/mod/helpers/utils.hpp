#pragma once

#include <mod/base.hpp>

#include <string_view>

namespace mod::hlp {

    consteval u32 magic(std::string_view str) {
        u32 magic = 0x00;

        for (const char &c : str) {
            magic >>= 4;
            magic |= c << 24;
        }

        return magic;
    }

    void redirectStdio(u32 pid);

}