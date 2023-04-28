#include <cstring>
#include <switch.h>
#include <unistd.h>
#include <vector>
#include "nlohmann/json.hpp"
#include <iomanip>
#include <sys/types.h>
using namespace std;
using json = nlohmann::json;
bool erase(string file);
bool isFileExist(string file);
void replace(string& subject, const string& search,const string& replace);
void mayus(string &s);
string scrapElement(string content, string get,string delim ="" );
vector<string> scrapElementAll(string content, string get,string delim ="",string addend="");
vector<string> split (string s, string delimiter);
string url_decode(string text);
void touch(string route);
bool copy_me(string origen, string destino);
std::string read_FL(std::string path);
std::string read_File(std::string path);
bool read_DB(json& base,string path);
bool write_DB(json base,string path);
void led_on(int inter);
vector<string> eraseVec(vector<string> array,string patther);
bool onTimeC(u64 sec,u64& time8);
bool inTimeN(u64 sec,int framesdelay = 100);
void TickerName(int& color,int sec,int min,int max);
void RemoveAccents(string& word);
string string_to_hex(const string& in);
bool isset(json& data,string key);
void NameOfLink(string& word);
string KeyOfLink(string word);
bool DoubleKill(vector<string>& data);
json StreamDev(string data = "");

namespace LOG {
void init();
int E(int r);
int getErrorCode();
void Memory();
void Screen();
void Files();
void deinit();
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
extern string rootdirectory;
extern string oldroot;
extern string rootsave;
extern string roottheme;
extern string CDNURL;
extern bool ClFlock;
extern bool Mromfs;
extern bool ThemeNeedUpdate;
extern bool reloadmain;
//Screen dimension constants
extern const int SCREEN_WIDTH, SCREEN_HEIGHT;

extern u32 __nx_applet_exit_mode;
extern string urlc;
extern bool quit;
extern int cancelcurl;
extern int porcendown;
extern int sizeestimated;
extern string speedD;
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
extern int imgNumbufferAll;
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