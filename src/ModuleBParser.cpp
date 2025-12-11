//
// Created by pengbin on 2025/12/11.
//
#include "ModuleBParser.hpp"
#include <cstring>

// ModuleB uses slightly different SET log format: logs are encoded as uppercase hex string
// We'll parse normally then convert the logInfo to hex representation
static std::string bytesToHex(const std::vector<uint8_t>& v) {
    static const char *hex = "0123456789ABCDEF";
    std::string out; out.reserve(v.size()*2);
    for (auto b : v) { out.push_back(hex[(b>>4)&0xF]); out.push_back(hex[b&0xF]); }
    return out;
}

Message* ModuleB_createFromRaw(const uint8_t* data, size_t len) {
    if (len < 11) return nullptr;
    uint8_t t = data[4];
    if (t == MSG_SET) {
        SetMessage* s = new SetMessage();
        if (!s->parse(data, len)) { delete s; return nullptr; }
        // convert each log string to uppercase-hex representation (simulate module B convention)
        for (auto &it : s->inputs) {
            // take original string bytes and convert to hex string for demo
            std::vector<uint8_t> bytes(it.logInfo.begin(), it.logInfo.end());
            it.logInfo = "[ModuleB]LOG=" + bytesToHex(bytes);
        }
        return s;
    }
    Message* msg = nullptr;
    switch (t) {
    case MSG_QUERY: msg = new QueryMessage(); break;
    case MSG_ALARM: msg = new AlarmMessage(); break;
    case MSG_PERF:  msg = new PerfMessage(); break;
    default: return nullptr;
    }
    if (!msg->parse(data, len)) { delete msg; return nullptr; }
    return msg;
}
