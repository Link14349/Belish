#ifndef CPPMODULE_LIBRARY_H
#define CPPMODULE_LIBRARY_H

#include <Belish.h>

Belish::Value* print(Belish::Stack*);
Belish::Value* println(Belish::Stack*);
Belish::Value* writeFile(Belish::Stack* argv);
Belish::Value* readFile(Belish::Stack* argv);
Belish::Value* appFile(Belish::Stack* argv);
MODULE_SETUP_DEC;

#endif //CPPMODULE_LIBRARY_H