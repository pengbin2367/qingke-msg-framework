//
// Created by pengbin on 2025/12/11.
//
#include "MessageBuilder.hpp"
#include "Parsers.hpp"
#include <algorithm>

MessageBuilder::MessageBuilder(SequenceManager* seqMgr) : seqManager(seqMgr) {}

void MessageBuilder::addQuery(uint16_t deviceId, uint16_t signalId, uint16_t dstModule) {
    pendingQueries[dstModule][{deviceId, signalId}] = true;
}

void MessageBuilder::addSet(uint16_t deviceId, uint16_t signalId, uint32_t value, uint16_t dstModule) {
    pendingSets[dstModule][deviceId][signalId] = value;
}

void MessageBuilder::addAlarm(uint16_t alarmId, uint16_t dstModule) {
    pendingAlarms[dstModule].push_back(alarmId);
}

void MessageBuilder::addPerf(uint16_t deviceId, uint16_t signalId, uint32_t period, uint16_t dstModule) {
    pendingPerfs[dstModule].push_back({deviceId, signalId, period});
}

std::vector<Message*> MessageBuilder::takeBuiltMessages() {
    std::vector<Message*> out;

    // For each destination module, build appropriate messages and set seq via SequenceManager
    for (auto &kv : pendingQueries) {
        uint16_t dst = kv.first;
        QueryMessage* qm = new QueryMessage();
        qm->header.dstModule = dst;
        qm->header.srcModule = 0;
        qm->header.msgType = MSG_QUERY;
        qm->header.seq = seqManager ? seqManager->nextSeq(dst) : 0;
        for (auto &e : kv.second) qm->inputs.push_back({e.first.first, e.first.second});
        out.push_back(qm);
    }

    for (auto &m : pendingSets) {
        uint16_t dst = m.first;
        for (auto &devEntry : m.second) {
            SetMessage* sm = new SetMessage();
            sm->header.dstModule = dst;
            sm->header.srcModule = 0;
            sm->header.msgType = MSG_SET;
            sm->header.seq = seqManager ? seqManager->nextSeq(dst) : 0;
            uint16_t dev = devEntry.first;
            for (auto &sigEntry : devEntry.second) {
                uint16_t sig = sigEntry.first;
                uint32_t val = sigEntry.second;
                SetInputItem it; it.deviceId = dev; it.signalId = sig; it.value = val;
                it.logInfo = sm->formatLog(dev, sig, val);
                sm->inputs.push_back(it);
            }
            out.push_back(sm);
        }
    }

    for (auto &m : pendingAlarms) {
        uint16_t dst = m.first;
        for (auto aid : m.second) {
            AlarmMessage* am = new AlarmMessage();
            am->header.dstModule = dst;
            am->header.srcModule = 0;
            am->header.msgType = MSG_ALARM;
            am->header.seq = seqManager ? seqManager->nextSeq(dst) : 0;
            out.push_back(am);
        }
    }

    for (auto &m : pendingPerfs) {
        uint16_t dst = m.first;
        for (auto &pi : m.second) {
            PerfMessage* pm = new PerfMessage();
            pm->header.dstModule = dst;
            pm->header.srcModule = 0;
            pm->header.msgType = MSG_PERF;
            pm->header.seq = seqManager ? seqManager->nextSeq(dst) : 0;
            pm->inputs.push_back(pi);
            out.push_back(pm);
        }
    }

    // clear pending
    pendingQueries.clear();
    pendingSets.clear();
    pendingAlarms.clear();
    pendingPerfs.clear();

    return out;
}
