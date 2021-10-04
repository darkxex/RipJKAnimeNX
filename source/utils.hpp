#include <cstring>
#include <switch.h>
#include <unistd.h>
#include <vector>
#include "nlohmann/json.hpp"
#include <iomanip>
#include <sys/types.h>
using namespace std;
using json = nlohmann::json;
bool isFileExist(std::string file);
void replace(std::string& subject, const std::string& search,const std::string& replace);
void mayus(std::string &s);
std::string scrapElement(std::string content, std::string get,std::string delim ="" );
std::vector<std::string> scrapElementAll(std::string content, std::string get,std::string delim ="",std::string addend="");
vector<string> split (string s, string delimiter);
void touch(std::string route);
bool copy_me(std::string origen, std::string destino);
bool read_DB(json& base,std::string path);
bool write_DB(json base,std::string path);
void led_on(int inter);
std::vector<std::string> eraseVec(std::vector<std::string> array,std::string patther);
bool onTimeC(u64 sec,u64& time8);
void TickerName(int& color,int sec,int min,int max);
void RemoveAccents(std::string& word);
std::string string_to_hex(const std::string& in);
bool isset(json data);
void NameOfLink(std::string& word);
std::string KeyOfLink(std::string word);

//extern to all
extern AccountUid uid;
extern std::string AccountID;
extern std::string rootdirectory;
extern std::string rootsave;
extern u32 __nx_applet_exit_mode;
extern std::string urlc;
extern bool quit;
extern int cancelcurl;
extern int porcendown;
extern int sizeestimated;
extern std::string speedD;
extern json BD;
extern json UD;
extern int mincapit;
extern int maxcapit;
extern int latest;
extern int latestcolor;
extern int porcentajebufferS;
extern int porcentajebufferAllS;
extern bool isConnected;
//img
extern bool reloading;
extern bool preview;
extern int selectchapter;
extern int imgNumbuffer;
extern int part;
extern int ofall;
extern int porcentajebufferF;
extern int porcentajebufferFF;
extern bool isDownloading;
extern bool AppletMode;
extern bool isChained;
extern string serverenlace;
extern string DownTitle;
extern string KeyName;
extern int statenow;
extern int returnnow;
extern bool reloadingsearch;
extern int Frames;