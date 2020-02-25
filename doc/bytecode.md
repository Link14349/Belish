# Bytecode

| op id | opname | format | describe |
|----|----|----|----|
| 00 | push_num | push_num <double> | push a number into stack |
| 01 | push_str | push_str <string> | push an object into stack |
| 02 | push_obj | push_obj | push an object into stack |
| 03 | push_null | push_null | push a null value into stack |
| 04 | push_und | push_und | push a undefined value into stack |
| 05 | refer | refer <offset> | Replace the value in the <offset> address in the stack with the value at the top of the stack |
| 06 | deb | deb | Display all the values in the current stack |
| 07 | push | push <offset> | Push the value in the <offset> to the top of stack |
| 08 | add | add | The result of adding two elements at the top of stack |
| 09 | sub | sub | The result of subtraction of two elements at the top of stack |
| 0a | mul | mul | The result of multiplication two elements at the top of stack |
| 0b | div | div | The result of division two elements at the top of stack |
| 0c | mod | mod | The result of moduli two elements at the top of stack |
| 0d | lsh | lsh | The result of left-shift two elements at the top of stack |
| 0e | rsh | rsh | The result of right-shift two elements at the top of stack |
| 0f | eq | eq | The result of equal two elements at the top of stack |
| 10 | neq | neq | The result of n-equal two elements at the top of stack |
| 11 | leq | leq | The result of l-equal two elements at the top of stack |
| 12 | meq | meq | The result of m-equal two elements at the top of stack |
| 13 | less | less | The result of less two elements at the top of stack |
| 14 | more | more | The result of more two elements at the top of stack |
| 15 | mand | momandre | The result of mand two elements at the top of stack |
| 16 | mor | mor | The result of mor two elements at the top of stack |
| 17 | mxor | mxor | The result of mxor two elements at the top of stack |
| 18 | mnot | mnot | The result of mnot two elements at the top of stack |
| 19 | land | land | The result of land two elements at the top of stack |
| 1a | lor | lor | The result of lor two elements at the top of stack |
| 1b | lnot | lnot | The result of lnot two elements at the top of stack |
| 1c | pow | pow | The result of pow two elements at the top of stack |
| 1d | mov | mov | Move the element at the top of stack into the second element of stack |
| 1e | pop | pop | Back 1 |
| 1f | popc | popc <unsigned int> | Back <unsigned int> |
| 20 | jt | jt <unsigned int> | Jump to <unsigned int> if the top is true |
| 21 | jf | jf <unsigned int> | Jump to <unsigned int> if the top is false |
| 22 | jmp | jmp <unsigned int> | Jump to <unsigned int> |
| 23 | sav | sav | Cache the value (copy) at the top of the stack into the register |
| 24 | bac | bac | Fetches cached values from registers back to the stack |
| 25 | set_attr | set_attr | Assign the first element in the stack to the attribute(the second element in the stack) of the third element in the stack |
| 26 | get_attr | get_attr | Get the attribute(the first element in the stack) of the second element in the stack |
| 27 | new_frame | new_frame <unsigned int> | Create a new frame and move the first n items in the original frame to the new frame |
| 28 | call | call <unsigned int> | Call function with index n |
| 29 | ret | ret | Return |
| 2a | change | change <unsigned int> | Move the stack top element to <unsigned int> |
| 2b | push_fun | push_fun <unsigned int> | Stack the function with number <unsigned int> |
| 2c | call_fun | call_fun | Call the function at the top of the stack |
| 2d | imp | imp | Import the module(the path is saved at the top of the stack) |
| 2e | load | load | Load the library(the path is saved at the top of the stack) |
| 2f | reg_eq | reg_eq <byte>, <double> | Compare the value in the register with the second parameter |
| 30 | reg_neq | reg_neq <byte>, <double> | Compare the value in the register with the second parameter |
| 31 | reg_leq | reg_leq <byte>, <double> | Compare the value in the register with the second parameter |
| 32 | reg_meq | reg_meq <byte>, <double> | Compare the value in the register with the second parameter |
| 32 | reg_less | reg_less <byte>, <double> | Compare the value in the register with the second parameter |
| 32 | reg_more | reg_more <byte>, <double> | Compare the value in the register with the second parameter |
| 33 | reg_add | reg_add <byte>, <double> | Add the value in the register to the second parameter |
| 34 | reg_sub | reg_sub <byte>, <double> | Sub the value in the register to the second parameter |
| 35 | push_true | push_true | Push a value: true |
| 36 | push_false | push_true | Push a value: false |
| ff | LINE | LINE <unsigned int> | Set the line of the source |

## Formats
- \<double\>: 64bits(is the storage method of C++ double precision floating-point number)
- \<string\>: <Arbitrarily-Length>bits format: 00 \<content\>
- \<offset\>: 32bits
- \<unsigned int\>: 32bits

## File Formats
- Magic code(4bytes)
- Main BDK(Belish Development Kits) version
- Sub BDK(Belish Development Kits) version
- Last edit time(8bytes)

## Versions (1byte + 1byte)
### Main BDK(Belish Development Kits) version
- 0x01 v1
### Sub BDK(Belish Development Kits) version
- 0x01 0.1-beta

## File Contents
```text
MAIN:
[magic-code: 0x9ad0755c]
header {
    [version: Main BDK]
    [version: SUB BDK]
    [timestamp: Last edit time]
    [address: Footer address]
}
body {
    [opcode] [argv]
}
footer {
    [array<[address: function addresses]>: function list]
    {function} -> [0:]
}

INTRUDE:
magic-code: 4
version: 1
timestamp: 8
opcode: 1
argv: [0:]
array<size>: length + {[size]...} -> [length] [0:]
address: 4
length: 4
index: 4
string: array
bytecode: [0:]
function: {
    [bytecode: body]
}

```