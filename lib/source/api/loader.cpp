#include <mod/base.hpp>

#include <mod/api/ipc.hpp>
#include <mod/helpers/path.hpp>

extern "C" [[maybe_unused]] mod::Result loadModule(mod::api::IPCMessage *message) {
    auto [in, out] = mod::api::unmarshal<wchar_t, HMODULE>(message);

    out = ::LoadLibraryW(std::wstring(&in).c_str());

    return mod::Result::Ok;
}