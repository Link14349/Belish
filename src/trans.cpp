#include "trans.h"
#include <sstream>
#include <climits>

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
    return *(int64_t*)(&n);
}
double Belish::transI64D_bin(int64_t n) {
    return *(double*)(&n);
}
string Belish::transI64S_bin(int64_t n) {
    string res;
    char t = 56;
    for (auto i = 0xff00000000000000; i; i >>= 8, t -= 8) {
        res += (char)((n & i) >> t);
    }
    return res;
}
string Belish::transI32S_bin(int32_t n) {
    string res;
    char t = 24;
    for (auto i = 0xff000000; i; i >>= 8, t -= 8) {
        res += (char)((n & i) >> t);
    }
    return res;
}

void Belish::escape(string& str) {
    bool sign = false;
    for (UL i = 0; i < str.length(); i++) {
        if (str[i] == '\\') {
            if (sign) str.replace(i - 1, 2, "\\");
            sign = !sign;
        } else if (sign) {
            char target = 0xff;
            switch (str[i]) {
                case 'a': target = '\a'; break;
                case 'b': target = '\b'; break;
                case 'f': target = '\f'; break;
                case 'n': target = '\n'; break;
                case 'r': target = '\r'; break;
                case 't': target = '\t'; break;
                case 'v': target = '\v'; break;
                case '?': target = '?'; break;
                case '\'': target = '\''; break;
                case '\"': target = '\"'; break;
                case 'x': {
                    i++;
                    target = (str[i] - (str[i] < 'A' ? '0' : (str[i] >= 'a' ? 'a' : 'A'))) << 4;
                    i++;
                    target |= str[i] - (str[i] < 'A' ? '0' : (str[i] >= 'a' ? 'a' : 'A'));
                    break;
                }
                case '0': {
                    target = ((str[++i] - '0') << 3) | (str[++i] - '0');
                    break;
                }
                default: target = str[i]; break;
            }
            string tmp;
            tmp += target;
            str.replace(i - 1, 2, tmp);
            sign = false;
        }
    }
}
