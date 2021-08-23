#include <switch.h>
#include <curl/curl.h>
#include <string>
#include <Vector>

std::string gethtml(std::string enlace,std::string POSTFIEL="",bool redirect = false);
bool downloadfile(std::string enlace, std::string directorydown,bool progress = true);
void CheckImgVector(std::vector<std::string> List,int& index);
bool CheckImgNet(std::string image,std::string url="");
bool HasConnection();