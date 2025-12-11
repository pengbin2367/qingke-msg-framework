//
// Created by pengbin on 2025/12/11.
//

#ifndef QINGKE_MSG_FRAMEWORK_MESSAGETYPES_H
#define QINGKE_MSG_FRAMEWORK_MESSAGETYPES_H

#pragma once
#include <cstdint>

enum MsgType : uint8_t {
    MSG_QUERY = 1,
    MSG_SET   = 2,
    MSG_ALARM = 3,
    MSG_PERF  = 4
};

#endif //QINGKE_MSG_FRAMEWORK_MESSAGETYPES_H