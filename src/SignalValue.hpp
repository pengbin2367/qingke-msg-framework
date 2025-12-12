//
// Created by pengbin on 12/13/25.
//

#ifndef QINGKE_MSG_FRAMEWORK_SIGNALVALUE_HPP
#define QINGKE_MSG_FRAMEWORK_SIGNALVALUE_HPP

#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

enum class SignalType : uint8_t { U8, U16, U32, F32, STRING };

class SignalValue {
public:
    SignalType type;
    union {
        uint8_t v_u8;
        uint16_t v_u16;
        uint32_t v_u32;
        float v_f32;
    };
    std::string v_str; // 仅用于 STRING 类型

    SignalValue() {}
    explicit SignalValue(uint8_t v) : type(SignalType::U8), v_u8(v) {}
    explicit SignalValue(uint16_t v) : type(SignalType::U16), v_u16(v) {}
    explicit SignalValue(uint32_t v) : type(SignalType::U32), v_u32(v) {}
    explicit SignalValue(float v) : type(SignalType::F32), v_f32(v) {}
    explicit SignalValue(const std::string& v) : type(SignalType::STRING), v_str(v) {}

    std::vector<uint8_t> encode() const;
    void decode(const uint8_t* data, size_t len);
};


#endif //QINGKE_MSG_FRAMEWORK_SIGNALVALUE_HPP