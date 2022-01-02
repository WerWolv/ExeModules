#include <mod/base.hpp>

#include <mod/helpers/path.hpp>
#include <mod/helpers/process.hpp>
#include <mod/helpers/inject_file.hpp>
#include <mod/helpers/guards.hpp>
#include <mod/helpers/logger.hpp>

#include <chrono>
#include <thread>
#include <vector>

using namespace mod;
using namespace std::literals::chrono_literals;

int main() {
    log::info("Starting loader...");

    STARTUPINFOW startupInfo = { };
    PROCESS_INFORMATION processInfo = { };

    // Start launch executable
    {
        log::info(L"Starting launcher executable '{}'...", LAUNCH_EXECUTABLE_PATH);

        if (!::CreateProcessW(
                LAUNCH_EXECUTABLE_PATH, ::GetCommandLineW(),
                nullptr, nullptr, TRUE, 0, nullptr, nullptr,
                &startupInfo, &processInfo
            ))
        {
            log::fatal("Failed to start launcher executable!");
            return EXIT_FAILURE;
        }

    }

    // Wait for real executable to be launched
    HANDLE hProcess = INVALID_HANDLE_VALUE;

    std::optional<u32> pid;
    {
        log::info(L"Waiting for main executable '{}' to be launched...", MAIN_PROCESS_NAME);

        // Wait until the process we want to attach to becomes available
        while (true) {
            pid = mod::hlp::getProcessId(MAIN_PROCESS_NAME);

            if (pid.has_value())
                break;
            else
                std::this_thread::sleep_for(100ms);
        }

        log::info("Main executable launched! PID: {}", pid.value());

        // Attach to process
        hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid.value());

        if (hProcess == INVALID_HANDLE_VALUE) {
            log::fatal("Failed to attach to main executable!");
            return EXIT_FAILURE;
        }

    }

    // Inject libmodule into target process
    mod::hlp::InjectFile libModule(mod::hlp::getLibraryPath());
    if (libModule.injectInto(hProcess) == Result::Err) {
        log::fatal("Failed to inject libmodule!");
        return EXIT_FAILURE;
    }


    // Inject all modules into target process
    for (const auto &entry : fs::recursive_directory_iterator(mod::hlp::getModulesFolderPath())) {
        // Skip all entries that are not regular files
        if (!entry.is_regular_file()) continue;

        // Skip all files that don't have the .mod file extension
        if (entry.path().extension() != ".mod") continue;

        log::info(L"Loading module '{}'...", entry.path().filename().native());

        // Ask the injected libmodule to load the new module and return its load address
        auto hModule = libModule.call<HMODULE>("loadModule", hlp::StaticString<wchar_t, MAX_PATH>(entry.path().native()));
        if (!hModule.has_value()) {
            log::error(L"Failed to load module '{}'!", entry.path().native());
            continue;
        }

        void *address = reinterpret_cast<void*>(*hModule);
        log::info("Loaded module at {}", address);

        // Ask module to attach stdout to the main console
        auto module = mod::hlp::InjectFile(entry.path(), hProcess, *hModule);
        if (module.call<void>("attachConsole", ::GetCurrentProcessId()) == Result::Err) {
            log::error("Module failed to attach to loader console!");
            continue;
        }

        // Call main function of module
        if (module.call<void>("main") == Result::Err) {
            log::error("Failed to initialize module!");
            continue;
        }

    }

    // Keep loader and its console open until main process exits
    ::WaitForSingleObject(hProcess, INFINITE);
}