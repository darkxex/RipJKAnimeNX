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
bool mount_theme(string in,bool mount=false);
bool GetAppletMode();
AccountUid LaunchPlayerSelect();
bool SelectUser();
bool GetUserID();
bool MountUserSave(FsFileSystem& acc);
bool GetUserImage();
json DInfo(string ver="");
bool ChainManager(bool Chain,bool AndChaing);
std::string KeyboardCall (std::string hint="", std::string text="");
Result WebBrowserCall(std::string url="",bool nag=false);
