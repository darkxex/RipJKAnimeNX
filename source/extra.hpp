#include <unistd.h>
#include <switch.h>
#include <dirent.h>
#define _CRT_SECURE_NO_WARNINGS

//test
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <SDL_mixer.h>
#include <errno.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <math.h>
#include <Vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <thread>
#include "Networking.hpp"
#include "SDLWork.hpp"
#include "applet.hpp"
#include "utils.hpp"
#include "Link.hpp"

//////////////////////////////////aquí empieza el pc.
//Screen dimension constants
const int SCREEN_WIDTH = 1280, SCREEN_HEIGHT = 720;
//Rendered texture
LTexture gTextTexture, Farest, Heart;
//Render Buttons
LTexture B_A, B_B, B_Y, B_X, B_L, B_R, B_ZR, B_M, B_P, B_RIGHT, B_LEFT, B_UP, B_DOWN;
//Render extra
LTexture SCREEN,
         BUS, BUSB,
         TOP, TOPB,
         DOW, DOWB,
         HOR, HORB,
         NFAV, FAV, FAVB,
         REC, BACK, USER,
         HIS, HISB,
         AFLV, NOP,
         CLEAR;
//Text and BOXES
LTexture VOX, T_T, T_N, T_D, T_R;

//main SLD funct (Grafics On Display == GOD)
SDLB GOD;

//Gui Vars
enum states { programationstate, downloadstate, chapterstate, searchstate, favoritesstate, historystate, hourglass, topstate, programationsliderstate};
int statenow = programationstate;
int returnnow = programationstate;
//net
std::string urltodownload = "";
int porcendown = 0;
int sizeestimated = 0;
int cancelcurl = 0;
bool lcdoff=false;
//img
bool reloading = false;
bool preview = false;
int selectchapter = 0;
int imgNumbuffer = 0;
int part = 0;
int ofall = 0;
int porcentajebufferF =0;
int porcentajebufferFF =0;
int porcentajebufferS=0, porcentajebufferAllS=0;
std::string serverenlace = "...";
std::string DownTitle="...";


//search
int searchchapter = 0;
bool reloadingsearch = false;
//downloads
bool isDownloading=false;
std::string speedD="";
//favs
int favchapter = 0;
bool gFAV = false;
//server
int selectserver = 0;
bool serverpront = false;
//slider
int selectelement = 0;
//history
int histchapter=0;
//top
int topchapter=0;
//Hour
int hourchapter=0;

//my vars
bool isHandheld=true;
bool isChained=false;
bool AppletMode=false;
bool isSXOS=false;
bool hasStealth=false;
bool ongrid=true;
bool ongridS=true;
bool ongridF=true;
AccountUid uid;
std::string AccountID="-.-";

//
bool isLoaded=false;

//Threads
SDL_Thread* searchthread = NULL;
SDL_Thread* downloadthread = NULL;
SDL_Thread* capithread = NULL;
SDL_Thread* Loaderthread = NULL;
bool quit=false;
std::string KeyName;
int Frames=1;

std::vector<std::string> arrayservers= {
	"Nozomi","Fembed 2.0","MixDrop","Desu","Xtreme S","Okru"
};

std::vector<std::string> arrayserversbak= {
	"Nozomi","Fembed 2.0","MixDrop","Desu","Xtreme S","Okru"
};

std::vector<std::string> Wday = {"Todos","Lunes","Martes","Miercoles","Jueves","Viernes","Sabado","Domingo"};
int WdayG=0;

std::vector<std::string> StatesList= {};
json BD;
json UD;

//caps vars
int maxcapit = 1;
int mincapit = 0;
int latest = 1;
int latestcolor = -1;

int sizeportraity = 300;
int sizeportraitx =424;
int xdistance = 1010;
int ydistance = 340;
bool isConnected = true;

//load images
void LoadImages(){
	//images that not change
	B_A.loadFromFile("romfs:/buttons/A.png");
	B_B.loadFromFile("romfs:/buttons/B.png");
	B_Y.loadFromFile("romfs:/buttons/Y.png");
	B_X.loadFromFile("romfs:/buttons/X.png");
	B_L.loadFromFile("romfs:/buttons/L.png");
	B_R.loadFromFile("romfs:/buttons/R.png");
	B_M.loadFromFile("romfs:/buttons/MINUS.png");
	B_P.loadFromFile("romfs:/buttons/PLUS.png");
	B_ZR.loadFromFile("romfs:/buttons/ZR.png");
	B_RIGHT.loadFromFile("romfs:/buttons/RIGHT.png");
	B_LEFT.loadFromFile("romfs:/buttons/LEFT.png");
	B_UP.loadFromFile("romfs:/buttons/UP.png");
	B_DOWN.loadFromFile("romfs:/buttons/DOWN.png");
	CLEAR.loadFromFile("romfs:/buttons/clear.png");
	SCREEN.loadFromFile("romfs:/buttons/screen.png");
	NOP.loadFromFile("romfs:/img/nop.png");

	FAV.loadFromFileCustom("romfs:/buttons/FAV.png",43,43);
	NFAV.loadFromFileCustom("romfs:/buttons/NFAV.png",43,43);
	BUS.loadFromFileCustom("romfs:/buttons/BUS.png",43,43);

	REC.loadFromFileCustom("romfs:/buttons/REC.png",43,43);
	HIS.loadFromFileCustom("romfs:/buttons/HIS.png",43,43);
	TOP.loadFromFileCustom("romfs:/buttons/TOP.png",43,43);
	DOW.loadFromFileCustom("romfs:/buttons/DOW.png",43,43);
	HOR.loadFromFileCustom("romfs:/buttons/HOR.png",43,43);

	TOPB.loadFromFileCustom("romfs:/buttons/TOP.png",55, 55);
	DOWB.loadFromFileCustom("romfs:/buttons/DOW.png",55, 55);
	HORB.loadFromFileCustom("romfs:/buttons/HOR.png",55, 55);

	HISB.loadFromFileCustom("romfs:/buttons/HIS.png",55, 55);
	BACK.loadFromFileCustom("romfs:/buttons/BACK.png",55, 55);
	FAVB.loadFromFileCustom("romfs:/buttons/FAV.png",55, 55);
	BUSB.loadFromFileCustom("romfs:/buttons/BUS.png",55, 55);
	AFLV.loadFromFileCustom("romfs:/buttons/AF.png",55, 55);
	isLoaded=true;
}
void PlayerGet(FsFileSystem& acc){
	if (SelectUser()) {
		if (MountUserSave(acc)) {
			rootsave = "save:/";
			if(isFileExist(rootdirectory+AccountID+"UserData.json")) {
				if(!isFileExist(rootsave+"UserData.json")) {
					if (copy_me(rootdirectory+AccountID+"UserData.json", rootsave+"UserData.json")) {
						fsdevCommitDevice("save");
						remove((rootdirectory+AccountID+"UserData.json").c_str());
						remove((rootdirectory+AccountID+"User.jpg").c_str());
					}
				}
			}
		}
		USER.loadFromFileCustom(rootsave+"User.jpg",58, 58);
		UD = "{}"_json;
		read_DB(UD,rootsave+"UserData.json");

		if(statenow==chapterstate) {
			capBuffer(BD["com"]["ActualLink"]);
			gFAV = isFavorite(BD["com"]["ActualLink"]);
		}
		Frames=1;
	}
}

//call states
void callmenuslide(){
	statenow=programationsliderstate;
	selectelement = 2;
}
void callsearch(){
	GOD.WorKey="0"; GOD.MasKey=-1;
	if (!reloadingsearch)
	{
		if (BD["searchtext"].empty()) {BD["searchtext"]="";}
		BD["searchtext"] = KeyboardCall("Buscar el Anime",BD["searchtext"]);
		if ((BD["searchtext"].get<std::string>()).length() > 0) {
			searchchapter = 0;
			reloadingsearch = true;
			statenow = searchstate;
			returnnow = searchstate;
			searchthread = SDL_CreateThread(searchjk, "searchthread", (void*)NULL);
		}
	}
}
void callfavs(){
	GOD.WorKey="0"; GOD.MasKey=-1;
	if (!reloading)
	{
		getFavorite();
		returnnow = favoritesstate;
		statenow = favoritesstate;
		Frames=1;
	}
}
void callAflv(){
	GOD.WorKey="0"; GOD.MasKey=-1;
	statenow=programationstate;
	WebBrowserCall("https://animeflv.net",true);
}
void callhistory(){
	GOD.WorKey="0"; GOD.MasKey=-1;
	if (UD["history"].size()>0) {
		statenow = historystate;
		returnnow = historystate;
		Frames=1;
	}
}
void calltop(){
	GOD.WorKey="0"; GOD.MasKey=-1;
	statenow = topstate;
	returnnow = topstate;
	Frames=1;
}
void callhourglass(){
	GOD.WorKey="0"; GOD.MasKey=-1;
	statenow = hourglass;
	returnnow = hourglass;
	Frames=1;
}
