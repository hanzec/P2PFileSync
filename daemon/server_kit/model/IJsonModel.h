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
#include <map>
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
  IJsonModel() { root.SetObject(); };

  /**
   * @brief Construct a new IJsonModel object
   *
   * @param json
   */
  explicit IJsonModel(char* json) : json_buf(json) {
    root.Parse(json);

    // check if is response model or not
    _response_flag = root.HasMember("success") && root.HasMember("responseBody");

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
    std::array<char, 1024> readBuffer{};
    rapidjson::FileReadStream is(fp, readBuffer.begin(), readBuffer.size());

    root.ParseStream(is);
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
    root.Accept(writer);

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

    std::array<char, 1024> writeBuffer{};
    rapidjson::FileWriteStream os(fp, writeBuffer.begin(), writeBuffer.size());

    rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
    root.Accept(writer);
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
      return root["success"].GetBool();
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
    if (_response_flag ? root.HasMember(key) : root["responseBody"].HasMember(key)) {
      if constexpr (std::is_arithmetic<T>::value) {
        if (!_response_flag) {
          return root[key].Get<T>();
        } else {
          return root["responseBody"][key].Get<T>();
        }
      } else {
        if (!_response_flag) {
          return root[key].GetString();
        } else {
          return std::string(root["responseBody"][key].GetString());
        }
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
  std::map<std::string, T> get_map(const char* key) {
    auto new_key = std::string(key);
    std::map<std::string, T> ret;
    if (_response_flag ? root.HasMember(key) : root["responseBody"].HasMember(key)) {
      for (auto const& in :
           _response_flag ? root[key].GetArray() : root["responseBody"].GetArray()) {
        // skip if current node is a list
        if (in.IsArray()) break;

        // add string and values to return
        auto string = in.MemberBegin()->name.GetString();
        if constexpr (std::is_arithmetic<T>::value) {
          ret.insert(std::make_pair(string, in.Get<T>()));
        } else {
          ret.insert(std::make_pair(string, in.GetString()));
        }
      }
      return {};
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
    auto& root_node = root["responseBody"];

    if (!_response_flag) {
      auto& rootNode = root;
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
  void add_value(const char* key, const T& val) {
    rapidjson::Value value;
    rapidjson::Value value_name(rapidjson::kStringType);

    if constexpr(std::is_arithmetic<T>::value) {
      // handle_difficult numerical type
      root.AddMember(value_name.SetString(key, strlen(key)), value.Set(val),
                     root.GetAllocator());
    } else {
      // handle_difficult c++ std::string
      auto new_value = std::string(val);
      rapidjson::Value string_value(rapidjson::kStringType);
      root.AddMember(
          value_name.SetString(key, strlen(key)),
          string_value.SetString(new_value.c_str(), new_value.size(), root.GetAllocator()),
          root.GetAllocator());
    }
  }

  template <typename T>
  void add_array(const std::string& name, const std::vector<T>& array) {
    rapidjson::Value json_array(rapidjson::kArrayType);
    rapidjson::Value array_name(rapidjson::kStringType);

    // adding variables to json
    for (auto item : array) {
      json_array.PushBack(item, root.GetAllocator());
    }

    // add array to document
    rapidjson::Value value_name(rapidjson::kStringType);
    root.AddMember(value_name.SetString(name.c_str(), name.size()), json_array,
                   root.GetAllocator());
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
  rapidjson::Document root;
};
}  // namespace P2PFileSync::Serverkit
#endif  // P2P_FILE_SYNC_Serverkit_MODEL_JSON_MODEL_H