#include <switch.h>
#include <curl/curl.h>
#include <string>

std::string getFirstKey(std::string enlace);
std::string getSecondKey(std::string data);
std::string getThirdKey(std::string secondKey);