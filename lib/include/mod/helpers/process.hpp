#pragma once

#include <mod/base.hpp>

#include <optional>
#include <string>

namespace mod::hlp {

    [[nodiscard]]
    std::optional<u32> getProcessId(const std::wstring &processName);

    [[nodiscard]]
    u64 getFunctionAddress(const std::string &moduleName, const std::string &functionName);

}