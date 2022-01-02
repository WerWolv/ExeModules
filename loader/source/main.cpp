#include <cht/base.hpp>

#include <cht/helpers/path.hpp>
#include <cht/helpers/process.hpp>
#include <cht/helpers/inject_file.hpp>
#include <cht/helpers/guards.hpp>
#include <cht/helpers/logger.hpp>

#include <chrono>
#include <thread>
#include <vector>

using namespace cht;
using namespace std::literals::chrono_literals;

int main() {
    log::info("Cheat Loader\n");

    STARTUPINFOW startupInfo = { };
    PROCESS_INFORMATION processInfo = { };

    // Start launch executable
    {
        log::info(L"Starting launcher executable '{}'...", LIBCHEAT_LAUNCH_EXECUTABLE_PATH);

        if (!::CreateProcessW(
                LIBCHEAT_LAUNCH_EXECUTABLE_PATH, ::GetCommandLineW(),
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
        log::info(L"Waiting for main executable '{}' to be launched...", LIBCHEAT_PROCESS_NAME);

        // Wait until the process we want to attach to becomes available
        while (true) {
            pid = cht::hlp::getProcessId(LIBCHEAT_PROCESS_NAME);

            if (pid.has_value())
                break;
            else
                std::this_thread::sleep_for(1s);
        }

        log::info("Main executable launched! PID: {}", pid.value());

        // Attach to process
        hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid.value());
        if (hProcess == INVALID_HANDLE_VALUE) {
            log::fatal("Failed to attach to main executable!");
            return EXIT_FAILURE;
        }

    }

    // Inject libcheat into target process
    cht::hlp::InjectFile libCheat(cht::hlp::getLibraryPath());
    if (libCheat.injectInto(hProcess) == Result::Err) {
        log::fatal("Failed to inject libcheat!");
        return EXIT_FAILURE;
    }


    // Inject all cheats into target process
    for (const auto &entry : fs::recursive_directory_iterator(cht::hlp::getCheatsFolderPath())) {
        // Skip all entries that are not regular files
        if (!entry.is_regular_file()) continue;

        // Skip all files that don't have the .cht file extension
        if (entry.path().extension() != ".cht") continue;

        log::info(L"Loading cheat '{}'...", entry.path().filename().native());

        // Ask the injected libcheat to load the new cheat and return its load address
        auto cheatModule = libCheat.call<HMODULE>("loadCheat", hlp::StaticString<wchar_t, MAX_PATH>(entry.path().native()));
        if (!cheatModule.has_value()) {
            log::error(L"Failed to load cheat '{}'!", entry.path().native());
            continue;
        }

        void *address = reinterpret_cast<void*>(*cheatModule);
        log::info("Loaded cheat at 0x{}", address);

        // Ask cheat to attach stdout to the main console
        auto cheat = cht::hlp::InjectFile(entry.path(), hProcess, *cheatModule);
        if (cheat.call<void>("attachConsole", ::GetCurrentProcessId()) == Result::Err) {
            log::error("Cheat failed to attach to loader console!");
            continue;
        }

        // Call main function of cheat
        if (cheat.call<void>("main") == Result::Err) {
            log::error("Failed to initialize cheat!");
            continue;
        }

    }

    // Keep loader and its console open until main process exits
    ::WaitForSingleObject(hProcess, INFINITE);
}