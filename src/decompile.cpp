#include <iostream>
#include <cstdio>
#include <ctime>
#include <vector>
#include "decompile.h"
#include "compiler.h"
#include "trans.h"
using std::vector;

void Belish::decompile(char* bytecode, ULL len) {
    std::cout << "[DECOMPILE]" << std::endl;
    Byte byte;
    Dbyte dbyte;
    Qbyte qbyte;
    Ebyte ebyte;
    UL i = 0;
    GETQBYTE
    if (qbyte != 0x9ad0755c) {
        std::cerr << "Wrong magic code" << std::endl;
        return;
    }
    GETDBYTE
    printf("Version: 0x%04x\n", dbyte);
    GETEBYTE
    auto timer = gmtime(reinterpret_cast<const time_t *>(&ebyte));
    printf("Built time: %lld(timestamp), %d/%02d/%02d %02d:%02d:%02d\n", ebyte, timer->tm_year + 1900, timer->tm_mon + 1, timer->tm_mday, timer->tm_hour + 8, timer->tm_min, timer->tm_sec);
    GETQBYTE
    UL footerAdr = i = qbyte;
    printf("Footer address: 0x%08x\n", footerAdr);
    // 关于定义函数、类等的处理
    // 获取所有函数
    vector<UL> functions;
    GETQBYTE
    UL functionLen(qbyte);
    std::cout << "=====[FUNCTIONS INDEX]=====" << std::endl;
    std::cout << "Function count: " << functionLen << std::endl;
    for (UL j = 0; j < functionLen; j++) {
        GETQBYTE
        printf("#%d 0x%08x\n", j, qbyte + 4);
        functions.push_back(qbyte);
    }
    auto indexEnd = i;
    UL functionsOffset = 0;
    std::cout << "=====[END]=====" << std::endl;
    std::cout << "=====[MAIN]=====" << std::endl;
    for (i = 18; i < len; ) {
        if (i >= footerAdr && i < indexEnd) {
            std::cout << "=====[END]=====" << std::endl;
            std::cout << "=====[FUNCTION DEFINITIONS]=====" << std::endl;
            i = indexEnd;
            if (i >= len) break;
        }
        if (!functions.empty() && i == functions[functionsOffset]) {
            std::cout << "----[FUNCTION: #" << (functionsOffset++) << "]----" << std::endl;
            GETQBYTE
            UL tmpI = i;
            i = qbyte;
            GETQBYTE
            std::cout << "Outer Values' Count: " << qbyte << std::endl;
            UL ovc = qbyte;
            std::cout << "Outer Values: " << (ovc ? "" : "[NONE]");
            for (UL j = 0; j < ovc; j++) {
                GETQBYTE
                printf("0x%08x ", qbyte);
            }
            std::cout << std::endl;
            i = tmpI;
        }
        printf("0x%08x\t", i);
        GETBYTE;
        auto op = (OPID)byte;
#define CASE_OP(name) case name: printf("%s ", #name);
        switch (op) {
            CASE_OP(ADD) printf("\n"); break;
            CASE_OP(SUB) printf("\n"); break;
            CASE_OP(MUL) printf("\n"); break;
            CASE_OP(DIV) printf("\n"); break;
            CASE_OP(MOD) printf("\n"); break;
            CASE_OP(EQ) printf("\n"); break;
            CASE_OP(NEQ) printf("\n"); break;
            CASE_OP(LEQ) printf("\n"); break;
            CASE_OP(MEQ) printf("\n"); break;
            CASE_OP(LESS) printf("\n"); break;
            CASE_OP(MORE) printf("\n"); break;
            CASE_OP(MAND) printf("\n"); break;
            CASE_OP(MOR) printf("\n"); break;
            CASE_OP(MXOR) printf("\n"); break;
            CASE_OP(MNOT) printf("\n"); break;
            CASE_OP(LAND) printf("\n"); break;
            CASE_OP(LOR) printf("\n"); break;
            CASE_OP(LNOT) printf("\n"); break;
            CASE_OP(POW) printf("\n"); break;
            CASE_OP(MOV) printf("\n"); break;
            CASE_OP(POP) printf("\n"); break;
            CASE_OP(PUSH_OBJ) printf("\n"); break;
            CASE_OP(PUSH_NULL) printf("\n"); break;
            CASE_OP(PUSH_UND) printf("\n"); break;
            CASE_OP(DEB) printf("\n"); break;
            CASE_OP(SAV) printf("\n"); break;
            CASE_OP(BAC) printf("\n"); break;
            CASE_OP(SET_ATTR) printf("\n"); break;
            CASE_OP(GET_ATTR) printf("\n"); break;
            CASE_OP(RET) printf("\n"); break;
            CASE_OP(CALL_FUN) printf("\n"); break;
            CASE_OP(IMP) printf("\n"); break;
            CASE_OP(LOAD) printf("\n"); break;
            CASE_OP(PUSH_TRUE) printf("\n"); break;
            CASE_OP(PUSH_FALSE) printf("\n"); break;
            CASE_OP(PUSH_NUM) {
                GETEBYTE
                printf("%lf\n", transI64D_bin(ebyte));
                break;
            }
            CASE_OP(REFER) goto DECOM_SWITCH_REFER_PUSH_CASE;
            CASE_OP(JT) goto DECOM_SWITCH_REFER_PUSH_CASE;
            CASE_OP(JF) goto DECOM_SWITCH_REFER_PUSH_CASE;
            CASE_OP(JMP) goto DECOM_SWITCH_REFER_PUSH_CASE;
            CASE_OP(CHANGE) goto DECOM_SWITCH_REFER_PUSH_CASE;
            CASE_OP(PUSH_OUTER) goto DECOM_SWITCH_REFER_PUSH_CASE;
            CASE_OP(PUSH)
            {
                DECOM_SWITCH_REFER_PUSH_CASE:
                GETQBYTE
                printf("0x%08x\n", qbyte);
                break;
            }
            CASE_OP(POPC) goto DECOM_SWITCH_POPC_NEW_FRAME_CASE;
            CASE_OP(PUSH_FUN) printf("#"); goto DECOM_SWITCH_POPC_NEW_FRAME_CASE;
            CASE_OP(CALL) printf("#"); goto DECOM_SWITCH_POPC_NEW_FRAME_CASE;
            CASE_OP(RESIZE) goto DECOM_SWITCH_POPC_NEW_FRAME_CASE;
            CASE_OP(NEW_FRAME)
            {
                DECOM_SWITCH_POPC_NEW_FRAME_CASE:
                GETQBYTE
                printf("%u\n", qbyte);
                break;
            }
            CASE_OP(REG_EQ) goto DECOM_SWITCH_REG_COM_CASE;
            CASE_OP(REG_NEQ) goto DECOM_SWITCH_REG_COM_CASE;
            CASE_OP(REG_LEQ) goto DECOM_SWITCH_REG_COM_CASE;
            CASE_OP(REG_MEQ) goto DECOM_SWITCH_REG_COM_CASE;
            CASE_OP(REG_MORE) goto DECOM_SWITCH_REG_COM_CASE;
            CASE_OP(REG_LESS) goto DECOM_SWITCH_REG_COM_CASE;
            CASE_OP(REG_ADD) goto DECOM_SWITCH_REG_COM_CASE;
            CASE_OP(REG_SUB)
            {
                DECOM_SWITCH_REG_COM_CASE:
                GETBYTE;
                printf("%%%02x, ", byte);
                GETEBYTE
                printf("%lf\n", transI64D_bin(ebyte));
                break;
            }
            CASE_OP(MOV_REG) {
                GETQBYTE
                printf("0x%08x, ", qbyte);
                GETBYTE;
                printf("%%%02x\n", byte);
                break;
            }
            CASE_OP(PUSH_STR) {
                GETQBYTE
                UL strlen = qbyte;
                string str;
                for (UL j = 0; j < strlen; j++, i++) {
                    str += bytecode[i];
                }
                std::cout << "[" << strlen << "] \"" << str << "\"" << std::endl;
                break;
            }
            default:
                printf("%02x\n", byte);
                break;
        }
    }
    std::cout << "=====[END]=====" << std::endl;
}