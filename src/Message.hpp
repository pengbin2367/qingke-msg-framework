//
// Created by pengbin on 2025/12/11.
//

#ifndef QINGKE_MSG_FRAMEWORK_MESSAGE_H
#define QINGKE_MSG_FRAMEWORK_MESSAGE_H

#pragma once
#include <vector>
#include <cstdint>
#include <string>

// Common header layout (big-endian when serialized):
// srcModule(2) dstModule(2) msgType(1) msgLength(2) seq(4)  ==> 11 bytes
struct MsgHeader {
    uint16_t srcModule = 0;
    uint16_t dstModule = 0;
    uint8_t  msgType   = 0;
    uint16_t msgLength = 0;
    uint32_t seq       = 0;
};

// Base message
class Message {
public:
    MsgHeader header;
    virtual ~Message() = default;

    // build full packet (header + payload)
    virtual std::vector<uint8_t> build() const = 0;

    // parse full packet (including header)
    virtual bool parse(const uint8_t* data, size_t len) = 0;

    // parse response payload (payload only). Called after extracting payload from cached raw bytes.
    virtual bool parseResponse(const uint8_t* payload, size_t payloadLen) = 0;

    virtual void print() const = 0;
};

// Query
struct QueryInputItem { uint16_t deviceId; uint16_t signalId; };
struct QueryOutputItem { uint16_t deviceId; uint16_t signalId; uint16_t valueLen; std::vector<uint8_t> value; };
class QueryMessage : public Message {
public:
    std::vector<QueryInputItem> inputs;
    std::vector<QueryOutputItem> outputs;

    QueryMessage(): outputs(), inputs() {}

    std::vector<uint8_t> build() const override;
    bool parse(const uint8_t* data, size_t len) override;
    bool parseResponse(const uint8_t* payload, size_t payloadLen) override;
    void print() const override;
};

// Set (with per-item logInfo)
struct SetInputItem { uint16_t deviceId; uint16_t signalId; uint32_t value; std::string logInfo; };
struct SetOutputItem { uint16_t signalId; uint8_t status; };
class SetMessage : public Message {
public:
    std::vector<SetInputItem> inputs;
    std::vector<SetOutputItem> outputs;

    // default log formatter; modules can override via their parser
    virtual std::string formatLog(uint16_t dev, uint16_t sig, uint32_t val) const;

    std::vector<uint8_t> build() const override;
    bool parse(const uint8_t* data, size_t len) override;
    bool parseResponse(const uint8_t* payload, size_t payloadLen) override;
    void print() const override;
};

// Alarm
struct AlarmOutputItem { uint16_t deviceId; uint16_t signalId; };
class AlarmMessage : public Message {
public:
    std::vector<AlarmOutputItem> outputs;

    std::vector<uint8_t> build() const override;
    bool parse(const uint8_t* data, size_t len) override;
    bool parseResponse(const uint8_t* payload, size_t payloadLen) override;
    void print() const override;
};

// Perf
struct PerfInputItem { uint16_t deviceId; uint16_t signalId; uint32_t period; };
struct PerfOutputItem { uint16_t deviceId; uint16_t signalId; uint32_t period; uint16_t valueLen; std::vector<uint8_t> value; };
class PerfMessage : public Message {
public:
    std::vector<PerfInputItem> inputs;
    std::vector<PerfOutputItem> outputs;

    std::vector<uint8_t> build() const override;
    bool parse(const uint8_t* data, size_t len) override;
    bool parseResponse(const uint8_t* payload, size_t payloadLen) override;
    void print() const override;
};


#endif //QINGKE_MSG_FRAMEWORK_MESSAGE_H