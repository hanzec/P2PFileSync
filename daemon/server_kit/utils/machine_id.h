#ifndef P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_UTILS_MACHINE_ID_H
#define P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_UTILS_MACHINE_ID_H
#include <string>
namespace P2PFileSync::ServerKit {

/**
 * @brief return the device id in string foramt
 * 
 * @return std::string the machine id generated
 */
std::string get_device_id();

}; // P2PFileSync::ServerKit
#endif // P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_UTILS_MACHINE_ID_H