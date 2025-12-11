//
// Created by pengbin on 2025/12/11.
//
#include "MessageCache.hpp"

void MessageCache::store(uint16_t module, uint32_t seq, const uint8_t* data, size_t len) {
    RawMsg r; r.data.assign(data, data + len);
    rawCache[module][seq] = std::move(r);
}

bool MessageCache::fetchRaw(uint16_t module, uint32_t seq, RawMsg& out) const {
    auto mit = rawCache.find(module);
    if (mit == rawCache.end()) return false;
    auto sit = mit->second.find(seq);
    if (sit == mit->second.end()) return false;
    out = sit->second;
    return true;
}

void MessageCache::storeMessage(uint16_t module, uint32_t seq, Message* msg) {
    // take ownership
    msgCache[module][seq] = msg;
}

Message* MessageCache::fetchMessage(uint16_t module, uint32_t seq) {
    auto mit = msgCache.find(module);
    if (mit == msgCache.end()) return nullptr;
    auto sit = mit->second.find(seq);
    if (sit == mit->second.end()) return nullptr;
    Message* m = sit->second;
    mit->second.erase(sit);
    return m;
}

void MessageCache::erase(uint16_t module, uint32_t seq) {
    auto mit = rawCache.find(module);
    if (mit != rawCache.end()) mit->second.erase(seq);
    auto mit2 = msgCache.find(module);
    if (mit2 != msgCache.end()) {
        auto sit = mit2->second.find(seq);
        if (sit != mit2->second.end()) {
            delete sit->second;
            mit2->second.erase(sit);
        }
    }
}

void MessageCache::clearAll() {
    for (auto &m : msgCache) {
        for (auto &kv : m.second) delete kv.second;
    }
    msgCache.clear();
    rawCache.clear();
}
