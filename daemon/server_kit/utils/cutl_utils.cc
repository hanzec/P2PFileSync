#include <curl/curl.h>
#include <glog/logging.h>

#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>

#include "curl_utils.h"

namespace P2PFileSync::ServerKit {

using ptr_data = struct ptr_data_t {
  void* data;
  size_t head;
  size_t current_size;
};

size_t write_to_ptr(char* ptr, size_t size, size_t nmemb, void* userdata) {
  ptr_data* file_ptr;
  size_t block_size = nmemb * size;

  // has to be had a type of ptr_data
  if ((file_ptr = static_cast<ptr_data*>(userdata)) != nullptr) {
    // check if need to do relocate
    if (file_ptr->current_size < file_ptr->head + block_size) {
      LOG(INFO) << "relocate ptr: [" << std::hex << file_ptr->data << "]";
      void* new_ptr = realloc(file_ptr->data, file_ptr->current_size * 2);

      // handled when malloc is failed
      if (new_ptr == nullptr) {
        free(file_ptr->data);
        return 0;
      } else {
        file_ptr->data = new_ptr;
        file_ptr->current_size *= 2;
      }
    }

    // write data to ptr
    memcpy((char*)file_ptr->data + file_ptr->head, ptr, block_size);
    file_ptr->head += block_size;

    return block_size;
  }

  // type cast failed return failed
  return 0;
}

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

bool GET_and_save_to_path(CURLSH* curl_handler, const std::string& request_url,
                          const std::filesystem::path& file, bool force_ssl) {
  std::ofstream file_stream(file, std::ios::out | std::ios::binary);

  // preform downloading
  bool ret = get_data_from_server("GET", &file_stream, nullptr, false, curl_handler,
                                  request_url, {}, write_to_disk, force_ssl);

  file_stream.close();

  if (!ret) {
    std::filesystem::remove(file);
  }

  return ret;
}

void* POST_and_save_to_ptr(CURLSH* curl_handler, const std::string& request_url,
                           const void* post_data, bool is_json, bool force_ssl) {
  auto* data = static_cast<ptr_data*>(malloc(sizeof(ptr_data)));

  // assign spaces
  data->head = 0;
  data->current_size = 1024;
  data->data = malloc(1024);

  if (get_data_from_server("POST", data, post_data, is_json, curl_handler, request_url, {},
                           write_to_ptr, force_ssl)) {
    VLOG(3) << "total of [" << data->head << "] bytes downloaded!";
    auto ret = data->data;
    free(data);
    return ret;
  } else {
    free(data);
    return nullptr;
  }
}

void* GET_and_save_to_ptr(CURLSH* curl_handler, const std::string& request_url,
                          const std::vector<std::string>& header, bool force_ssl) {
  auto* data = static_cast<ptr_data*>(malloc(sizeof(ptr_data)));

  // assign spaces
  data->data = malloc(1024 * 1024);
  data->head = 0;
  data->current_size = 1024 * 1024;

  if (get_data_from_server("GET", data, nullptr, false, curl_handler, request_url, header,
                           write_to_ptr, force_ssl)) {
    if (VLOG_IS_ON(3)) VLOG(3) << "total of [" << data->head << "] bytes downloaded!";
    auto ret = data->data;
    static_cast<char*>(ret)[data->head] = '\0';
    free(data);
    return ret;
  } else {
    free(data);
    return nullptr;
  }
}

inline bool get_data_from_server(const std::string& http_method, const void* input_data,
                                 const void* post_data, bool json_string, CURLSH* curl_share,
                                 const std::string& request_url,
                                 const std::vector<std::string>& header,
                                 size_t (*write_function)(char*, size_t, size_t, void*),
                                 bool force_ssl) {
  if (VLOG_IS_ON(3))
    LOG(INFO) << "seeding request to [" << request_url << "] with " << http_method
              << " request";

  CURLcode ret;
  CURL* curl = curl_easy_init();
  struct curl_slist* list = nullptr;

  if (curl) {
    if (curl_share != nullptr) {
      /**
       * Had to set following two lines in order to keep cookies remain after redirect
       */
      curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "");
      curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
      if (CURLE_OK != (ret = curl_easy_setopt(curl, CURLOPT_SHARE, curl_share))) goto error;
    }

    // set post header
    if (post_data != nullptr) {
      if (json_string) {
        // by default all post_data are type of json in server kit
        list = curl_slist_append(list, "Content-Type:application/json");
        if ((ret = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list)) != CURLE_OK) goto error;
      } else{
        if ((ret = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list)) != CURLE_OK) goto error;
      }

      if ((ret = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data)) != CURLE_OK)
        goto error;
    }

    // set up http method
    // todo need to switch to enum switch later
    if (http_method == "POST") {
      if ((ret = curl_easy_setopt(curl, CURLOPT_POST, 1L)) != CURLE_OK) goto error;
    } else if (http_method == "GET") {
      if ((ret = curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L)) != CURLE_OK) goto error;
    } else {
      LOG(ERROR) << "unsupported http method [" << http_method << "]";
      goto error;
    }

    // add headers to curl request
    if (!header.empty()) {
      for (const auto& line : header) {
        list = curl_slist_append(list, line.c_str());
      }
      if ((ret = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list)) != CURLE_OK) goto error;
    }

    if ((ret = curl_easy_setopt(curl, CURLOPT_URL, request_url.c_str())) != CURLE_OK)
      goto error;

    if ((ret = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function)) != CURLE_OK)
      goto error;

    if ((ret = curl_easy_setopt(curl, CURLOPT_WRITEDATA, input_data)) != CURLE_OK) goto error;

    // ALLOW_REDIRECTS
    if ((ret = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L)) != CURLE_OK) goto error;

    /* Switch on full packet/debug output */
    if (VLOG_IS_ON(3)) curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    // SSL configuration
    if ((ret = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, force_ssl)) != CURLE_OK)
      goto error;
    if ((ret = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, force_ssl)) != CURLE_OK)
      goto error;
    if ((ret = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, force_ssl)) != CURLE_OK)
      goto error;

    if ((ret = curl_easy_perform(curl)) != CURLE_OK) {
      goto error;
    }
  } else {
    LOG(ERROR) << "failed to init curl struct";
    return false;
  }

  return true;

// error handling
error:
  curl_easy_cleanup(curl);
  LOG(ERROR) << curl_easy_strerror(ret);
  return false;
};

};  // namespace P2PFileSync::ServerKit