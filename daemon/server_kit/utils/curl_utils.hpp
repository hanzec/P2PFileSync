#ifndef P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_UTILS_CURL_UTILS_H
#define P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_UTILS_CURL_UTILS_H
#include <curl/curl.h>
#include <curl/easy.h>
#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>
#include "functional"
#include <fstream>
#include <iostream>
#include "../../utils/status.h"

namespace P2PFileSync::Server_kit {

Status GET_and_save_to_path(CURLSH * curl_handler, 
                            const std::string& request_url,
                            const std::filesystem::path& file, 
                            bool force_ssl = true);

Status get_file_from_server(void * input_data,
                            CURLSH * curl_share, 
                            const std::string& request_url,
                            size_t (*write_function)(char *, size_t , size_t , void *),bool force_ssl = true);

}; // P2PFileSync::Server_kit

#endif // P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_UTILS_CURL_UTILS_H