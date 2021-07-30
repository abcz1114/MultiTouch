//
// Created by Matthew Good on 29/7/21.
//

#include "Log.h"

#include <DiligentCore/Primitives/interface/Errors.hpp>

void Log::Log_String(const Log::SEVERITY & severity, const char * msg) {
    if (Diligent::DebugMessageCallback != nullptr) {
        Diligent::DebugMessageCallback(
                severity == Log::INFO ? Diligent::DEBUG_MESSAGE_SEVERITY_INFO
                                      : (
                        severity == Log::ERROR ? Diligent::DEBUG_MESSAGE_SEVERITY_ERROR
                                               : Diligent::DEBUG_MESSAGE_SEVERITY_FATAL_ERROR
                ), msg, nullptr, nullptr, 0
        );
    }
    if (severity == Log::FATAL) {
        throw std::runtime_error(msg);
    }
}

void Log::Log_String(const Log::SEVERITY & severity, const std::string & msg) {
    Log_String(severity, msg.c_str());
}
