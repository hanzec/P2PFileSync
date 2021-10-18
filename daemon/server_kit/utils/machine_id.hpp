#ifndef P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_UTILS_MACHINE_ID_H
#define P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_UTILS_MACHINE_ID_H
#include <string>
namespace P2PFileSync::Server_kit {

/**
 * @brief return the device id in string foramt
 * 
 * @return std::string the machine id generated
 */
std::string get_device_id();

}; // P2PFileSync::Server_kit
#endif // P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_UTILS_MACHINE_ID_H