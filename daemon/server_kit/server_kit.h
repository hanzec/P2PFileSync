/*
 * @Author: Hanze CHen 
 * @Date: 2021-10-03 00:16:32 
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-10-03 00:17:15
 */
#ifndef P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H
#define P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H

#include <stddef.h>
#include <stdbool.h> 

#ifdef __cplusplus
extern "C" {
#endif

// -------------------------------------------- Return Structure --------------------------------------------


/**
 * @brief return structure for server management kit
 * 
 */
typedef struct P2P_SYNC_RETURN_t * P2P_SYNC_RETURN;

/**
 * @brief return if command is success returned or not
 * 
 * @param message return structure contain return code and return data
 * @return true 
 * @return false 
 */
bool P2PFileSync_SK_success(const P2P_SYNC_RETURN message);

/**
 * @brief return the return data if success or null if return contains error
 *
 * @param message return structure contain return code and return data
 * @return char* 
 */
char * P2PFileSync_SK_get_data(const P2P_SYNC_RETURN message);

/**
 * @brief get the error message of the giving P2P_SYNC_RETURN
 *  [[Note that the CALLER need to take responsible for free the return pointer]]
 * @param message return structure contain return code and return data
 * @param message_length  the length of the error message
 * @return char* error message in UTF-8
 */
char * P2PFileSync_SK_get_error_message(const P2P_SYNC_RETURN message, size_t * message_length);

// -------------------------------------------- Server Handler --------------------------------------------

/**
 * @brief return structure for server management kit
 * 
 */
typedef struct P2P_SYNC_SERVER_HANDLER_t P2P_SYNC_SERVER_HANDLER;

/**
 * @brief Create the P2P_SYNC_SERVER_HANDLER
 * 
 * @param server_url then url of management server
 * @param file_path the certificate path of ther server
 * @return P2P_SYNC_SERVER_HANDLER the handler of the server
 */
P2P_SYNC_SERVER_HANDLER * P2PFileSync_SK_init(const char * server_url, const char * file_path, const char * data_path);

// -------------------------------------------- Server Request --------------------------------------------

/**
 * @brief Register client when first start server or certificate lost and return new certificate
 * 
 * @param server_domain the server domain address for management server
 * @param file_path the PCSK#12 certificate location where will write to
 * @param return_link_size the size of returned activation link 
 * @return P2P_SYNC_RETURN return structure contain return code and return data
 */
P2P_SYNC_RETURN P2PFileSync_SK_register_client(P2P_SYNC_SERVER_HANDLER server_handler, size_t * return_link_size);

#ifdef __cplusplus
}
#endif
#endif //P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_H