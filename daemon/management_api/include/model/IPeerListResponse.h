//
// Created by chen_ on 2022/1/2.
//

#ifndef P2P_FILE_SYNC_IPEERLISTRESPONSE_H
#define P2P_FILE_SYNC_IPEERLISTRESPONSE_H
#include <map>
#include "management_api_export.h"

namespace P2PFileSync::ManagementAPI {

class IPeerListResponse {
 public:
  MANAGEMENT_API_NO_EXPORT IPeerListResponse() = default;

  MANAGEMENT_API_NO_EXPORT virtual ~IPeerListResponse() = default;

  MANAGEMENT_API_EXPORT virtual std::map<std::string,std::string> peer_list() = 0;

};
}  // namespace P2PFileSync::ManagementAPI
#endif  // P2P_FILE_SYNC_IPEERLISTRESPONSE_H
