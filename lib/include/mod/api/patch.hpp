#pragma once

#include <mod/base.hpp>

#include <optional>

namespace mod::api {

    template<typename T>
    [[nodiscard]] std::optional<T> readMemory(u64 address) {
        T result;

        SIZE_T bytesRead = 0;
        if (!::ReadProcessMemory(::GetCurrentProcess(), reinterpret_cast<void*>(address), &result, sizeof(T), &bytesRead) || bytesRead != sizeof(T)) {
            log::error("Failed to read remote memory at 0x{:08X}", address);
            return std::nullopt;
        }

        return result;
    }

    template<typename T>
    void writeMemory(u64 address, const T &value) {
        SIZE_T bytesWritten = 0;
        if (!::WriteProcessMemory(::GetCurrentProcess(), reinterpret_cast<void*>(address), &value, sizeof(T), &bytesWritten) || bytesWritten != sizeof(T)) {
            log::error("Failed to write remote memory at 0x{:08X}", address);
        }
    }

    template<typename T>
    struct PointerChain {
        PointerChain(u64 base) : m_address(base) { }

        [[nodiscard]]
        PointerChain& add(u64 offset) {
            auto result = readMemory<u64>(this->m_address);
            if (!result) return nullptr;

            this->m_address = *result;
            this->m_address += offset;

            return *this;
        }

        [[nodiscard]]
        PointerChain& sub(u64 offset) {
            auto result = readMemory<u64>(this->m_address);
            if (!result) return nullptr;

            this->m_address = *result;
            this->m_address -= offset;

            return *this;
        }

        T& operator*() {
            return *reinterpret_cast<T*>(this->m_address);
        }

        T& operator->() {
            return *reinterpret_cast<T*>(this->m_address);
        }

    private:
        HANDLE m_process;
        u64 m_address;
    };


    void nopInstruction(u64 address, size_t instructionSize) {
        constexpr u8 NOP = 0x90;
        for (u32 i = 0; i < instructionSize; i++) {
            writeMemory<u8>(address, NOP);
        }
    }

}