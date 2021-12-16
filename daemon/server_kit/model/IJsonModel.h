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
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <limits>
#include <map>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "../export.h"
namespace P2PFileSync::ServerKit {
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
   */
  IJsonModel(const IJsonModel& new_models) {
    if (new_models.json_buf) {
      auto json_buf_size = strlen(new_models.json_buf);
      memcpy(json_buf, new_models.json_buf, json_buf_size);
    }
    root.CopyFrom(new_models.root, root.GetAllocator());
  };

  /**
   * @brief Construct a new IJsonModel object
   *
   * @param json
   */
  explicit IJsonModel(char* json) : json_buf(json) {
    rapidjson::ParseResult ret = root.Parse(json);

    if (!ret) {
      LOG(ERROR) << "parse json error: " << rapidjson::GetParseError_En(ret.Code()) << ":"
                 << ret.Offset() << ":" << json;
      throw std::runtime_error("parse json error");
    }

    // check if is response model or not
    _response_flag = root.HasMember("success") && root.HasMember("responseBody");
  }

  /**
   * @brief Construct a new IJsonModel object
   *
   * @param json
   */
  explicit IJsonModel(const std::filesystem::path& json_file) {
#ifdef UNDER_UNIX
    FILE* fp = fopen(json_file.c_str(), "rb");  // non-Windows use "r"
#elifdef UNDER_WINDOWS
    FILE* fp = fopen(json_file.c_str(), "r");  // non-Windows use "r"
#endif
    std::array<char, 1024> readBuffer{};
    rapidjson::FileReadStream is(fp, readBuffer.begin(), readBuffer.size());

    root.ParseStream(is);
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
    if (std::filesystem::exists(file)) {
      std::filesystem::remove(file);
      LOG(WARNING) << "configuration file " << file << " already exists, remove it";
    }
    std::ofstream ofs(file);
    ofs << get_json();
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
   * @brief Get the root object
   *
   * @return rapidjson::Value& the root object
   */
  rapidjson::Document& get_root() { return root; };

  auto& get_raw_node(const char* key) const {
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

  // todo, an generate get method for any type

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
  T get_value(const char* key) const {
    if (_response_flag ? root["responseBody"].HasMember(key) : root.HasMember(key)) {
      auto& node = _response_flag ? root["responseBody"][key] : root[key];

      if constexpr (std::is_arithmetic<T>::value) {
        return node.Get<T>();
      } else if constexpr (std::is_same<T, std::string>::value) {
        return std::string(node.GetString());
      } else if constexpr (std::is_same<T, std::vector<T>>::value) {
        T ret;
        for (auto const& in : node.GetArray()) {
          // skip if current node is a list
          if (in.IsArray()) break;

          // add key and values to return
          if constexpr (std::is_arithmetic<T>::value) {
            ret.emplace_back(in.Get<T>());
          } else {
            ret.emplace_back(in.GetString());
          }
        }
        return ret;
      } else{
        LOG(ERROR) << "unsupported type";
        throw std::exception();
      }
    } else {
      if constexpr (std::is_arithmetic<T>::value) {
        return 0;
      } else if constexpr (std::is_same<T, std::vector<T>>::value) {
        return {};
      } else if constexpr (std::is_same<T, std::string>::value) {
        return std::string();
      } else {
        LOG(ERROR) << "unsupported type";
        throw std::exception();
      }
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
    std::map<std::string, T> ret;
    if (_response_flag ? root["responseBody"].HasMember(key) : root.HasMember(key)) {
      auto& node = _response_flag ? root["responseBody"][key] : root[key];
      for (auto itr = node.MemberBegin(); itr != node.MemberEnd(); ++itr) {
        // skip if current node is a list
        if (itr->value.IsArray() || itr->value.IsObject()) {
          VLOG(3) << "skip array node: " << itr->name.GetString()
                  << " with type: " << itr->value.GetType();
          break;
        };

        if constexpr (std::is_arithmetic<T>::value) {
          ret.insert(std::make_pair(itr->name.GetString(), itr->value.Get<T>()));
        } else {
          ret.insert(std::make_pair(itr->name.GetString(), itr->value.GetString()));
        }
      }
      VLOG(3) << "get_map, size: " << ret.size();
      return ret;
    } else {
      LOG(WARNING) << "key [" << key << "] not found!";
      return {};
    }
  }

  template <typename T>
  void add_value(const char* key, const T& val) {
    rapidjson::Value value;
    rapidjson::Value value_name(rapidjson::kStringType);

    if constexpr (std::is_arithmetic<T>::value) {
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
      if constexpr (std::is_arithmetic<T>::value) {
        json_array.PushBack(item, root.GetAllocator());
      } else {
        // handle_difficult c++ std::string
        auto new_value = std::string(item);
        rapidjson::Value string_value(rapidjson::kStringType);
        json_array.PushBack(
            string_value.SetString(new_value.c_str(), new_value.size(), root.GetAllocator()),
            root.GetAllocator());
      }
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
}  // namespace P2PFileSync::ServerKit
#endif  // P2P_FILE_SYNC_Serverkit_MODEL_JSON_MODEL_H