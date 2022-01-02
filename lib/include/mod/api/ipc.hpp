#pragma once

#include <mod/base.hpp>

#include <mod/helpers/utils.hpp>

#include <array>
#include <span>
#include <string>

namespace mod::api {

    struct IPCMessage {
        u32 magic;
        std::span<std::byte> inBuffer, outBuffer;
    };

    template<typename In, typename Out = void> requires (!std::same_as<Out, void>)
    [[nodiscard]] std::pair<const In&, Out&> unmarshal(const IPCMessage *message) {
        return { *reinterpret_cast<const In*>(message->inBuffer.data()), *reinterpret_cast<Out*>(message->outBuffer.data()) };
    }

    template<typename In, typename Out = void> requires (std::same_as<Out, void>)
    [[nodiscard]] const In& unmarshal(const IPCMessage *message) {
        return *reinterpret_cast<const In*>(message->inBuffer.data());
    }

}