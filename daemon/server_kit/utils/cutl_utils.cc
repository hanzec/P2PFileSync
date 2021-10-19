#include <curl/curl.h>
#include <glog/logging.h>

#include <cerrno>
#include <cstddef>
#include <filesystem>
#include <fstream>

#include "curl_utils.hpp"

namespace P2PFileSync::Server_kit {

size_t write_to_disk(char* ptr, size_t size, size_t nmemb, void* userdata) {
  std::ofstream* file_stream;

  // has to be have a type of ofstream
  if ((file_stream = static_cast<std::ofstream*>(userdata)) != nullptr) {
    // write file
    size_t nbyte = size * nmemb;
    file_stream->write(ptr, nbyte);
    return nbyte;
  }
  // return
  return 0;
}

Status GET_and_save_to_path(CURLSH* curl_handler,
                            const std::string& request_url,
                            const std::filesystem::path& file, bool force_ssl) {
  std::ofstream file_stream(file, std::ios::out | std::ios::binary);

  // preform downloading
  auto ret = get_file_from_server(&file_stream, curl_handler,  request_url,
                                  write_to_disk, force_ssl);

  file_stream.close();

  if (!ret.ok()) {
    std::filesystem::remove(file);
  }

  return ret;
}

Status get_file_from_server(
    void* input_data, CURLSH* curl_share, const std::string& request_url,
    size_t (*write_function)(char*, size_t, size_t, void*), bool force_ssl) {
  CURLcode ret;
  CURL* curl = curl_easy_init();

  if (curl) {
    if (curl_share &&
        (ret = curl_easy_setopt(curl, CURLOPT_SHARE, curl_share)) != CURLE_OK)
      goto error;

    if ((ret = curl_easy_setopt(curl, CURLOPT_URL, request_url.c_str())) !=
        CURLE_OK)
      goto error;

    if ((ret = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET")) !=
        CURLE_OK)
      goto error;

    if ((ret = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function)) !=
        CURLE_OK)
      goto error;

    if ((ret = curl_easy_setopt(curl, CURLOPT_WRITEDATA, input_data)) !=
        CURLE_OK)
      goto error;

    // /* Switch on full protocol/debug output */
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    // SSL configuration
    if ((ret = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, force_ssl)) !=
        CURLE_OK)
      goto error;
    if ((ret = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, force_ssl)) !=
        CURLE_OK)
      goto error;
    if ((ret = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, force_ssl)) !=
        CURLE_OK)
      goto error;

    if ((ret = curl_easy_perform(curl)) != CURLE_OK) {
      goto error;
    }
  } else {
    return {StatusCode::INTERNAL, "Failed to initial curl struct"};
  }

  return Status::OK();

// error handling
error:
  // curl_easy_cleanup(curl);
  return {StatusCode::INTERNAL, curl_easy_strerror(ret)};
};


Status post_file_from_server(
    void* input_data, CURLSH* curl_share, const std::string& request_url,
    size_t (*write_function)(char*, size_t, size_t, void*), bool force_ssl) {
  CURLcode ret;
  CURL* curl = curl_easy_init();

  if (curl) {
    if (curl_share &&
        (ret = curl_easy_setopt(curl, CURLOPT_SHARE, curl_share)) != CURLE_OK)
      goto error;

    if ((ret = curl_easy_setopt(curl, CURLOPT_URL, request_url.c_str())) !=
        CURLE_OK)
      goto error;

    if ((ret = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET")) !=
        CURLE_OK)
      goto error;

    if ((ret = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function)) !=
        CURLE_OK)
      goto error;

    if ((ret = curl_easy_setopt(curl, CURLOPT_WRITEDATA, input_data)) !=
        CURLE_OK)
      goto error;

    // /* Switch on full protocol/debug output */
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    // SSL configuration
    if ((ret = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, force_ssl)) !=
        CURLE_OK)
      goto error;
    if ((ret = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, force_ssl)) !=
        CURLE_OK)
      goto error;
    if ((ret = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, force_ssl)) !=
        CURLE_OK)
      goto error;

    if ((ret = curl_easy_perform(curl)) != CURLE_OK) {
      goto error;
    }
  } else {
    return {StatusCode::INTERNAL, "Failed to initial curl struct"};
  }

  return Status::OK();

// error handling
error:
  // curl_easy_cleanup(curl);
  return {StatusCode::INTERNAL, curl_easy_strerror(ret)};
};

};  // namespace P2PFileSync::Server_kit