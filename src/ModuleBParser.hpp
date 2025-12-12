//
// Created by pengbin on 2025/12/11.
//

#ifndef QINGKE_MSG_FRAMEWORK_MODULEBPARSER_H
#define QINGKE_MSG_FRAMEWORK_MODULEBPARSER_H
#pragma once
#include "MessageParser.hpp"

class ModuleBParser : public MessageParser {
public:
    Message* parse(const uint8_t* data, size_t len) override;
};

extern "C" MessageParser* CreateModuleBParser();

#endif //QINGKE_MSG_FRAMEWORK_MODULEBPARSER_H