//
// Created by pengbin on 2025/12/11.
//
#include "MessageCache.hpp"

MessageCache::MessageCache(SeqManager& sm, uint16_t modId)
    : seqMgr(sm), moduleId(modId) {}

MessageCache::~MessageCache() {
    for(auto& kv: cache){
        delete kv.second.request;
        delete kv.second.response;
    }
    cache.clear();
}

uint32_t MessageCache::pushRequest(Message* req){
    uint32_t seq = seqMgr.nextSeq(moduleId);
    req->header.seq = seq;
    cache[seq] = { req, nullptr };
    return seq;
}

void MessageCache::pushResponse(uint32_t seq, Message* resp){
    if(cache.count(seq)){
        CacheEntry& entry = cache[seq];
        if(entry.response) delete entry.response;
        entry.response = resp;
    } else {
        // 如果 seq 不存在，可选择忽略或加入缓存
        cache[seq] = { nullptr, resp };
    }
}

Message* MessageCache::getResponse(uint32_t seq){
    if(cache.count(seq)) return cache[seq].response;
    return nullptr;
}

void MessageCache::clear(){
    for(auto& kv: cache){
        delete kv.second.request;
        delete kv.second.response;
    }
    cache.clear();
}
