# myasmjit
_A lightweight machine code dynamic executor written in C ++ based on x86 platform_

# Platform supportability
- CPU
    + x86 platform
- OS
    + OSX
    + Linux
    + Windows

# Usage
```cpp
#include <MAsmJit.h>// Import MAsmJit

int main(int argc, char *argv[]) {
// Create a asm executor
    MAsmJit::MAsmJit mAsmJit;
    // Write instructions
    mAsmJit.movq(MAsmJit::RAX, 0x0fedcba987654321);
    mAsmJit.movq(MAsmJit::RBX, 0x123456780abcdef0);
    mAsmJit.movq(MAsmJit::RCX, 0x123456780abcdef0);
    mAsmJit.movq(MAsmJit::RDX, 0x123456780abcdef0);
    // Run the machine code that has been written
    mAsmJit.run();
    // Delete the written machine code
    mAsmJit.clear();
    // Define a sequence of instructions in bytes
    mAsmJit.db({ 0x48, 0x89, 0xd8 });
    mAsmJit.run();
    return 0;
}
```