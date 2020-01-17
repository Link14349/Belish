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
| ff | LINE | LINE <unsigned int> | Set the line of the source |

## Formats
- \<double\>: 64bits(is the storage method of C++ double precision floating-point number)
- \<str\>: <Arbitrarily-Length>bits format: 00 \<content\> 00
- \<offset\>: 32bits
- \<unsigned int\>: 32bits

## File Formats
- Magic code
- Main BDK(Belish Development Kits) version
- Sub BDK(Belish Development Kits) version
- Last edit time(8bytes)

## Versions (1byte + 1byte)
### Main BDK(Belish Development Kits) version
- 0x00 v1
### Sub BDK(Belish Development Kits) version
- 0x00 0.1-beta
