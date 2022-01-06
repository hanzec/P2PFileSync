#ifndef P2P_FILE_SYNC_Serverkit_MODEL_JSON_MODEL_H
#define P2P_FILE_SYNC_Serverkit_MODEL_JSON_MODEL_H

#include <glog/logging.h>
#include <rapidjson/allocators.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <limits>
#include <map>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "utils/macro.h"
namespace P2PFileSync::ManagementAPI {
/**
 * @brief base model of request/response model which used in server
 * communication
 * @note for any raw pointer give to this object as argument, this object will
 * take over the management
 */
class JsonModel {
 public:
  /**
   * @brief Construct an empty JsonModel object
   *
   */
  JsonModel() noexcept { root.SetObject(); }

  /**
   * @brief Construct a new JsonModel object from existing model
   *
   * @param other
   */
  JsonModel(const JsonModel& other) noexcept {
    if (other.json_buf) {
      auto json_buf_size = strlen(other.json_buf);
      memcpy(json_buf, other.json_buf, json_buf_size);
    }
    root.CopyFrom(other.root, root.GetAllocator());
  };

  /**
   * @brief Construct a new JsonModel object from json string
   *
   * @param json the raw c-style string contain the json object
   */
  explicit JsonModel(char* json) noexcept : json_buf(json) {
    if (json_buf == nullptr) {
      root.SetObject();
      VLOG(1) << "failed to parse json because input char array is nullptr";
    } else {
      VLOG(3) << "parse json from: " << json_buf;
      rapidjson::ParseResult ret = root.Parse(json);

      if (!ret) {
        VLOG(1) << "parse json error: " << rapidjson::GetParseError_En(ret.Code()) << ":"
                << ret.Offset() << ":" << json;
        root.SetObject();
        return;
      }

      // check if is response model or not
      _response_flag = root.HasMember("success") && root.HasMember("responseBody");
    }
  }

  /**
   * @brief Construct a new JsonModel object from the disk
   *
   * @param json_file the file path of the json object
   */
  explicit JsonModel(const std::filesystem::path& json_file) noexcept {
    if (std::filesystem::exists(json_file)) {
#if defined(UNIX) or defined(MACOSX)
      FILE* fp = fopen(json_file.c_str(), "rb");
#elif defined(WIN32)
      FILE* fp = fopen(json_file.c_str(), "r");  // non-Windows use "r"
#endif
      std::array<char, 1024> readBuffer{};
      rapidjson::FileReadStream is(fp, readBuffer.begin(), readBuffer.size());

      VLOG(3) << "parse json from: " << json_buf;
      rapidjson::ParseResult ret = root.ParseStream(is);

      if (!ret) {
        VLOG(1) << "parse json error: " << rapidjson::GetParseError_En(ret.Code());
        root.SetObject();
        return;
      }

      // check if is response model or not
      _response_flag = root.HasMember("success") && root.HasMember("responseBody");
    } else {
      VLOG(1) << "failed to parse json because file not exist: " << json_file;
    }
  }

  /**
   * @brief Return the response return a server error or not
   * @note this function only return meaningful return value only when this json
   * object are convert from raw json and as an reponse model
   * @return true the response contains a success response
   * @return false the response does not contains a server error
   */
  bool success() noexcept {
    if (_response_flag) {
      return root["success"].GetBool();
    } else {
      return false;
    }
  }

  /**
   * @brief Get the server message from the response if existed
   *
   * @note will be empty string ("") if no message avaliable
   * @return std::string server response message
   */
  std::string message() noexcept {
    auto& node = _response_flag ? root["responseBody"] : root;
    if (node.HasMember("message")) {
      return node["message"].GetString();
    } else {
      return "";
    }
  }

  /**
   * @brief Get the parsed json object
   *
   * @return std::string the parsed string object
   */
  std::string get_json() noexcept {
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
  void save(const std::filesystem::path& file) noexcept {
    if (std::filesystem::exists(file)) {
      std::filesystem::remove(file);
      LOG(WARNING) << "file [" << file << "] already exists, remove it";
    }
    std::ofstream ofs(file);
    ofs << get_json();
  };

 protected:
  [[maybe_unused]] auto& get_raw_node(const char* key) {
    if (_response_flag ? root["responseBody"].HasMember(key) : root.HasMember(key)) {
      if (!_response_flag) {
        return root[key];
      } else {
        return root["responseBody"][key];
      }
    } else {
      LOG(ERROR) << "key [" << key << "] not found!";
      throw std::exception();
    }
  }

  /**
   * @brief Get the vector array of value object
   *
   * @param key the key of the json object
   * @tparam T the type of the values in array
   * @return std::optional<std::vector<T>> the list of value for the  key in the json document.
   *         If key not exist or T is not one of arithmetic type or std::string, method will
   *         return std::nullopt
   */
  template <typename T>
  std::vector<T> get_array(const char* key) const noexcept {
    if (_response_flag ? root["responseBody"].HasMember(key) : root.HasMember(key)) {
      auto& node = _response_flag ? root["responseBody"][key] : root[key];

      if (node.IsArray()) {
        std::vector<T> ret;
        for (auto& item : node.GetArray()) {
          if constexpr (std::is_arithmetic<T>::value) {  // is arithmetic type
            ret.emplace_back(item.Get<T>());
          } else if constexpr (std::is_same<T, std::string>::value) {  // is string
            ret.emplace_back(node.GetString());
          } else {
            VLOG(1) << "unsupported type : " << typeid(T).name() << "for key [" << key << "]";
            return {};
          }
        }
        return ret;
      } else {
        VLOG(1) << "key [" << key << "] is not array";
        return {};
      }
    } else {
      VLOG(1) << "key [" << key << "] not found!";
      return {};
    }
  }

  /**
   * @brief Get the value object
   *
   * @param key the key of the json object
   * @tparam T the type of the values saved to json document
   * @return std::optional<T> the value of key in the json document. If key not exist or T is
   *         not one of arithmetic type or std::string, method will return std::nullopt
   */
  template <typename T>
  std::optional<T> get_value(const char* key) const noexcept {
    if (_response_flag ? root["responseBody"].HasMember(key) : root.HasMember(key)) {
      auto& node = _response_flag ? root["responseBody"][key] : root[key];

      if constexpr (std::is_arithmetic<T>::value) {  // is arithmetic type
        return {node.Get<T>()};
      } else if constexpr (std::is_same<T, std::string>::value) {  // is string
        return std::string(node.GetString());
      } else {
        VLOG(1) << "unsupported type : " << typeid(T).name() << "for key [" << key << "]";
        return std::nullopt;
      }
    } else {
      return std::nullopt;
    }
  }

  /**
   * @brief Get the Key-Value Map of value object
   *
   * @note the value(T) only support arithmetic type or std::string
   * @note will not convert array-like object to map
   * @param key the key of the json object
   * @tparam T the type of the values in map
   * @return std::optional<std::map<std::string, T>> the map value for the key in the json
   *         document. If key not exist or T is not one of arithmetic type or std::string,
   *         method will return std::nullopt
   */
  template <typename T>
  std::map<std::string, T> get_map(const char* key) const noexcept {
    std::map<std::string, T> ret;
    if (_response_flag ? root["responseBody"].HasMember(key) : root.HasMember(key)) {
      auto& node = _response_flag ? root["responseBody"][key] : root[key];
      for (auto itr = node.MemberBegin(); itr != node.MemberEnd(); ++itr) {
        // skip if current node is a list
        if (itr->value.IsArray() || itr->value.IsObject()) {
          VLOG(3) << "skip array node: " << itr->name.GetString()
                  << " with type: " << itr->value.GetType();
          break;
        }

        if constexpr (std::is_arithmetic<T>::value) {
          ret.insert(std::make_pair(itr->name.GetString(), itr->value.Get<T>()));
        } else if constexpr (std::is_same<T, std::string>::value) {
          ret.insert(std::make_pair(itr->name.GetString(), itr->value.GetString()));
        } else {
          VLOG(1) << "unsupported type : " << typeid(T).name() << "for key [" << key << "]";
          return {};
        }
      }
      VLOG(3) << "get_map, size: " << ret.size();
      return ret;
    } else {
      LOG(WARNING) << "key [" << key << "] not found!";
      return {};
    }
  }

  /**
   * @brief save the value object to json document
   *
   * @param key the key of the object
   * @param value the value of the object
   * @tparam T the type of the values saved to json document
   * @return true if successful add to json document
   * @return false if failed to add to json document
   */
  template <typename T>
  bool add_value(const char* key, const T& val) {
    rapidjson::Value value;
    rapidjson::Value value_name(rapidjson::kStringType);

    // check existence of the key
    if (_response_flag ? root["responseBody"].HasMember(key) : root.HasMember(key)) {
      VLOG(1) << "key [" << key << "] already exists!";
      return false;
    }

    // add value to json
    if constexpr (std::is_arithmetic<T>::value) {
      // handle_difficult numerical type
      root.AddMember(value_name.SetString(key, strlen(key)), value.Set(val),
                     root.GetAllocator());
      return true;
    } else if constexpr (std::is_same<T, std::string>::value) {
      // handle_difficult c++ std::string
      auto new_value = std::string(val);
      rapidjson::Value string_value(rapidjson::kStringType);
      root.AddMember(
          value_name.SetString(key, strlen(key)),
          string_value.SetString(new_value.c_str(), new_value.size(), root.GetAllocator()),
          root.GetAllocator());
      return true;
    } else {
      VLOG(1) << "unsupported type : " << typeid(T).name() << "for key [" << key << "]";
      return false;
    }
  }

  /**
   * @brief save the array of value object to json document
   *
   * @param key the key of the array of object
   * @param array the value of the array of object
   * @tparam T the type of the values saved to json document
   * @return true if successful add to json document
   * @return false if failed to add to json document
   */
  template <typename T>
  bool add_array(const char* key, const std::vector<T>& array) {
    rapidjson::Value json_array(rapidjson::kArrayType);
    rapidjson::Value array_name(rapidjson::kStringType);

    // check existence of the key
    if (_response_flag ? root["responseBody"].HasMember(key) : root.HasMember(key)) {
      VLOG(1) << "key [" << key << "] already exists!";
      return false;
    }

    // adding variables to json
    for (auto item : array) {
      if constexpr (std::is_arithmetic<T>::value) {
        json_array.PushBack(item, root.GetAllocator());
      } else if constexpr (std::is_same<T, std::string>::value) {
        // handle_difficult c++ std::string
        auto new_value = std::string(item);
        rapidjson::Value string_value(rapidjson::kStringType);
        json_array.PushBack(
            string_value.SetString(new_value.c_str(), new_value.size(), root.GetAllocator()),
            root.GetAllocator());
      } else {
        VLOG(1) << "unsupported type : " << typeid(T).name() << "for key [" << key << "]";
        return false;
      }
    }

    // add array to document
    rapidjson::Value value_name(rapidjson::kStringType);
    root.AddMember(value_name.SetString(key, strlen(key)), json_array, root.GetAllocator());
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
}  // namespace P2PFileSync::ManagementAPI
#endif  // P2P_FILE_SYNC_Serverkit_MODEL_JSON_MODEL_H