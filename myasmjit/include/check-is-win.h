#ifndef MYASMJIT_CHECK_IS_WIN_H
#define MYASMJIT_CHECK_IS_WIN_H

#if !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  if defined(WINAPI_FAMILY)
#    ifndef WINAPI_FAMILY_PC_APP
#      define WINAPI_FAMILY_PC_APP WINAPI_FAMILY_APP
#    endif
#    if defined(WINAPI_FAMILY_PHONE_APP) && WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP
#      define I_OS_WINRT
#    elif WINAPI_FAMILY==WINAPI_FAMILY_PC_APP
#      define I_OS_WINRT
#    else
#      define I_OS_WIN32
#    endif
#  else
#    define I_OS_WIN32
#  endif
#endif

#endif //MYASMJIT_CHECK_IS_WIN_H
