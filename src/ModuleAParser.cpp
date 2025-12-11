//
// Created by pengbin on 2025/12/11.
//
#include "ModuleAParser.hpp"
#include <cstring>

// ModuleA uses default formats but adds richer logInfo extraction for Set messages
Message* ModuleA_createFromRaw(const uint8_t* data, size_t len) {
    if (len < 11) return nullptr;
    uint8_t t = data[4];
    if (t == MSG_SET) {
        SetMessage* s = new SetMessage();
        if (!s->parse(data, len)) { delete s; return nullptr; }
        // module-specific enhancement: prepend module tag to each logInfo
        for (auto &it : s->inputs) {
            it.logInfo = "[ModuleA]" + it.logInfo;
        }
        return s;
    }
    // fallback to default parse-by-type
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
