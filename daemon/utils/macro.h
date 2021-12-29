#ifndef P2P_FILE_SYNC_SERVER_KIT_EXPORT_H
#define P2P_FILE_SYNC_SERVER_KIT_EXPORT_H

#ifndef __has_attribute         // Optional of course.
  #define __has_attribute(x) 0  // Compatibility with non-clang compilers.
#endif

#if defined(__MSVC__)
  #define EXPORT_FUNC __declspec(dllexport)
  #define ALWAYS_INLINE __always_inline
#elif defined(__GNUC__) || defined(__clang__)
  #define EXPORT_FUNC __attribute__((visibility("default")))

  // always inline
  #if __has_attribute(always_inline)
    #define ALWAYS_INLINE __attribute__((always_inline))
  #else
    #define ALWAYS_INLINE
  #endif
#endif

#endif // P2P_FILE_SYNC_SERVER_KIT_EXPORT_H