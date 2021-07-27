#include <vector>
bool isFileExist(std::string file);
void replace(std::string& subject, const std::string& search,const std::string& replace);
void mayus(std::string &s);
bool onlinejkanimevideo(std::string onlineenlace,std::string server);
bool linktodownoadjkanime(std::string urltodownload,std::string directorydownload);
std::string scrapElement(std::string content, std::string get,std::string delim ="" );
std::vector<std::string> scrapElementAll(std::string content, std::string get,std::string delim ="" );
void touch(std::string route);
bool copy_me(std::string origen, std::string destino);
