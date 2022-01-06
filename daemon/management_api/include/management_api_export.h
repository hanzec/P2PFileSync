
#ifndef MANAGEMENT_API_EXPORT_H
#define MANAGEMENT_API_EXPORT_H

#ifdef MANAGEMENT_API_STATIC_DEFINE
#  define MANAGEMENT_API_EXPORT
#  define MANAGEMENT_API_NO_EXPORT
#else
#  ifndef MANAGEMENT_API_EXPORT
#    ifdef management_api_EXPORTS
        /* We are building this library */
#      define MANAGEMENT_API_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define MANAGEMENT_API_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef MANAGEMENT_API_NO_EXPORT
#    if __has_attribute(always_inline)
#       define MANAGEMENT_API_NO_EXPORT __attribute__((visibility("hidden")))
#    else
#       define MANAGEMENT_API_NO_EXPORT
#    endif
#  endif
#endif

#ifndef MANAGEMENT_API_DEPRECATED
# if __has_attribute(deprecated)
#  define MANAGEMENT_API_DEPRECATED __attribute__ ((__deprecated__))
# else
#       define MANAGEMENT_API_DEPRECATED
#endif
#endif

#ifndef MANAGEMENT_API_DEPRECATED_EXPORT
#  define MANAGEMENT_API_DEPRECATED_EXPORT MANAGEMENT_API_EXPORT MANAGEMENT_API_DEPRECATED
#endif

#ifndef MANAGEMENT_API_DEPRECATED_NO_EXPORT
#  define MANAGEMENT_API_DEPRECATED_NO_EXPORT MANAGEMENT_API_NO_EXPORT MANAGEMENT_API_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef MANAGEMENT_API_NO_DEPRECATED
#    define MANAGEMENT_API_NO_DEPRECATED
#  endif
#endif

#endif /* MANAGEMENT_API_EXPORT_H */
