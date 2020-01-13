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
| 07 | push | push <offset> | Display all the values in the current stack |
| 08 | add | add | The result of adding two elements at the top of stack |
| 09 | sub | sub | The result of subtraction of two elements at the top of stack |
| 0a | mul | mul | The result of multiplication two elements at the top of stack |
| 0b | div | div | The result of division two elements at the top of stack |
| 0c | mod | mod | The result of moduli two elements at the top of stack |

## Formats
- \<double\>: 64bits(is the storage method of C++ double precision floating-point number)
- \<str\>: <Arbitrarily-Length>bits format: 00 \<content\> 00
- \<offset\>: 32bits
