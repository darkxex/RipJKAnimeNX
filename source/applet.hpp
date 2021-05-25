
#include <string>
bool LoadNRO(std::string path);
bool GetAppletMode();
std::string KeyboardCall (std::string hint="", std::string text="");
Result WebBrowserCall(std::string url="",bool nag=false);
