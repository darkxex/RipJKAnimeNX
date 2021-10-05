#include <switch.h>
#include <curl/curl.h>
#include <string>
#include <vector>
#include "nlohmann/json.hpp"
#include <iomanip>
#include <sys/types.h>
using namespace std;
using json = nlohmann::json;

json HEAD(string url);
std::string gethtml(std::string enlace,std::string POSTFIEL="",bool redirect = false);
bool downloadfile(std::string enlace, std::string directorydown,bool progress = true);
void CheckImgVector(json List,int& index);
bool CheckImgNet(std::string image,std::string url="");
bool HasConnection();
bool CheckUpdates(bool force=false);