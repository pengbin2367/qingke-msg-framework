//
// Created by pengbin on 2025/12/11.
//
#include "Message.hpp"
#include "Parsers.hpp"
#include <vector>
#include <cstring>
#include <iostream>

// helpers for BE encoding/decoding
static void put_u8(std::vector<uint8_t>& b, uint8_t v){ b.push_back(v); }
static void put_u16(std::vector<uint8_t>& b, uint16_t v){ b.push_back((v>>8)&0xFF); b.push_back(v&0xFF); }
static void put_u32(std::vector<uint8_t>& b, uint32_t v){ b.push_back((v>>24)&0xFF); b.push_back((v>>16)&0xFF); b.push_back((v>>8)&0xFF); b.push_back(v&0xFF); }
static uint16_t get_u16(const uint8_t* p){ return (uint16_t(p[0])<<8) | uint16_t(p[1]); }
static uint32_t get_u32(const uint8_t* p){ return (uint32_t(p[0])<<24) | (uint32_t(p[1])<<16) | (uint32_t(p[2])<<8) | uint32_t(p[3]); }

static const size_t HEADER_SIZE = 11; // 2+2+1+2+4

// ---------------- Query ----------------
std::vector<uint8_t> QueryMessage::build() const {
    std::vector<uint8_t> payload;
    put_u16(payload, (uint16_t)inputs.size());
    for (auto &it : inputs) { put_u16(payload, it.deviceId); put_u16(payload, it.signalId); }

    std::vector<uint8_t> out;
    put_u16(out, header.srcModule);
    put_u16(out, header.dstModule);
    put_u8(out, header.msgType);
    put_u16(out, (uint16_t)(HEADER_SIZE + payload.size()));
    put_u32(out, header.seq);
    out.insert(out.end(), payload.begin(), payload.end());
    return out;
}

bool QueryMessage::parse(const uint8_t* data, size_t len) {
    inputs.clear();
    if (len < HEADER_SIZE) return false;
    size_t pos = HEADER_SIZE;
    if (pos + 2 > len) return true;
    uint16_t cnt = get_u16(data + pos); pos += 2;
    for (uint16_t i=0;i<cnt;i++){
        if (pos + 4 > len) return false;
        uint16_t d = get_u16(data + pos); pos += 2;
        uint16_t s = get_u16(data + pos); pos += 2;
        inputs.push_back({d,s});
    }
    return true;
}

bool QueryMessage::parseResponse(const uint8_t* payload, size_t payloadLen) {
    outputs.clear();
    IQueryParser* p = getDefaultQueryParser();
    return p->parse(payload, payloadLen, outputs);
}

void QueryMessage::print() const {
    std::cout << "[QueryMessage] outputs=" << outputs.size() << "\n";
    for (auto &o : outputs) {
        std::cout << "  dev="<<o.deviceId<<" sig="<<o.signalId<<" len="<<o.valueLen<<" val:";
        for (auto b : o.value) printf(" %02X", b);
        std::cout << "\n";
    }
}

// ---------------- Set ----------------
// default formatLog
std::string SetMessage::formatLog(uint16_t dev, uint16_t sig, uint32_t val) const {
    char buf[128];
    int n = snprintf(buf, sizeof(buf), "dev=%u sig=%u val=%u", unsigned(dev), unsigned(sig), unsigned(val));
    return std::string(buf, (n>0)?n:0);
}

// build: repeated [dev(2) sig(2) val(4) logLen(2) logBytes]
std::vector<uint8_t> SetMessage::build() const {
    std::vector<uint8_t> payload;
    for (auto &it : inputs) {
        put_u16(payload, it.deviceId);
        put_u16(payload, it.signalId);
        put_u32(payload, it.value);
        uint16_t ln = (uint16_t)it.logInfo.size();
        put_u16(payload, ln);
        payload.insert(payload.end(), it.logInfo.begin(), it.logInfo.end());
    }
    std::vector<uint8_t> out;
    put_u16(out, header.srcModule);
    put_u16(out, header.dstModule);
    put_u8(out, header.msgType);
    put_u16(out, (uint16_t)(HEADER_SIZE + payload.size()));
    put_u32(out, header.seq);
    out.insert(out.end(), payload.begin(), payload.end());
    return out;
}

bool SetMessage::parse(const uint8_t* data, size_t len) {
    inputs.clear();
    if (len < HEADER_SIZE) return false;
    size_t pos = HEADER_SIZE;
    while (pos + 2+2+4+2 <= len) {
        uint16_t dev = get_u16(data + pos); pos += 2;
        uint16_t sig = get_u16(data + pos); pos += 2;
        uint32_t val = get_u32(data + pos); pos += 4;
        uint16_t ln = get_u16(data + pos); pos += 2;
        if (pos + ln > len) return false;
        std::string log((const char*)(data + pos), ln);
        pos += ln;
        inputs.push_back({dev, sig, val, log});
    }
    return true;
}

bool SetMessage::parseResponse(const uint8_t* payload, size_t payloadLen) {
    outputs.clear();
    ISetParser* p = getDefaultSetParser();
    return p->parse(payload, payloadLen, outputs);
}

void SetMessage::print() const {
    std::cout << "[SetMessage] inputs=" << inputs.size() << " outputs=" << outputs.size() << "\n";
    for (auto &it : inputs) {
        std::cout << "  dev="<<it.deviceId<<" sig="<<it.signalId<<" val="<<it.value<<" log=\"" << it.logInfo << "\"\n";
    }
    for (auto &o : outputs) {
        std::cout << "  out sig="<<o.signalId<<" status="<<(int)o.status<<"\n";
    }
}

// ---------------- Alarm ----------------
std::vector<uint8_t> AlarmMessage::build() const {
    std::vector<uint8_t> payload;
    // simple payload: dev(2) sig(2)
    put_u16(payload, 0); // placeholder
    std::vector<uint8_t> out;
    put_u16(out, header.srcModule);
    put_u16(out, header.dstModule);
    put_u8(out, header.msgType);
    put_u16(out, (uint16_t)(HEADER_SIZE + payload.size()));
    put_u32(out, header.seq);
    out.insert(out.end(), payload.begin(), payload.end());
    return out;
}

bool AlarmMessage::parse(const uint8_t* data, size_t len) {
    // not used for request here
    (void)data; (void)len;
    return true;
}

bool AlarmMessage::parseResponse(const uint8_t* payload, size_t payloadLen) {
    IAlarmParser* p = getDefaultAlarmParser();
    return p->parse(payload, payloadLen, outputs);
}

void AlarmMessage::print() const {
    std::cout << "[AlarmMessage] outputs="<<outputs.size()<<"\n";
    for (auto &o : outputs) {
        std::cout << "  dev="<<o.deviceId<<" sig="<<o.signalId<<"\n";
    }
}

// ---------------- Perf ----------------
std::vector<uint8_t> PerfMessage::build() const {
    std::vector<uint8_t> payload;
    put_u16(payload, (uint16_t)inputs.size());
    for (auto &it : inputs) { put_u16(payload, it.deviceId); put_u16(payload, it.signalId); put_u32(payload, it.period); }
    std::vector<uint8_t> out;
    put_u16(out, header.srcModule);
    put_u16(out, header.dstModule);
    put_u8(out, header.msgType);
    put_u16(out, (uint16_t)(HEADER_SIZE + payload.size()));
    put_u32(out, header.seq);
    out.insert(out.end(), payload.begin(), payload.end());
    return out;
}

bool PerfMessage::parse(const uint8_t* data, size_t len) {
    inputs.clear();
    if (len < HEADER_SIZE) return false;
    size_t pos = HEADER_SIZE;
    if (pos + 2 > len) return true;
    uint16_t cnt = get_u16(data + pos); pos += 2;
    for (uint16_t i=0;i<cnt;i++) {
        if (pos + 8 > len) return false;
        uint16_t d = get_u16(data + pos); pos += 2;
        uint16_t s = get_u16(data + pos); pos += 2;
        uint32_t p = get_u32(data + pos); pos += 4;
        inputs.push_back({d,s,p});
    }
    return true;
}

bool PerfMessage::parseResponse(const uint8_t* payload, size_t payloadLen) {
    IPerfParser* p = getDefaultPerfParser();
    return p->parse(payload, payloadLen, outputs);
}

void PerfMessage::print() const {
    std::cout << "[PerfMessage] outputs="<<outputs.size()<<"\n";
    for (auto &o : outputs) {
        std::cout << "  dev="<<o.deviceId<<" sig="<<o.signalId<<" period="<<o.period<<" len="<<o.valueLen<<" val:";
        for (auto b : o.value) printf(" %02X", b);
        std::cout << "\n";
    }
}
