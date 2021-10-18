#include <string>
#include <vector>
#include "nlohmann/json.hpp"
#include <iomanip>
#include <sys/types.h>
using namespace std;
using json = nlohmann::json;
bool LoadNRO(std::string path);
bool InstallNSP(std::string nsp);
bool ReloadDNS();
bool IsRunning(string servname);
Result txStealthMode(uint64_t enable);
bool mount_theme(string in,bool mount=false);
namespace user {
bool initUser();
bool GetUserID();
bool SelectUser();
bool MountUserSave(FsFileSystem& acc);
bool GetUserImage();
}
bool GetAppletMode();
json DInfo(string ver="");
bool ChainManager(bool Chain,bool AndChaing);
std::string KeyboardCall (std::string hint="", std::string text="");
Result WebBrowserCall(std::string url="",bool nag=false);
