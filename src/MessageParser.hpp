//
// Created by pengbin on 12/13/25.
//

#ifndef QINGKE_MSG_FRAMEWORK_MESSAGEPARSER_HPP
#define QINGKE_MSG_FRAMEWORK_MESSAGEPARSER_HPP
#pragma once
#include "Message.hpp"
#include <cstddef>
#include <cstdint>

class MessageParser {
public:
    virtual ~MessageParser() {}
    virtual Message* parse(const uint8_t* data, size_t len) = 0;
};

#endif //QINGKE_MSG_FRAMEWORK_MESSAGEPARSER_HPP