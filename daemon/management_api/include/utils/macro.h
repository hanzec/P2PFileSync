#ifndef P2P_FILE_SYNC_UTILS_MACRO_H
#define P2P_FILE_SYNC_UTILS_MACRO_H

#ifndef __has_attribute       // Optional of course.
#define __has_attribute(x) 0  // Compatibility with non-clang compilers.
#endif

#if defined(__MSVC__)
  #define Management_KIT_EXPORT __declspec(dllexport)
  #define ALWAYS_INLINE __always_inline
  #define ALWAYS_INLINE __stdcall
#elif defined(__GNUC__) || defined(__clang__)

  // always inline
  #if __has_attribute(always_inline)
    #define ALWAYS_INLINE __attribute__((always_inline))
  #else
    #define ALWAYS_INLINE
  #endif

  // always inline
  #if __has_attribute(packed)
    #define ENUM_PACKED __attribute__((packed))
  #else
    #define ENUM_PACKED
  #endif

  // stdcall
  #if __has_attribute(stdcall)  && defined(WIN32)
    #define STD_CALL_CONV __attribute__((stdcall))
  #else
    #define STD_CALL_CONV
  #endif
#endif
#endif  // P2P_FILE_SYNC_SERVER_KIT_EXPORT_H