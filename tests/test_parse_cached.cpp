//
// Created by pengbin on 2025/12/11.
//
#include <gtest/gtest.h>
#include "MessageBuilder.hpp"
#include "SequenceManager.hpp"
#include "MessageCache.hpp"
#include "MessageParserFactory.hpp"
#include "ModuleAParser.hpp"

static std::vector<uint8_t> makeFakeQueryResponse(uint16_t src, uint16_t dst, uint32_t seq) {
    std::vector<uint8_t> payload;
    payload.push_back(0); payload.push_back(1);
    payload.push_back(0); payload.push_back(10);
    payload.push_back(0); payload.push_back(2);
    payload.push_back(0x12); payload.push_back(0x34);
    std::vector<uint8_t> pkt;
    pkt.push_back((src>>8)&0xFF); pkt.push_back(src&0xFF);
    pkt.push_back((dst>>8)&0xFF); pkt.push_back(dst&0xFF);
    pkt.push_back(MSG_QUERY);
    uint16_t total = (uint16_t)(11 + payload.size());
    pkt.push_back((total>>8)&0xFF); pkt.push_back(total&0xFF);
    pkt.push_back((seq>>24)&0xFF); pkt.push_back((seq>>16)&0xFF); pkt.push_back((seq>>8)&0xFF); pkt.push_back(seq&0xFF);
    pkt.insert(pkt.end(), payload.begin(), payload.end());
    return pkt;
}

TEST(ParseCached, BasicQuery) {
    SequenceManager seqMgr;
    seqMgr.registerModule(10, 0x1000, 0x1FFF);

    MessageBuilder builder(&seqMgr);
    builder.addQuery(1, 10, 10);
    auto msgs = builder.takeBuiltMessages();
    ASSERT_EQ(msgs.size(), 1);
    uint32_t seq = msgs[0]->header.seq;
    delete msgs[0];

    MessageCache cache;
    auto resp = makeFakeQueryResponse(10, 99, seq);
    cache.store(10, seq, resp.data(), resp.size());

    MessageParserFactory::registerModuleParser(10, ModuleA_createFromRaw);
    Message* parsed = MessageParserFactory::parseCached(10, seq, cache);
    ASSERT_NE(parsed, nullptr);
    MessageCache::RawMsg raw;
    ASSERT_TRUE(cache.fetchRaw(10, seq, raw));
    ASSERT_TRUE(parsed->parseResponse(raw.data.data() + 11, raw.data.size() - 11));
    QueryMessage* q = dynamic_cast<QueryMessage*>(parsed);
    ASSERT_NE(q, nullptr);
    ASSERT_EQ(q->outputs.size(), 1);
    EXPECT_EQ(q->outputs[0].deviceId, 1);
    EXPECT_EQ(q->outputs[0].signalId, 10);
    EXPECT_EQ(q->outputs[0].valueLen, 2);
    EXPECT_EQ(q->outputs[0].value[0], 0x12);
    EXPECT_EQ(q->outputs[0].value[1], 0x34);
    delete parsed;
    cache.erase(10, seq);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
