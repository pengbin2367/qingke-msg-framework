//
// Created by pengbin on 2025/12/11.
//

#ifndef QINGKE_MSG_FRAMEWORK_MODULEAPARSER_H
#define QINGKE_MSG_FRAMEWORK_MODULEAPARSER_H

#pragma once
#include "MessageParser.hpp"

class ModuleAParser : public MessageParser {
public:
    Message* parse(const uint8_t* data, size_t len) override;
};

extern "C" MessageParser* CreateModuleAParser();

#endif //QINGKE_MSG_FRAMEWORK_MODULEAPARSER_H