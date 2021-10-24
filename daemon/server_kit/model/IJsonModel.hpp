#ifndef P2P_FILE_SYNC_SERVER_KIT_MODEL_JSON_MODEL_H
#define P2P_FILE_SYNC_SERVER_KIT_MODEL_JSON_MODEL_H

#include <rapidjson/allocators.h>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <algorithm>
#include <limits>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace P2PFileSync::Server_kit {
/**
 * @brief base model of request/response model which used in server
 * communication
 * @note for any raw pointer give to this object as argument, this object will
 * take over the management
 */
class IJsonModel {
 public:
  IJsonModel() = default;
  ~IJsonModel() {
    if (json_buf) free(json_buf);
  }
  IJsonModel(char* json) : json_buf(json) { document.ParseInsitu(json); }

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

 protected:
  /**
   * @brief Get the value object
   *
   * @tparam T the type of the values saved to json document
   * @param key the key of the json object
   * @return T the value of key in the json document
   */
  template <typename T>
  T get_value(const char* key) {
    return document[key].Get<T>();
  }

  template <>
  std::string get_value<std::string>(const char* key) {
    return document[key].GetString();
  }

  template <typename T>
  typename std::enable_if<std::numeric_limits<T>::is_integer>::type
  addValue(const char *name, const T& number){
    rapidjson::Value value;
    rapidjson::Value value_name(rapidjson::kStringType);
    document.AddMember(value_name.SetString(name, strlen(name)),
                       value.Set(number), document.GetAllocator());
  }

  template <typename T>
  typename std::enable_if<std::is_same<std::string, T>::value>::type
  addValue(const char *name, const T& number){
    rapidjson::Value value_name(rapidjson::kStringType);
    rapidjson::Value string_value(rapidjson::kStringType);
    document.AddMember(value_name.SetString(name, strlen(name)),
                       string_value.SetString(number.c_str(), number.size()),
                       document.GetAllocator());
  }

  template <typename T>
  void addArray(const std::string& name, const std::vector<T>& array) {
    rapidjson::Value json_array(rapidjson::kArrayType);
    rapidjson::Value array_name(rapidjson::kStringType);

    // adding variables to json
    for (auto item : array) {
      json_array.PushBack(item, document.GetAllocator());
    }

    // add array to document
    rapidjson::Value value_name(rapidjson::kStringType);
    document.AddMember(value_name.SetString(name.c_str(), name.size()),json_array,
                       document.GetAllocator());
  }

 protected:
 private:
  char* json_buf = nullptr;
  rapidjson::Document document;
};
}  // namespace P2PFileSync::Server_kit
#endif  // P2P_FILE_SYNC_SERVER_KIT_MODEL_JSON_MODEL_H