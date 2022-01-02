#include <mod/helpers/path.hpp>

namespace mod::hlp {

    fs::path getModulePath(HMODULE module) {
        std::wstring buffer(MAX_PATH, '\x00');
        ::GetModuleFileNameW(module, buffer.data(), buffer.size());

        return buffer;
    }

    fs::path getLibraryPath() {
        return getModulePath(::GetModuleHandle(LIBRARY_FILE_NAME));
    }

    fs::path getModulesFolderPath() {
        return getLibraryPath().parent_path() / "modules";
    }

}