#pragma once

#include <cht/base.hpp>

#include <filesystem>

namespace fs = std::filesystem;

namespace cht::hlp {

    [[nodiscard]]
    fs::path getModulePath(HMODULE module);

    [[nodiscard]]
    fs::path getLibraryPath();

    [[nodiscard]]
    fs::path getCheatsFolderPath();

}