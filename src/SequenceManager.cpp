//
// Created by pengbin on 2025/12/11.
//
#include "SequenceManager.hpp"

void SequenceManager::registerModule(uint16_t moduleId, uint32_t start, uint32_t end) {
    ranges[moduleId] = {start, end, start};
}

uint32_t SequenceManager::nextSeq(uint16_t moduleId) {
    auto it = ranges.find(moduleId);
    if (it == ranges.end()) return 0;
    Range &r = it->second;
    uint32_t cur = r.current;
    if (r.current >= r.end) r.current = r.start;
    else r.current++;
    return cur;
}
