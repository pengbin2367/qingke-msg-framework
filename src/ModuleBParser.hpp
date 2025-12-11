//
// Created by pengbin on 2025/12/11.
//

#ifndef QINGKE_MSG_FRAMEWORK_MODULEBPARSER_H
#define QINGKE_MSG_FRAMEWORK_MODULEBPARSER_H

#pragma once
#include "Message.hpp"

Message* ModuleB_createFromRaw(const uint8_t* data, size_t len);

#endif //QINGKE_MSG_FRAMEWORK_MODULEBPARSER_H