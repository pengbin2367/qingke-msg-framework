//
// Created by pengbin on 2025/12/11.
//

#ifndef QINGKE_MSG_FRAMEWORK_MESSAGECACHE_H
#define QINGKE_MSG_FRAMEWORK_MESSAGECACHE_H
#pragma once
#include "Message.hpp"
#include "SeqManager.hpp"
#include <map>
#include <cstdint>
#include <vector>

class MessageCache {
private:
    struct CacheEntry {
        Message* request;  // 持有请求消息所有权
        Message* response; // 持有响应消息所有权（可为空）
    };

    std::map<uint32_t, CacheEntry> cache; // key = seq
    SeqManager& seqMgr;
    uint16_t moduleId;

public:
    explicit MessageCache(SeqManager& sm, uint16_t modId);

    ~MessageCache();

    // 将请求消息加入缓存并分配序列号
    uint32_t pushRequest(Message* req);

    // 将响应消息关联到对应请求
    void pushResponse(uint32_t seq, Message* resp);

    // 获取响应消息（不删除缓存）
    Message* getResponse(uint32_t seq);

    // 清理缓存
    void clear();
};


#endif //QINGKE_MSG_FRAMEWORK_MESSAGECACHE_H