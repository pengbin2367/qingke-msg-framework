//
// Created by pengbin on 2025/12/11.
//

#ifndef QINGKE_MSG_FRAMEWORK_MESSAGEPARSERFACTORY_H
#define QINGKE_MSG_FRAMEWORK_MESSAGEPARSERFACTORY_H

#pragma once
#include "MessageParser.hpp"
#include <map>

typedef MessageParser* (*ParserCreator)();

class MessageParserFactory {
private:
    static std::map<MsgType, ParserCreator>& registry() {
        static std::map<MsgType, ParserCreator> inst;
        return inst;
    }

public:
    static void registerParser(MsgType type, ParserCreator creator) {
        registry()[type] = creator;
    }

    static MessageParser* create(MsgType type) {
        if(registry().count(type)) return registry()[type]();
        return nullptr;
    }

    static Message* parseMessage(const uint8_t* data, size_t len) {
        if(len < sizeof(MsgHeader)) return nullptr;

        MsgHeader hdr;
        memcpy(&hdr, data, sizeof(MsgHeader));

        MessageParser* parser = create(hdr.type);
        if(!parser) return nullptr;

        Message* msg = parser->parse(data, len);
        delete parser;
        return msg;
    }
};

#endif //QINGKE_MSG_FRAMEWORK_MESSAGEPARSERFACTORY_H