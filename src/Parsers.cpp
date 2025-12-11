//
// Created by pengbin on 2025/12/11.
//
#include "Parsers.hpp"
#include <vector>
#include <cstring>

// Default Query parser: [dev(2) sig(2) vlen(2) value...]* repeated
class DefaultQueryParser : public IQueryParser {
public:
    bool parse(const uint8_t* payload, size_t payloadLen, std::vector<QueryOutputItem>& out) override {
        out.clear();
        size_t pos = 0;
        while (pos + 6 <= payloadLen) {
            uint16_t dev = (uint16_t(payload[pos])<<8) | uint16_t(payload[pos+1]); pos += 2;
            uint16_t sig = (uint16_t(payload[pos])<<8) | uint16_t(payload[pos+1]); pos += 2;
            uint16_t vlen = (uint16_t(payload[pos])<<8) | uint16_t(payload[pos+1]); pos += 2;
            if (pos + vlen > payloadLen) return false;
            std::vector<uint8_t> val(payload + pos, payload + pos + vlen);
            pos += vlen;
            out.push_back({dev, sig, vlen, val});
        }
        return true;
    }
};

// Default Set parser: [sig(2) status(1)]*
class DefaultSetParser : public ISetParser {
public:
    bool parse(const uint8_t* payload, size_t payloadLen, std::vector<SetOutputItem>& out) override {
        out.clear();
        size_t pos = 0;
        while (pos + 3 <= payloadLen) {
            uint16_t sig = (uint16_t(payload[pos])<<8) | uint16_t(payload[pos+1]); pos += 2;
            uint8_t st = payload[pos]; pos += 1;
            out.push_back({sig, st});
        }
        return true;
    }
};

// Default Alarm parser: [dev(2) sig(2)]*
class DefaultAlarmParser : public IAlarmParser {
public:
    bool parse(const uint8_t* payload, size_t payloadLen, std::vector<AlarmOutputItem>& out) override {
        out.clear();
        size_t pos = 0;
        while (pos + 4 <= payloadLen) {
            uint16_t dev = (uint16_t(payload[pos])<<8) | uint16_t(payload[pos+1]); pos += 2;
            uint16_t sig = (uint16_t(payload[pos])<<8) | uint16_t(payload[pos+1]); pos += 2;
            out.push_back({dev, sig});
        }
        return true;
    }
};

// Default Perf parser: [dev(2) sig(2) period(4) vlen(2) value... ]*
class DefaultPerfParser : public IPerfParser {
public:
    bool parse(const uint8_t* payload, size_t payloadLen, std::vector<PerfOutputItem>& out) override {
        out.clear();
        size_t pos = 0;
        while (pos + 12 <= payloadLen) {
            uint16_t dev = (uint16_t(payload[pos])<<8) | uint16_t(payload[pos+1]); pos += 2;
            uint16_t sig = (uint16_t(payload[pos])<<8) | uint16_t(payload[pos+1]); pos += 2;
            uint32_t per = (uint32_t(payload[pos])<<24) | (uint32_t(payload[pos+1])<<16) | (uint32_t(payload[pos+2])<<8) | uint32_t(payload[pos+3]); pos += 4;
            uint16_t vlen = (uint16_t(payload[pos])<<8) | uint16_t(payload[pos+1]); pos += 2;
            if (pos + vlen > payloadLen) return false;
            std::vector<uint8_t> val(payload + pos, payload + pos + vlen);
            pos += vlen;
            out.push_back({dev, sig, per, vlen, val});
        }
        return true;
    }
};

// Singletons
IQueryParser* getDefaultQueryParser()    { static DefaultQueryParser inst; return &inst; }
ISetParser*   getDefaultSetParser()      { static DefaultSetParser inst; return &inst; }
IAlarmParser* getDefaultAlarmParser()    { static DefaultAlarmParser inst; return &inst; }
IPerfParser*  getDefaultPerfParser()     { static DefaultPerfParser inst; return &inst; }
