//
// Created by hanzech on 11/29/21.
//

#include "user_detail_response_impl.h"
#include "model/response/public_method.h"

namespace P2PFileSync::ManagementAPI {
// todo do style checking
UserDetailResponseImpl::UserDetailResponseImpl(char* json) : JsonModel(json){};

std::string UserDetailResponseImpl::name() const { return get_value<std::string>("username").value(); }

std::string UserDetailResponseImpl::email() const { return get_value<std::string>("email").value(); }

std::unordered_map<int, std::pair<std::string, std::string>> UserDetailResponseImpl::group()
    const {
  return {};
}

}  // namespace P2PFileSync::ManagementAPI
