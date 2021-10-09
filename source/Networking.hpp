#include <switch.h>
#include <curl/curl.h>
#include <string>
#include <vector>
#include "nlohmann/json.hpp"
#include <iomanip>
#include <sys/types.h>
using namespace std;
using json = nlohmann::json;

namespace Net {
bool HasConnection();
json HEAD(string url);
json REQUEST(string url,string POSTFIEL="");
bool DOWNLOAD(string url,string path,bool progress = true);

json REDIRECT(string url,string POSTFIEL="");
json POST(string url,string POSTFIEL);
}


std::string gethtml(std::string enlace,std::string POSTFIEL="",bool redirect = false);
bool downloadfile(std::string enlace, std::string directorydown,bool progress = true);
void CheckImgVector(json List,int& index);
bool CheckImgNet(std::string image,std::string url="");
bool CheckUpdates(bool force=false);