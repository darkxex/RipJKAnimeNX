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
#include<ctime>
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
//////////////////////////////////aquí empieza el pc.
//Screen dimension constants
const int SCREEN_WIDTH = 1280, SCREEN_HEIGHT = 720;
//Rendered texture
LTexture gTextTexture, Farest, Heart;
//Render Buttons
LTexture B_A, B_B, B_Y, B_X, B_L, B_R, B_ZR, B_M, B_P, B_RIGHT, B_LEFT, B_UP, B_DOWN;
//Render extra
LTexture BUS, BUSB, REC, BACK, USER, NFAV, FAV, FAVB, AFLV, NOP, CLEAR, SCREEN;
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
int porcentajebuffer = 0;
int porcentajebufferAll = 0;
int porcentajebufferF =0;
int porcentajebufferFF =0;
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
//Threads
SDL_Thread* prothread = NULL;
SDL_Thread* searchthread = NULL;
SDL_Thread* downloadthread = NULL;
SDL_Thread* capithread = NULL;
bool quit=false;
std::string KeyName;
int Frames=1;

std::vector<std::string> arrayservers= {
"Nozomi","Fembed 2.0","MixDrop","Desu","Xtreme S","Okru"
};

std::vector<std::string> arrayserversbak= {
"Nozomi","Fembed 2.0","MixDrop","Desu","Xtreme S","Okru"
};

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


//call states
void callsearch(){
	GOD.WorKey="0";GOD.MasKey=-1;
	if (!reloadingsearch)
	{
		if (BD["searchtext"].empty()){BD["searchtext"]="";}
		BD["searchtext"] = KeyboardCall("Buscar el Anime",BD["searchtext"]);
		if ((BD["searchtext"].get<std::string>()).length() > 0){
			searchchapter = 0;
			reloadingsearch = true;	 
			statenow = searchstate;
			returnnow = searchstate;
			searchthread = SDL_CreateThread(searchjk, "searchthread", (void*)NULL);
		}
	}
}

void callfavs(){
	GOD.WorKey="0";GOD.MasKey=-1;
	if (!reloading)
	{
		getFavorite();
		returnnow = favoritesstate;
		statenow = favoritesstate;
		Frames=1;
	}
}

void callAflv(){
	GOD.WorKey="0";GOD.MasKey=-1;
	statenow=programationstate;
	WebBrowserCall("https://animeflv.net",true);
}