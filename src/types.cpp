#include <sstream>
#include "types.h"

bool Belish::isNumber(const string &n) {
    if (n[0] == '.') return false;
    bool doted = false;
    for (UL i = 0; i < n.length(); i++) {
        if (n[i] == '.') {
            if (doted) return false;
            doted = true;
        } else if (!(n[i] >= '0' && n[i] <= '9')) return false;
    }
    return true;
}
bool Belish::isString(const string &n) {
    char c = n[0];
    if (c != '\'' && c != '"') return false;
    return n[n.length() - 1] == c;
}
double Belish::transSD(const string &s) {
    std::stringstream ss(s);
    double t;
    ss >> t;
    return t;
}
int64_t Belish::transDI64_bin(double n) {
    union {
        double a;
        int64_t b;
    } tmp;
    tmp.a = n;
    return tmp.b;
}
string Belish::transI64S_bin(int64_t n) {
    string res;
    for (unsigned long long i = 0xff00000000000000; i >= 1; i >>= 8) {
        res += (char)(n & i);
    }
    return res;
}