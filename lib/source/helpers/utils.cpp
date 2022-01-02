#include <cht/helpers/utils.hpp>

#include <cstdio>
#include <fmt/format.h>

namespace cht::hlp {

    void redirectStdio(u32 pid) {
        AttachConsole(pid);

        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);
        setvbuf(stdin,  nullptr, _IONBF, 0);
        setvbuf(stdout, nullptr, _IONBF, 0);
    }

}