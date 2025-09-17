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
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <thread>
#include "Networking.hpp"
#include "SDLWork.hpp"
#include "applet.hpp"
#include "utils.hpp"
#include "Link.hpp"

//use the nand of the switch
string rootdirectory = "emmc:/RipJKAnime_NX/";
string rootsave = "save:/";
string roottheme = "themes:/";
string oldroot = "sdmc:/RipJKAnimeNX/";
string CDNURL = "cdn.jkdesu.com";
bool ClFlock = false;
bool UAG = false;
bool Mromfs=true;
bool ThemeNeedUpdate=false;
bool reloadmain = false;

//Screen dimension constants
const int SCREEN_WIDTH = 1280, SCREEN_HEIGHT = 720;
//Rendered texture
LTexture gTextTexture, Farest, Heart;
//Render Buttons
LTexture B_A, B_B, B_Y, B_X, B_L, B_R, B_ZR, B_ZL, B_M, B_P, B_RIGHT, B_LEFT, B_UP, B_DOWN;
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


//Gui Vars
int statenow = programation_s;
int returnnow = programation_s;
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
int imgNumbufferAll = 30;

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
//agregados
int agregadosidx=0;
//banner
int bannersize=8;
int bannersel=0;
bool Btimer=true;
//my vars
bool isHandheld=true;
bool isChained=false;
bool AppletMode=false;
bool isSXOS=false;
bool hasStealth=false;
bool ongrid=true;
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

std::vector<std::string> Wday = {"Todos","Lunes","Martes","Miercoles","Jueves","Viernes","Sabado","Domingo"};
int WdayG=0;

std::vector<std::string> StatesList= {};
json BD;
json UD;
json AB;

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
	B_A.loadFromFile("romfs:/img/A.png");
	B_B.loadFromFile("romfs:/img/B.png");
	B_Y.loadFromFile("romfs:/img/Y.png");
	B_X.loadFromFile("romfs:/img/X.png");
	B_L.loadFromFile("romfs:/img/L.png");
	B_R.loadFromFile("romfs:/img/R.png");
	B_M.loadFromFile("romfs:/img/MINUS.png");
	B_P.loadFromFile("romfs:/img/PLUS.png");

	//GOD.MapT[""].loadFromFileCustom("", 38,38)
	GOD.MapT["EXIT"].loadFromFileCustom("romfs:/img/EXIT.png", 38,38);
	GOD.MapT["MUSIC"].loadFromFileCustom("romfs:/img/MUSIC.png", 38,38);

	GOD.MapT["ULT"].loadFromFileCustom("romfs:/img/ULT.png", 43,43);
	GOD.MapT["ULTB"].loadFromFileCustom("romfs:/img/ULT.png",55,55);

	B_ZR.loadFromFile("romfs:/img/ZR.png");
	B_ZL.loadFromFile("romfs:/img/ZL.png");
	B_RIGHT.loadFromFile("romfs:/img/RIGHT.png");
	B_LEFT.loadFromFile("romfs:/img/LEFT.png");
	B_UP.loadFromFile("romfs:/img/UP.png");
	B_DOWN.loadFromFile("romfs:/img/DOWN.png");
	CLEAR.loadFromFile("romfs:/img/CLEAR.png");
	SCREEN.loadFromFile("romfs:/img/SCREEN.png");
	NOP.loadFromFile("romfs:/img/nop.png");

	FAV.loadFromFileCustom("romfs:/img/FAV.png",43,43);
	NFAV.loadFromFileCustom("romfs:/img/NFAV.png",43,43);
	BUS.loadFromFileCustom("romfs:/img/BUS.png",43,43);

	REC.loadFromFileCustom("romfs:/img/REC.png",43,43);
	HIS.loadFromFileCustom("romfs:/img/HIS.png",43,43);
	TOP.loadFromFileCustom("romfs:/img/TOP.png",43,43);
	DOW.loadFromFileCustom("romfs:/img/DOW.png",43,43);
	HOR.loadFromFileCustom("romfs:/img/HOR.png",43,43);

	TOPB.loadFromFileCustom("romfs:/img/TOP.png",55, 55);
	DOWB.loadFromFileCustom("romfs:/img/DOW.png",55, 55);
	HORB.loadFromFileCustom("romfs:/img/HOR.png",55, 55);

	HISB.loadFromFileCustom("romfs:/img/HIS.png",55, 55);
	BACK.loadFromFileCustom("romfs:/img/BACK.png",55, 55);
	FAVB.loadFromFileCustom("romfs:/img/FAV.png",55, 55);
	BUSB.loadFromFileCustom("romfs:/img/BUS.png",55, 55);
	AFLV.loadFromFileCustom("romfs:/img/AF.png",55, 55);
	isLoaded=true;
}
