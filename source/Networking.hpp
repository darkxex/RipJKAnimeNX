#include <switch.h>
#include <curl/curl.h>
#include <string>


std::string gethtml(std::string enlace,std::string POSTFIEL="",bool redirect = false);
bool downloadfile(std::string enlace, std::string directorydown,bool progress = true);
bool CheckImgNet(std::string image);
bool HasConnection();