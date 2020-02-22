#ifdef I_OS_WIN32
// windows
#include<wtypes.h>
#include <winbase.h>
#else
// linux, mac, unix等
#include <dlfcn.h>
#endif
#include "dylib.h"

bool Belish::Dylib::open() {
    if (dyhandle) close();
#ifdef I_OS_WIN32
    // windows
    dyhandle = LoadLibrary((name).c_str());
#else
    dyhandle = dlopen((name).c_str(), RTLD_NOW);
#endif
    return (status = (bool)dyhandle);
}
void* Belish::Dylib::resolve(const string& sym) {
#ifdef I_OS_WIN32
    // windows
    return (void*)GetProcAddress(dyhandle, sym.c_str());
#else
    // linux, mac, unix等
    return dlsym(dyhandle, sym.c_str());
#endif
}
void Belish::Dylib::close() {
#ifdef I_OS_WIN32
    // windows
    FreeLibrary(dyhandle);
#else
    // linux, mac, unix等
    dlclose(dyhandle);
#endif
    status = false;
}