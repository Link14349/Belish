#ifndef MYASMJIT_MASMJIT_H
#define MYASMJIT_MASMJIT_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <check-is-win.h>
#ifdef I_OS_WIN32
#include <Windows.h>
#else
#include <sys/mman.h>
#endif

namespace MAsmJit {
    enum REG8BIT {
        AH = 0xb4, AL = 0xb0, BH = 0xb7, BL = 0xb3, CH = 0xb5, CL = 0xb1, DH = 0xb6, DL = 0xb2,
    };
    enum REG16BIT {
        AX = 0xb8, BX = 0xbb, CX = 0xb9, DX = 0xba,
    };
    enum REG32BIT {
        EAX = 0xb8, EBX = 0xbb, ECX = 0xb9, EDX = 0xba,
    };
    enum REG64BIT {
        RAX = 0xb8, RBX = 0xbb, RCX = 0xb9, RDX = 0xba,
    };
    class MAsmJit {
    public:
#define MAJ_APP machineCodeAdr[machineCodeIndex++]
        typedef void (*JIT_FUNC)();

#ifdef I_OS_WIN32
        MAsmJit(size_t cl = 1024) : codeLength(cl), machineCodeAdr((uint8_t*)VirtualAlloc(NULL, codeLength, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)), machineCodeIndex(0) { }
#else
        MAsmJit(size_t cl = 1024) : codeLength(cl), machineCodeAdr((uint8_t*) mmap(
                NULL,
                codeLength,
                PROT_READ | PROT_WRITE | PROT_EXEC,
                MAP_ANONYMOUS | MAP_PRIVATE,
                0,
                0)), machineCodeIndex(0) {

        }
#endif

#define MAJ_APP_8(source) MAJ_APP = source;
#define MAJ_APP_16(source) { MAJ_APP = source & 0xff; MAJ_APP = source >> 8;}
#define MAJ_APP_32(source) { MAJ_APP = source & 0xff; MAJ_APP = (source >> 8) & 0xff; MAJ_APP = (source >> 16) & 0xff; MAJ_APP = source >> 24; }
#define MAJ_APP_64(source) { MAJ_APP = source & 0xff; \
        MAJ_APP = (source >> 8) & 0xff; \
        MAJ_APP = (source >> 16) & 0xff; \
        MAJ_APP = (source >> 24) & 0xff; \
        MAJ_APP = (source >> 32) & 0xff; \
        MAJ_APP = (source >> 40) & 0xff; \
        MAJ_APP = (source >> 48) & 0xff; \
        MAJ_APP = source >> 56; }
#define MASMJIT_SIZE_NORM(NAME, SIZE) uint8_t sizeof_##NAME() { return SIZE; }
        MASMJIT_SIZE_NORM(movb, 2)
        void movb(REG8BIT reg, uint8_t source) {
            MAJ_APP = reg;
            MAJ_APP_8(source)
        }
        MASMJIT_SIZE_NORM(movw, 4)
        void movw(REG16BIT reg, uint16_t source) {
            MAJ_APP = 0x66;
            MAJ_APP = reg;
            MAJ_APP_16(source)
        }
        MASMJIT_SIZE_NORM(movl, 5)
        void movl(REG32BIT reg, uint32_t source) {
            MAJ_APP = reg;
            MAJ_APP_32(source)
        }
        MASMJIT_SIZE_NORM(movq, 10)
        void movq(REG64BIT reg, uint64_t source) {
            MAJ_APP = 0x48;
            MAJ_APP = reg;
            // Note, low byte order!
            MAJ_APP_64(source)
        }
        uint8_t sizeof_cmp(REG64BIT reg) {
            if (reg == RAX) return 6;
            return 7;
        }
        void cmp(REG64BIT reg, uint32_t source) {
            MAJ_APP = 0x48;
            if (reg == RAX) {
                MAJ_APP = 0x3d;
                MAJ_APP_32(source)
                return;
            }
            MAJ_APP = 0x81;
            switch (reg) {
                case RBX: MAJ_APP = 0xfb; break;
                case RCX: MAJ_APP = 0xf9; break;
                case RDX: MAJ_APP = 0xfa; break;
            }
            MAJ_APP_32(source)
        }
        MASMJIT_SIZE_NORM(movb_to_rbp, 7)
        void movb_to_rbp(uint32_t offset, uint8_t val) {
            MAJ_APP = 0xc6;
            MAJ_APP = 0x85;
            MAJ_APP_32(offset)
            MAJ_APP = val;
        }
        MASMJIT_SIZE_NORM(movl_to_rbp, 10)
        void movl_to_rbp(uint32_t offset, uint32_t val) {
            MAJ_APP = 0xc7;
            MAJ_APP = 0x85;
            MAJ_APP_32(offset)
            MAJ_APP_32(val)
        }
        MASMJIT_SIZE_NORM(ret, 1)
        void ret() {
            MAJ_APP = 0xc3;
        }
#define MASMJIT_JMP_B_SIZE(NAME) MASMJIT_SIZE_NORM(NAME, 2)
        MASMJIT_JMP_B_SIZE(jmp)
        void jmp(uint8_t offset) {
            MAJ_APP = 0xeb;
            MAJ_APP = offset;
        }
        MASMJIT_SIZE_NORM(jmp32, 5)
        void jmp(uint32_t offset) {
            MAJ_APP = 0xe9;
            MAJ_APP_32(offset)
        }
        MASMJIT_JMP_B_SIZE(je)
        void je(uint8_t offset) {
            MAJ_APP = 0x74;
            MAJ_APP = offset;
        }
        MASMJIT_JMP_B_SIZE(jne)
        void jne(uint8_t offset) {
            MAJ_APP = 0x75;
            MAJ_APP = offset;
        }
        MASMJIT_JMP_B_SIZE(jg)
        void jg(uint8_t offset) {
            MAJ_APP = 0x7f;
            MAJ_APP = offset;
        }
        MASMJIT_JMP_B_SIZE(jge)
        void jge(uint8_t offset) {
            MAJ_APP = 0x7d;
            MAJ_APP = offset;
        }
        MASMJIT_JMP_B_SIZE(jl)
        void jl(uint8_t offset) {
            MAJ_APP = 0x7c;
            MAJ_APP = offset;
        }
        MASMJIT_JMP_B_SIZE(jle)
        void jle(uint8_t offset) {
            MAJ_APP = 0x7e;
            MAJ_APP = offset;
        }
        uint8_t sizeof_addq(REG64BIT reg) {
            if (reg == RAX) return 6;
            return 7;
        }
        void addq(REG64BIT reg, uint32_t val) {
            MAJ_APP = 0x48;
            switch (reg) {
                case RAX:
                    MAJ_APP = 0x05;
                    goto ADD_SOURCE;
                case RBX:
                    MAJ_APP = 0x81;
                    MAJ_APP = 0xc3;
                    goto ADD_SOURCE;
                case RCX:
                    MAJ_APP = 0x81;
                    MAJ_APP = 0xc1;
                    goto ADD_SOURCE;
                case RDX:
                    MAJ_APP = 0x81;
                    MAJ_APP = 0xc2;
                    ADD_SOURCE: MAJ_APP_32(val)
            }
        }
        uint8_t sizeof_movabsq_to_rcx() { return 10; }
        void movabsq_to_rcx(uint64_t adr) {
            MAJ_APP = 0x48;
            MAJ_APP = 0xb9;
            MAJ_APP_64(adr);
        }
        uint8_t sizeof_mov_to_rcx_adr() { return 3; }
        void mov_to_rcx_adr(REG64BIT reg) {
            MAJ_APP = 0x48;
            MAJ_APP = 0x89;
            switch (reg) {
                case RAX: MAJ_APP = 0x01; break;
                case RBX: MAJ_APP = 0x19; break;
                case RCX: MAJ_APP = 0x09; break;
                case RDX: MAJ_APP = 0x0a; break;
            }
        }
        void db(std::initializer_list<uint8_t> il) {
            for (auto& i : il) MAJ_APP_8(i)
        }
        void dw(std::initializer_list<uint16_t> il) {
            for (auto& i : il) MAJ_APP_16(i)
        }
        void dd(std::initializer_list<uint32_t> il) {
            for (auto& i : il) MAJ_APP_32(i)
        }
        void dq(std::initializer_list<uint64_t> il) {
            for (auto& i : il) MAJ_APP_64(i)
        }

        void run(bool addRet = true) {
            if (addRet) MAJ_APP = 0xc3;// ret, MUSTN'T lose it!!! Otherwise, you will not be able to return to the place where the machine code is called
            auto func = (JIT_FUNC) machineCodeAdr;
            func();
            asm("nop");
        }
        template <class Func>
        decltype(auto) run_as(bool addRet = true) {
            if (addRet) MAJ_APP = 0xc3;
            auto func = (Func*)machineCodeAdr;
            return func();
        }
        template <class Func, class ...Args>
        decltype(auto) run_as(bool addRet = true, Args&& ...args) {
            if (addRet) MAJ_APP = 0xc3;
            auto func = (Func*)machineCodeAdr;
            return func(std::forward<Args>(args)...);
        }
        void runAI() {// AI: add index
            auto func = (JIT_FUNC) (machineCodeAdr + machineCodeIndex);
            func();
        }
        template <class Func>
        decltype(auto) runAI_as() {
            auto func = (Func*)(machineCodeAdr + machineCodeIndex);
            return func();
        }
        template <class Func, class ...Args>
        decltype(auto) runAI_as(Args&& ...args) {
            auto func = (Func*)(machineCodeAdr + machineCodeIndex);
            return func(std::forward<Args>(args)...);
        }
        void clear() {
            machineCodeIndex = 0;
        }
        void setIndex(size_t ci) {
            machineCodeIndex = ci;
        }
        size_t getIndex() const {
            return machineCodeIndex;
        }
        void resize(size_t size) {
            auto tmp = machineCodeAdr;
#ifdef I_OS_WIN32
            machineCodeAdr = (uint8_t*)VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#else
            machineCodeAdr = (uint8_t *) mmap(
                    NULL,
                    size,
                    PROT_READ | PROT_WRITE | PROT_EXEC,
                    MAP_ANONYMOUS | MAP_PRIVATE,
                    0,
                    0);
#endif
            memcpy(machineCodeAdr, tmp, codeLength);
#ifdef I_OS_WIN32
            VirtualFree(tmp, 0, MEM_RELEASE);
#else
            munmap(tmp, codeLength);
#endif
            codeLength = size;
        }
        void reserve(size_t size) {
#ifdef I_OS_WIN32
            VirtualFree(machineCodeAdr, 0, MEM_RELEASE);
            machineCodeAdr = (uint8_t*)VirtualAlloc(NULL, codeLength = size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#else
            munmap(machineCodeAdr, codeLength);
            machineCodeAdr = (uint8_t *) mmap(
                    NULL,
                    codeLength = size,
                    PROT_READ | PROT_WRITE | PROT_EXEC,
                    MAP_ANONYMOUS | MAP_PRIVATE,
                    0,
                    0);
#endif
            machineCodeIndex = 0;
        }
        ~MAsmJit() {
#ifdef I_OS_WIN32
            VirtualFree(machineCodeAdr, 0, MEM_RELEASE);
#else
            munmap(machineCodeAdr, codeLength);
#endif
        }
    private:
        size_t machineCodeIndex;
        size_t codeLength;
        uint8_t *machineCodeAdr;
    };
}


#endif //MYASMJIT_MASMJIT_H
