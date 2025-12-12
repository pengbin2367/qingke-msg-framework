//
// Created by pengbin on 2025/12/11.
//

#ifndef QINGKE_MSG_FRAMEWORK_MESSAGEBUILDER_H
#define QINGKE_MSG_FRAMEWORK_MESSAGEBUILDER_H

#pragma once
#include "Message.hpp"
#include <vector>
#include <map>
#include <utility>
#include <string>

class MessageBuilder {
private:
    std::vector<Message*> builtMsgs;

    // 去重 Query： key = {devId,sigId}
    std::map<std::pair<uint16_t,uint16_t>, bool> queryDedup;

    // 只保留 Set 最新值： key = {devId,sigId} -> SignalValue*
    std::map<std::pair<uint16_t,uint16_t>, SignalValue*> setLatest;

    std::map<std::pair<uint16_t,uint16_t>, std::string> setLogInfo; // 可选日志信息

public:
    ~MessageBuilder();

    void addQuery(uint16_t devId, uint16_t sigId);
    void addSet(uint16_t devId, uint16_t sigId, SignalValue* val, const std::string& logInfo="");
    void addAlarm();
    void addPerformance(uint16_t devId, uint16_t sigId, SignalValue* val, uint16_t period);

    // 返回构建好的消息，Builder 不再持有它们
    std::vector<Message*> takeBuiltMessages();
};


#endif //QINGKE_MSG_FRAMEWORK_MESSAGEBUILDER_H