#include <cht/helpers/path.hpp>

namespace cht::hlp {

    fs::path getModulePath(HMODULE module) {
        std::wstring buffer(MAX_PATH, '\x00');
        ::GetModuleFileNameW(module, buffer.data(), buffer.size());

        return buffer;
    }

    fs::path getLibraryPath() {
        return getModulePath(::GetModuleHandle("libcheat.dll"));
    }

    fs::path getCheatsFolderPath() {
        return getLibraryPath().parent_path() / "cheats";
    }

}