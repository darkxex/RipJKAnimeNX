#include <switch.h>
#include <curl/curl.h>
#include <string>

std::string gethtml(std::string enlace);
void downloadfile(std::string enlace, std::string directorydown,bool progress = true);