//
// Created by pengbin on 2025/12/11.
//
#include "Message.hpp"
#include <cstring>

// ---------------- QueryMessage ----------------
std::vector<uint8_t> QueryMessage::build() const {
    std::vector<uint8_t> buf(sizeof(MsgHeader) + requests.size()*4);
    MsgHeader* hdr = reinterpret_cast<MsgHeader*>(buf.data());
    *hdr = header;

    uint8_t* ptr = buf.data() + sizeof(MsgHeader);
    for(auto& r: requests){
        memcpy(ptr, &r->devId, 2); ptr += 2;
        memcpy(ptr, &r->sigId, 2); ptr += 2;
    }
    return buf;
}

void QueryMessage::parse(const uint8_t* data, size_t len){
    if(len < sizeof(MsgHeader)) return;
    size_t offset = sizeof(MsgHeader);
    while(offset + 4 <= len){
        SignalItem* item = new SignalItem();
        memcpy(&item->devId, data+offset, 2);
        memcpy(&item->sigId, data+offset+2, 2);
        requests.push_back(item);
        offset += 4;
    }
}

// ---------------- SetMessage ----------------
std::vector<uint8_t> SetMessage::build() const {
    std::vector<uint8_t> buf(sizeof(MsgHeader));
    MsgHeader* hdr = reinterpret_cast<MsgHeader*>(buf.data());
    *hdr = header;

    for(auto& s: sets){
        std::vector<uint8_t> vbuf = s->value->encode();
        buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&s->devId), reinterpret_cast<uint8_t*>(&s->devId)+2);
        buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&s->sigId), reinterpret_cast<uint8_t*>(&s->sigId)+2);
        buf.insert(buf.end(), vbuf.begin(), vbuf.end());
    }

    if(!logInfo.empty()){
        buf.push_back(static_cast<uint8_t>(logInfo.size()));
        buf.insert(buf.end(), logInfo.begin(), logInfo.end());
    }
    return buf;
}

void SetMessage::parse(const uint8_t* data, size_t len){
    size_t offset = sizeof(MsgHeader);
    while(offset + 4 <= len){
        SignalItem* item = new SignalItem();
        memcpy(&item->devId, data+offset, 2);
        memcpy(&item->sigId, data+offset+2, 2);
        item->value = new SignalValue();
        sets.push_back(item);
        offset += 4;
    }
}

// ---------------- AlarmMessage ----------------
std::vector<uint8_t> AlarmMessage::build() const {
    std::vector<uint8_t> buf(sizeof(MsgHeader));
    MsgHeader* hdr = reinterpret_cast<MsgHeader*>(buf.data());
    *hdr = header;
    return buf;
}

void AlarmMessage::parse(const uint8_t*, size_t){}

// ---------------- PerformanceMessage ----------------
std::vector<uint8_t> PerformanceMessage::build() const {
    std::vector<uint8_t> buf(sizeof(MsgHeader));
    MsgHeader* hdr = reinterpret_cast<MsgHeader*>(buf.data());
    *hdr = header;

    for(auto& p: perfReqs){
        buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&p->devId), reinterpret_cast<uint8_t*>(&p->devId)+2);
        buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&p->sigId), reinterpret_cast<uint8_t*>(&p->sigId)+2);
        std::vector<uint8_t> vbuf = p->value->encode();
        buf.insert(buf.end(), vbuf.begin(), vbuf.end());
    }
    return buf;
}

void PerformanceMessage::parse(const uint8_t* data, size_t len) {
    if(len < sizeof(MsgHeader)) return;

    size_t offset = sizeof(MsgHeader);
    while(offset + 6 <= len) { // devId(2) + sigId(2) + period(2) 最少6字节
        SignalItem* item = new SignalItem();
        memcpy(&item->devId, data + offset, 2); offset += 2;
        memcpy(&item->sigId, data + offset, 2); offset += 2;

        uint16_t period = 0;
        memcpy(&period, data + offset, 2); offset += 2;
        
        // 假设剩下就是信号值，根据类型长度解析
        // 这里以 uint32 为示例，你可以根据实际类型调整
        if(offset + 4 <= len) {
            item->value = new SignalValue();
            item->value->type = SignalType::U32;
            memcpy(&item->value->v_u32, data + offset, 4);
            offset += 4;
        }

        perfReqs.push_back(item);
    }
}
