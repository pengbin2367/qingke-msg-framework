//
// Created by pengbin on 2025/12/11.
//

#ifndef QINGKE_MSG_FRAMEWORK_PARSERS_HPP
#define QINGKE_MSG_FRAMEWORK_PARSERS_HPP

#pragma once
#include <vector>
#include <cstdint>
#include "Message.hpp"

// Parser interfaces; no ownership transfer of output vectors.
class IQueryParser {
public:
    virtual ~IQueryParser() = default;
    virtual bool parse(const uint8_t* payload, size_t payloadLen, std::vector<QueryOutputItem>& out) = 0;
};
class ISetParser {
public:
    virtual ~ISetParser() = default;
    virtual bool parse(const uint8_t* payload, size_t payloadLen, std::vector<SetOutputItem>& out) = 0;
};
class IAlarmParser {
public:
    virtual ~IAlarmParser() = default;
    virtual bool parse(const uint8_t* payload, size_t payloadLen, std::vector<AlarmOutputItem>& out) = 0;
};
class IPerfParser {
public:
    virtual ~IPerfParser() = default;
    virtual bool parse(const uint8_t* payload, size_t payloadLen, std::vector<PerfOutputItem>& out) = 0;
};

// Default parser getters (singletons)
IQueryParser* getDefaultQueryParser();
ISetParser*   getDefaultSetParser();
IAlarmParser* getDefaultAlarmParser();
IPerfParser*  getDefaultPerfParser();

#endif //QINGKE_MSG_FRAMEWORK_PARSERS_HPP