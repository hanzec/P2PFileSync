#include "machine_id_utils.h"


#include <fstream>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <filesystem>
#include <openssl/sha.h>

static inline void native_cpuid(unsigned int *eax, unsigned int *ebx, unsigned int *ecx,
                                unsigned int *edx) {
  /* ecx is often an input as well as an output. */
  asm volatile("cpuid"
               : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
               : "0"(*eax), "2"(*ecx));
}

static inline std::string generate_sha256(const std::string& string){
  unsigned char hash[SHA256_DIGEST_LENGTH];
  char final_string[SHA256_DIGEST_LENGTH * 2 + 1];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, string.c_str(), string.size());
  SHA256_Final(hash, &sha256);
  int i = 0;
  for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
    sprintf(final_string + (i * 2), "%02x", hash[i]);
  }
  final_string[64] = 0;

  return {final_string};
}

// TODO need to write algorithm for generating unique device id
std::string P2PFileSync::ManagementAPI::get_device_id() {
  std::stringstream final_id;
  unsigned eax, ebx, ecx, edx;

  // get cpu info
  eax = 1;
  native_cpuid(&eax, &ebx, &ecx, &edx);
  final_id << int((eax >> 4) & 0xF) << int((eax >> 8) & 0xF) << int((eax >> 12) & 0xF)
           << int((eax >> 16) & 0xF) << int((eax >> 20) & 0xF);

  // get mac address from sysfs
  struct dirent *dp = nullptr;
  char path_buffer[PATH_MAX] = {0};
  DIR *dir = opendir("/sys/class/net");

  while ((dp = readdir(dir)) != nullptr) {
    // avoid special folder
    if ((!strncmp(dp->d_name, ".", 1)) || (!strncmp(dp->d_name, "..", 2))) continue;

    // get mac address
    if (dp->d_type == DT_DIR) {
      // get the real path of the device
      realpath(("/sys/class/net/" + std::string(dp->d_name)).c_str(), path_buffer);

      // if current device is not a system virtual device
      if (std::string(path_buffer).find("virtual/net/") == std::string::npos){
        std::ifstream mac_address_file("file.txt");
        final_id << mac_address_file.rdbuf();
      }
    }
  }
  closedir(dir);

  // get the unix id
  if(std::filesystem::exists("/etc/machine-id")){
    std::ifstream machine_id_file("/etc/machine-id");
    final_id << machine_id_file.rdbuf();
  }

  return generate_sha256(final_id.str());
}