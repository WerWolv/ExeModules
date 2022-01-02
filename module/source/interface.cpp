#include <mod/api/ipc.hpp>
#include <mod/helpers/utils.hpp>
#include <mod/helpers/logger.hpp>

extern "C" [[maybe_unused]] mod::Result attachConsole(mod::api::IPCMessage *message) {
    auto loaderPid = mod::api::unmarshal<u32>(message);

    mod::hlp::redirectStdio(loaderPid);

    mod::log::info("Attached to console!");

    return mod::Result::Ok;
}
