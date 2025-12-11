//
// Created by pengbin on 2025/12/11.
//

#ifndef QINGKE_MSG_FRAMEWORK_MESSAGECACHE_H
#define QINGKE_MSG_FRAMEWORK_MESSAGECACHE_H

#pragma once
#include <map>
#include <vector>
#include <cstdint>
#include "Message.hpp"

class MessageCache {
public:
    struct RawMsg { std::vector<uint8_t> data; };

    // store raw bytes copy
    void store(uint16_t module, uint32_t seq, const uint8_t* data, size_t len);

    // fetch raw copy
    bool fetchRaw(uint16_t module, uint32_t seq, RawMsg& out) const;

    // store Message* and take ownership
    void storeMessage(uint16_t module, uint32_t seq, Message* msg);

    // fetch Message* (ownership transferred to caller; entry removed)
    Message* fetchMessage(uint16_t module, uint32_t seq);

    // erase entries
    void erase(uint16_t module, uint32_t seq);

    // clear all
    void clearAll();

private:
    std::map<uint16_t, std::map<uint32_t, RawMsg>> rawCache;
    std::map<uint16_t, std::map<uint32_t, Message*>> msgCache;
};


#endif //QINGKE_MSG_FRAMEWORK_MESSAGECACHE_H