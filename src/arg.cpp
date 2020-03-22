#include "arg.h"

Belish::Arg::Arg(int ac, char **av)
        : argc(--ac), argv(new std::string[ac]) {
    av++;
    bool preIsIndFlag = false;
    std::string preFlagName;
    for (size_t i = 0; i < ac; i++, av++) {
        std::string &str(*(argv + i) = *av);
        bool nowIsIndFlag = true;
        std::string flagName;
        std::string value;
        auto *now = &flagName;
        for (size_t j = 0; j < str.length(); j++) {
            if (str[j] == '=') {
                nowIsIndFlag = false;
                now = &value;
                continue;
            }
            now->operator+=(str[j]);
        }
        if (preIsIndFlag) {
            if (flagName[0] == '-') {// 说明上一个就是一个独立的flag
                flags.insert(preFlagName);
                preIsIndFlag = false;
            } else {// 说明这里就是给上一个flag赋值的
                values[preFlagName] = flagName;
                preIsIndFlag = false;
                continue;
            }
        }
        if (nowIsIndFlag) {
            if (flagName[0] == '-') {// 独立的flag
                preIsIndFlag = true;
                preFlagName = flagName;
            } else {// 说明是一个独立值
                indValues.push_back(flagName);
            }
            continue;
        }
        values[flagName] = value;
    }
    if (preIsIndFlag) {
        flags.insert(preFlagName);
    }
}