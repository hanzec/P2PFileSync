#ifndef P2P_FILE_SYNC_SERVER_KIT_EXPORT_H
#define P2P_FILE_SYNC_SERVER_KIT_EXPORT_H

#if defined(__MSVC__)
  #define EXPORT_FUNC __declspec(dllexport)
#elif defined(__GNUC__) || defined(__clang__)
  #define EXPORT_FUNC __attribute__((visibility("default")))
#endif

#endif // P2P_FILE_SYNC_SERVER_KIT_EXPORT_H