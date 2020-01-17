#ifndef BELISH_TRANS_H
#define BELISH_TRANS_H

#include <string>
using std::string;

typedef uint64_t ULL;
typedef uint32_t UL;
typedef uint32_t U;
typedef int64_t LL;
typedef int32_t L;
typedef uint8_t Byte;
typedef uint16_t Dbyte;
typedef uint32_t Qbyte;
typedef uint64_t Ebyte;

namespace Belish {
    inline bool isNumber(const string&);
    inline bool isString(const string&);
    inline double transSD(const string&);// trans string to double
    inline int64_t transDI64_bin(double);// trans double to int64_t by binary
    inline double transI64D_bin(int64_t);// trans int64_t to double by binary
    inline string transI64S_bin(int64_t);// trans int64_t to string by binary
    inline string transI32S_bin(int32_t);// trans transI32S_bin to string by binary
    inline void escape(string& str);// escape sequence
}

#endif //BELISH_TRANS_H
