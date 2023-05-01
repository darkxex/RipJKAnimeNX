#include <string>
#include <math.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <vector>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <switch.h>
#include <thread>
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "applet.hpp"
#include "utils.hpp"
#include "Networking.hpp"
#include "Link.hpp"
#include "SDLWork.hpp"
#include "input.hpp"
#include "JKanime.hpp"

//temporal
extern LTexture SCREEN, BUS, BUSB, TOP, TOPB, DOW, DOWB, HOR, HORB, NFAV, FAV, FAVB, REC, BACK, USER, HIS, HISB, AFLV, NOP, CLEAR;
extern LTexture VOX, T_T, T_N, T_D, T_R;
extern SDL_Thread* searchthread;
extern SDL_Thread* downloadthread;
extern SDL_Thread* capithread;
extern SDL_Thread* Loaderthread;

extern std::vector<std::string> Wday;
extern int WdayG;
extern bool isHandheld;

extern bool gFAV;
extern bool lcdoff;
extern bool serverpront;
extern bool reloadingsearch;

extern int selectelement;
extern int selectserver;
extern int searchchapter;
extern int favchapter;
extern int histchapter;
extern int agregadosidx;
extern int hourchapter;
extern int topchapter;

extern int bannersize;
extern int bannersel;
extern bool Btimer;

extern std::string urltodownload;
extern std::vector<std::string> StatesList;
extern bool ongrid;

//Event handler
SDL_Event e;

//Private
void PlayerGet(){
	if (user::SelectUser()) {
		//if select a new user save old user data
		write_DB(UD,rootsave+"UserData.json");
		user::MountUserSave();
		USER.loadFromFileCustom(rootsave+"User.jpg",58, 58);
		UD = "{}"_json;
		read_DB(UD,rootsave+"UserData.json");
		GOD.loadSkin();
		if(statenow==chapter_s) {
			capBuffer(BD["com"]["ActualLink"]);
			gFAV = isFavorite(BD["com"]["ActualLink"]);
		}
		Frames=1;
	}
	USER.offboom_size=3;
	USER.TickerBomb();
}

//call states
void callmenuslide(){
	statenow=menu_s;
	selectelement = 2;
}
void callsearch(){
	GOD.WorKey="0"; GOD.MasKey=-1;
	if (!reloadingsearch)
	{
		if (!isset(BD,"searchtext")) {BD["searchtext"]="";}
		BD["searchtext"] = KeyboardCall("Buscar el Anime",BD["searchtext"]);
		if ((BD["searchtext"].get<std::string>()).length() > 0) {
			searchchapter = 0;
			reloadingsearch = true;
			statenow = search_s;
			returnnow = search_s;
			searchthread = SDL_CreateThread(searchjk, "searchthread", (void*)NULL);
		}
	}
}
void callfavs(){
	GOD.WorKey="0"; GOD.MasKey=-1;
	if (!reloading) {
		if (UD["favoritos"].size()>0) {
			getFavorite();
			returnnow = favorites_s;
			statenow = favorites_s;
			Frames=1;
		}
	}
}
void callAflv(){
	GOD.WorKey="0"; GOD.MasKey=-1;
	statenow=programation_s;
	WebBrowserCall("https://animeflv.net",true);
}
void callhistory(){
	GOD.WorKey="0"; GOD.MasKey=-1;
	if (UD["history"].size()>0) {
		statenow = history_s;
		returnnow = statenow;
		Frames=1;
	}
}
void calltop(){
	GOD.WorKey="0"; GOD.MasKey=-1;
	statenow = top_s;
	returnnow = statenow;
	Frames=1;
}
void callagr(){
	GOD.WorKey="0"; GOD.MasKey=-1;
	statenow = agregados_s;
	returnnow = statenow;
	Frames=1;
}
void callhourglass(){
	GOD.WorKey="0"; GOD.MasKey=-1;
	statenow = hourglass_s;
	returnnow = statenow;
	Frames=1;
}


void Inputinit(){
	for (int i = 0; i < 2; i++) {
		if (SDL_JoystickOpen(i) == NULL) {
			SDL_Log("SDL_JoystickOpen: %s\n", SDL_GetError());
			SDL_Quit();
			quit=true;
		}
	}
}

//Public
void InputHandle(){
	//Handle events on queue
	while (SDL_PollEvent(&e))
	{
		//User requests quit
		if (e.type == SDL_QUIT)
		{
			cancelcurl = 1;
			quit = true;
			cout << "Saliendo" << endl;
		}
		GOD.GenState = statenow;
		switch (e.type) {
		case SDL_FINGERDOWN:
			GOD.TouchX = e.tfinger.x * SCREEN_WIDTH;
			GOD.TouchY = e.tfinger.y * SCREEN_HEIGHT;
			if (!GOD.fingerdown) {GOD.fingerdown = true; printf("F Down\n");}
			break;
		case SDL_FINGERMOTION:
			if(e.tfinger.dy * SCREEN_HEIGHT > 30 || e.tfinger.dy * SCREEN_HEIGHT < -30 || e.tfinger.dx * SCREEN_WIDTH > 30 || e.tfinger.dx * SCREEN_WIDTH < -30) {
				SDL_Log("motion %f \n",e.tfinger.dy * SCREEN_HEIGHT);
				if(!GOD.fingermotion) {
					//swipe up go down
					if(e.tfinger.dy * SCREEN_HEIGHT > 30 )
					{
						GOD.fingermotion_DOWN = true;
					}
					//swipe down go up
					else if(e.tfinger.dy * SCREEN_HEIGHT < -30 )
					{
						GOD.fingermotion_UP = true;
					}

					//left right
					if(e.tfinger.dx * SCREEN_WIDTH > 30 )
					{
						GOD.fingermotion_RIGHT = true;
					}
					else if(e.tfinger.dx * SCREEN_WIDTH < -30 )
					{
						GOD.fingermotion_LEFT = true;
					}

				} else {
					GOD.fingermotion_DOWN = false;
					GOD.fingermotion_UP = false;
					GOD.fingermotion_LEFT = false;
					GOD.fingermotion_RIGHT = false;
					GOD.TouchX = -1;
					GOD.TouchY = -1;
				}
				GOD.fingermotion=true;
			}
			break;
		case SDL_FINGERUP:
			if (e.type == SDL_FINGERUP) {
				if(lcdoff) {
					lcdoff=false;
					appletSetLcdBacklightOffEnabled(lcdoff);
				} else{
					printf("F UP\n");


					GOD.fingerdown = false;
					if(GOD.fingermotion) {
						GOD.TouchXU=-1;
						GOD.TouchYU=-1;
					} else {
						GOD.TouchXU = e.tfinger.x * SCREEN_WIDTH;
						GOD.TouchYU = e.tfinger.y * SCREEN_HEIGHT;
					}
					GOD.fingermotion=false;
					e.jbutton.button=-1;
					if (BUSB.SPr()) {callsearch();}
					else if (FAVB.SPr()) {callfavs();}
					else if (AFLV.SPr()) {callAflv();}
					else if (HISB.SPr()) {callhistory();}
					else if (NFAV.SPr() && !gFAV) {
						GOD.WorKey="0"; GOD.MasKey=-1;
						{                //AGREGAR A FAVORITOS
							addFavorite(BD["com"]["ActualLink"]);
							gFAV = true;
							FAV.TickerBomb();
							GOD.TouchX = -1;
							GOD.TouchY = -1;
						}
						break;
					}
					else if (GOD.MasKey >=0) {

						if(GOD.MapT[GOD.WorKey].SPr()) {
							e.jbutton.button=GOD.MasKey;
						}
						GOD.WorKey="0"; GOD.MasKey=-1;
					}
					else if (GOD.MapT["EXIT"].SPr()) {cancelcurl = 1;quit = true;}//e.jbutton.button = BT_B ? BT_p;
					else if (GOD.MapT["MUSIC"].SPr()) e.jbutton.button = BT_M;

					else if (USER.SPr()) {PlayerGet();}
					else if (B_A.SPr()) {e.jbutton.button = BT_A; B_A.TickerBomb();}
					else if (T_T.SPr() ) e.jbutton.button = BT_A;
					else if (B_B.SPr()) e.jbutton.button = BT_B;
					else if (BACK.SPr()) e.jbutton.button = BT_B;
					else if (B_X.SPr()) e.jbutton.button = BT_X;
					else if (B_Y.SPr()) e.jbutton.button = BT_Y;
					else if (B_L.SPr()) e.jbutton.button = BT_L;
					else if (B_R.SPr()) e.jbutton.button = BT_R;
					else if (B_ZR.SPr()) e.jbutton.button = BT_ZR;
					else if (B_ZL.SPr()) e.jbutton.button = BT_ZL;
					else if (B_P.SPr()) e.jbutton.button = BT_P;
					else if (B_M.SPr()) e.jbutton.button = BT_M;
					else if (B_LEFT.SPr()) e.jbutton.button = BT_LEFT;
					else if (B_RIGHT.SPr()) e.jbutton.button = BT_RIGHT;
					else if (B_UP.SPr()) e.jbutton.button = BT_UP;
					else if (B_DOWN.SPr()) e.jbutton.button = BT_DOWN;
					else if (T_D.SPr()&&isDownloading) statenow = download_s;
					else if (SCREEN.SPr()) e.jbutton.button = BT_ZR;
					else if (CLEAR.SPr()) {
						if (GOD.Confirm("Desea Borrar todos los datos que usa esta App, excepto los datos por usuario")) {
							preview = false;
							quit = true;
							cancelcurl = 1;
							GOD.PleaseWait("Borrando cache");
							read_DB(AB,"romfs:/AnimeBase.json");
							BD="{}"_json;
							mount_theme("themes",false);
							fsdevDeleteDirectoryRecursively((rootdirectory).c_str());
							statenow=99;
						}

						break;
					}
					SDL_Log("ScreenX %d    ScreenY %d butt %d\n",GOD.TouchX, GOD.TouchY,e.jbutton.button);
				}
				GOD.TouchX = -1;
				GOD.TouchY = -1;
			}
		case SDL_JOYAXISMOTION:
			if (e.type == SDL_JOYAXISMOTION)
			{
				if (e.jaxis.axis == 1 && statenow==chapter_s) {
					e.jbutton.button=-1;
					//SDL_Log("Joystick %d axis %d value: %d\n",e.jaxis.which,e.jaxis.axis, e.jaxis.value);
					if (e.jaxis.value < -22000) {
						e.jbutton.button = BT_UP;
					}
					if (e.jaxis.value > 22000) {
						e.jbutton.button = BT_DOWN;
					}
				} else break;

			}
		case SDL_JOYBUTTONDOWN:
			//SDL_Log("Joystick %d button %d down\n",e.jbutton.which, e.jbutton.button);
			// https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L52
			// seek for joystick #0
			if (e.jbutton.which == 0) {
				if (e.jbutton.button == BT_A) {                // (A) button down

					switch (statenow)
					{
					case menu_s:
						if(selectelement==0) {callsearch();}
						if(selectelement==1) {callhistory();}
						if(selectelement==2) {callfavs();}
						if(selectelement==3) {callhourglass();}
						if(selectelement==4) {calltop();}
						if(selectelement==5) {callagr();}
						if(selectelement==6) {callAflv();}
						if(selectelement==7) {if(isDownloading) {statenow = download_s;}}
						break;
					case programation_s:
					{
						if (!reloading&&BD["arrays"]["chapter"]["link"].size()>=1)
						{
							capBuffer(BD["arrays"]["chapter"]["link"][selectchapter]);
							gFAV = isFavorite(BD["com"]["ActualLink"]);
						}
					}
					break;

					case search_s:
					{
						if (!reloadingsearch && BD["arrays"]["search"]["link"].size()>=1)
						{
							capBuffer(BD["arrays"]["search"]["link"][searchchapter]);
							gFAV = isFavorite(BD["com"]["ActualLink"]);
						}

					}
					break;
					case hourglass_s:
					{
						if (BD["arrays"]["HourGlass"]["link"].size()>0)
						{
							capBuffer(BD["arrays"]["HourGlass"]["link"][hourchapter]);
							gFAV = isFavorite(BD["com"]["ActualLink"]);
						}
					}
					break;
					case top_s:
					{
						if (BD["arrays"]["Top"]["link"].size()>0)
						{
							capBuffer(BD["arrays"]["Top"]["link"][topchapter]);
							gFAV = isFavorite(BD["com"]["ActualLink"]);
						}
					}
					break;
					case agregados_s:
					{
						if (BD["arrays"]["Agregados"]["link"].size()>0)
						{
							capBuffer(BD["arrays"]["Agregados"]["link"][agregadosidx]);
							gFAV = isFavorite(BD["com"]["ActualLink"]);
						}
					}
					break;
					case history_s:
					{
						if (UD["history"].size()>0)
						{
							capBuffer(UD["history"][histchapter]);
							gFAV = isFavorite(BD["com"]["ActualLink"]);
						}

					}
					break;

					case favorites_s:
					{
						if ((int)UD["favoritos"].size() >= 1 ) {
							capBuffer(UD["favoritos"][favchapter]);
							gFAV = true;
						}
					}
					break;
					case chapter_s:
						if(serverpront) {
                            if (arrayservers.size() == 0) break;
							string tempurl = BD["com"]["ActualLink"].get<string>() + to_string(latest) + "/";
							string temp = tempurl;
							NameOfLink(temp);
							string text = "Cargando "+temp.substr(0,42)+"... desde "+arrayservers[selectserver]+" ...";
							GOD.PleaseWait(text);
							if(!onlinejkanimevideo(tempurl,arrayservers[selectserver])) {
								arrayservers.erase(arrayservers.begin()+selectserver);
							} else {
								serverpront = false;
								UD["chapter"][KeyName]["latest"] = latest;
								latestcolor = latest;

								string item=BD["com"]["ActualLink"].get<string>();
								int hsize = UD["history"].size();
//										if (hsize > 49) {UD["history"].erase(UD["history"].end());}//limit history
								if (hsize > 0) {
									UD["history"] = eraseVec(UD["history"],item);
									//UD["history"].erase(remove(UD["history"].begin(), UD["history"].end(), item), UD["history"].end());
									UD["history"].insert(UD["history"].begin(),tempurl);
								} else {
									UD["history"].push_back(tempurl);
								}
							}
						} else {
							if (isConnected) serverpront = true;
							T_N.TickerBomb();
						}
						break;
					}
				}
				if (e.jbutton.button == BT_P) {                // (+) button down close to home menu
                    //mimetizá
					switch (statenow)
					{
					case programation_s:
						callmenuslide();
						break;                        
					default:
						returnnow = programation_s;
						statenow = programation_s;
						break;
                    }
				}
				if (e.jbutton.button == BT_M) {                // (-) button down
					GOD.SwapMusic();
				}
				if (e.jbutton.button == BT_L) {                // (L) button down
					if (statenow == chapter_s) {
						if (isset(AB["AnimeBase"][KeyName],"Precuela")) {
							if (!serverpront) {
								capBuffer(AB["AnimeBase"][KeyName]["Precuela"]);
								gFAV = isFavorite(BD["com"]["ActualLink"]);
							} else {
								serverpront=false;
							}
						}
					}

					if (statenow == programation_s)
					{
						callAflv();
					}
					if (statenow == hourglass_s) {
						if (WdayG > 0) {
							WdayG--;
							BD["arrays"]["HourGlass"]["link"] = BD["arrays"]["HourGlass"][Wday[WdayG]];
						}
					}
					if (statenow == menu_s)
					{
						Btimer=false;
						bannersel--;
						if (bannersel<0) bannersel=bannersize;
					}
				}
				if (e.jbutton.button == BT_ZL) {                // (ZL) button down
					switch (statenow)
					{
					case programation_s:
						WebBrowserCall(urlc,true);
						break;
					case chapter_s:
					case menu_s:
						GOD.selectskin();
						break;
					}
				}
				if (e.jbutton.button == BT_ZR) {                // (ZR) button down
					if(isDownloading && isHandheld) {
						lcdoff = !lcdoff;
						appletSetLcdBacklightOffEnabled(lcdoff);
					}

					switch (statenow) {
					case programation_s:
					case menu_s:
					case chapter_s:
					case search_s:
					case favorites_s:
					case history_s:
					case hourglass_s:
					case top_s:
					case agregados_s:
					case themas_s:
						PlayerGet();
						break;
					}
				}
				if (e.jbutton.button == BT_B) {                // (B) button down

					switch (statenow)
					{
					case programation_s:
						if (GOD.Confirm("Desea Salir al Home?")) {
                            cancelcurl = 1;
                            quit = true;
						}
						break;
					case download_s:
						statenow = chapter_s;
						if (porcendown >= 100) led_on(0);
						break;
					case chapter_s:
						if(serverpront) {
							serverpront=false;
							arrayservers=arrayserversbak;
						} else {
							statenow=returnnow;
						}
						break;
					case search_s:
						if (!reloadingsearch)
						{
							returnnow = programation_s;
							statenow = programation_s;
						}
						break;

					case top_s:
					case agregados_s:
					case hourglass_s:
					case history_s:
					case favorites_s:
					case menu_s:
						returnnow = programation_s;
						statenow = programation_s;
						break;
					}
				}
				if (e.jbutton.button == BT_X) {                // (X) button down
					switch (statenow)
					{
					case programation_s:
						if(isDownloading)
							statenow = download_s;
						break;
					case download_s:
						cancelcurl = 1;
						break;
					case chapter_s:
						if (!isConnected) break;
						statenow = download_s;
						cancelcurl = 0;
//							GOD.PleaseWait("Calculando Links Espere...");
						urltodownload  = BD["com"]["ActualLink"].get<string>() + to_string(latest) + "/";
						if(isDownloading) {
							bool gogo = false;
							for (u64 x=0; x < BD["arrays"]["downloads"]["queue"].size(); x++) {
								if (BD["arrays"]["downloads"]["queue"][x] == urltodownload) gogo = true;
							}
							if(gogo) break;
							BD["arrays"]["downloads"]["queue"].push_back(urltodownload);
							BD["arrays"]["downloads"]["log"].push_back(urltodownload);
						}else{
							BD["arrays"]["downloads"]["queue"].clear();
							BD["arrays"]["downloads"]["queue"].push_back(urltodownload);
							BD["arrays"]["downloads"]["log"] = BD["arrays"]["downloads"]["queue"];
							downloadthread = SDL_CreateThread(downloadjkanimevideo, "jkthread", (void*)NULL);
						}
						break;
					case search_s:

						break;
					case history_s: {
						/*
						   string nameh = UD["history"][histchapter];
						   NameOfLink(nameh);
						   if (GOD.Confirm("Desea Borrar "+nameh+" del Historial ?")){
						 */
						if (GOD.Confirm("Desea Borrarlo del Historial ?")) {
							UD["history"].erase(histchapter);
							if (histchapter > 0) histchapter--;
						}
					}
					break;
					case favorites_s: {
						/*
						   string nameh = UD["favoritos"][favchapter];
						   NameOfLink(nameh);
						   if (GOD.Confirm("Desea Borrar\n"+nameh+"?")){
						 */
						if (GOD.Confirm("Desea Borrarlo?")) {
							delFavorite(favchapter);
							if (favchapter > 0) favchapter--;
						}
					}
					break;

					}
				}
				if (e.jbutton.button == BT_Y) {                // (Y) button down

					switch (statenow)
					{
					case programation_s:
						//callmenuslide();
						callfavs();
						break;
					case menu_s:
						callfavs();
						break;
					/*
					        if (!reloading)
					        {
					                getFavorite();
					                returnnow = favorites_s;
					                statenow = favorites_s;
					                Frames=1;
					        }
					        break;
					 */
					case download_s:
						break;
					case chapter_s:
					{                //AGREGAR A FAVORITOS
						addFavorite(BD["com"]["ActualLink"]);
						gFAV = true;
						FAV.TickerBomb();
					}

					break;
					case favorites_s:

						break;
					}
				}
				if (e.jbutton.button == BT_R3) {                // (R3) button down
					switch (statenow)
					{                //only for test
					case chapter_s: {
						string tempurl = BD["com"]["ActualLink"].get<string>();
						if(serverpront) {
							tempurl = BD["com"]["ActualLink"].get<string>() + to_string(latest) + "/";
						}
						WebBrowserCall(tempurl);
						break;
					}
					case programation_s:
					case search_s:
					case favorites_s:
						ongrid = !ongrid;
						break;
					}
				}
				if (e.jbutton.button == BT_L3) {                // (L3) button down
					switch (statenow)
					{
						//Change UI
					}

				}
				if (e.jbutton.button == BT_R) {                // (R) button down
					switch (statenow)
					{
					case chapter_s:
						if (isset(AB["AnimeBase"][KeyName],"Secuela")) {
							if (!serverpront) {
								capBuffer(AB["AnimeBase"][KeyName]["Secuela"]);
								gFAV = isFavorite(BD["com"]["ActualLink"]);
							} else {
								serverpront=false;
							}
						}
						break;
					case programation_s:
					case search_s:
						callsearch();
						break;

					}
					if (statenow == hourglass_s) {
						if (WdayG < 7) {
							WdayG++;
							BD["arrays"]["HourGlass"]["link"] = BD["arrays"]["HourGlass"][Wday[WdayG]];
						}
					}

					if (statenow == menu_s)
					{
						Btimer=false;
						bannersel++;
						if (bannersel>bannersize) bannersel=0;
					}

				}
				if (e.jbutton.button == BT_LEFT || e.jbutton.button == BT_LS_LEFT) {                // (left) button down
					switch (statenow)
					{
					case chapter_s:
						if(serverpront) {serverpront = false;}
						if (latest > mincapit)
						{
							latest--;
						}
						if (latest < mincapit)
						{
							latest = mincapit;
						}
						break;
					case menu_s:
						if (BD["arrays"]["Benner"].size()>0)
						{
							string path = BD["arrays"]["Benner"][bannersel]["file"];
							Farest.free();
							Farest.loadFromFileCustom(path,720,1280);
						}
						break;
					}
				}
				if (e.jbutton.button == BT_RIGHT || e.jbutton.button == BT_LS_RIGHT) {                // (right) button down
					switch (statenow)
					{
					case menu_s:
						if (BD["arrays"]["Benner"].size()>0)
						{
							returnnow = menu_s;
							capBuffer(BD["arrays"]["Benner"][bannersel]["link"]);
							gFAV = isFavorite(BD["com"]["ActualLink"]);
						}
						break;
					case chapter_s:
						if(serverpront) {serverpront = false;}
						if (latest < maxcapit)
						{
							latest++;
						}
						if (latest > maxcapit)
						{
							latest = maxcapit;
						}
						break;
					}
				}
				if (e.jbutton.button == BT_UP || e.jbutton.button == BT_LS_UP) {                // (up) button down
					switch (statenow)
					{
					case chapter_s:
						if(!serverpront) {                //selectserver
							if (latest < maxcapit)
							{
								latest = latest + 10;
							}
							if (latest > maxcapit)
							{
								latest = maxcapit;
							}
						} else {
							if (selectserver > 0)
							{
								selectserver--;
							}
						}
						break;
					}
				}
				if (e.jbutton.button == BT_DOWN || e.jbutton.button == BT_LS_DOWN) {                // (down) button down
					switch (statenow)
					{
					case chapter_s:
						if(!serverpront) {                //selectserver
							if (latest > 1)
							{
								latest = latest - 10;
							}
							if (latest < mincapit)
							{
								latest = mincapit;
							}
						} else {
							if (selectserver < (int)arrayservers.size()-1)
							{
								selectserver++;
							}
						}
						break;
					}
				}

				//Global HID
				switch (e.jbutton.button) {
				case BT_LEFT:
				case BT_UP:
				case BT_RIGHT:
				case BT_DOWN:
				case BT_LS_LEFT:
				case BT_LS_UP:
				case BT_LS_RIGHT:
				case BT_LS_DOWN:
				{
					switch (statenow)
					{
					case programation_s:
						if (!reloading) GOD.HandleList(selectchapter, BD["arrays"]["chapter"]["link"].size(), e.jbutton.button, ongrid);
						break;
					case search_s:
						GOD.HandleList(searchchapter, BD["arrays"]["search"]["link"].size(), e.jbutton.button, ongrid);
						break;
					case top_s:
						GOD.HandleList(topchapter, BD["arrays"]["Top"]["link"].size(), e.jbutton.button, ongrid);
						break;
					case agregados_s:
						GOD.HandleList(agregadosidx, BD["arrays"]["Agregados"]["link"].size(), e.jbutton.button, ongrid);
						break;
					case hourglass_s:
						GOD.HandleList(hourchapter, BD["arrays"]["HourGlass"]["link"].size(), e.jbutton.button, ongrid);
						break;
					case history_s:
						GOD.HandleList(histchapter, UD["history"].size(), e.jbutton.button, ongrid);
						break;
					case favorites_s:
						GOD.HandleList(favchapter, UD["favoritos"].size(), e.jbutton.button, ongrid);
						break;
					case menu_s:
						GOD.HandleList(selectelement, StatesList.size(), e.jbutton.button, false);
						break;
					}
				}
				break;
				}
				//
			}
			break;
		default:
			break;
		}

	}
	GOD.GenState = statenow;

}