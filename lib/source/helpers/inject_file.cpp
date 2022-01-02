#include <cht/helpers/inject_file.hpp>

#include <cht/helpers/guards.hpp>
#include <cht/api/ipc.hpp>

#include <utility>

namespace cht::hlp {

    InjectFile::InjectFile(fs::path path) : m_path(std::move(path)) {

    }

    InjectFile::InjectFile(fs::path path, HANDLE process, HMODULE module) : m_path(std::move(path)), m_process(process), m_module(module) {

    }

    Result InjectFile::injectInto(HANDLE process) {
        void *remoteLibraryPath = VirtualAllocEx(process, nullptr, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (remoteLibraryPath == nullptr)
            return Result::Err;

        auto pathString = this->m_path.wstring();
        if (!::WriteProcessMemory(process, remoteLibraryPath, pathString.c_str(), (pathString.length() + 1) * sizeof(decltype(pathString)::value_type), nullptr))
            return Result::Err;

        auto hThread = ::CreateRemoteThread(process, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryW), remoteLibraryPath, 0, nullptr);
        if (hThread == INVALID_HANDLE_VALUE)
            return Result::Err;

        ON_SCOPE_EXIT {
            ::CloseHandle(hThread);
            ::VirtualFreeEx(process, remoteLibraryPath, MAX_PATH, MEM_RELEASE);
        };

        ::WaitForSingleObject(hThread, INFINITE);

        DWORD result;
        if (!::GetExitCodeThread(hThread, reinterpret_cast<LPDWORD>(&result)))
            return Result::Err;

        this->m_process = process;
        this->m_module = ::LoadLibraryW(this->m_path.c_str());

        return Result::Ok;
    }

    Result InjectFile::callImpl(const std::string &function, std::span<const std::byte> inBuffer, std::span<std::byte> outBuffer) {
        auto hModule = ::LoadLibraryW(this->m_path.c_str());
        if (hModule == nullptr)
            return Result::Err;

        auto localFunctionAddress = ::GetProcAddress(hModule, function.c_str());
        if (localFunctionAddress == nullptr)
            return Result::Err;

        ptrdiff_t offset = reinterpret_cast<std::byte*>(localFunctionAddress) - reinterpret_cast<std::byte*>(hModule);

        auto remoteFunctionAddress = reinterpret_cast<std::byte*>(this->m_module) + offset;

        ::FreeLibrary(hModule);

        api::IPCMessage message = { hlp::magic("CHT0") };

        if (!inBuffer.empty()) {
            auto buffer = VirtualAllocEx(this->m_process, nullptr, inBuffer.size_bytes(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            if (buffer == nullptr)
                return Result::Err;

            if (!::WriteProcessMemory(this->m_process, buffer, inBuffer.data(), inBuffer.size_bytes(), nullptr))
                return Result::Err;

            message.inBuffer = { reinterpret_cast<std::byte*>(buffer), inBuffer.size_bytes() };
        }

        if (!outBuffer.empty()) {
            auto buffer = VirtualAllocEx(this->m_process, nullptr, outBuffer.size_bytes(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            if (buffer == nullptr)
                return Result::Err;

            message.outBuffer = { reinterpret_cast<std::byte*>(buffer), outBuffer.size_bytes() };
        }

        void* messageBuffer = nullptr;
        {
            messageBuffer = VirtualAllocEx(this->m_process, nullptr, sizeof(api::IPCMessage), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            if (messageBuffer == nullptr)
                return Result::Err;

            if (!::WriteProcessMemory(this->m_process, messageBuffer, &message, sizeof(api::IPCMessage), nullptr))
                return Result::Err;
        }

        auto hThread = ::CreateRemoteThread(this->m_process, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(remoteFunctionAddress), messageBuffer, 0, nullptr);
        if (hThread == INVALID_HANDLE_VALUE)
            return Result::Err;

        ::WaitForSingleObject(hThread, INFINITE);

        ON_SCOPE_EXIT {
            ::CloseHandle(hThread);
        };

        DWORD result = 0;
        if (!::GetExitCodeThread(hThread, &result))
            return Result::Err;

        SIZE_T bytesRead = 0;
        if (!::ReadProcessMemory(this->m_process, message.outBuffer.data(), outBuffer.data(), outBuffer.size_bytes(), &bytesRead))
            return Result::Err;

        return Result::Ok;
    }

}