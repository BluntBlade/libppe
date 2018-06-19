#ifndef PPE_MACROS_H
#define PPE_MACROS_H

#if defined(PPE_DEF_MSVC)

    #if defined(PPE_DEF_STATIC_LIBRARY)
        #define PPE_API
    #else
        #if defined(PPE_DEF_COMPILING)
            #define PPE_API __declspec(dllexport)
        #else
            #define PPE_API __declspec(dllimport)
        #endif
    #endif

    #define restrict __restrict

#else

    #define PPE_API

#endif

#endif /* PPE_MACROS_H */

