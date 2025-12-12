//
// Created by pengbin on 2025/12/11.
//

#ifndef QINGKE_MSG_FRAMEWORK_MESSAGE_H
#define QINGKE_MSG_FRAMEWORK_MESSAGE_H
#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include "SignalValue.hpp"

enum class MsgType : uint16_t { QUERY = 1, SET = 2, ALARM = 3, PERFORMANCE = 4 };

struct MsgHeader {
    uint16_t srcModule;
    uint16_t dstModule;
    MsgType type;
    uint16_t length;
    uint32_t seq;
};

class Message {
public:
    MsgHeader header;
    virtual ~Message() {}
    virtual std::vector<uint8_t> build() const = 0;
    virtual void parse(const uint8_t* data, size_t len) = 0;
};

// 用于存储单个信号信息
struct SignalItem {
    uint16_t devId;
    uint16_t sigId;
    SignalValue* value; // 指针方便支持多类型，析构时释放
    SignalItem() : value(nullptr) {}
    ~SignalItem() { delete value; }
};

// ------------------- 消息类型 -------------------
class QueryMessage : public Message {
public:
    std::vector<SignalItem*> requests;
    ~QueryMessage() { for(auto r: requests) delete r; }
    std::vector<uint8_t> build() const override;
    void parse(const uint8_t* data, size_t len) override;
};

class SetMessage : public Message {
public:
    std::vector<SignalItem*> sets;
    std::string logInfo; // 可选日志信息
    ~SetMessage() { for(auto s: sets) delete s; }
    std::vector<uint8_t> build() const override;
    void parse(const uint8_t* data, size_t len) override;
};

class AlarmMessage : public Message {
public:
    std::vector<uint8_t> build() const override;
    void parse(const uint8_t* data, size_t len) override;
};

class PerformanceMessage : public Message {
public:
    std::vector<SignalItem*> perfReqs;
    ~PerformanceMessage() { for(auto p: perfReqs) delete p; }
    std::vector<uint8_t> build() const override;
    void parse(const uint8_t* data, size_t len) override;
};


#endif //QINGKE_MSG_FRAMEWORK_MESSAGE_H