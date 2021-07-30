//
// Created by Matthew Good on 29/7/21.
//

#ifndef GRAPHICAL_TOOL_KIT_LOG_H
#define GRAPHICAL_TOOL_KIT_LOG_H

#include <sstream>

class Log {
public:
    enum SEVERITY {
        INFO,
        ERROR,
        FATAL
    };

private:
    template <typename SSType, typename ArgType>
    static void FormatStrSS(SSType& ss, const ArgType& Arg)
    {
        ss << Arg;
    }

    template <typename SSType, typename FirstArgType, typename... RestArgsType>
    static void FormatStrSS(SSType& ss, const FirstArgType& FirstArg, const RestArgsType&... RestArgs)
    {
        FormatStrSS(ss, FirstArg);
        FormatStrSS(ss, RestArgs...); // recursive call using pack expansion syntax
    }

public:
    static void Log_String(const SEVERITY & severity, const char * msg);
    static void Log_String(const SEVERITY & severity, const std::string & msg);

    template <typename... RestArgsType>
    static std::string FormatString(const RestArgsType&... Args)
    {
        std::stringstream ss;
        FormatStrSS(ss, Args...);
        return ss.str();
    }

    template<typename... RestArgsType>
    static void Log_With_Severity(const SEVERITY & severity, const RestArgsType &... Args) {
        std::string msg = FormatString(Args...);
        Log_String(severity, msg);
    }

    template<typename... RestArgsType>
    static void Debug(const RestArgsType &... Args) {
        Log_With_Severity(INFO, Args...);
    }

    template<typename... RestArgsType>
    static void Error_And_Throw(const RestArgsType &... Args) {
        Log_With_Severity(FATAL, Args...);
    }
};

#endif //GRAPHICAL_TOOL_KIT_LOG_H
