
#include <string>
bool LoadNRO(std::string path);
bool GetAppletMode();
AccountUid LaunchPlayerSelect();
bool SelectUser();
bool GetUserID();
bool MountUserSave(FsFileSystem& acc);
bool GetUserImage();
bool ChainManager(bool Chain,bool AndChaing);
std::string KeyboardCall (std::string hint="", std::string text="");
Result WebBrowserCall(std::string url="",bool nag=false);
