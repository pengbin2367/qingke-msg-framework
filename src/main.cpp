#include <iostream>
#include <vector>
#include <cstdint>
#include "MessageBuilder.hpp"
#include "MessageParserFactory.hpp"
#include "ModuleAParser.hpp"
#include "ModuleBParser.hpp"
#include "MessageCache.hpp"
#include "SeqManager.hpp"

int main() {
    // 模块ID定义
    uint16_t MODULE_A = 1;
    uint16_t MODULE_B = 2;

    // 初始化序列号管理
    SeqManager seqMgr;

    // 初始化缓存
    MessageCache cacheA(seqMgr, MODULE_A);
    MessageCache cacheB(seqMgr, MODULE_B);

    // 注册模块解析器
    MessageParserFactory::registerParser(MsgType::QUERY, CreateModuleAParser);
    MessageParserFactory::registerParser(MsgType::SET, CreateModuleAParser);
    MessageParserFactory::registerParser(MsgType::ALARM, CreateModuleAParser);
    MessageParserFactory::registerParser(MsgType::PERFORMANCE, CreateModuleAParser);

    // ---------------- MessageBuilder ----------------
    MessageBuilder builder;

    // 添加查询
    builder.addQuery(100, 1);
    builder.addQuery(101, 2);

    // 添加设置
    builder.addSet(100, 1, new SignalValue(uint32_t(42)), "admin操作");
    builder.addSet(101, 2, new SignalValue(float(3.14)));

    // 添加告警
    builder.addAlarm();

    // 添加性能信号
    builder.addPerformance(100, 1, new SignalValue(uint32_t(123)), 60);

    // 获取构建好的消息
    std::vector<Message*> msgs = builder.takeBuiltMessages();

    // ---------------- 模拟发送消息 ----------------
    for(auto msg : msgs) {
        if(msg->header.type == MsgType::QUERY) {
            uint32_t seq = cacheA.pushRequest(msg);
            std::cout << "QueryMessage sent, seq=" << seq << std::endl;
        } else if(msg->header.type == MsgType::SET) {
            uint32_t seq = cacheA.pushRequest(msg);
            std::cout << "SetMessage sent, seq=" << seq << std::endl;
        } else if(msg->header.type == MsgType::ALARM) {
            uint32_t seq = cacheA.pushRequest(msg);
            std::cout << "AlarmMessage sent, seq=" << seq << std::endl;
        } else if(msg->header.type == MsgType::PERFORMANCE) {
            uint32_t seq = cacheA.pushRequest(msg);
            std::cout << "PerformanceMessage sent, seq=" << seq << std::endl;
        }
    }

    // ---------------- 模拟收到响应 ----------------
    for(auto msg : msgs){
        std::vector<uint8_t> raw = msg->build();

        // 使用解析器解析
        Message* resp = MessageParserFactory::parseMessage(raw.data(), raw.size());

        // 将响应放入缓存
        cacheA.pushResponse(msg->header.seq, resp);

        std::cout << "Response parsed for seq=" << msg->header.seq << std::endl;
    }

    // ---------------- 获取响应 ----------------
    for(auto msg : msgs){
        Message* resp = cacheA.getResponse(msg->header.seq);
        if(resp) {
            std::cout << "Got response for seq=" << msg->header.seq << ", type=" << static_cast<int>(resp->header.type) << std::endl;
        }
    }

    // 清理
    cacheA.clear();
    cacheB.clear();

    return 0;
}
