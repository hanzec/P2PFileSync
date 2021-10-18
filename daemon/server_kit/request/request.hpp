#ifndef P2P_FILE_SYNC_SERVER_KIT_REQUEST_REQUEST_H
#define P2P_FILE_SYNC_SERVER_KIT_REQUEST_REQUEST_H

#include <algorithm>
#include <limits>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <rapidjson/allocators.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

class ServerRequestModel{
  public:
    ServerRequestModel() = default;
    ServerRequestModel(char * json){document.ParseInsitu(json);}

    std::string get_json() {
      rapidjson::StringBuffer strBuf;
      rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);
      document.Accept(writer);

      return strBuf.GetString();
    };
  protected:
    template <typename T>
    void addValue(const std::string& name, const std::vector<T>& array){
      rapidjson::Value json_array(rapidjson::kArrayType);
      rapidjson::Value array_name(rapidjson::kStringType);

      // adding variables to json
      for(auto item : array){
        json_array.PushBack(item, document.GetAllocator());
      }

      // add array to document
      document.AddMember(array_name.SetString(name.c_str(), name.size()),json_array , document.GetAllocator());
    }

    template <typename T, typename = typename std::enable_if<std::numeric_limits<T>::is_integer>::type>
    void addValue(const std::string& name, const T& number){
      rapidjson::Value value;
      rapidjson::Value value_name(rapidjson::kStringType);
      document.AddMember(value_name.SetString(name.c_str(), name.size()), value.Set(number), document.GetAllocator());
    }

    void addValue(const std::string& name, const std::string& number){
      rapidjson::Value value_name(rapidjson::kStringType);
      rapidjson::Value string_value(rapidjson::kStringType);
      document.AddMember(value_name.SetString(name.c_str(), name.size()), 
                         string_value.SetString(number.c_str(),number.size()), document.GetAllocator());
    }

  private:
    rapidjson::Document document;

};
#endif // P2P_FILE_SYNC_SERVER_KIT_REQUEST_REQUEST_H