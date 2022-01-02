#pragma once

#include <cht/base.hpp>

#include <optional>
#include <string>

namespace cht::hlp {

    [[nodiscard]]
    std::optional<u32> getProcessId(const std::wstring &processName);

}