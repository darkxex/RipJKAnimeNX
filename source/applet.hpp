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
AccountUid g_uid();
string g_ID();
bool initUser();
bool GetUserID();
bool SelectUser();
bool MountUserSave();
bool GetUserImage();
bool commit();
bool deinitUser();
}
namespace emmc {
bool init();
bool commit();
bool Save();
bool deinit();
}

bool GetAppletMode();
json DInfo(string ver="");
bool ChainManager(bool Chain,bool AndChaing);
std::string KeyboardCall (std::string hint="", std::string text="");
Result WebBrowserCall(std::string url="",bool nag=false);
Result WebBrowserCloud(std::string url);
Result WebWiFiCall(std::string url);

bool GetCookies(string &cookies);
bool ClearWebData(u64 uidsave);
