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
bool inTimeN(u64 sec,int framesdelay = 100);
void TickerName(int& color,int sec,int min,int max);
void RemoveAccents(std::string& word);
std::string string_to_hex(const std::string& in);
bool isset(json data);
void NameOfLink(std::string& word);
std::string KeyOfLink(std::string word);

namespace LOG {
	void init();
	void SaveFile();
	void Mem();
	void Screen();
}

typedef enum {
	programation_s,
	menu_s,
	chapter_s,
	download_s,
	search_s,
	favorites_s,
	history_s,
	hourglass_s,
	top_s,
	agregados_s,
	themas_s
} states;

typedef enum {
	BT_A, BT_B, BT_X, BT_Y,
	BT_L3, BT_R3,
	BT_L, BT_R, BT_ZL, BT_ZR,
	BT_P, BT_M,
	BT_LEFT, BT_UP, BT_RIGHT, BT_DOWN,
	BT_LS_LEFT, BT_LS_UP, BT_LS_RIGHT, BT_LS_DOWN,
	BT_RS_LEFT, BT_RS_UP, BT_RS_RIGHT, BT_RS_DOWN,
	BT_1
} SDL_Keys;

//extern to all
extern AccountUid uid;
extern std::string AccountID;

extern std::string rootdirectory;
extern std::string rootsave;
extern std::string roottheme;

extern int hasError;
//Screen dimension constants
extern const int SCREEN_WIDTH, SCREEN_HEIGHT;

extern u32 __nx_applet_exit_mode;
extern std::string urlc;
extern bool quit;
extern int cancelcurl;
extern int porcendown;
extern int sizeestimated;
extern std::string speedD;
extern json BD;
extern json UD;
extern json AB;
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
extern bool isSXOS;
extern string serverenlace;
extern string DownTitle;
extern string KeyName;
extern int statenow;
extern int returnnow;
extern bool reloadingsearch;
extern int Frames;