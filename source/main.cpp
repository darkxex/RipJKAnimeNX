#ifdef __SWITCH__
#include <unistd.h>
#include <switch.h>
#include <dirent.h>
#endif 
#ifndef __SWITCH__
#define _CRT_SECURE_NO_WARNINGS
#endif

//test
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <SDL_mixer.h>
#include <math.h> 
#include <errno.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <iostream>
#include <math.h>  
#include <Vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include "Networking.hpp"
#include "SDLWork.hpp"
#include "applet.hpp"
#include <thread>
#include "utils.hpp"

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
LTexture TSearchPreview;///////
LTexture TFavorite;

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


LTexture FAV;
LTexture NOP;

//main SLD funct
SDLB GOD;
	
//Gui
enum states { programationstate, downloadstate, chapterstate, searchstate, favoritesstate };
enum statesreturn { toprogramation, tosearch, tofavorite };
int statenow = programationstate;
int returnnow = toprogramation;
//net
std::string  urltodownload = "";
int porcendown = 0;
int sizeestimated = 0;
std::string temporallink = "";
int cancelcurl = 0;
bool fulldownloaded = false;

//img
bool reloading = false;
bool preview = false;
int selectchapter = 0;
int porcentajereload = 0;
int imgNumbuffer = 0;
int porcentajebuffer = 0;
int porcentajebufferA = 0;
bool activatefirstimage = true;

//search
int searchchapter = 0;
bool reloadingsearch = false;
bool activatefirstsearchimage = true;
std::string serverenlace = "";
std::string searchtext = "";
std::string tempimage = "";
//favs
int favchapter = 0;
bool gFAV = false;
#ifdef __SWITCH__
std::string favoritosdirectory = "sdmc:/switch/RipJKAnime_NX/favoritos.txt";
#else
std::string favoritosdirectory = "C:/respaldo2017/C++/test/Debug/favoritos.txt";
#endif // SWITCH

//my vars
std::string rootdirectory = "sdmc:/switch/RipJKAnime_NX/DATA/";
bool AppletMode=false;
bool isSXOS=false;
bool hasStealth=false;
AccountUid uid;


#ifdef __SWITCH__
HidsysNotificationLedPattern blinkLedPattern(u8 times);
void blinkLed(u8 times);
#endif // ___SWITCH___



int quit = 0;

//make some includes to clean alittle the main
#include "JKanime.hpp"

//MAIN INT
int main(int argc, char **argv)
{
#ifdef __SWITCH__
	romfsInit();
	socketInitializeDefault();
	//nxlinkStdio();
	struct stat st = { 0 };
	nxlinkStdio();
	printf("printf output now goes to nxlink server\n");
	mkdir("sdmc:/switch/RipJKAnime_NX", 0777);
	mkdir("sdmc:/switch/RipJKAnime_NX/DATA", 0777);
	if (stat("sdmc:/RipJKAnime", &st) != -1) {
		fsdevDeleteDirectoryRecursively("sdmc:/RipJKAnime");		
	}
	
	Result rc = 0;	
	rc =  accountInitialize(AccountServiceType_Application);
	if (R_SUCCEEDED(rc)) {
		//accountGetServiceSession ();
		accountGetPreselectedUser(&uid);
		printf("Goted user\n");
		accountExit();       
	} else printf("failed tu get user \n"); 
	
	AppletMode=GetAppletMode();
#endif
	
	
	SDL_Thread* prothread = NULL;
	SDL_Thread* searchthread = NULL;
	SDL_Thread* threadID = NULL;

	//Set main Thread get images and descriptions
	prothread = SDL_CreateThread(refrescarpro, "prothread", (void*)NULL);

	GOD.intA();//init the SDL
	if (isFileExist("wada.ogg")){
		GOD.gMusic = Mix_LoadMUS("wada.ogg");
		Mix_PlayMusic(GOD.gMusic, -1);
	}
#ifdef __SWITCH__
	
	if (isFileExist("texture.png")){
		Farest.loadFromFile("texture.png");
	} else {
		Farest.loadFromFile("romfs:/texture.png");
	}
	
	Heart.loadFromFile("romfs:/heart.png");
#else
	Farest.loadFromFile("C:\\respaldo2017\\C++\\test\\Debug\\texture.png");
	Heart.loadFromFile("C:\\respaldo2017\\C++\\test\\Debug\\heart.png");
#endif // SWITCH

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
	B_RIGHT.loadFromFile("romfs:/buttons/RIGHT.png");/////
	B_LEFT.loadFromFile("romfs:/buttons/LEFT.png");
	B_UP.loadFromFile("romfs:/buttons/UP.png");
	B_DOWN.loadFromFile("romfs:/buttons/DOWN.png");
	FAV.loadFromFile("romfs:/buttons/FAV.png");/////
	NOP.loadFromFile("romfs:/nop.png");


	SDL_Color textColor = { 50, 50, 50 };
	
	int posxbase = 20;
	int posybase = 10;
	int maxcapit = 1;
	int mincapit = 0;
	int capmore = 1;

	//Event handler
	SDL_Event e;
#ifdef __SWITCH__
	for (int i = 0; i < 2; i++) {
		if (SDL_JoystickOpen(i) == NULL) {
			SDL_Log("SDL_JoystickOpen: %s\n", SDL_GetError());
			SDL_Quit();
			return -1;
		}
	}
#endif // SWITCH

if (AppletMode) {//close on applet mode
	//Clear screen
	SDL_SetRenderDrawColor(GOD.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(GOD.gRenderer);

	//wallpaper
	Farest.render((0), (0));
	SDL_Rect fillRect = { 0, SCREEN_HEIGHT/2 - 25, 1280, 50 };
	SDL_SetRenderDrawColor(GOD.gRenderer, 255, 255, 255, 255);

	SDL_RenderFillRect(GOD.gRenderer, &fillRect); 
	gTextTexture.loadFromRenderedText(GOD.gFont3, "Esta App No funciona en Modo Applet. Pulsa R Al Abrir un Juego", textColor);
	gTextTexture.render(SCREEN_WIDTH/2 - gTextTexture.getWidth()/2, SCREEN_HEIGHT/2 - gTextTexture.getHeight() / 2);
	SDL_RenderPresent(GOD.gRenderer);
	quit=1;
	SDL_Delay(3000);
}

	//While application is running
	while (!quit)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e))
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				cancelcurl = 1;
				quit = 1;
			}
			//#include "keyboard.h"
#ifdef __SWITCH__

			switch (e.type) {
			case SDL_JOYAXISMOTION:
				SDL_Log("Joystick %d axis %d value: %d\n",
					e.jaxis.which,
					e.jaxis.axis, e.jaxis.value);
				break;

			case SDL_JOYBUTTONDOWN:
				SDL_Log("Joystick %d button %d down\n",
					e.jbutton.which, e.jbutton.button);
				// https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L52
				// seek for joystick #0
				if (e.jbutton.which == 0) {
					if (e.jbutton.button == 0) {// (A) button down
						
						switch (statenow)
						{
							case programationstate:
							{
								if (!reloading&&arraychapter.size()>=1)
								{activatefirstimage=true;
									TChapters.free();
									TChapters.loadFromFileCustom(tempimage, 550, 400);
									temporallink = arraychapter[selectchapter];
									int val1 = temporallink.find("/", 20);
									temporallink = temporallink.substr(0, val1 + 1);
									printf("q wea es esto? %s  : %d\n",temporallink.c_str() ,selectchapter);
									if (selectchapter > (int)con_maxcapit.size()-1)
									{
										maxcapit = atoi(capit(temporallink).c_str());
									}else {
										try{
											rese = con_rese[selectchapter];
											nextdate = con_nextdate[selectchapter];
											enemision = con_enemision[selectchapter];
											tienezero = con_tienezero[selectchapter];										
											maxcapit = con_maxcapit[selectchapter];
											printf("Goted %d\n",selectchapter);
										}catch(...){
											printf("Error \n");
										}
									}
									
									
									statenow = chapterstate;
									if (tienezero) {
										maxcapit = maxcapit - 1;
										mincapit = 0;
										capmore = maxcapit;
									}
									else
									{
										mincapit = 1;
										capmore = maxcapit;
									}
									std::cout << maxcapit << std::endl;
								gFAV = isFavorite(temporallink);
								}
							}
							break;

							case searchstate:
							{
								activatefirstimage=true;
								if (!reloadingsearch && arraysearch.size()>=1)
								{
									TChapters.free();
									TChapters.loadFromFileCustom(tempimage, 550, 400);
									statenow = chapterstate;
									temporallink = arraysearch[searchchapter];

									std::cout << temporallink << std::endl;
									maxcapit = atoi(capit(temporallink).c_str());
									if (tienezero) {
										maxcapit = maxcapit - 1;
										mincapit = 0;
										capmore = maxcapit;
									}
									else
									{
										mincapit = 1;
										capmore = maxcapit;
									}
									std::cout << maxcapit << std::endl;
									gFAV = isFavorite(temporallink);
								}

							}
							break;

							case favoritesstate:
							{
								activatefirstimage=true;
								if ((int)arrayfavorites.size() >= 1 ){
								TChapters.free();
								CheckImgNet(tempimage);
								TChapters.loadFromFileCustom(tempimage, 550, 400);
								temporallink = arrayfavorites[favchapter];

								std::cout << temporallink << std::endl;
								maxcapit = atoi(capit(temporallink).c_str());
								statenow = chapterstate;
								if (tienezero) {
									maxcapit = maxcapit - 1;
									mincapit = 0;
									capmore = maxcapit;
								}
								else
								{
									mincapit = 1;
									capmore = maxcapit;
								}
								std::cout << maxcapit << std::endl;
								gFAV = true;
								}
							}
							break;
							case chapterstate:
								std::string tempurl = temporallink + std::to_string(capmore) + "/";
								onlinejkanimevideo(tempurl);
							break;
						}
					}
					else if (e.jbutton.button == 10) {// (+) button down close to home menu
						cancelcurl = 1;
						quit = 1;
					}
					else if (e.jbutton.button == 11) {// (-) button down
						if (Mix_PlayingMusic() == 0)
						{
							//Play the music
							Mix_PlayMusic(GOD.gMusic, -1);
						}
						//If music is being played
						else
						{
							//If the music is paused
							if (Mix_PausedMusic() == 1)
							{
								//Resume the music
								Mix_ResumeMusic();
							}
							//If the music is playing
							else
							{
								//Pause the music
								Mix_PauseMusic();
							}
						}
					}
					else if (e.jbutton.button == 6 || e.jbutton.button == 8) {// (L & ZL) button down
						if (statenow == programationstate)
						{
							if (e.jbutton.button == 6)
								WebBrowserCall("https://animeflv.net",true);
							else 
								WebBrowserCall("https://jkanime.net",true);
						}
					}
					else if (e.jbutton.button == 9) {// (ZR) button down
						switch (statenow)
						{
						case favoritesstate:
							delFavorite();
							favchapter=0;
							arrayfavorites.clear();
							break;
						}
					}
					else if (e.jbutton.button == 1) {// (B) button down
						switch (statenow)
						{
						case downloadstate:
							cancelcurl = 1;
							statenow = chapterstate;
							break;
						case chapterstate:
							cancelcurl = 1;
							switch (returnnow)
							{
							case toprogramation:
								statenow = programationstate;
								break;
							case tosearch:
								statenow = searchstate;
								break;
							case tofavorite:
								statenow = favoritesstate;
								break;
							}
							break;
						case searchstate:
							if (!reloadingsearch)
							{
								returnnow = toprogramation;
								statenow = programationstate;
								TSearchPreview.free();
							}
							break;

						case favoritesstate:
							returnnow = toprogramation;
							statenow = programationstate;
							break;
						}
					}
					else if (e.jbutton.button == 2) {// (X) button down
						switch (statenow)
						{
						case chapterstate:
							//just for test
							statenow = downloadstate;
							cancelcurl = 0;
							urltodownload = temporallink + std::to_string(capmore) + "/";
							threadID = SDL_CreateThread(downloadjkanimevideo, "jkthread", (void*)NULL);
							break;
						case searchstate:

							break;
						case favoritesstate:

							delFavorite(favchapter);
							if (!reloading)
							{
								if (favchapter > 0) favchapter--;
								arrayfavorites.clear();
								statenow = favoritesstate;
								std::string temp;
								std::ifstream infile;

								std::ifstream file(favoritosdirectory);
								std::string str;
								while (std::getline(file, str)) {
									std::cout << str << "\n";
									if (str.find("jkanime"))
									{
										arrayfavorites.push_back(str);
									}
								}
								file.close();
							}
							break;

						}
					}
					else if (e.jbutton.button == 3) {// (Y) button down
						

						switch (statenow)
						{
						case programationstate:
							if (!reloading)
							{activatefirstimage=true;
								returnnow = tofavorite;
								arrayfavorites.clear();
								statenow = favoritesstate;
								std::string temp;
								std::ifstream infile;

								std::ifstream file(favoritosdirectory);
								std::string str;
								while (std::getline(file, str)) {
									std::cout << str << "\n";
									if (str.find("jkanime"))
									{
										arrayfavorites.push_back(str);
									}
								}
								file.close();
								callimagefavorites(favchapter);


							}
							break;
						case downloadstate:



							break;
						case chapterstate:
						{//AGREGAR A FAVORITOS
							if(!isFavorite(temporallink)){
								std::ofstream outfile;
								outfile.open(favoritosdirectory, std::ios_base::app); // append instead of overwrite
								outfile << temporallink;
								outfile << "\n";
								outfile.close();
							}

							gFAV = true;
						}

						break;
						case favoritesstate:

							break;




						}
					}
					else if (e.jbutton.button == 5) {// (R3) button down
						switch (statenow)
						{//only for test 
							case chapterstate:
								std::string tempurl = temporallink + std::to_string(capmore) + "/";
								WebBrowserCall(tempurl);
							break;
						}

					}
					else if (e.jbutton.button == 7) {// (R) button down
						
						switch (statenow)
						{
						case programationstate:
						case searchstate:
							if (!reloadingsearch)
							{activatefirstimage=true;
#ifdef __SWITCH__
								searchtext = KeyboardCall("Buscar Anime (2 letras mínimo.)",searchtext);
								//blinkLed(1);//LED
#endif // __SWITCH__
								if (searchtext.length() > 1){
									searchchapter = 0;
									TSearchPreview.free();
									arraysearch.clear();
									arraysearchimages.clear();
									statenow = searchstate;
									returnnow = tosearch;
									searchthread = SDL_CreateThread(searchjk, "searchthread", (void*)NULL);
								}
								break;
							}

						}

					}
					else if (e.jbutton.button == 12 || e.jbutton.button == 16) {// (left) button down
						
						switch (statenow)
						{
						case chapterstate:
							if (capmore > mincapit)
							{
								capmore--;
							}
							if (capmore < mincapit)
							{
								capmore = mincapit;
							}
							break;
						}
					}
					else if (e.jbutton.button == 14 || e.jbutton.button == 18) {// (right) button down
						
						switch (statenow)
						{
						case chapterstate:
							if (capmore < maxcapit)
							{
								capmore++;
							}
							if (capmore > maxcapit)
							{
								capmore = maxcapit;
							}
							break;
						}
					}
					else if (e.jbutton.button == 17 || e.jbutton.button == 13) {// (up) button down
						
						switch (statenow)
						{
						case programationstate:
							if (!reloading)
							{
								TPreview.free();
								if (selectchapter > 0)
								{
									selectchapter--;
									std::cout << selectchapter << std::endl;
								}
								else {
									selectchapter = arraychapter.size() - 1;
								}
								callimage(selectchapter);

							}

							break;
						case chapterstate:

							if (capmore < maxcapit)
							{
								capmore = capmore + 10;
							}
							if (capmore > maxcapit)
							{
								capmore = maxcapit;
							}
							break;

						case searchstate:
							if (!reloadingsearch)
							{
								TSearchPreview.free();
								if (searchchapter > 0)
								{
									searchchapter--;
									std::cout << searchchapter << std::endl;
								}
								else {
									searchchapter = arraysearch.size() - 1;
								}
								callimagesearch(searchchapter);

							}
							break;

						case favoritesstate:

							TFavorite.free();
							if (favchapter > 0)
							{
								favchapter--;
								std::cout << favchapter << std::endl;
							}
							else {
								favchapter = (int)arrayfavorites.size() - 1;
							}
							callimagefavorites(favchapter);
							break;

						}
					}
					else if (e.jbutton.button == 19 || e.jbutton.button == 15) {// (down) button down
						
						switch (statenow)
						{
						case searchstate:
							if (!reloadingsearch)
							{
								TSearchPreview.free();
								if (searchchapter < (int)arraysearch.size() - 1)
								{
									searchchapter++;

									std::cout << searchchapter << std::endl;
								}
								else {
									searchchapter = 0;
								}
								callimagesearch(searchchapter);

							}
							break;

						case programationstate:
							if (!reloading)
							{
								TPreview.free();


								if (selectchapter < (int)arraychapter.size() - 1)
								{
									selectchapter++;

									std::cout << selectchapter << std::endl;
								}
								else {
									selectchapter = 0;
								}

								callimage(selectchapter);

							}
							break;

						case chapterstate:

							if (capmore > 1)
							{
								capmore = capmore - 10;
							}
							if (capmore < mincapit)
							{
								capmore = mincapit;
							}
							break;

						case favoritesstate:
							TFavorite.free();
							if (favchapter < (int)arrayfavorites.size() - 1)
							{
								favchapter++;

								std::cout << favchapter << std::endl;
							}
							else {
								favchapter = 0;
							}
							callimagefavorites(favchapter);
							break;

						}
					}
				}
				break;

			default:
				break;
			}
#endif // SWITCH

		}


		//Clear screen
		SDL_SetRenderDrawColor(GOD.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(GOD.gRenderer);

		//Render current frame
		//wallpaper
		Farest.render((0), (0));
		switch (statenow)
		{
		case chapterstate:
		{
		//Draw a bacground to a nice view
		{SDL_SetRenderDrawColor(GOD.gRenderer, 170, 170, 170, 100);
		SDL_Rect HeaderRect = {0,0, 1280, 720};
		SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);}
		
		std::string temptext = temporallink;
		replace(temptext, "https://jkanime.net/", "");
		replace(temptext, "/", " ");
		replace(temptext, "-", " ");
		mayus(temptext);


		//warning , only display in sxos ToDo
		gTextTexture.loadFromRenderedText(GOD.gFont, "(*En SXOS desactiva Stealth Mode*)", textColor);
		gTextTexture.render(posxbase, 0 );

		//draw Title
		gTextTexture.loadFromRenderedText(GOD.gFont3, temptext.substr(0,62)+ ":", textColor);
		gTextTexture.render(posxbase+10, posybase);
		
		{//draw description
		SDL_SetRenderDrawColor(GOD.gRenderer, 255, 255, 255, 100);
		SDL_Rect HeaderRect = {25,60, 770, 400};
		SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);
		
		gTextTexture.loadFromRenderedTextWrap(GOD.gFont, rese, textColor, 750);
		gTextTexture.render(posxbase+15, posybase + 65);
		}
		
		{//draw preview image
		SDL_Rect fillRect = { SCREEN_WIDTH - 442,SCREEN_HEIGHT / 2 - 302, 404, 595 };
		SDL_SetRenderDrawColor(GOD.gRenderer, 0, 0, 0, 200);

		SDL_RenderFillRect(GOD.gRenderer, &fillRect); 
		TChapters.render(SCREEN_WIDTH - 440, SCREEN_HEIGHT / 2 - 300);
		
		}
		
		if (enemision)
		{
			gTextTexture.loadFromRenderedText(GOD.gFont3, "En Emisión ", { 16,191,0 });
			gTextTexture.render(posxbase + 820, posybase + 598);
			gTextTexture.loadFromRenderedText(GOD.gFont, nextdate, { 255,255,255 });
			gTextTexture.render(posxbase + 1020, posybase + 615);
		}
		else
		{
			gTextTexture.loadFromRenderedText(GOD.gFont3, "Concluido", { 140,0,0 });
			gTextTexture.render(posxbase + 820, posybase + 598);
		}

		//use this to move the element
		{//draw caps Scroll
		int XS=100 , YS =0;
		SDL_Rect fillRect = {posxbase + 70+XS, posybase + 570+YS, 420, 35 };
			SDL_SetRenderDrawColor(GOD.gRenderer, 50, 50, 50, 80);
			SDL_RenderFillRect(GOD.gRenderer, &fillRect);
			if (capmore-2 >= mincapit) {
				gTextTexture.loadFromRenderedText(GOD.gFont3,  std::to_string(capmore-2), textColor);
				gTextTexture.render(posxbase + 150 +XS-gTextTexture.getWidth()/2, posybase + 558+YS);
			}
			if (capmore-1 >= mincapit) {
				gTextTexture.loadFromRenderedText(GOD.gFont3,  std::to_string(capmore-1), textColor);
				gTextTexture.render(posxbase + 215+XS-gTextTexture.getWidth()/2, posybase + 558+YS);
			}
			
			gTextTexture.loadFromRenderedText(GOD.gFont3, std::to_string(capmore), { 255, 255, 255 });
			gTextTexture.render(posxbase + 280+XS-gTextTexture.getWidth()/2, posybase + 558+YS);

			if (capmore+1 <= maxcapit) {
				gTextTexture.loadFromRenderedText(GOD.gFont3,  std::to_string(capmore+1), textColor);
				gTextTexture.render(posxbase + 345+XS-gTextTexture.getWidth()/2, posybase + 558+YS);
			}
			if (capmore+2 <= maxcapit) {
				gTextTexture.loadFromRenderedText(GOD.gFont3,  std::to_string(capmore+2), textColor);
				gTextTexture.render(posxbase + 410+XS-gTextTexture.getWidth()/2, posybase + 558+YS);
			}

			if (maxcapit >= 10){
				B_UP.render_T(280+XS, 530+YS,"+10");
				B_DOWN.render_T(280+XS, 630+YS,"-10");
			}
			B_LEFT.render_T(80+XS, 580+YS,std::to_string(mincapit),capmore == mincapit);
			B_RIGHT.render_T(480+XS, 580+YS,std::to_string(maxcapit),capmore == maxcapit);
		}
				
		//Draw Footer Buttons
		int dist = 1120,posdist = 160;
		B_A.render_T(dist, 680,"Ver Online");dist -= posdist;
		B_B.render_T(dist, 680,"Atras");dist -= posdist;
		if(gFAV){FAV.render_T(1190, 70,"");}
		else {B_Y.render_T(dist, 680,"Favorito");}
		}

		break;
		case programationstate:
			if (!reloading&&arraychapter.size()>=1) {
				{//Draw a rectagle to a nice view
				SDL_SetRenderDrawColor(GOD.gRenderer, 200, 200, 200, 105);
				SDL_Rect HeaderRect = {0,0, 620, 670};
				SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);}
				
				for (int x = 0; x < (int)arraychapter.size(); x++) {
					std::string temptext = arraychapter[x];
					replace(temptext, "https://jkanime.net/", "");
					replace(temptext, "/", " ");
					replace(temptext, "-", " ");
					mayus(temptext);

					if (x == selectchapter) {
						gTextTexture.loadFromRenderedText(GOD.digifont, temptext.substr(0,58), { 255,255,255 });
						{SDL_SetRenderDrawColor(GOD.gRenderer, 0, 0, 0, 105);
						SDL_Rect HeaderRect = {posxbase-2,posybase + (x * 22), 590, gTextTexture.getHeight()};
						SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);}
						gTextTexture.render(posxbase, posybase + (x * 22));

						Heart.render(posxbase - 18, posybase + 3 + (x * 22));
					}
					else
					{
						gTextTexture.loadFromRenderedText(GOD.digifont, temptext.substr(0,58), textColor);
						gTextTexture.render(posxbase, posybase + (x * 22));

					}

				}
				if (activatefirstimage)
				{
					TPreview.free();
					callimage(selectchapter);
					activatefirstimage = false;
				}
				if (preview)
				{
					
					{int ajuX = -390, ajuY = -450;
					SDL_Rect fillRect = { xdistance + 18 +ajuX, ydistance + 8  + ajuY, sizeportraity + 4, sizeportraitx + 24};
					SDL_SetRenderDrawColor(GOD.gRenderer, 0, 0, 0, 200);

					SDL_RenderFillRect(GOD.gRenderer, &fillRect);
					TPreview.render(posxbase + xdistance + ajuX, posybase + ydistance + ajuY);
					}
				}
				
				//Draw footer buttons
				int dist = 1100,posdist = 160;
				B_A.render_T(dist, 680,"Aceptar");dist -= posdist;
				B_B.render_T(dist, 680,"Buscar");dist -= posdist;
				B_L.render_T(dist, 680,"AnimeFLV");dist -= posdist;
				B_Y.render_T(dist, 680,"Favoritos");dist -= posdist;
			
				//Draw Header
				gTextTexture.loadFromRenderedText(GOD.gFont, "(Ver 1.8) #KASTXUPALO", {100,0,0});
				gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 30, 20);
				if (imgNumbuffer > 0){
					gTextTexture.loadFromRenderedText(GOD.gFont, "Imagenes: ("+std::to_string(imgNumbuffer)+"/30)", {0,100,0});
					gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 30, 40);
					Heart.render(posxbase + 570, posybase + 3 + (imgNumbuffer-1) * 22);
				}
				if (porcentajebuffer > 0){
					gTextTexture.loadFromRenderedText(GOD.gFont, "Buffering: ("+std::to_string(porcentajebuffer)+"/30)", {0,100,0});
					gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 30, 40);
					Heart.render(posxbase + 570, posybase + 3 + (porcentajebuffer-1) * 22);
					Heart.render(posxbase + 550, posybase + 3 + (porcentajebufferA-1) * 22);
				}



			}
			else
			{
				{SDL_Rect fillRect = { 0, SCREEN_HEIGHT/2 - 25, 1280, 50 };
				SDL_SetRenderDrawColor(GOD.gRenderer, 255, 255, 255, 255);

				SDL_RenderFillRect(GOD.gRenderer, &fillRect); }

				
				gTextTexture.loadFromRenderedText(GOD.gFont3, "Cargando programación... ", textColor);
				gTextTexture.render(SCREEN_WIDTH/2 - gTextTexture.getWidth()/2, SCREEN_HEIGHT/2 - gTextTexture.getHeight() / 2);
			}
			break;
		case searchstate:
			if (!reloadingsearch) {
				//Draw Header
				gTextTexture.loadFromRenderedText(GOD.gFont, "Busqueda", {100,0,0});
				gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 30, 20);
				if ((int)arraysearch.size() >= 1){
					
					{//Draw a rectagle to a nice view
					SDL_SetRenderDrawColor(GOD.gRenderer, 100, 100, 100, 105);
					SDL_Rect HeaderRect = {0,0, 620, 670};
					SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);}

					int of = searchchapter < 28 ? 0 : searchchapter - 26;
					if (arraysearch.size() > 30) {
						gTextTexture.loadFromRenderedText(GOD.gFont, std::to_string(searchchapter+1)+"/"+std::to_string(arraysearch.size()), {0,0,0});
						gTextTexture.render(400, 690);
					}
					for (int x = of; x < (int)arraysearch.size(); x++) {
						std::string temptext = arraysearch[x];
					
						replace(temptext, "https://jkanime.net/", "");
						replace(temptext, "/", " ");
						replace(temptext, "-", " ");
						mayus(temptext);
						if (x == searchchapter) {
							gTextTexture.loadFromRenderedText(GOD.digifont, temptext.substr(0,58), { 255,255,255 });
							{SDL_SetRenderDrawColor(GOD.gRenderer, 0, 0, 0, 105);
							SDL_Rect HeaderRect = {posxbase-2,posybase + ((x-of) * 22), 590, gTextTexture.getHeight()};
							SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);}
							gTextTexture.render(posxbase, posybase + ((x-of) * 22));

							Heart.render(posxbase - 18, posybase + 3 + ((x-of) * 22));
						}
						else if ((x-of)<30)
						{

							gTextTexture.loadFromRenderedText(GOD.digifont, temptext.substr(0,58), textColor);
							gTextTexture.render(posxbase, posybase + ((x-of) * 22));

						}

					}

					if (activatefirstsearchimage)
					{
						TSearchPreview.free();
						callimagesearch(searchchapter);
						activatefirstsearchimage = false;
					}
					if (preview)
					{
						{int ajuX = -390, ajuY = -450;
						SDL_Rect fillRect = { xdistance + 18 +ajuX, ydistance + 8  + ajuY, sizeportraity + 4, sizeportraitx + 4};
						SDL_SetRenderDrawColor(GOD.gRenderer, 0, 0, 0, 200);

						SDL_RenderFillRect(GOD.gRenderer, &fillRect);
						TSearchPreview.render(posxbase + xdistance + ajuX, posybase + ydistance + ajuY);}
						
					}
				}else NOP.render_T(230, 355,searchtext);
				
				{//Draw footer buttons
				int dist = 1100,posdist = 160;
				B_A.render_T(dist, 680,"Aceptar");dist -= posdist;
				B_B.render_T(dist, 680,"Atras");dist -= posdist;
				B_R.render_T(dist, 680,"Buscar");dist -= posdist;}
			}
			else
			{
				{SDL_Rect fillRect = { 0, SCREEN_HEIGHT / 2 - 25, 1280, 50 };
				SDL_SetRenderDrawColor(GOD.gRenderer, 255, 255, 255, 255);

				SDL_RenderFillRect(GOD.gRenderer, &fillRect); }

				textColor = { 50, 50, 50 };
				gTextTexture.loadFromRenderedText(GOD.gFont3, "Cargando búsqueda... (" + std::to_string(porcentajereload) + "%)", textColor);
				gTextTexture.render(SCREEN_WIDTH / 2 - gTextTexture.getWidth() / 2, SCREEN_HEIGHT / 2 - gTextTexture.getHeight() / 2);
			}
			break;
		case favoritesstate:
		{
			//Draw Header
			gTextTexture.loadFromRenderedText(GOD.gFont, "Favoritos", {100,0,0});
			gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 30, 20);
			if ((int)arrayfavorites.size() >= 1 ){

			{//Draw a rectagle to a nice view
			SDL_SetRenderDrawColor(GOD.gRenderer, 150, 150, 150, 105);
			SDL_Rect HeaderRect = {0,0, 620, 670};
			SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);}
			int of = favchapter < 28 ? 0 : favchapter - 26;
			if (arrayfavorites.size() > 30) {
				gTextTexture.loadFromRenderedText(GOD.gFont, std::to_string(favchapter+1)+"/"+std::to_string(arrayfavorites.size()), {0,0,0});
				gTextTexture.render(400, 690);
			}
			for (int x = of; x < (int)arrayfavorites.size(); x++) {
				std::string temptext = arrayfavorites[x];

				replace(temptext, "https://jkanime.net/", "");
				replace(temptext, "/", "");
//				std::string machu = rootdirectory+temptext+".jpg";
				replace(temptext, "-", " ");
					mayus(temptext);
					if (x == favchapter) {
//						CheckImgNet(machu);
//						tempimage = machu;
							gTextTexture.loadFromRenderedText(GOD.digifont, temptext.substr(0,58), { 255,255,255 });
							{SDL_SetRenderDrawColor(GOD.gRenderer, 0, 0, 0, 105);
							SDL_Rect HeaderRect = {posxbase-2,posybase + ((x-of) * 22), 590, gTextTexture.getHeight()};
							SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);}
							gTextTexture.render(posxbase, posybase + ((x-of) * 22));
							
							{int ajuX = -390, ajuY = -450;

							SDL_Rect HeaderRect = {posxbase + xdistance + ajuX+5,posybase + ydistance + ajuY+5, TFavorite.getWidth()+6, TFavorite.getHeight()+6};
							SDL_SetRenderDrawColor(GOD.gRenderer, 0, 0, 0, 100);
							
							SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);
							TFavorite.render(posxbase + xdistance + ajuX, posybase + ydistance + ajuY);

//							GOD.Cover(machu,610,scroll > 570 ? 570 : scroll ,"",200);
							Heart.render(posxbase - 18, posybase + 3 + ((x-of) * 22));}
					}
					else if((x-of) < 30)
					{
						gTextTexture.loadFromRenderedText(GOD.digifont, temptext.substr(0,58), textColor);
						gTextTexture.render(posxbase, posybase + ((x-of) * 22));
					}
				}
			}
			{//Draw footer buttons
				int dist = 1100,posdist = 160;
				B_A.render_T(dist, 680,"Aceptar");dist -= posdist;
				B_B.render_T(dist, 680,"Buscar");dist -= posdist;
				if ((int)arrayfavorites.size() >= 1){
					B_X.render_T(dist, 680,"Borrar #"+std::to_string(favchapter+1));dist -= posdist;
					B_ZR.render_T(dist, 680,"Limpiar");dist -= posdist;
				}else NOP.render_T(230, 355,"");
			}
			
		}
		break;
		case downloadstate:
			std::string temptext = urltodownload;
			replace(temptext, "https://jkanime.net/", "");
			replace(temptext, "/", " ");
			replace(temptext, "-", " ");
			mayus(temptext);
			gTextTexture.loadFromRenderedText(GOD.gFont, "Descargando actualmente:", textColor);
			gTextTexture.render(posxbase, posybase);
			gTextTexture.loadFromRenderedText(GOD.gFont3, temptext, textColor);
			gTextTexture.render(posxbase, posybase + 20);

			gTextTexture.loadFromRenderedText(GOD.gFont2, std::to_string(porcendown) + "\%", textColor);
			gTextTexture.render(posxbase + 40, posybase + 40);
			gTextTexture.loadFromRenderedText(GOD.gFont, "Peso estimado: " + std::to_string((int)(sizeestimated / 1000000)) + "mb.", textColor);
			gTextTexture.render(posxbase + 100, posybase + 220);


			gTextTexture.loadFromRenderedText(GOD.gFont, serverenlace, {168,0,0});
			gTextTexture.render(posxbase , posybase + 280);
			gTextTexture.loadFromRenderedText(GOD.gFont, "Usa el HomeBrew PPlay para reproducir el video.", textColor);
			gTextTexture.render(posxbase, posybase + 260);
			
			{	
				B_B.render_T(1000, 680,"Cancelar la descarga");
			}/*
			gTextTexture.loadFromRenderedText(GOD.gFont, "Cancelar la descarga \"B\" - \"+\" para salir", textColor);
			gTextTexture.render(posxbase, SCREEN_HEIGHT - 50);*/

			if (std::to_string(porcendown) == "100") {


#ifdef __SWITCH__
				if (!fulldownloaded)
				{
					//blinkLed(6);//LED
					fulldownloaded = true;
				}
#endif // __SWITCH__


				//Render red filled quad
				SDL_Rect fillRect = { posxbase + 98, posybase + 400, 580, 50 };
				SDL_SetRenderDrawColor(GOD.gRenderer, 255, 255, 255, 255);
				SDL_RenderFillRect(GOD.gRenderer, &fillRect);
				gTextTexture.loadFromRenderedText(GOD.gFont3, "¡Descarga Completada! Revisa tu SD.", textColor);
				gTextTexture.render(posxbase + 100, posybase + 400);
			}
			break;
		}

		B_P.render_T(160, 680,"Salir",quit);
		B_M.render_T(10, 680,"Música",Mix_PausedMusic() == 1);
		//Update screen
		SDL_SetRenderDrawBlendMode(GOD.gRenderer, SDL_BLENDMODE_BLEND);//enable alpha blend
		SDL_RenderPresent(GOD.gRenderer);


	}

	if (NULL == threadID) {
		printf("SDL_CreateThread failed: %s\n", SDL_GetError());
	}
	else {
		SDL_WaitThread(threadID, NULL);

	}
	if (NULL == prothread) {
		printf("SDL_CreateThread failed: %s\n", SDL_GetError());
	}
	else {
		SDL_WaitThread(prothread, NULL);

	}
	if (NULL == searchthread) {
		printf("SDL_CreateThread failed: %s\n", SDL_GetError());
	}
	else {
		SDL_WaitThread(searchthread,NULL);
		
	}
	//Free resources and close SDL
#ifdef __SWITCH__
	accountExit();
	hidsysExit();
	socketExit();
	romfsExit();
	
#endif // SWITCH
	
	//Free loaded images
	gTextTexture.free();
	Farest.free();
	Heart.free();
	TPreview.free();
	TChapters.free();
	TSearchPreview.free();
	B_A.free();
	B_B.free();
	B_Y.free();
	B_X.free();
	B_L.free();
	B_R.free();
	B_ZR.free();
	B_M.free();
	B_P.free();
	B_RIGHT.free();
	B_LEFT.free();
	B_UP.free();
	B_DOWN.free();
	FAV.free();
	NOP.free();	
	
	GOD.deint();

	return 0;
}