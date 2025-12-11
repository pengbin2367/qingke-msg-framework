//
// Created by pengbin on 2025/12/11.
//

#ifndef QINGKE_MSG_FRAMEWORK_SEQUENCEMANAGER_HPP
#define QINGKE_MSG_FRAMEWORK_SEQUENCEMANAGER_HPP

#pragma once
#include <map>
#include <cstdint>

class SequenceManager {
public:
    struct Range { uint32_t start; uint32_t end; uint32_t current; };

    void registerModule(uint16_t moduleId, uint32_t start, uint32_t end);
    uint32_t nextSeq(uint16_t moduleId);

private:
    std::map<uint16_t, Range> ranges;
};

#endif //QINGKE_MSG_FRAMEWORK_SEQUENCEMANAGER_HPP