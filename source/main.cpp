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
#include <thread>
#include "Networking.hpp"
#include "SDLWork.hpp"
#include "applet.hpp"
#include "utils.hpp"

//make some includes to clean alittle the main
#include "JKanime.hpp"

//MAIN INT
int main(int argc, char **argv)
{
#ifdef __SWITCH__
	romfsInit();
	socketInitializeDefault();
	struct stat st = { 0 };
	nxlinkStdio();
	printf("printf output now goes to nxlink server\n");
	mkdir(rootdirectory.c_str(), 0777);
	mkdir((rootdirectory+"Video").c_str(), 0777);
	mkdir((rootdirectory+"DATA").c_str(), 0777);
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


	SDL_Thread* capithread = NULL;
	SDL_Thread* prothread = NULL;
	SDL_Thread* searchthread = NULL;
	SDL_Thread* downloadthread = NULL;
	
	//Set main Thread get images and descriptions
	prothread = SDL_CreateThread(refrescarpro, "prothread", (void*)NULL);

	//set custom music 
	GOD.intA();//init the SDL
#ifdef __SWITCH__

	if (isFileExist("texture.png")){
		Farest.loadFromFile("texture.png");
	} else {
		Farest.loadFromFile("romfs:/texture.png");
	}
	
	if (isFileExist("heart.png")){
		Heart.loadFromFile("heart.png");
	} else {
		Heart.loadFromFile("romfs:/heart.png");
	}

#else
	Farest.loadFromFile("C:\\respaldo2017\\C++\\test\\Debug\\texture.png");
	Heart.loadFromFile("C:\\respaldo2017\\C++\\test\\Debug\\heart.png");
#endif // SWITCH
	gTextTexture.mark=false;
	Farest.mark=false;

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
	SDL_Color textWhite = { 255, 255, 255 };
	SDL_Color textGray = { 200, 200, 200 };

	int posxbase = 20;
	int posybase = 10;

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
				quit = true;
				std::cout << "Saliendo" << std::endl;
			}
			//#include "keyboard.h"
#ifdef __SWITCH__
			GOD.GenState = statenow;
			switch (e.type) {
			case SDL_JOYAXISMOTION:
				//SDL_Log("Joystick %d axis %d value: %d\n",e.jaxis.which,e.jaxis.axis, e.jaxis.value);
				break;
			case SDL_FINGERDOWN:
			GOD.TouchX = e.tfinger.x * SCREEN_WIDTH;
			GOD.TouchY = e.tfinger.y * SCREEN_HEIGHT;
			break;
			case SDL_FINGERMOTION:
				if(GOD.TouchX > 55 && GOD.TouchX < 620 && statenow != chapterstate){
					
					//swipe down go up
					if(e.tfinger.dy * SCREEN_HEIGHT > 15)
					{
						e.jbutton.button = 15;
					}
					//swipe up go down
					else if(e.tfinger.dy * SCREEN_HEIGHT < -15)
					{
						e.jbutton.button = 13;
					} else {
						break;
					}
				SDL_Log("motion %f \n",e.tfinger.dy * SCREEN_HEIGHT);
				} else break;
			case SDL_FINGERUP:
			if (e.type == SDL_FINGERUP){
				GOD.TouchX = e.tfinger.x * SCREEN_WIDTH;
				GOD.TouchY = e.tfinger.y * SCREEN_HEIGHT;
				e.jbutton.button=-1;
				if (B_A.SP() || T_T.SP() || TChapters.SP() || TPreview.SP() || TSearchPreview.SP() || TFavorite.SP()) e.jbutton.button = 0;
				if (B_B.SP()) e.jbutton.button = 1;
				if (B_X.SP()) e.jbutton.button = 2;
				if (B_Y.SP()) e.jbutton.button = 3;
				if (B_L.SP()) e.jbutton.button = 6;
				if (B_R.SP()) e.jbutton.button = 7;
	//			if (B_ZR.SP()) e.jbutton.button = 9;
				if (B_P.SP()) e.jbutton.button = 10;
				if (B_M.SP()) e.jbutton.button = 11;
				if (B_LEFT.SP()) e.jbutton.button = 12;
				if (B_RIGHT.SP()) e.jbutton.button = 18;
				if (B_UP.SP()) e.jbutton.button = 13;
				if (B_DOWN.SP()) e.jbutton.button = 15;
				if (T_D.SP()&&isDownloading) statenow = downloadstate;
				
				SDL_Log("ScreenX %d    ScreenY %d butt %d\n",GOD.TouchX, GOD.TouchY,e.jbutton.button);
				GOD.TouchX = -1;
				GOD.TouchY = -1;
			}
			case SDL_JOYBUTTONDOWN :
				//SDL_Log("Joystick %d button %d down\n",e.jbutton.which, e.jbutton.button);
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
										rese = "......";
										nextdate = "......";
										maxcapit = -1;
										mincapit = 1;
										capmore = 1;
										generos = "......";
										capithread = SDL_CreateThread(capit, "capithread", (void*)NULL);									
									}else {
										try{
											rese = con_rese[selectchapter];
											nextdate = con_nextdate[selectchapter];
											enemision = con_enemision[selectchapter];
											tienezero = con_tienezero[selectchapter];
											maxcapit = con_maxcapit[selectchapter];
											generos = con_generos[selectchapter];
											printf("Goted All of sel %d\n",selectchapter);
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
									rese = "......";
									nextdate = "......";
									maxcapit = -1;
									mincapit = 1;
									capmore = 1;
									generos = "......";
									std::cout << temporallink << std::endl;
									capithread = SDL_CreateThread(capit, "capithread", (void*)NULL);
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
								statenow = chapterstate;
								temporallink = arrayfavorites[favchapter];

								std::cout << temporallink << std::endl;
								//init 
								rese = "......";
								nextdate = "......";
								maxcapit = -1;
								mincapit = 1;
								capmore = 0;
								generos = "......";
								capithread = SDL_CreateThread(capit, "capithread", (void*)NULL);
								gFAV = true;
								}
							}
							break;
							case chapterstate:
							if(serverpront){
								std::string tempurl = temporallink + std::to_string(capmore) + "/";
								if(!onlinejkanimevideo(tempurl,arrayservers[selectserver])){
									arrayservers.erase(arrayservers.begin()+selectserver);
								} else {
									serverpront = false;
								}
							}else {
								serverpront = true;
							}
							break;
						}
					}
					else if (e.jbutton.button == 10) {// (+) button down close to home menu
						cancelcurl = 1;
						quit = true;
					}
					else if (e.jbutton.button == 11) {// (-) button down
						if (Mix_PlayingMusic() == 0)
						{
							//Play the music
							Mix_PlayMusic(GOD.gMusic, -1);
							touch("play");
						}
						//If music is being played
						else
						{
							//If the music is paused
							if (Mix_PausedMusic() == 1)
							{
								//Resume the music
								Mix_ResumeMusic();
								touch("play");
								
							}
							//If the music is playing
							else
							{
								//Pause the music
								Mix_PauseMusic();
								remove("play");
							}
						}
					}
					else if (e.jbutton.button == 6 || e.jbutton.button == 8) {// (L & ZL) button down
						if (statenow == programationstate)
						{
							if (e.jbutton.button == 8)
								WebBrowserCall("https://jkanime.net",true);
							else 
								WebBrowserCall("https://animeflv.net",true);
						}
					}
					else if (e.jbutton.button == 9) {// (ZR) button down
						switch (statenow)
						{
						case favoritesstate:/*
							//please don't do-it
							delFavorite();
							TFavorite.free();
							favchapter=0;
							statenow = programationstate;
							arrayfavorites.clear();*/
							break;
						}
					}
					else if (e.jbutton.button == 1) {// (B) button down
						switch (statenow)
						{
						case downloadstate:
							statenow = chapterstate;
							break;
						case chapterstate:
							if(serverpront){
								serverpront=false;
								arrayservers=arrayserversbak;
							} else {
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
						case programationstate:
							if(isDownloading)
								statenow = downloadstate;
							break;
						case downloadstate:
							cancelcurl = 1;
							break;
						case chapterstate:
							statenow = downloadstate;
							cancelcurl = 0;
//							GOD.PleaseWait("Calculando Links Espere...");
							urltodownload  = temporallink + std::to_string(capmore) + "/";
							if(isDownloading){
								bool gogo = false;
								for (u64 x=0; x < downqueue.size();x++){
									if (downqueue[x] == urltodownload) gogo = true;
								}
								if(gogo) break;
								downqueue.push_back(urltodownload);
								logqueue.push_back(urltodownload);
							}else{
								downqueue.clear();
								downqueue.push_back(urltodownload);
								logqueue = downqueue;
								downloadthread = SDL_CreateThread(downloadjkanimevideo, "jkthread", (void*)NULL);
							}
							break;
						case searchstate:

							break;
						case favoritesstate:
							delFavorite(favchapter);
							TFavorite.free();
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
							callimagefavorites(favchapter);
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
							if(serverpront){
								arrayservers.push_back("test");
							} else {
								std::string tempurl = temporallink + std::to_string(capmore) + "/";
								WebBrowserCall(tempurl);
							}
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
								searchtext = KeyboardCall("Buscar Anime (2 letras minimo.)",searchtext);
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
							if(serverpront){serverpront = false;}
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
						
							if(serverpront){serverpront = false;}
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
							if(!serverpront){//selectserver
								if (capmore < maxcapit)
								{
									capmore = capmore + 10;
								}
								if (capmore > maxcapit)
								{
									capmore = maxcapit;
								}
							} else {
								if (selectserver > 0)
								{
									selectserver--;
								}
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
							if(!serverpront){//selectserver
								if (capmore > 1)
								{
									capmore = capmore - 10;
								}
								if (capmore < mincapit)
								{
									capmore = mincapit;
								}
							} else {
								if (selectserver < (int)arrayservers.size()-1)
								{
									selectserver++;
								}else if(serverpront){serverpront = false;selectserver=0;}

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

		//wallpaper
		Farest.render((0), (0));
		
		//render states
		switch (statenow)
		{
			case chapterstate:		{
			//Draw a background to a nice view
			VOX.render_VOX({0,0, SCREEN_WIDTH, SCREEN_HEIGHT} ,170, 170, 170, 100);
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
			VOX.render_VOX({25,60, 770, 340}, 255, 255, 255, 100);
			static std::string rese_prot = "..";
			if (rese_prot != rese){//load texture on text change 
				T_R.loadFromRenderedTextWrap(GOD.gFont, rese, textColor, 750);
				rese_prot = rese;
			}
			T_R.render(posxbase+15, posybase + 65);

			gTextTexture.loadFromRenderedTextWrap(GOD.gFont3, generos, textColor,750);
			gTextTexture.render(posxbase+25, posybase + 380-gTextTexture.getHeight());
			}
			
			{//draw back rectangle
			VOX.render_VOX({ SCREEN_WIDTH - 442,SCREEN_HEIGHT / 2 - 302, 404, 595 }, 0, 0, 0, 200);
			//draw preview image
			TChapters.render(SCREEN_WIDTH - 440, SCREEN_HEIGHT / 2 - 300);
			}
			
			if (maxcapit >= 0){
				
				if (nextdate == "Pelicula"){
					gTextTexture.loadFromRenderedText(GOD.gFont3, "Pelicula", { 250,250,250 });
					gTextTexture.render(posxbase + 820, posybase + 598);
				}else {
					if (enemision)
					{
						gTextTexture.loadFromRenderedText(GOD.gFont3, "En Emisión ", { 16,191,0 });
						gTextTexture.render(posxbase + 820, posybase + 598);
					}
					else
					{
						gTextTexture.loadFromRenderedText(GOD.gFont3, "Concluido", { 140,0,0 });
						gTextTexture.render(posxbase + 820, posybase + 598);
					}
					gTextTexture.loadFromRenderedText(GOD.gFont, nextdate, { 255,255,255 });
					gTextTexture.render(posxbase + 1020, posybase + 615);
				}

				int sizefix = 0;
				sizefix = (int)arrayservers.size() * 52;
				bool anend = VOX.render_AH(310, 610, 190, sizefix, serverpront);
				if(serverpront){
					if (anend){
						for (int x = 0; x < (int)arrayservers.size(); x++) {
							if (x == selectserver){
								T_T.loadFromRenderedText(GOD.gFont3, arrayservers[x], textWhite);
								VOX.render_VOX({ posxbase+300,615 - sizefix + (x * 52), 170, T_T.getHeight()-5}, 50, 50, 50, 200);
								T_T.render(posxbase+310, 610 - sizefix + (x * 52));
								} else {
								gTextTexture.loadFromRenderedText(GOD.gFont3, arrayservers[x],textGray);
								gTextTexture.render(posxbase+310, 610 - sizefix + (x * 52));
							}
						}
					}
				}
			}
			//use this to move the element
			int XS=100 , YS =30;
			if(serverpront) B_DOWN.render_T(280+XS, 630+YS,"");
			if (maxcapit >= 0&&nextdate != "Pelicula"){//draw caps Scroll
				VOX.render_VOX({posxbase + 70+XS, posybase + 570+YS, 420, 35 }, 50, 50, 50, 200);
				if (capmore-2 >= mincapit) {
					gTextTexture.loadFromRenderedText(GOD.gFont3,  std::to_string(capmore-2), textGray);
					gTextTexture.render(posxbase + 150 +XS-gTextTexture.getWidth()/2, posybase + 558+YS);
				}
				if (capmore-1 >= mincapit) {
					gTextTexture.loadFromRenderedText(GOD.gFont3,  std::to_string(capmore-1), textGray);
					gTextTexture.render(posxbase + 215+XS-gTextTexture.getWidth()/2, posybase + 558+YS);
				}
				
				if (serverpront){
					gTextTexture.loadFromRenderedText(GOD.gFont3, std::to_string(capmore), { 255, 255, 255 });
					gTextTexture.render(posxbase + 280+XS-gTextTexture.getWidth()/2, posybase + 558+YS);
				} else {
					T_T.loadFromRenderedText(GOD.gFont3, std::to_string(capmore), { 255, 255, 255 });
					T_T.render(posxbase + 280+XS-T_T.getWidth()/2, posybase + 558+YS);
				}

				if (capmore+1 <= maxcapit) {
					gTextTexture.loadFromRenderedText(GOD.gFont3,  std::to_string(capmore+1), textGray);
					gTextTexture.render(posxbase + 345+XS-gTextTexture.getWidth()/2, posybase + 558+YS);
				}
				if (capmore+2 <= maxcapit) {
					gTextTexture.loadFromRenderedText(GOD.gFont3,  std::to_string(capmore+2), textGray);
					gTextTexture.render(posxbase + 410+XS-gTextTexture.getWidth()/2, posybase + 558+YS);
				}

				if (maxcapit >= 10 && !serverpront){
					B_UP.render_T(280+XS, 530+YS,"+10",serverpront);
					B_DOWN.render_T(280+XS, 630+YS,"-10",serverpront);
				}
				
				B_LEFT.render_T(75+XS, 580+YS,std::to_string(mincapit),capmore == mincapit);
				B_RIGHT.render_T(485+XS, 580+YS,std::to_string(maxcapit),capmore == maxcapit);
			} else {
				VOX.render_VOX({posxbase + 185+XS, posybase + 570+YS, 200, 35 }, 50, 50, 50, 200);
				if (nextdate == "Pelicula"){
					T_T.loadFromRenderedText(GOD.gFont3, "Reproducir...", { 255, 255, 255 });
					T_T.render(posxbase + 282+XS-T_T.getWidth()/2, posybase + 558+YS);
				} else {
					gTextTexture.loadFromRenderedText(GOD.gFont3, "Cargando...", { 255, 255, 255 });
					gTextTexture.render(posxbase + 282+XS-gTextTexture.getWidth()/2, posybase + 558+YS);
				}
				
			}
					
			//Draw Footer Buttons
			int dist = 1100,posdist = 160;
			if(serverpront){
				B_A.render_T(dist, 680,"Ver Online");dist -= posdist;
				B_B.render_T(dist, 680,"Cerrar");dist -= posdist;
			} else {
				B_A.render_T(dist, 680,"Seleccionar");dist -= posdist;
				B_B.render_T(dist, 680,"Atras");dist -= posdist;
				B_X.render_T(dist, 680,"Descargar");dist -= posdist;
			}

			if(gFAV){FAV.render_T(1190, 70,"");}
			else {B_Y.render_T(dist, 680,"Favorito");}
			break;
			}
			case programationstate:	{
				if (!reloading&&arraychapter.size()>=1) {
					VOX.render_VOX({0,0, 620, 670}, 200, 200, 200, 105);//Draw a rectagle to a nice view

					if(GOD.TouchY < 670 && GOD.TouchX < 530 && GOD.TouchY > 5 && GOD.TouchX > 15){
						u32 sel=(GOD.TouchY*30/660);
						if (sel >= 0 && sel < arraychapter.size()){
							selectchapter = sel;
							activatefirstimage=true;
						} 
					}
					
					for (int x = 0; x < (int)arraychapter.size(); x++) {
						std::string temptext = arraychapter[x];
						replace(temptext, "https://jkanime.net/", "");
						replace(temptext, "/", " ");
						replace(temptext, "-", " ");
						mayus(temptext);

						if (x == selectchapter) {
							T_T.loadFromRenderedText(GOD.digifont, temptext.substr(0,58), { 255,255,255 });
							VOX.render_VOX({posxbase-2,posybase + (x * 22), 590, T_T.getHeight()}, 0, 0, 0, 105);
							T_T.render(posxbase, posybase + (x * 22));

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
						{int ajuX = -350, ajuY = -450;
						VOX.render_VOX({ xdistance + 18 +ajuX, ydistance + 8  + ajuY, sizeportraity + 4, sizeportraitx + 4}, 0, 0, 0, 200);
						TPreview.render(posxbase + xdistance + ajuX, posybase + ydistance + ajuY);
						}
					}

					//Draw Header
					gTextTexture.loadFromRenderedText(GOD.gFont, "(Ver 1.8.3) #KASTXUPALO", {100,0,0});
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

					//Draw footer buttons
					int dist = 1100,posdist = 160;
					B_A.render_T(dist, 680,"Aceptar");dist -= posdist;
					B_R.render_T(dist, 680,"Buscar");dist -= posdist;
					B_L.render_T(dist, 680,"AnimeFLV");dist -= posdist;
					B_Y.render_T(dist, 680,"Favoritos");dist -= posdist;
					if(isDownloading) {B_X.render_T(dist, 680,"Descargas");dist -= posdist;}
					B_UP.render_T(580, 5,"");
					B_DOWN.render_T(580, 630,"");
				}
				else
				{
					GOD.PleaseWait("Cargando programación... ",false);
				}
				break;
			}
			case searchstate:		{
				if (!reloadingsearch) {
					//Draw Header
					gTextTexture.loadFromRenderedText(GOD.gFont, "Busqueda", {100,0,0});
					gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 30, 20);

					if(GOD.TouchY < 670 && GOD.TouchX < 530 && GOD.TouchY > 1 && GOD.TouchX > 15){
						u32 sel=(GOD.TouchY*30/670);
						if (sel >= 0 && sel < arraysearch.size()){
							searchchapter = sel;
							TSearchPreview.free();
							callimagesearch(searchchapter);					
						}
					}
					if ((int)arraysearch.size() >= 1){
						
						VOX.render_VOX({0,0, 620, 670}, 100, 100, 100, 105);

						int of = searchchapter < 30 ? 0 : searchchapter - 26;
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
								T_T.loadFromRenderedText(GOD.digifont, temptext.substr(0,58), { 255,255,255 });
								VOX.render_VOX({posxbase-2,posybase + ((x-of) * 22), 590, T_T.getHeight()}, 0, 0, 0, 105);
								T_T.render(posxbase, posybase + ((x-of) * 22));

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
							{int ajuX = -350, ajuY = -450;
							VOX.render_VOX({ xdistance + 18 +ajuX, ydistance + 8  + ajuY, sizeportraity + 4, sizeportraitx + 4}, 0, 0, 0, 200);
							TSearchPreview.render(posxbase + xdistance + ajuX, posybase + ydistance + ajuY);}
							
						}
					}else NOP.render_T(230, 355,searchtext);
					
					{//Draw footer buttons
					int dist = 1100,posdist = 160;
					B_A.render_T(dist, 680,"Aceptar");dist -= posdist;
					B_B.render_T(dist, 680,"Atras");dist -= posdist;
					B_R.render_T(dist, 680,"Buscar");dist -= posdist;}
					B_UP.render_T(580, 5,"");
					B_DOWN.render_T(580, 630,"");
				}
				else
				{
					GOD.PleaseWait("Cargando búsqueda... (" + std::to_string(porcentajereload) + "%)",false);
				}
				break;
			}
			case favoritesstate:	{
				//Draw Header
				gTextTexture.loadFromRenderedText(GOD.gFont, "Favoritos", {100,0,0});
				gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 30, 20);
				
				if(GOD.TouchY < 670 && GOD.TouchX < 530 && GOD.TouchY > 5 && GOD.TouchX > 15){
					u32 sel=(GOD.TouchY*30/660);
					if (sel >= 0 && sel < arrayfavorites.size()){
						favchapter = sel;
						TFavorite.free();
						callimagefavorites(favchapter);
					}
				}

				if ((int)arrayfavorites.size() >= 1 ){
				VOX.render_VOX({0,0, 620, 670}, 150, 150, 150, 105);
				int of = favchapter < 30 ? 0 : favchapter - 26;
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
								T_T.loadFromRenderedText(GOD.digifont, temptext.substr(0,58), { 255,255,255 });
								VOX.render_VOX({posxbase-2,posybase + ((x-of) * 22), 590, T_T.getHeight()}, 0, 0, 0, 105);
								T_T.render(posxbase, posybase + ((x-of) * 22));

								{int ajuX = -350, ajuY = -450;
								VOX.render_VOX({posxbase + xdistance + ajuX -2,posybase + ydistance + ajuY -2, TFavorite.getWidth()+4, TFavorite.getHeight()+4}, 0, 0, 0, 200);
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
					B_B.render_T(dist, 680,"Volver");dist -= posdist;
					if ((int)arrayfavorites.size() >= 1){
						B_X.render_T(dist, 680,"Borrar #"+std::to_string(favchapter+1));dist -= posdist;
					}else NOP.render_T(230, 355,"");
					
					B_UP.render_T(580, 5,"");
					B_DOWN.render_T(580, 630,"");
				}
			break;
			}
			case downloadstate:		{
				gTextTexture.loadFromRenderedText(GOD.gFont, "Descargando Actualmente:", textColor);
				gTextTexture.render(posxbase, posybase);
				gTextTexture.loadFromRenderedText(GOD.gFont3, DownTitle, textColor);
				gTextTexture.render(posxbase, posybase + 20);

				gTextTexture.loadFromRenderedText(GOD.gFont, serverenlace, {168,0,0});
				gTextTexture.render(posxbase , posybase + 280);
				if (serverenlace != "Error de descarga"){
					gTextTexture.loadFromRenderedText(GOD.gFont2, std::to_string(porcendown) + "\%", textColor);
					gTextTexture.render(posxbase + 40, posybase + 40);

					gTextTexture.loadFromRenderedText(GOD.gFont, "Peso estimado: " + std::to_string((int)(sizeestimated / 1000000)) + "mb.", textColor);
					gTextTexture.render(posxbase + 100, posybase + 220);
					

					gTextTexture.loadFromRenderedText(GOD.gFont, "Usa el HomeBrew PPlay para reproducir el video.", textColor);
					gTextTexture.render(posxbase, posybase + 260);

					if (std::to_string(porcendown) == "100"&&!isDownloading) {
						//Render red filled quad
						VOX.render_VOX({ posxbase + 98, posybase + 400, 580, 50 }, 255, 255, 255, 255);
						gTextTexture.loadFromRenderedText(GOD.gFont3, "¡Descarga Completada! Revisa tu SD.", textColor);
						gTextTexture.render(posxbase + 100, posybase + 400);
					 }else{
						VOX.render_VOX({ posxbase + 340, posybase + 150, 500, 50 }, 255, 255, 255, 145);
						gTextTexture.loadFromRenderedText(GOD.gFont3, "Velocidad: " +speedD, textColor);
						gTextTexture.render(posxbase + 350, posybase + 150);
						
						gTextTexture.loadFromRenderedText(GOD.gFont3, "M/S", textColor);
						gTextTexture.render(posxbase + 770, posybase + 150);
					 }
				} else {
					porcendown=0;
				}
				
				VOX.render_VOX({posxbase-5,posybase + 300 , 750, ((int)logqueue.size() * 22)+53}, 200, 200, 200, 105);
				gTextTexture.loadFromRenderedText(GOD.digifont, "Cola De Descarga::", textColor);
				gTextTexture.render(posxbase, posybase+310);
				for (u64 x = 0; x < logqueue.size(); x++) {
					std::string descarga = logqueue[x];
					replace(descarga, "https://jkanime.net/", "");
					replace(descarga, "/", " ");
					replace(descarga, "-", " ");
					mayus(descarga);
					SDL_Color txtColor = textColor;//{ 50, 50, 50 };
					
					if(descarga.substr(0,3) == "100") txtColor = { 50, 150, 50 };
					if(descarga.substr(0,3) == "Err") txtColor = { 150, 50, 50 };
					if(descarga.substr(0,3) == ">>>") txtColor = { 0, 0, 0 };

					gTextTexture.loadFromRenderedText(GOD.digifont, descarga, txtColor);
					gTextTexture.render(posxbase, posybase+350 + ((x) * 22));
					
				}
				
				if(isDownloading)
				B_X.render_T(800, 680,"Cancelar la descarga");
				B_B.render_T(1100, 680,"Volver");
			break;
			}
		}
		//global render
		if(isDownloading&& downloadstate != statenow){
			int het=40;
			T_D.loadFromRenderedText(GOD.digifont, "Downloading: "+DownTitle.substr(0,22)+"... ("+std::to_string(porcendown)+"\%)", {100,100,0});
			if (statenow == programationstate){
				het = porcentajebuffer > 0 ? T_D.getHeight()+42 : 40;
			}
			if (statenow == chapterstate){
				het=10;
			}
			T_D.render(SCREEN_WIDTH - T_D.getWidth() - 30, het);
		}
		if (AppletMode) GOD.PleaseWait("Esta App No funciona en Modo Applet. Pulsa R Al Abrir un Juego",false);
		B_P.render_T(160, 680,"Salir",quit);
		B_M.render_T(10, 680,"Música",(Mix_PausedMusic() == 1 || Mix_PlayingMusic() == 0));
		SDL_SetRenderDrawBlendMode(GOD.gRenderer, SDL_BLENDMODE_BLEND);//enable alpha blend
		
		if (!HasConnection()) {
			GOD.PleaseWait("No Hay Red Conectada, Esperando por la red");
			SDL_Delay(1000);
		}
		
		//Update screen
		SDL_RenderPresent(GOD.gRenderer);
	}



	if (NULL == capithread) {
		printf("SDL_CreateThread failed: %s\n", SDL_GetError());
	}
	else {
		SDL_WaitThread(capithread, NULL);
	}
	if (NULL == downloadthread) {
		printf("SDL_CreateThread failed: %s\n", SDL_GetError());
	}
	else {
		SDL_WaitThread(downloadthread, NULL);
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