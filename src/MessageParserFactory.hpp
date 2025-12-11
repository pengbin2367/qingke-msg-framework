//
// Created by pengbin on 2025/12/11.
//

#ifndef QINGKE_MSG_FRAMEWORK_MESSAGEPARSERFACTORY_H
#define QINGKE_MSG_FRAMEWORK_MESSAGEPARSERFACTORY_H

#pragma once
#include <map>
#include <cstdint>
#include "Message.hpp"
#include "MessageCache.hpp"

typedef Message* (*ModuleParserCreateFromRaw)(const uint8_t* data, size_t len);

// Module-specific parser registry and parseCached helper
class MessageParserFactory {
public:
    static void registerModuleParser(uint16_t moduleId, ModuleParserCreateFromRaw creator);
    static Message* parseRaw(const uint8_t* data, size_t len);
    static Message* parseCached(uint16_t moduleId, uint32_t seq, const MessageCache& cache);
};


#endif //QINGKE_MSG_FRAMEWORK_MESSAGEPARSERFACTORY_H