//
// Created by pengbin on 2025/12/11.
//#include <gtest/gtest.h>
#include "MessageBuilder.hpp"
#include "MessageParserFactory.hpp"
#include "ModuleAParser.hpp"
#include "Message.hpp"

TEST(MessageTest, QueryBuildParse) {
    // 注册解析器
    MessageParserFactory::registerParser(MsgType::QUERY, CreateModuleAParser);

    // 构建 Query 消息
    MessageBuilder builder;
    builder.addQuery(100, 1);
    builder.addQuery(101, 2);
    auto msgs = builder.takeBuiltMessages();

    ASSERT_EQ(msgs.size(), 1);
    QueryMessage* q = dynamic_cast<QueryMessage*>(msgs[0]);
    ASSERT_NE(q, nullptr);
    ASSERT_EQ(q->requests.size(), 2);

    // 构建原始数据
    std::vector<uint8_t> raw = q->build();

    // 解析
    Message* parsed = MessageParserFactory::parseMessage(raw.data(), raw.size());
    QueryMessage* pq = dynamic_cast<QueryMessage*>(parsed);
    ASSERT_NE(pq, nullptr);
    ASSERT_EQ(pq->requests.size(), 2);
    EXPECT_EQ(pq->requests[0]->devId, 100);
    EXPECT_EQ(pq->requests[0]->sigId, 1);
    EXPECT_EQ(pq->requests[1]->devId, 101);
    EXPECT_EQ(pq->requests[1]->sigId, 2);

    delete parsed;
    for(auto m: msgs) delete m;
}

TEST(MessageTest, SetBuildParse) {
    MessageParserFactory::registerParser(MsgType::SET, CreateModuleAParser);

    MessageBuilder builder;
    builder.addSet(100, 1, new SignalValue(uint32_t(42)), "logInfo");
    auto msgs = builder.takeBuiltMessages();
    ASSERT_EQ(msgs.size(), 1);
    SetMessage* s = dynamic_cast<SetMessage*>(msgs[0]);
    ASSERT_NE(s, nullptr);
    ASSERT_EQ(s->sets.size(), 1);
    EXPECT_EQ(s->sets[0]->devId, 100);
    EXPECT_EQ(s->sets[0]->sigId, 1);
    EXPECT_EQ(s->logInfo, "logInfo");

    std::vector<uint8_t> raw = s->build();
    Message* parsed = MessageParserFactory::parseMessage(raw.data(), raw.size());
    SetMessage* ps = dynamic_cast<SetMessage*>(parsed);
    ASSERT_NE(ps, nullptr);
    EXPECT_EQ(ps->sets.size(), 1);

    delete parsed;
    for(auto m: msgs) delete m;
}
