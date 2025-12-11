//
// Created by pengbin on 2025/12/11.
//

#ifndef QINGKE_MSG_FRAMEWORK_MESSAGEBUILDER_H
#define QINGKE_MSG_FRAMEWORK_MESSAGEBUILDER_H

#pragma once
#include <vector>
#include <map>
#include "Message.hpp"
#include "SequenceManager.hpp"

// MessageBuilder collects inputs and produces newly-allocated Message* objects.
// Ownership: caller receives vector<Message*> and must either delete or transfer ownership to MessageCache.
class MessageBuilder {
public:
    MessageBuilder(SequenceManager* seqMgr);

    void addQuery(uint16_t deviceId, uint16_t signalId, uint16_t dstModule);
    void addSet(uint16_t deviceId, uint16_t signalId, uint32_t value, uint16_t dstModule);
    void addAlarm(uint16_t alarmId, uint16_t dstModule);
    void addPerf(uint16_t deviceId, uint16_t signalId, uint32_t period, uint16_t dstModule);

    std::vector<Message*> takeBuiltMessages();

private:
    SequenceManager* seqManager;

    // per-dstModule collections
    std::map<uint16_t, std::map<std::pair<uint16_t,uint16_t>, bool>> pendingQueries;
    std::map<uint16_t, std::map<uint16_t, std::map<uint16_t, uint32_t>>> pendingSets; // dst -> dev -> sig -> val
    std::map<uint16_t, std::vector<uint16_t>> pendingAlarms; // dst -> list
    std::map<uint16_t, std::vector<PerfInputItem>> pendingPerfs; // dst -> list
};

#endif //QINGKE_MSG_FRAMEWORK_MESSAGEBUILDER_H