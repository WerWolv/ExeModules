#pragma once

#include <cht/base.hpp>

#include <cht/helpers/utils.hpp>

#include <array>
#include <span>
#include <string>

namespace cht::api {

    struct IPCMessage {
        u32 magic;
        std::span<std::byte> inBuffer, outBuffer;
    };

    template<typename In, typename Out = void> requires (!std::same_as<Out, void>)
    std::pair<const In&, Out&> unmarshal(const IPCMessage *message) {
        return { *reinterpret_cast<const In*>(message->inBuffer.data()), *reinterpret_cast<Out*>(message->outBuffer.data()) };
    }

    template<typename In, typename Out = void> requires (std::same_as<Out, void>)
    const In& unmarshal(const IPCMessage *message) {
        return *reinterpret_cast<const In*>(message->inBuffer.data());
    }

}