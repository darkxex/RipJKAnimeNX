//////////////////////////////////aquí empieza el pc.
//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
//Rendered texture
LTexture gTextTexture;
LTexture Farest;
LTexture Heart;
LTexture TChapters;
LTexture TPreview;
LTexture TPreviewb;
LTexture TPreviewa;
//Render Buttons
LTexture B_A;
LTexture B_B;
LTexture B_Y;
LTexture B_X;
LTexture B_L;
LTexture B_R;
LTexture B_ZR;
LTexture B_M;
LTexture B_P;
LTexture B_RIGHT;
LTexture B_LEFT;
LTexture B_UP;
LTexture B_DOWN;
//Render extra
LTexture FAV;
LTexture NOP;
LTexture CLEAR;
//Text and BOXES
LTexture VOX;
LTexture T_T;
LTexture T_D;
LTexture T_R;

//main SLD funct (Grafics On Display = GOD)
SDLB GOD;
	
//Gui Vars
enum states { programationstate, downloadstate, chapterstate, searchstate, favoritesstate };
enum statesreturn { toprogramation, tosearch, tofavorite };
int statenow = programationstate;
int returnnow = toprogramation;
//net
std::string urltodownload = "";
int porcendown = 0;
int sizeestimated = 0;
std::string temporallink = "";
int cancelcurl = 0;

//img
bool reloading = false;
bool preview = false;
int selectchapter = 0;
int porcentajereload = 0;
int imgNumbuffer = 0;
int porcentajebuffer = 0;
int porcentajebufferA = 0;
bool activatefirstimage = true;
std::string serverenlace = "...";
std::string DownTitle="...";

int maxcapit = 1;
int mincapit = 0;
int capmore = 1;

//search
int searchchapter = 0;
bool reloadingsearch = false;
bool activatefirstsearchimage = true;
std::string searchtext = "";
std::string tempimage = "";
//downloads
bool isDownloading=false;
std::string speedD="";
//favs
int favchapter = 0;
bool gFAV = false;
//server
int selectserver = 0;
bool serverpront = false;

//my vars
bool AppletMode=false;
bool isSXOS=false;
bool hasStealth=false;
AccountUid uid;
SDL_Thread* capithread = NULL;
bool quit=false;

#ifdef __SWITCH__
HidsysNotificationLedPattern blinkLedPattern(u8 times);
void blinkLed(u8 times);
#endif // ___SWITCH___

std::vector<std::string> arrayservers= {
"Fembed 2.0","Nozomi","MixDrop","Desu","Xtreme S","Okru"
};

std::vector<std::string> arrayserversbak= {
"Fembed 2.0","Nozomi","MixDrop","Desu","Xtreme S","Okru"
};

json BigData;

//caps vars
std::string sinopsis = "";
bool enemision = false;
std::string nextdate = "";
std::string generos = "";

int sizeportraity = 210;
int sizeportraitx = 297;
int xdistance = 1010;
int ydistance = 340;

