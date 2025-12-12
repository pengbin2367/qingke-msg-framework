//
// Created by pengbin on 2025/12/11.
//
#include "MessageBuilder.hpp"

MessageBuilder::~MessageBuilder() {
    // 清理还没生成消息的 SignalValue
    for(auto& kv: setLatest) delete kv.second;

    for(auto m: builtMsgs) delete m;
}

void MessageBuilder::addQuery(uint16_t devId, uint16_t sigId) {
    auto key = std::make_pair(devId, sigId);
    if(!queryDedup[key]) queryDedup[key] = true;
}

void MessageBuilder::addSet(uint16_t devId, uint16_t sigId, SignalValue* val, const std::string& logInfo) {
    auto key = std::make_pair(devId, sigId);
    if(setLatest.count(key)) delete setLatest[key];
    setLatest[key] = val;
    setLogInfo[key] = logInfo;
}

void MessageBuilder::addAlarm() {
    builtMsgs.push_back(new AlarmMessage());
}

void MessageBuilder::addPerformance(uint16_t devId, uint16_t sigId, SignalValue* val, uint16_t period) {
    PerformanceMessage* msg = new PerformanceMessage();
    SignalItem* item = new SignalItem();
    item->devId = devId;
    item->sigId = sigId;
    item->value = val;
    msg->perfReqs.push_back(item);
    builtMsgs.push_back(msg);
}

std::vector<Message*> MessageBuilder::takeBuiltMessages() {
    // Query 消息
    if(!queryDedup.empty()){
        QueryMessage* q = new QueryMessage();
        for(auto& kv: queryDedup){
            SignalItem* item = new SignalItem();
            item->devId = kv.first.first;
            item->sigId = kv.first.second;
            q->requests.push_back(item);
        }
        builtMsgs.push_back(q);
        queryDedup.clear();
    }

    // Set 消息
    if(!setLatest.empty()){
        SetMessage* s = new SetMessage();
        for(auto& kv: setLatest){
            SignalItem* item = new SignalItem();
            item->devId = kv.first.first;
            item->sigId = kv.first.second;
            item->value = kv.second;
            item->logInfo = setLogInfo[kv.first];
            s->sets.push_back(item);
        }
        builtMsgs.push_back(s);
        setLatest.clear();
        setLogInfo.clear();
    }

    auto result = builtMsgs;
    builtMsgs.clear();
    return result;
}
