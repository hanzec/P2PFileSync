#ifndef P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_UTILS_CURL_UTILS_H
#define P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_UTILS_CURL_UTILS_H
#include <curl/curl.h>
#include <curl/easy.h>

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "functional"

namespace P2PFileSync::Server_kit {

/**
 * @brief Perform HTTP GET for giving address and saving the response body to
 * giving file
 * @note this function is intend for replace the old file
 *
 * @param curl_handler he shard curl structure if available, COULD BE NULLPTR if
 * not used
 * @param request_url  the request server url which want to sent GET request
 * @param file the file path where the response body will writes to
 * @param force_ssl the flag where enable SSL verify or not
 * @return true success download
 * @return false failed to download
 */
bool GET_and_save_to_path(CURLSH* curl_handler, const std::string& request_url,
                          const std::filesystem::path& file,
                          bool force_ssl = true);

/**
 * @brief Perfrom HTTP POST for giving address and post data and return raw ptr
 * @note the user need to take responsible for manage return prt including
 * free()
 *
 * @param curl_handler the shard curl structure if available, COULD BE NULLPTR
 * if not used
 * @param request_url the request server url which want to sent POST request
 * @param post_data the raw post data wants including in post request, COULD BE
 * NULLPTR
 * @param force_ssl the flag where enable SSL verify or not
 * @return void* the returned data in response body
 */
void * POST_and_save_to_ptr(CURLSH * curl_handler, const std::string& request_url, const void * post_data, bool force_ssl);

// TODO write later
/**
 * @brief curl warraper for general downloading from server
 *
 * @param http_method
 * @param input_data
 * @param curl_share
 * @param request_url
 * @param write_function
 * @param force_ssl
 * @return true
 * @return false
 */
inline bool get_file_from_server(const std::string& http_method, const void* input_data, const void * post_data,
                          CURLSH* curl_share, const std::string& request_url,
                          size_t (*write_function)(char*, size_t, size_t, void*), bool force_ssl);

};  // namespace P2PFileSync::Server_kit

#endif  // P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_UTILS_CURL_UTILS_H