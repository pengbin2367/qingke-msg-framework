//
// Created by pengbin on 12/13/25.
//
#include "SignalValue.hpp"

std::vector<uint8_t> SignalValue::encode() const {
    std::vector<uint8_t> buf;
    switch(type){
        case SignalType::U8:
            buf.push_back(v_u8);
            break;
        case SignalType::U16:
            buf.resize(2);
            std::memcpy(buf.data(), &v_u16, 2);
            break;
        case SignalType::U32:
            buf.resize(4);
            std::memcpy(buf.data(), &v_u32, 4);
            break;
        case SignalType::F32:
            buf.resize(4);
            std::memcpy(buf.data(), &v_f32, 4);
            break;
        case SignalType::STRING:
            buf.push_back(static_cast<uint8_t>(v_str.size()));
            buf.insert(buf.end(), v_str.begin(), v_str.end());
            break;
    }
    return buf;
}

void SignalValue::decode(const uint8_t* data, size_t len){
    switch(type){
        case SignalType::U8:
            if(len>=1) v_u8 = data[0];
            break;
        case SignalType::U16:
            if(len>=2) std::memcpy(&v_u16, data, 2);
            break;
        case SignalType::U32:
            if(len>=4) std::memcpy(&v_u32, data, 4);
            break;
        case SignalType::F32:
            if(len>=4) std::memcpy(&v_f32, data, 4);
            break;
        case SignalType::STRING:
            if(len>0){
                uint8_t sl = data[0];
                if(len >= 1 + sl) v_str.assign((const char*)(data+1), sl);
            }
            break;
    }
}
