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
        // Allocate space in the remote process to store the dll path in it
        void *remoteLibraryPath = VirtualAllocEx(process, nullptr, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (remoteLibraryPath == nullptr)
            return Result::Err;

        // Write dll path to previously allocated memory
        auto pathString = this->m_path.wstring();
        if (!::WriteProcessMemory(process, remoteLibraryPath, pathString.c_str(), (pathString.length() + 1) * sizeof(decltype(pathString)::value_type), nullptr))
            return Result::Err;

        // Start a new thread in the remote process and make it load the dll
        auto hThread = ::CreateRemoteThread(process, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryW), remoteLibraryPath, 0, nullptr);
        if (hThread == INVALID_HANDLE_VALUE)
            return Result::Err;

        ON_SCOPE_EXIT {
            ::CloseHandle(hThread);
            ::VirtualFreeEx(process, remoteLibraryPath, MAX_PATH, MEM_RELEASE);
        };

        // Wait until thread has finished
        ::WaitForSingleObject(hThread, INFINITE);

        DWORD result;
        if (!::GetExitCodeThread(hThread, reinterpret_cast<LPDWORD>(&result)))
            return Result::Err;

        this->m_process = process;
        this->m_module = ::LoadLibraryW(this->m_path.c_str());

        return Result::Ok;
    }

    Result InjectFile::callImpl(const std::string &function, std::span<const std::byte> inBuffer, std::span<std::byte> outBuffer) {
        // Load library that contains this function into the loader
        auto hModule = ::LoadLibraryW(this->m_path.c_str());
        if (hModule == nullptr)
            return Result::Err;

        // Get local address of function to call
        auto localFunctionAddress = ::GetProcAddress(hModule, function.c_str());
        if (localFunctionAddress == nullptr)
            return Result::Err;

        // Calculate offset of the function from the start of the dll
        ptrdiff_t offset = reinterpret_cast<std::byte*>(localFunctionAddress) - reinterpret_cast<std::byte*>(hModule);

        // Calculate function address in remote process
        auto remoteFunctionAddress = reinterpret_cast<std::byte*>(this->m_module) + offset;

        ::FreeLibrary(hModule);

        // Marshal IPC message and place it in the remote process's memory
        void *messageBuffer = nullptr;
        api::IPCMessage message = { hlp::magic("CHT0") };

        // Allocate space in the remote process to fit the marshaled input parameters
        if (!inBuffer.empty()) {
            auto buffer = VirtualAllocEx(this->m_process, nullptr, inBuffer.size_bytes(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            if (buffer == nullptr)
                return Result::Err;

            if (!::WriteProcessMemory(this->m_process, buffer, inBuffer.data(), inBuffer.size_bytes(), nullptr))
                return Result::Err;

            message.inBuffer = { reinterpret_cast<std::byte*>(buffer), inBuffer.size_bytes() };
        }
        ON_SCOPE_EXIT { ::VirtualFreeEx(this->m_process, message.inBuffer.data(), message.inBuffer.size_bytes(), MEM_RELEASE); };

        // Allocate space in the remote process to fit the return values
        if (!outBuffer.empty()) {
            auto buffer = VirtualAllocEx(this->m_process, nullptr, outBuffer.size_bytes(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            if (buffer == nullptr)
                return Result::Err;

            message.outBuffer = { reinterpret_cast<std::byte*>(buffer), outBuffer.size_bytes() };
        }
        ON_SCOPE_EXIT { ::VirtualFreeEx(this->m_process, message.outBuffer.data(), message.outBuffer.size_bytes(), MEM_RELEASE); };

        // Allocate space in the remote process for the IPC message structure
        {
            messageBuffer = VirtualAllocEx(this->m_process, nullptr, sizeof(api::IPCMessage), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            if (messageBuffer == nullptr)
                return Result::Err;

            if (!::WriteProcessMemory(this->m_process, messageBuffer, &message, sizeof(api::IPCMessage), nullptr))
                return Result::Err;
        }
        ON_SCOPE_EXIT { ::VirtualFreeEx(this->m_process, messageBuffer, sizeof(api::IPCMessage), MEM_RELEASE); };

        // Call the function in a new thread in the remote process passing the pointer of the ipc message in as parameter
        auto hThread = ::CreateRemoteThread(this->m_process, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(remoteFunctionAddress), messageBuffer, 0, nullptr);
        if (hThread == INVALID_HANDLE_VALUE)
            return Result::Err;

        // Wait until function returns
        ::WaitForSingleObject(hThread, INFINITE);

        ON_SCOPE_EXIT {
            ::CloseHandle(hThread);
        };

        // Check result of thread function
        DWORD result = 0;
        if (!::GetExitCodeThread(hThread, &result) || Result(result) == Result::Err)
            return Result::Err;

        // Read return values of function from remote process
        SIZE_T bytesRead = 0;
        if (!::ReadProcessMemory(this->m_process, message.outBuffer.data(), outBuffer.data(), outBuffer.size_bytes(), &bytesRead))
            return Result::Err;

        return Result::Ok;
    }

}