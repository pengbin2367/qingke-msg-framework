//
// Created by pengbin on 2025/12/11.
//
#include "ModuleAParser.hpp"
#include "Message.hpp"
#include <cstring>

Message* ModuleAParser::parse(const uint8_t* data, size_t len) {
    if(len < sizeof(MsgHeader)) return nullptr;

    MsgHeader hdr;
    memcpy(&hdr, data, sizeof(MsgHeader));

    Message* msg = nullptr;
    switch(hdr.type){
        case MsgType::QUERY: msg = new QueryMessage(); break;
        case MsgType::SET: msg = new SetMessage(); break;
        case MsgType::ALARM: msg = new AlarmMessage(); break;
        case MsgType::PERFORMANCE: msg = new PerformanceMessage(); break;
    }
    if(msg) msg->parse(data, len);
    return msg;
}

extern "C" MessageParser* CreateModuleAParser() {
    return new ModuleAParser();
}
