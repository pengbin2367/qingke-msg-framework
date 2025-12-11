//
// Created by pengbin on 2025/12/11.
//

#ifndef QINGKE_MSG_FRAMEWORK_MODULEAPARSER_H
#define QINGKE_MSG_FRAMEWORK_MODULEAPARSER_H

#pragma once
#include "Message.hpp"

// ModuleA-specific raw creator (allocates and parses message)
Message* ModuleA_createFromRaw(const uint8_t* data, size_t len);

#endif //QINGKE_MSG_FRAMEWORK_MODULEAPARSER_H