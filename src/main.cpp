#include "MessageBuilder.hpp"
#include "SequenceManager.hpp"
#include "MessageCache.hpp"
#include "MessageParserFactory.hpp"
#include "ModuleAParser.hpp"
#include "ModuleBParser.hpp"
#include <iostream>
#include <vector>

// helper to build fake response for Query (payload only: [dev sig vlen val...])
static std::vector<uint8_t> makeFakeQueryResponse(uint16_t src, uint16_t dst, uint32_t seq) {
    std::vector<uint8_t> payload;
    // one record: dev=1 sig=10 vlen=2 val=0x12 0x34
    payload.push_back(0); payload.push_back(1);
    payload.push_back(0); payload.push_back(10);
    payload.push_back(0); payload.push_back(2);
    payload.push_back(0x12); payload.push_back(0x34);

    std::vector<uint8_t> pkt;
    // header
    pkt.push_back((src>>8)&0xFF); pkt.push_back(src&0xFF);
    pkt.push_back((dst>>8)&0xFF); pkt.push_back(dst&0xFF);
    pkt.push_back(MSG_QUERY);
    uint16_t total = (uint16_t)(11 + payload.size());
    pkt.push_back((total>>8)&0xFF); pkt.push_back(total&0xFF);
    pkt.push_back((seq>>24)&0xFF); pkt.push_back((seq>>16)&0xFF); pkt.push_back((seq>>8)&0xFF); pkt.push_back(seq&0xFF);
    pkt.insert(pkt.end(), payload.begin(), payload.end());
    return pkt;
}

// helper to build fake response for Set (payload: [sig(2) status(1)] )
static std::vector<uint8_t> makeFakeSetResponse(uint16_t src, uint16_t dst, uint32_t seq) {
    std::vector<uint8_t> payload;
    put_u16(payload, 10); // sig
    payload.push_back(0); // success
    std::vector<uint8_t> pkt;
    pkt.push_back((src>>8)&0xFF); pkt.push_back(src&0xFF);
    pkt.push_back((dst>>8)&0xFF); pkt.push_back(dst&0xFF);
    pkt.push_back(MSG_SET);
    uint16_t total = (uint16_t)(11 + payload.size());
    pkt.push_back((total>>8)&0xFF); pkt.push_back(total&0xFF);
    pkt.push_back((seq>>24)&0xFF); pkt.push_back((seq>>16)&0xFF); pkt.push_back((seq>>8)&0xFF); pkt.push_back(seq&0xFF);
    pkt.insert(pkt.end(), payload.begin(), payload.end());
    return pkt;
}

// small helper for put_u16 in this file
static void put_u16(std::vector<uint8_t>& b, uint16_t v){ b.push_back((v>>8)&0xFF); b.push_back(v&0xFF); }

int main() {
    SequenceManager seqMgr;
    seqMgr.registerModule(10, 0x1000, 0x1FFF); // module 10
    seqMgr.registerModule(20, 0x2000, 0x2FFF); // module 20

    // register module-specific parsers
    MessageParserFactory::registerModuleParser(10, ModuleA_createFromRaw);
    MessageParserFactory::registerModuleParser(20, ModuleB_createFromRaw);

    MessageBuilder builder(&seqMgr);
    // build a query to module 10
    builder.addQuery(1, 10, 10);
    // build a set to module 20 (device 1 signal 10 value 123)
    builder.addSet(1, 10, 123, 20);

    auto msgs = builder.takeBuiltMessages();
    MessageCache cache;

    // simulate send: for each message, simulate response and store raw into cache, and also
    // move the built Message* into cache if desired (here we delete after storing raw)
    for (auto *m : msgs) {
        // set header srcModule (us)
        m->header.srcModule = 999;
        uint16_t dst = m->header.dstModule;
        uint32_t seq = m->header.seq;

        // build bytes (would be sent on wire)
        std::vector<uint8_t> bytes = m->build();
        std::cout << "Built msg type=" << int(m->header.msgType) << " dst="<<dst<<" seq="<<seq<<" bytes="<<bytes.size()<<"\n";

        // simulate response raw
        std::vector<uint8_t> resp;
        if (m->header.msgType == MSG_QUERY) resp = makeFakeQueryResponse(dst, m->header.srcModule, seq);
        else if (m->header.msgType == MSG_SET) resp = makeFakeSetResponse(dst, m->header.srcModule, seq);
        else resp = std::vector<uint8_t>(); // other not simulated

        // store raw response in cache under module == dst
        if (!resp.empty()) cache.store(dst, seq, resp.data(), resp.size());

        // optionally transfer built message ownership to cache (uncomment to demo)
        // cache.storeMessage(dst, seq, m);
        // if not storing the message, we must delete it
        delete m;
    }

    // Now parse cached response for module 10 seq=0x1000
    uint32_t seq1 = 0x1000;
    Message* parsed = MessageParserFactory::parseCached(10, seq1, cache);
    if (!parsed) { std::cerr << "parseCached failed for module 10 seq\n"; }
    else {
        // get raw payload from cache to call parseResponse
        MessageCache::RawMsg raw;
        if (cache.fetchRaw(10, seq1, raw)) {
            // payload offset is HEADER_SIZE (11)
            if (!parsed->parseResponse(raw.data.data() + 11, raw.data.size() - 11)) {
                std::cerr << "parseResponse failed\n";
            } else {
                parsed->print();
            }
        }
        delete parsed;
        cache.erase(10, seq1);
    }

    // parse module 20 set response (seq from seqMgr start 0x2000)
    uint32_t seq2 = 0x2000;
    Message* parsed2 = MessageParserFactory::parseCached(20, seq2, cache);
    if (!parsed2) std::cerr << "parseCached failed for module 20\n";
    else {
        MessageCache::RawMsg raw; if (cache.fetchRaw(20, seq2, raw)) {
            if (!parsed2->parseResponse(raw.data.data() + 11, raw.data.size() - 11)) std::cerr << "parseResponse failed\n";
            else parsed2->print();
        }
        delete parsed2;
        cache.erase(20, seq2);
    }

    cache.clearAll();
    return 0;
}
