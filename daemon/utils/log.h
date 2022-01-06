/*
 * @Author: Hanze Chen 
 * @Date: 2021-08-27 20:36:46 
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-08-29 19:54:45
 */
#ifndef P2P_FILE_SYNC_UTILS_LOG_H
#define P2P_FILE_SYNC_UTILS_LOG_H
#include <glog/logging.h>

namespace P2PFileSync {
   const static int ERROR = 0;
   const static int WARRING = 1;
   const static int INFO = 2;
   const static int VERBOSE = 2;

} // P2PFileSync
#endif