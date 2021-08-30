#include <vector>
#include "nlohmann/json.hpp"
#include <iomanip>
#include <sys/types.h>
using namespace std;
using json = nlohmann::json;
bool isFileExist(std::string file);
void replace(std::string& subject, const std::string& search,const std::string& replace);
void mayus(std::string &s);
bool onlinejkanimevideo(std::string onlineenlace,std::string server);
bool linktodownoadjkanime(std::string urltodownload,std::string directorydownload);
std::string scrapElement(std::string content, std::string get,std::string delim ="" );
std::vector<std::string> scrapElementAll(std::string content, std::string get,std::string delim ="" );
void touch(std::string route);
bool copy_me(std::string origen, std::string destino);
bool read_DB(json& base,std::string path);
bool write_DB(json base,std::string path);
void led_on(int inter);
std::vector<std::string> eraseVec(std::vector<std::string> array,std::string patther);
bool onTimeC(unsigned long long sec,unsigned long long& time2);
void TickerName(int& color,int sec,int min,int max);
void RemoveAccents(std::string& word);
bool isset(json data);
void NameOfLink(std::string& word);
std::string KeyOfLink(std::string word);
