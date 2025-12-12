//
// Created by pengbin on 2025/12/11.
//
#include "MessageParserFactory.hpp"
#include "Message.hpp"
#include <cstring>

// registry 静态局部变量，保证唯一性
std::map<MsgType, ParserCreator>& MessageParserFactory::registry() {
    static std::map<MsgType, ParserCreator> inst;
    return inst;
}

void MessageParserFactory::registerParser(MsgType t, ParserCreator c) {
    registry()[t] = c;
}

MessageParser* MessageParserFactory::create(MsgType t) {
    if(registry().count(t)) return registry()[t]();
    return nullptr;
}

Message* MessageParserFactory::parseMessage(const uint8_t* data, size_t len) {
    if(len < sizeof(MsgHeader)) return nullptr;

    MsgHeader hdr;
    memcpy(&hdr, data, sizeof(MsgHeader));

    MessageParser* parser = create(hdr.type);
    if(!parser) return nullptr;

    Message* msg = parser->parse(data, len);
    delete parser; // Parser 工厂返回的对象临时使用完即删
    return msg;
}
