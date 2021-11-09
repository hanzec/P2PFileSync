#ifndef P2P_FILE_SYNC_Serverkit_MODEL_JSON_MODEL_H
#define P2P_FILE_SYNC_Serverkit_MODEL_JSON_MODEL_H

#include <glog/logging.h>
#include <rapidjson/allocators.h>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "../export.h"

namespace P2PFileSync::Serverkit {
/**
 * @brief base model of request/response model which used in server
 * communication
 * @note for any raw pointer give to this object as argument, this object will
 * take over the management
 */
class IJsonModel {
 public:
  /**
   * @brief Construct a new IJsonModel object
   *
   */
  IJsonModel() { document.SetObject(); };

  /**
   * @brief Construct a new IJsonModel object
   *
   * @param json
   */
  IJsonModel(char* json) : json_buf(json) {
    document.Parse(json);

    // check if is response model or not
    _response_flag =
        document.HasMember("success") && document.HasMember("responseBody");

    // debug message
    if (VLOG_IS_ON(3)) {
      VLOG(3) << "parsed new JSON :" << std::endl << get_json();
    }
  }

  /**
   * @brief Construct a new IJsonModel object
   *
   * @param json
   */
  IJsonModel(const std::filesystem::path& json_file) {
#ifdef UNDER_UNIX
    FILE* fp = fopen(json_file.c_str(), "rb");  // non-Windows use "r"
#elifdef UNDER_WINDOWS
    FILE* fp = fopen(json_file.c_str(), "r");  // non-Windows use "r"
#endif
    std::array<char, 1024> readBuffer;
    rapidjson::FileReadStream is(fp, readBuffer.begin(), readBuffer.size());

    document.ParseStream(is);
  }

  ~IJsonModel() {
    if (json_buf) free(json_buf);
  }

  /**
   * @brief Get the parsed json object
   *
   * @return std::string the parsed object
   */
  std::string get_json() {
    rapidjson::StringBuffer strBuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);
    document.Accept(writer);

    return strBuf.GetString();
  };

  /**
   * @brief Write current json object to file
   *
   * @param file the path of the file
   */
  void save_to_disk(const std::filesystem::path& file) {
#ifdef UNDER_UNIX
    FILE* fp = fopen(file.c_str(), "wb");  // non-Windows use "w"
#elifdef UNDER_WINDOWS
    FILE* fp = fopen(file.c_str(), "w");       // non-Windows use "w"
#endif

    std::array<char, 1024> writeBuffer;
    rapidjson::FileWriteStream os(fp, writeBuffer.begin(), writeBuffer.size());

    rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
    document.Accept(writer);
    fclose(fp);
  }

  /**
   * @brief Return the response return a server error or not
   * @note this function only return meaningful return value only when this json
   * object are convert from raw json and as an reponse model
   * @return true the response contains a success response
   * @return false the response does not contains a server error
   */
  bool success() {
    if (_response_flag) {
      return document["success"].GetBool();
    } else {
      return false;
    }
  }

 protected:
  /**
   * @brief Get the value object
   * @note by defult if key is not exist then this fuction will return nullptr
   * for object and 0/0.0 or nurmical values. And fot string variables, this
   * function will return an empty string contains NOTHINS which size=0
   * @tparam T the type of the values saved to json document
   * @param key the key of the json object
   * @return T the value of key in the json document
   */
  template <typename T>
  T get_value(const char* key) {
    auto& root_node = document["responseBody"];

    if (!_response_flag) {
      auto& root_node = document;
    }

    if (root_node.HasMember(key)) {
      if (!std::is_same_v<T, std::string>) {
        return root_node[root_node].Get<T>();
      } else {
        return root_node[root_node].GetString();
      }
    } else {
      return 0;
    }
  }

  /**
   * @brief Get the list of value
   * @note by default if the key is not exist then will return an empty vector
   * contains nothing
   * @note this function will skip any sub-nodes with array-like type(map or array)
   * @tparam T the datatype of vector
   * @param key key of the array
   * @return std::unordered_map<std::string,T> map of list value with key-value pair
   */
  template <typename T>
  std::unordered_map<std::string,T> get_map(const char* key) {
    std::unordered_map<std::string,T> ret;
    auto& root_node = document["responseBody"];

    if (!_response_flag) {
      auto& root_node = document;
    }

    if (root_node.HasMember(key)) {
      for (auto const& in : root_node[key].GetArray()) {
        // skip if current node is a list
        if (in.IsArray()) break;

        // add key and values to return
        auto key = in.MemberBegin()->name.GetString();
        if (!std::is_same_v<T, std::string>) {
          ret.emplace_back(key, in.Get<T>());
        } else {
          ret.emplace_back(key, in.GetString());
        }
      }
    } else {
      return {};
    }
  }

  /**
   * @brief Get the list of value
   * @note by default if the key is not exist then will return an empty vector
   * contains nothing
   * @note this function will skip any sub-nodes with array-like type(map or array)
   * @tparam T the datatype of vector
   * @param key key of the array
   * @return std::vector<T> vector of list value
   */
  template <typename T>
  std::vector<T> get_array(const char* key) {
    std::vector<T> ret;
    auto& root_node = document["responseBody"];

    if (!_response_flag) {
      auto& root_node = document;
    }

    if (root_node.HasMember(key)) {
      for (auto const& in : root_node[key].GetArray()) {
        // skip if current node is a list
        if (in.IsArray()) break;

        // add key and values to return
        if (!std::is_same_v<T, std::string>) {
          ret.emplace_back(in.Get<T>());
        } else {
          ret.emplace_back(in.GetString());
        }
      }
    } else {
      return {};
    }
  }

  template <typename T>
  void add_value(const char* name, const T& number) {
    rapidjson::Value value;
    rapidjson::Value value_name(rapidjson::kStringType);

    if (!std::is_same_v<T, std::string>) {
      document.AddMember(value_name.SetString(name, strlen(name)),
                         value.Set(number), document.GetAllocator());
    } else {
      rapidjson::Value string_value(rapidjson::kStringType);
      document.AddMember(value_name.SetString(name, strlen(name)),
                         string_value.SetString(number.c_str(), number.size(),
                                                document.GetAllocator()),
                         document.GetAllocator());
    }
  }

  template <typename T>
  void add_array(const std::string& name, const std::vector<T>& array) {
    rapidjson::Value json_array(rapidjson::kArrayType);
    rapidjson::Value array_name(rapidjson::kStringType);

    // adding variables to json
    for (auto item : array) {
      json_array.PushBack(item, document.GetAllocator());
    }

    // add array to document
    rapidjson::Value value_name(rapidjson::kStringType);
    document.AddMember(value_name.SetString(name.c_str(), name.size()),
                       json_array, document.GetAllocator());
  }

 private:
  /**
   * Indicated this model is an response model or request model
   */
  bool _response_flag = false;

  /**
   * The json buffer will later free when this object is destroy
   */
  char* json_buf = nullptr;

  /**
   * The document object where contains the json object
   */
  rapidjson::Document document;
};
}  // namespace P2PFileSync::Serverkit
#endif  // P2P_FILE_SYNC_Serverkit_MODEL_JSON_MODEL_H