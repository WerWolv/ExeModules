#include <mod/helpers/process.hpp>

#include <mod/helpers/guards.hpp>

#include <TlHelp32.h>

namespace mod::hlp {

    std::optional<u32> getProcessId(const std::wstring &processName) {
        // Create process snapshot
        auto hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) {
            return std::nullopt;
        }

        ON_SCOPE_EXIT { ::CloseHandle(hSnapshot); };

        PROCESSENTRY32W processEntry = { 0 };
        processEntry.dwSize = sizeof(PROCESSENTRY32W);

        // Iterate over all processes in snapshot
        if (::Process32FirstW(hSnapshot, &processEntry)) {
            do {
                // If a process with the requested name is found, return its pid
                if (std::wstring(processEntry.szExeFile) == processName)
                    return processEntry.th32ProcessID;

            } while (::Process32NextW(hSnapshot, &processEntry));
        }

        return std::nullopt;
    }

}