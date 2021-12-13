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

namespace P2PFileSync::ServerKit {

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
                          const std::filesystem::path& file, bool force_ssl = true);

/**
 * @brief Perfrom HTTP GET for giving address and post data and return raw ptr
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
void* GET_and_save_to_ptr(CURLSH* curl_handler, const std::string& request_url,
                          const std::vector<std::string>& header, bool force_ssl);

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
void* POST_and_save_to_ptr(CURLSH* curl_handler, const std::string& request_url,
                           const void* post_data, bool is_json, bool force_ssl);

// TODO write document later
inline bool get_data_from_server(const std::string& http_method, const void* input_data,
                                 const void* post_data, bool json_string, CURLSH* curl_share,
                                 const std::string& request_url,
                                 const std::vector<std::string>& header,
                                 size_t (*write_function)(char*, size_t, size_t, void*),
                                 bool force_ssl);

};  // namespace P2PFileSync::ServerKit

#endif  // P2P_FILE_SYNC_SERVER_KIT_SERVER_KIT_UTILS_CURL_UTILS_H