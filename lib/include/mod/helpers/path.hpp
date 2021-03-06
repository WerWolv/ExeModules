#pragma once

#include <mod/base.hpp>

#include <filesystem>

namespace fs = std::filesystem;

namespace mod::hlp {

    [[nodiscard]]
    fs::path getModulePath(HMODULE module);

    [[nodiscard]]
    fs::path getLibraryPath();

    [[nodiscard]]
    fs::path getModulesFolderPath();

}