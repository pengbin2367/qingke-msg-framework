//
// Created by pengbin on 2025/12/11.
//
#include "MessageParserFactory.hpp"
#include "Parsers.hpp"
#include <cstring>
#include <map>

#include "MessageTypes.hpp"

static std::map<uint16_t, ModuleParserCreateFromRaw> g_moduleParsers;

void MessageParserFactory::registerModuleParser(uint16_t moduleId, ModuleParserCreateFromRaw creator) {
    g_moduleParsers[moduleId] = creator;
}

static Message* createByTypeAndParse(const uint8_t* data, size_t len) {
    if (len < 11) return nullptr;
    uint8_t t = data[4];
    Message* msg = nullptr;
    switch (t) {
    case MSG_QUERY: msg = new QueryMessage(); break;
    case MSG_SET:   msg = new SetMessage(); break;
    case MSG_ALARM: msg = new AlarmMessage(); break;
    case MSG_PERF:  msg = new PerfMessage(); break;
    default: return nullptr;
    }
    if (!msg->parse(data, len)) { delete msg; return nullptr; }
    return msg;
}

Message* MessageParserFactory::parseRaw(const uint8_t* data, size_t len) {
    if (len < 11) return nullptr;
    // read dstModule from header bytes 2..3
    uint16_t dst = (uint16_t(data[2])<<8) | uint16_t(data[3]);
    auto it = g_moduleParsers.find(dst);
    if (it != g_moduleParsers.end()) {
        return it->second(data, len);
    }
    return createByTypeAndParse(data, len);
}

Message* MessageParserFactory::parseCached(uint16_t moduleId, uint32_t seq, const MessageCache& cache) {
    MessageCache::RawMsg raw;
    if (!cache.fetchRaw(moduleId, seq, raw)) return nullptr;
    return parseRaw(raw.data.data(), raw.data.size());
}
