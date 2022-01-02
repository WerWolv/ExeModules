#include <cht/api/ipc.hpp>
#include <cht/helpers/utils.hpp>
#include <cht/helpers/logger.hpp>

extern "C" [[maybe_unused]] cht::Result attachConsole(cht::api::IPCMessage *message) {
    auto loaderPid = cht::api::unmarshal<u32>(message);

    cht::hlp::redirectStdio(loaderPid);

    cht::log::info("Attached to console!");

    return cht::Result::Ok;
}
