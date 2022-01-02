#pragma once

#include <cht/base.hpp>
#include <cht/helpers/path.hpp>
#include <cht/helpers/utils.hpp>

#include <optional>
#include <span>
#include <concepts>

namespace cht::hlp {

    template<typename T>
    concept pod_type = std::is_standard_layout_v<T> && std::is_trivial_v<T>;

    template<typename T, size_t Size>
    class StaticString {
    public:
        constexpr StaticString() = default;
        constexpr explicit StaticString(const std::basic_string<T> &data) {
            std::copy(data.begin(), data.end(), this->m_data.begin());
        }
    private:
        std::array<T, Size> m_data = { 0 };
    };

    class InjectFile {
    public:
        explicit InjectFile(fs::path path);
        InjectFile(fs::path path, HANDLE process, HMODULE module);

        Result injectInto(HANDLE process);

        template<typename Ret = void> requires (!std::same_as<Ret, void>)
        std::optional<Ret> call(const std::string &function, auto ... params) {
            std::tuple inBuffer = { params... };
            Ret outBuffer;

            auto result = this->callImpl(function,
                                         std::span{ reinterpret_cast<const std::byte*>(&inBuffer), sizeof(inBuffer) },
                                         std::span{ reinterpret_cast<std::byte*>(&outBuffer), sizeof(outBuffer) }
            );

            if (result == Result::Err)
                return std::nullopt;
            else
                return outBuffer;
        }

        template<typename Ret = void> requires (std::same_as<Ret, void>)
        Result call(const std::string &function, auto ... params) {
            std::tuple inBuffer = { params... };

            return this->callImpl(function,
                                         std::span{ reinterpret_cast<const std::byte*>(&inBuffer), sizeof(inBuffer) },
                                         {  }
            );
        }
    private:
        fs::path m_path;

        HANDLE m_process = INVALID_HANDLE_VALUE;
        HMODULE m_module = nullptr;

        Result callImpl(const std::string &function, std::span<const std::byte> inBuffer, std::span<std::byte> outBuffer);
    };

}