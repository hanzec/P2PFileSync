#include <curl/curl.h>
#include <glog/logging.h>

#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>

#include "curl_utils.h"

namespace P2PFileSync::Serverkit {

using ptr_data = struct ptr_data_t{
  void * data;
  size_t head;
  size_t current_size;
};

size_t write_to_ptr(char* ptr, size_t size, size_t nmemb, void* userdata) {
  ptr_data* file_ptr;
  size_t block_size = nmemb * size;

  // has to be have a type of ptr_data
  if ((file_ptr = static_cast<ptr_data*>(userdata)) != nullptr) {
    // check if need to do relocate
    if(file_ptr->current_size < file_ptr->head + block_size){
      void* new_ptr = realloc(file_ptr->data, file_ptr->current_size*2);
      
      // handled when realloc is failed
      if(new_ptr == nullptr){
        free(file_ptr->data);
        return 0;
      }else{
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

bool GET_and_save_to_path(CURLSH* curl_handler,
                          const std::string& request_url,
                          const std::filesystem::path& file, bool force_ssl) {
  std::ofstream file_stream(file, std::ios::out | std::ios::binary);

  // preform downloading
  bool ret = get_file_from_server("GET",&file_stream,nullptr, curl_handler,  request_url, {},
                                  write_to_disk, force_ssl);

  file_stream.close();

  if (!ret) {
    std::filesystem::remove(file);
  }

  return ret;
}

void * POST_and_save_to_ptr(CURLSH * curl_handler, const std::string& request_url, const void * post_data, bool force_ssl){
  auto * data = static_cast<ptr_data *>(malloc(sizeof(ptr_data)));
  
  //assign spaces
  data->data = malloc(1024*1024);
  data->head = 0;
  data->current_size = 1024*1024;

  if(get_file_from_server("POST", data, post_data, curl_handler, request_url, {}, write_to_ptr,force_ssl)){
    VLOG(3) << "total of [" << data->head << "] bytes downloaded!";
    auto ret = data->data;
    free(data);
    return ret;
  }else{
    free(data);
    return nullptr;
  }
}

void * GET_and_save_to_ptr(CURLSH * curl_handler, const std::string& request_url, std::vector<std::string> header, bool force_ssl){
  auto * data = static_cast<ptr_data *>(malloc(sizeof(ptr_data)));
  
  //assign spaces
  data->data = malloc(1024*1024);
  data->head = 0;
  data->current_size = 1024*1024;

  if(get_file_from_server("GET", data, nullptr, curl_handler, request_url,header, write_to_ptr,force_ssl)){
    VLOG(3) << "total of [" << data->head << "] bytes downloaded!";
    auto ret = data->data;
    free(data);
    return ret;
  }else{
    free(data);
    return nullptr;
  }
}

inline bool get_file_from_server(const std::string& http_method, const void* input_data, const void * post_data,
                          CURLSH* curl_share, const std::string& request_url, const std::vector<std::string>& header,
                          size_t (*write_function)(char*, size_t, size_t, void*), bool force_ssl) {
  
  VLOG(3) << "seeding request to [" << request_url << "] with " << http_method << " request";


  CURLcode ret;
  CURL* curl = curl_easy_init();
  struct curl_slist *list = nullptr;


  if (curl) {
    if (curl_share && (ret = curl_easy_setopt(curl, CURLOPT_SHARE, curl_share)) != CURLE_OK)
      goto error;

    // set post header
    if(post_data){
      // by default all post_data are type of json in server kit
      struct curl_slist *list = curl_slist_append(nullptr, "Content-Type:application/json");

      if((ret = curl_easy_setopt(curl, CURLOPT_HTTPHEADER , list)) != CURLE_OK)
        goto error;

      if((ret = curl_easy_setopt(curl, CURLOPT_POSTFIELDS , post_data)) != CURLE_OK)
        goto error;
    }

    // add headers to curl request
    if(header.size() != 0){
      for(const auto& line :header){
        list = curl_slist_append(list, line.c_str());
      }
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
    }

    if ((ret = curl_easy_setopt(curl, CURLOPT_URL, request_url.c_str())) !=
        CURLE_OK)
      goto error;

    if ((ret = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, http_method.c_str())) !=
        CURLE_OK)
      goto error;

    if ((ret = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function)) !=
        CURLE_OK)
      goto error;

    if ((ret = curl_easy_setopt(curl, CURLOPT_WRITEDATA, input_data)) !=
        CURLE_OK)
      goto error;

    // /* Switch on full packet/debug output */
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

};  // namespace P2PFileSync::Serverkit