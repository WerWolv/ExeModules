#include <cht/base.hpp>

#include <cht/api/ipc.hpp>
#include <cht/helpers/path.hpp>

extern "C" [[maybe_unused]] cht::Result loadCheat(cht::api::IPCMessage *message) {
    auto [in, out] = cht::api::unmarshal<wchar_t, HMODULE>(message);

    std::wstring path(&in);
    out = ::LoadLibraryW(path.c_str());

    return cht::Result::Ok;
}