#include "decompile.h"
#include "trans.h"

void Belish::decompile(char* bytecode, ULL len, string& res) {
    Byte byte;
    Dbyte dbyte;
    Qbyte qbyte;
    Ebyte ebyte;
    ULL i = 0;
    GETQBYTE
    GETDBYTE
    GETEBYTE
    for (; i < len; ) {
    }
}