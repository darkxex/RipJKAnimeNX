

#include "JKanime.hpp"

//use the nand of the switch
#ifdef USENAND
std::string rootdirectory = "user:/RipJKAnime_NX/";
std::string rootsave = "save:/";
std::string oldroot = "sdmc:/switch/RipJKAnime_NX/";
#else
std::string rootdirectory = "sdmc:/switch/RipJKAnime_NX/";
std::string rootsave = rootdirectory;

#endif


//make some includes to clean a little the main
#include "extra.hpp"
std::string urlc = "https://myrincon.duckdns.org";

//MAIN INT
int main(int argc, char **argv)
{
	socketInitializeDefault();
	romfsInit();
	nxlinkStdio();
	printf("Nxlink server Conected\n");
	AppletMode=GetAppletMode();
	#ifdef USENAND
		//Mount user
		FsFileSystem data;
		fsOpenBisFileSystem(&data, FsBisPartitionId_User, "");
		fsdevMountDevice("user", data);
		
		//Mount Save Data
		FsFileSystem acc;
		if (MountUserSave(acc)){
			if(isFileExist(rootdirectory+AccountID+"UserData.json")){
				if(!isFileExist(rootsave+"UserData.json")){
					if (copy_me(rootdirectory+AccountID+"UserData.json", rootsave+"UserData.json")){
						fsdevCommitDevice("save");
						remove((rootdirectory+AccountID+"UserData.json").c_str());
						remove((rootdirectory+AccountID+"User.jpg").c_str());
					}
				}
			}
			if(isFileExist(rootdirectory+"UserData.json")){
				if(!isFileExist(rootsave+"UserData.json")){
					if (copy_me(rootdirectory+"UserData.json", rootsave+"UserData.json")){
						fsdevCommitDevice("save");
						remove((rootdirectory+"UserData.json").c_str());
					}
				}
			}
		} else {
			rootsave = rootdirectory+AccountID;
			GetUserImage();
		}	
	#endif
	struct stat st = { 0 };
	if (stat("sdmc:/RipJKAnime", &st) != -1) {
		fsdevDeleteDirectoryRecursively("sdmc:/RipJKAnime");
	}

	// read a JSON file
	read_DB(BD,rootdirectory+"DataBase.json");
	BD["com"] = "{}"_json;
	if(!BD["USER"].empty()){
		UD=BD["USER"];
	}
	BD.erase("USER");
	read_DB(UD,rootsave+"UserData.json");
	
	GOD.intA();//init the SDL

	#ifdef USENAND
		if (stat((rootdirectory+"DATA").c_str(), &st) == -1) {
			mkdir(rootdirectory.c_str(), 0777);
			mkdir((rootdirectory+"DATA").c_str(), 0777);
			if (stat((oldroot+"DATA").c_str(), &st) != -1){
				GOD.PleaseWait("Copiando Archivos Importantes Espere...",true);
				copy_me(oldroot+"favoritos.txt",rootdirectory+"favoritos.txt");
				
				GOD.PleaseWait("Copiando Archivos Importantes Espere.",true);
				copy_me(oldroot+"texture.png",rootdirectory+"texture.png");
				GOD.PleaseWait("Copiando Archivos Importantes Espere..",true);
				copy_me(oldroot+"heart.png",rootdirectory+"heart.png");
				GOD.PleaseWait("Copiando Archivos Importantes Espere....",true);
				copy_me(oldroot+"wada.ogg",rootdirectory+"wada.ogg");
				GOD.PleaseWait("Copiando Archivos Importantes Espere......",true);
				//set custom music 
				if (isFileExist(rootdirectory+"wada.ogg")){
					GOD.gMusic = Mix_LoadMUS((rootdirectory+"wada.ogg").c_str());
				}
				//if (!isFileExist("RipJKAnime_NX.nro"))//detect the nro path
				fsdevDeleteDirectoryRecursively(oldroot.c_str());
			}
		}
	#else
	mkdir(rootdirectory.c_str(), 0777);
	mkdir((rootdirectory+"DATA").c_str(), 0777);
	#endif

	if (isFileExist(rootdirectory+"texture.png")){
		Farest.loadFromFile(rootdirectory+"texture.png");
	} else {
		Farest.loadFromFile("romfs:/texture.png");
	}
	
	if (isFileExist(rootdirectory+"heart.png")){
		Heart.loadFromFile(rootdirectory+"heart.png");
	} else {
		Heart.loadFromFile("romfs:/heart.png");
	}

	gTextTexture.mark=false;
	Farest.mark=false;
	
	USER.loadFromFileCustom(rootsave+"User.jpg",58, 58);

	SDL_Color textColor = { 50, 50, 50 };
	SDL_Color textWhite = { 255, 255, 255 };
	SDL_Color textGray = { 200, 200, 200 };
	SDL_Color textGrayGreen = { 100, 200, 100 };
	SDL_Color textWhiteGreen = { 80, 255, 80 };

	int posxbase = 20;
	int posybase = 10;

	//Event handler
	SDL_Event e;

	for (int i = 0; i < 2; i++) {
		if (SDL_JoystickOpen(i) == NULL) {
			SDL_Log("SDL_JoystickOpen: %s\n", SDL_GetError());
			SDL_Quit();
			return -1;
		}
	}

try{
	//Set main Thread get images and descriptions
	prothread = SDL_CreateThread(refrescarpro, "prothread", (void*)NULL);

	//While application is running
	while (!quit&&appletMainLoop())
	{
		//get if console is dokked
		AppletOperationMode stus=appletGetOperationMode();
		if (stus == AppletOperationMode_Handheld){isHandheld=true;}
		if (stus == AppletOperationMode_Console){isHandheld=false;}

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

			GOD.GenState = statenow;
			switch (e.type) {
			case SDL_FINGERDOWN:
			GOD.TouchX = e.tfinger.x * SCREEN_WIDTH;
			GOD.TouchY = e.tfinger.y * SCREEN_HEIGHT;
			if (!GOD.fingerdown){
				GOD.fingerdown = true;
			}
			break;
			case SDL_FINGERMOTION:
				if(e.tfinger.dy * SCREEN_HEIGHT > 30 || e.tfinger.dy * SCREEN_HEIGHT < -30 || e.tfinger.dx * SCREEN_WIDTH > 30 || e.tfinger.dx * SCREEN_WIDTH < -30){
				SDL_Log("motion %f \n",e.tfinger.dy * SCREEN_HEIGHT);
					if(!GOD.fingermotion){
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
			if (e.type == SDL_FINGERUP){
				if(lcdoff){
					lcdoff=false;
					appletSetLcdBacklightOffEnabled(lcdoff);
				} else{
					GOD.fingerdown = false;
					GOD.fingermotion=false;
					GOD.TouchX = e.tfinger.x * SCREEN_WIDTH;
					GOD.TouchY = e.tfinger.y * SCREEN_HEIGHT;
					e.jbutton.button=-1;
					if (BUSB.SP()) {callsearch();}
					else if (FAVB.SP()) {callfavs();}
					else if (AFLV.SP()) {callAflv();}
					else if (HISB.SP()) {callhistory();}
					else if (NFAV.SP() && !gFAV){
						GOD.WorKey="0";GOD.MasKey=-1;
						{//AGREGAR A FAVORITOS
							addFavorite(BD["com"]["ActualLink"]);
							gFAV = true;
							FAV.TickerBomb();
							GOD.TouchX = -1;
							GOD.TouchY = -1;
						}
						break;
					}
					else if (GOD.MasKey >=0){
						if (statenow != programationsliderstate){
							if(GOD.MapT[GOD.WorKey].SP()){
								e.jbutton.button=GOD.MasKey;
							}
						}
						GOD.WorKey="0";GOD.MasKey=-1;
					}
					else if (USER.SP()) {
						if (SelectUser()){
							if (MountUserSave(acc)){
								rootsave = "save:/";
								if(isFileExist(rootdirectory+AccountID+"UserData.json")){
									if(!isFileExist(rootsave+"UserData.json")){
										if (copy_me(rootdirectory+AccountID+"UserData.json", rootsave+"UserData.json")){
											fsdevCommitDevice("save");
											remove((rootdirectory+AccountID+"UserData.json").c_str());
											remove((rootdirectory+AccountID+"User.jpg").c_str());
										}
									}
								}
							}else {
								rootsave = rootdirectory+AccountID;
								GetUserImage();
							}
							USER.loadFromFileCustom(rootsave+"User.jpg",58, 58);
							UD = "{}"_json;
							read_DB(UD,rootsave+"UserData.json");
							
							if(statenow==chapterstate){
								capBuffer(BD["com"]["ActualLink"]);
								gFAV = isFavorite(BD["com"]["ActualLink"]);
							}
							if(statenow==favoritesstate){
								getFavorite();
								Frames=1;							
							}
						}
					}
					else if (B_A.SP()) {e.jbutton.button = GOD.BT_A; B_A.TickerBomb();}
					else if (T_T.SP() ) e.jbutton.button = GOD.BT_A;
					else if (B_B.SP()) e.jbutton.button = GOD.BT_B;
					else if (BACK.SP()) e.jbutton.button = GOD.BT_B;
					else if (B_X.SP()) e.jbutton.button = GOD.BT_X;
					else if (B_Y.SP()) e.jbutton.button = GOD.BT_Y;
					else if (B_L.SP()) e.jbutton.button = GOD.BT_L;
					else if (B_R.SP()) e.jbutton.button = GOD.BT_R;
					else if (B_ZR.SP()) e.jbutton.button = GOD.BT_ZR;
					else if (B_P.SP()) e.jbutton.button = GOD.BT_P;
					else if (B_M.SP()) e.jbutton.button = GOD.BT_M;
					else if (B_LEFT.SP()) e.jbutton.button = GOD.BT_LEFT;
					else if (B_RIGHT.SP()) e.jbutton.button = GOD.BT_RIGHT;
					else if (B_UP.SP()) e.jbutton.button = GOD.BT_UP;
					else if (B_DOWN.SP()) e.jbutton.button = GOD.BT_DOWN;
					else if (T_D.SP()&&isDownloading) statenow = downloadstate;
					else if (SCREEN.SP()) e.jbutton.button = GOD.BT_ZR;
					else if (CLEAR.SP()){
						preview = false;
						quit = true;
						GOD.PleaseWait("Borrando cache");
						BD["DataBase"] = "{}"_json;
						BD["arrays"]["chapter"] = "{}"_json;
						BD["latestchapter"] = "";
						fsdevDeleteDirectoryRecursively((rootdirectory+"DATA").c_str());
						cancelcurl = 1;
						statenow=99;
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
				if (e.jaxis.axis == 1 && statenow==chapterstate){
				e.jbutton.button=-1;
					//SDL_Log("Joystick %d axis %d value: %d\n",e.jaxis.which,e.jaxis.axis, e.jaxis.value);
					if (e.jaxis.value < -22000){
						e.jbutton.button = GOD.BT_UP;
					}
					if (e.jaxis.value > 22000){
						e.jbutton.button = GOD.BT_DOWN;
					}
				} else break;				

			}
			case SDL_JOYBUTTONDOWN :
				//SDL_Log("Joystick %d button %d down\n",e.jbutton.which, e.jbutton.button);
				// https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L52
				// seek for joystick #0
				if (e.jbutton.which == 0) {
					if (e.jbutton.button == GOD.BT_A) {// (A) button down

						switch (statenow)
						{
						case programationsliderstate:
							if(selectelement==0){callsearch();}
							if(selectelement==1){callfavs();}
							if(selectelement==2){callhistory();}
							if(selectelement==5){callAflv();}
							if(selectelement==6){if(isDownloading){statenow = downloadstate;}}
							break;
							case programationstate:
							{
								if (!reloading&&BD["arrays"]["chapter"]["link"].size()>=1)
								{
									capBuffer(BD["arrays"]["chapter"]["link"][selectchapter]);								
									gFAV = isFavorite(BD["com"]["ActualLink"]);
								}
							}
							break;

							case searchstate:
							{
								if (!reloadingsearch && BD["arrays"]["search"]["link"].size()>=1)
								{
									capBuffer(BD["arrays"]["search"]["link"][searchchapter]);
									gFAV = isFavorite(BD["com"]["ActualLink"]);
								}

							}
							break;
							case historystate:
							{
								if (UD["history"].size()>0)
								{
									capBuffer(UD["history"][histchapter]);
									gFAV = isFavorite(BD["com"]["ActualLink"]);
								}

							}
							break;

							case favoritesstate:
							{
								if ((int)BD["arrays"]["favorites"]["link"].size() >= 1 ){
									capBuffer(BD["arrays"]["favorites"]["link"][favchapter]);
									gFAV = true;
								}
							}
							break;
							case chapterstate:
							if(serverpront){
								std::string tempurl = BD["com"]["ActualLink"].get<std::string>() + std::to_string(latest) + "/";
								if(!onlinejkanimevideo(tempurl,arrayservers[selectserver])){
									arrayservers.erase(arrayservers.begin()+selectserver);
								} else {
									serverpront = false;
									UD["chapter"][KeyName]["latest"] = latest;
									latestcolor = latest;
									
									std::string item=BD["com"]["ActualLink"].get<std::string>();
									int hsize = UD["history"].size();
									if (hsize > 29){UD["history"].erase(UD["history"].end());}//limit history
									if (hsize > 0){
										UD["history"] = eraseVec(UD["history"],item);
										//UD["history"].erase(std::remove(UD["history"].begin(), UD["history"].end(), item), UD["history"].end());
									}
									UD["history"].insert(UD["history"].begin(),tempurl);
									//UD["history"].push_back(tempurl);
									write_DB(UD,rootsave+"UserData.json");

								}
							} else {
								if (isConnected) serverpront = true;
								T_N.TickerBomb();
							}
							break;
						}
					}
					else if (e.jbutton.button == GOD.BT_P) {// (+) button down close to home menu
						cancelcurl = 1;
						quit = true;
					}
					else if (e.jbutton.button == GOD.BT_M) {// (-) button down
						if (Mix_PlayingMusic() == 0)
						{
							//Play the music
							Mix_PlayMusic(GOD.gMusic, -1);
							touch(rootdirectory+"play");
						}
						//If music is being played
						else
						{
							//If the music is paused
							if (Mix_PausedMusic() == 1)
							{
								//Resume the music
								Mix_ResumeMusic();
								touch(rootdirectory+"play");
								
							}
							//If the music is playing
							else
							{
								//Pause the music
								Mix_PauseMusic();
								remove((rootdirectory+"play").c_str());
							}
						}
					}
					else if (e.jbutton.button == GOD.BT_L || e.jbutton.button == GOD.BT_ZL) {// (L & ZL) button down
						if (statenow == chapterstate&&e.jbutton.button == GOD.BT_L){
							if(!BD["DataBase"][KeyName]["Precuela"].empty()){
								if (!serverpront){
									capBuffer(BD["DataBase"][KeyName]["Precuela"]);
									gFAV = isFavorite(BD["com"]["ActualLink"]);
								} else {
									serverpront=false;
								}
							}
						}
				
						if (statenow == programationstate)
						{
							if (e.jbutton.button == GOD.BT_ZL)
								WebBrowserCall(urlc,true);
							else 
								callAflv();
						}
					}
					else if (e.jbutton.button == GOD.BT_ZR) {// (ZR) button down
						if(isDownloading && isHandheld){
							lcdoff = !lcdoff;
							appletSetLcdBacklightOffEnabled(lcdoff);
						}

						switch (statenow)
						{
						case programationstate:
							statenow=programationsliderstate;
							std::cout  << BD << std::endl;
							std::cout  << UD << std::endl;
							break;
						case programationsliderstate:
							statenow=programationstate;
							break;
						case favoritesstate:
							break;
						}
					}
					else if (e.jbutton.button == GOD.BT_B) {// (B) button down

						switch (statenow)
						{
						case downloadstate:
							statenow = chapterstate;
							if (porcendown >= 100)led_on(0);
							break;
						case chapterstate:
							if(serverpront){
								serverpront=false;
								arrayservers=arrayserversbak;
							} else {
								statenow=returnnow;
							}
							break;
						case searchstate:
							if (!reloadingsearch)
							{
								returnnow = programationstate;
								statenow = programationstate;
							}
							break;
						case historystate:
						case favoritesstate:
						case programationsliderstate:
							returnnow = programationstate;
							statenow = programationstate;
							break;
						}
					}
					else if (e.jbutton.button == GOD.BT_X) {// (X) button down
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
							if (!isConnected) break;
							statenow = downloadstate;
							cancelcurl = 0;
//							GOD.PleaseWait("Calculando Links Espere...");
							urltodownload  = BD["com"]["ActualLink"].get<std::string>() + std::to_string(latest) + "/";
							if(isDownloading){
								bool gogo = false;
								for (u64 x=0; x < BD["arrays"]["downloads"]["queue"].size();x++){
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
						case searchstate:

							break;
						case favoritesstate:							
							delFavorite(favchapter);
							if (favchapter > 0) favchapter--;
							getFavorite();
							statenow = favoritesstate;
						break;

						}
					}
					else if (e.jbutton.button == GOD.BT_Y) {// (Y) button down

						switch (statenow)
						{
						case programationstate:
							callmenuslide();
							break;
						case programationsliderstate:
							callfavs();
							break;
						/*
							if (!reloading)
							{
								getFavorite();
								returnnow = favoritesstate;
								statenow = favoritesstate;
								Frames=1;
							}
							break;
						*/
						case downloadstate:
							break;
						case chapterstate:
						{//AGREGAR A FAVORITOS
							addFavorite(BD["com"]["ActualLink"]);
							gFAV = true;
							FAV.TickerBomb();
						}

						break;
						case favoritesstate:

							break;




						}
					}
					else if (e.jbutton.button == GOD.BT_R3) {// (R3) button down
						switch (statenow)
						{//only for test 
						case chapterstate:
							if(serverpront){
								arrayservers.push_back("test");
							} else {
								std::string tempurl = BD["com"]["ActualLink"].get<std::string>() + std::to_string(latest) + "/";
								WebBrowserCall(tempurl);
							}
							break;
						case programationstate:
							ongrid = !ongrid;
							break;
						case searchstate:
							ongridS = !ongridS;
							break;
						case favoritesstate:
							ongridF = !ongridF;
							break;
						}
					}
					else if (e.jbutton.button == GOD.BT_L3) {// (L3) button down
						switch (statenow)
						{
						//Change UI
						}

					}
					else if (e.jbutton.button == GOD.BT_R) {// (R) button down

						switch (statenow)
						{
						case chapterstate:
							if(!BD["DataBase"][KeyName]["Secuela"].empty()&&!serverpront){
								if (!serverpront){
									capBuffer(BD["DataBase"][KeyName]["Secuela"]);
									gFAV = isFavorite(BD["com"]["ActualLink"]);
								} else {
									serverpront=false;
								}
							}
						break;
						case programationstate:
						case searchstate:
							callsearch();
							break;

						}

					}
					else if (e.jbutton.button == GOD.BT_LEFT || e.jbutton.button == GOD.BT_LS_LEFT) {// (left) button down

						switch (statenow)
						{
						case chapterstate:
							if(serverpront){serverpront = false;}
							if (latest > mincapit)
							{
								latest--;
							}
							if (latest < mincapit)
							{
								latest = mincapit;
							}
							break;
						case programationstate:
							if (!reloading)
							{
								GOD.HandleList(selectchapter, BD["arrays"]["chapter"]["link"].size(), e.jbutton.button, ongrid);								
							}
							break;
						case searchstate:
							if (!reloadingsearch&&(BD["arrays"]["search"]["link"].size() >= 1))
							{
								GOD.HandleList(searchchapter, BD["arrays"]["search"]["link"].size(), e.jbutton.button, ongridS);
							}
							break;
						case historystate:
							GOD.HandleList(histchapter, UD["history"].size(), e.jbutton.button, ongrid);
							break;
						case favoritesstate:
							GOD.HandleList(favchapter, BD["arrays"]["favorites"]["link"].size(), e.jbutton.button, ongridF);
							break;
						}
					}
					else if (e.jbutton.button == GOD.BT_RIGHT || e.jbutton.button == GOD.BT_LS_RIGHT) {// (right) button down
						switch (statenow)
						{
						case chapterstate:
						
							if(serverpront){serverpront = false;}
							if (latest < maxcapit)
							{
								latest++;
							}
							if (latest > maxcapit)
							{
								latest = maxcapit;
							}
							break;
						case programationstate:
							if (!reloading)
							{
								GOD.HandleList(selectchapter, BD["arrays"]["chapter"]["link"].size(), e.jbutton.button, ongrid);
							}
							break;
						case searchstate:
							if (!reloadingsearch&&(BD["arrays"]["search"]["link"].size() >= 1))
							{
								GOD.HandleList(searchchapter, BD["arrays"]["search"]["link"].size(), e.jbutton.button, ongridS);
							}
							break;
						case historystate:
							GOD.HandleList(histchapter, UD["history"].size(), e.jbutton.button, ongrid);
							break;
						case favoritesstate:
							GOD.HandleList(favchapter, BD["arrays"]["favorites"]["link"].size(), e.jbutton.button, ongridF);
							break;
						}
					}
					else if (e.jbutton.button == GOD.BT_UP || e.jbutton.button == GOD.BT_LS_UP) {// (up) button down

						switch (statenow)
						{
						case programationstate:
							if (!reloading)
							{
								GOD.HandleList(selectchapter, BD["arrays"]["chapter"]["link"].size(), e.jbutton.button, ongrid);
							}
							break;
						case chapterstate:
							if(!serverpront){//selectserver
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
						case searchstate:
							if (!reloadingsearch&&(BD["arrays"]["search"]["link"].size() >= 1))
							{
								GOD.HandleList(searchchapter, BD["arrays"]["search"]["link"].size(), e.jbutton.button, ongridS);
							}
							break;

						case historystate:
							GOD.HandleList(histchapter, UD["history"].size(), e.jbutton.button, ongrid);
							break;
						case favoritesstate:
							GOD.HandleList(favchapter, BD["arrays"]["favorites"]["link"].size(), e.jbutton.button, ongridF);
							break;
						case programationsliderstate:
							GOD.HandleList(selectelement, StatesList.size(), e.jbutton.button, false);
							break;
						}
					}
					else if (e.jbutton.button == GOD.BT_DOWN || e.jbutton.button == GOD.BT_LS_DOWN) {// (down) button down
						switch (statenow)
						{
						case programationstate:
							if (!reloading)
							{
								GOD.HandleList(selectchapter, BD["arrays"]["chapter"]["link"].size(), e.jbutton.button, ongrid);
							}
							break;

						case searchstate:
							if (!reloadingsearch&&(BD["arrays"]["search"]["link"].size() >= 1))
							{
								GOD.HandleList(searchchapter, BD["arrays"]["search"]["link"].size(), e.jbutton.button, ongridS);
							}
							break;

						case chapterstate:
							if(!serverpront){//selectserver
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

						case historystate:
							GOD.HandleList(histchapter, UD["history"].size(), e.jbutton.button, ongrid);
							break;
						case favoritesstate:
							GOD.HandleList(favchapter, BD["arrays"]["favorites"]["link"].size(), e.jbutton.button, ongridF);
							break;
						case programationsliderstate:
							GOD.HandleList(selectelement, StatesList.size(), e.jbutton.button, false);
							break;
						}
					}
				}
				break;

			default:
				break;
			}

		}
		GOD.GenState = statenow;
		//Clear screen
		SDL_SetRenderDrawColor(GOD.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(GOD.gRenderer);

		//wallpaper
		Farest.render((0), (0));
		
		//Draw footer
		VOX.render_VOX({0,671, 1280, 50}, 210, 210, 210, 115);
		
		//render states
		switch (statenow)
		{
			case chapterstate:		{
			//Draw a background to a nice view
			USER.render(SCREEN_WIDTH - USER.getWidth()-1,1);
			VOX.render_VOX({0,0, SCREEN_WIDTH, 670} ,170, 170, 170, 100);
			VOX.render_VOX({0,0, 1280, 60} ,200, 200, 200, 130);
			GOD.HR=200;GOD.HG=200;GOD.HB=200;
			std::string temptext = BD["com"]["ActualLink"];
			NameOfLink(temptext);

			/*
			//warning , only display in sxos ToDo
			gTextTexture.loadFromRenderedText(GOD.gFont, "(*En SXOS desactiva Stealth Mode*)", textColor);
			gTextTexture.render(posxbase, 0 );
			*/

			//draw Title
			gTextTexture.loadFromRenderedText(GOD.gFont3, temptext.substr(0,62)+ ":", textColor);
			gTextTexture.render(posxbase, posybase);

			{//draw preview image
				VOX.render_VOX({ SCREEN_WIDTH - 412,63, 404, 590}, 0, 0, 0, 200);
				GOD.Image(BD["com"]["ActualLink"],SCREEN_WIDTH - 410, 65,400, 550,GOD.BT_B);
			}	

			{//draw description
				VOX.render_VOX({10,63, 770, 340}, 255, 255, 255, 100);
				static std::string rese_prot = "..";
				if (rese_prot != BD["com"]["sinopsis"]){//load texture on text change 
					T_R.loadFromRenderedTextWrap(GOD.gFont, BD["com"]["sinopsis"], textColor, 750);
					rese_prot = BD["com"]["sinopsis"];
				}
				T_R.render(posxbase, posybase + 65);

				gTextTexture.loadFromRenderedTextWrap(GOD.gFont, BD["com"]["Emitido"], textColor,750);
				gTextTexture.render(posxbase, posybase + 350-gTextTexture.getHeight());

				gTextTexture.loadFromRenderedTextWrap(GOD.gFont, BD["com"]["generos"], textColor,750);
				gTextTexture.render(posxbase, posybase + 380-gTextTexture.getHeight());
			}
			bool anend=false;
			int sizefix = 0;
			if (maxcapit >= 0){
				if (BD["com"]["nextdate"] == "Pelicula"){
					gTextTexture.loadFromRenderedText(GOD.gFont3, "Pelicula", { 250,250,250 });
					gTextTexture.render(posxbase + 855, posybase + 598);
				} else {
					if (BD["com"]["enemision"] == "true")
					{
						gTextTexture.loadFromRenderedText(GOD.gFont3, "En Emisión ", { 16,191,0 });
						gTextTexture.render(posxbase + 855, posybase + 598);
					} else {
						gTextTexture.loadFromRenderedText(GOD.gFont3, "Concluido", { 140,0,0 });
						gTextTexture.render(posxbase + 855, posybase + 598);
					}
					gTextTexture.loadFromRenderedText(GOD.gFont, BD["com"]["nextdate"], { 255,255,255 });
					gTextTexture.render(posxbase + 1055, posybase + 615);
				}
			}

			{//use this to move the element
				int XS=250 , YS =0;
				if (maxcapit >= 0){
					int mwide = 35;//52
					int XD = 210+XS, YD = 582+YS;
					sizefix = (int)arrayservers.size() * mwide;
					anend=VOX.render_AH(XD, YD, 190, sizefix, serverpront);
					if(serverpront){
						if (anend){
							for (int x = 0; x < (int)arrayservers.size(); x++) {
								if (x == selectserver){
									T_T.loadFromRenderedText(GOD.gFont4, arrayservers[x], textWhite);
									VOX.render_VOX({ posxbase+XD-10,YD + 5 - sizefix + (x * mwide), 170, T_T.getHeight()-5}, 50, 50, 50, 200);
									T_T.render(posxbase+XD, YD - sizefix + (x * mwide));
								} else {
									gTextTexture.loadFromRenderedText(GOD.gFont4, arrayservers[x],textGray);
									gTextTexture.render(posxbase+XD, YD - sizefix + (x * mwide));
								}
							}
						}
					}
				}
				if(serverpront){
					if (anend){
						B_UP.render_T(280+XS, 530+YS-sizefix,"");
					}
					B_DOWN.render_T(280+XS, 630+YS,"");
				}
				if (maxcapit >= 0&&BD["com"]["nextdate"] != "Pelicula"){//draw caps numbers Slider
					VOX.render_VOX({posxbase + 70+XS, posybase + 571+YS, 420, 33 }, 50, 50, 50, 200);
					SDL_Color com = {};
					if (latest-2 >= mincapit) {
						com=textGray;
						if (latest-2 == latestcolor) com=textGrayGreen;
						gTextTexture.loadFromRenderedText(GOD.gFont3,  std::to_string(latest-2), com);
						gTextTexture.render(posxbase + 150 +XS-gTextTexture.getWidth()/2, posybase + 558+YS);
					}
					if (latest-1 >= mincapit) {
						com=textGray;
						if (latest-1 == latestcolor) com=textGrayGreen;
						gTextTexture.loadFromRenderedText(GOD.gFont3,  std::to_string(latest-1), com);
						gTextTexture.render(posxbase + 215+XS-gTextTexture.getWidth()/2, posybase + 558+YS);
					}
					
					{
						com=textWhite;
						if (latest == latestcolor) com=textWhiteGreen;
						if (serverpront){
							T_N.loadFromRenderedText(GOD.gFont3, std::to_string(latest), com);
							T_N.render(posxbase + 280+XS-T_N.getWidth()/2, posybase + 558+YS);
						} else {
							T_T.loadFromRenderedText(GOD.gFont3, std::to_string(latest), com);
							T_T.render(posxbase + 280+XS-T_T.getWidth()/2, posybase + 558+YS);
						}
					}

					if (latest+1 <= maxcapit) {
						com=textGray;
						if (latest+1 == latestcolor) com=textGrayGreen;
						gTextTexture.loadFromRenderedText(GOD.gFont3,  std::to_string(latest+1), com);
						gTextTexture.render(posxbase + 345+XS-gTextTexture.getWidth()/2, posybase + 558+YS);
					}
					if (latest+2 <= maxcapit) {
						com=textGray;
						if (latest+2 == latestcolor) com=textGrayGreen;
						gTextTexture.loadFromRenderedText(GOD.gFont3,  std::to_string(latest+2), com);
						gTextTexture.render(posxbase + 410+XS-gTextTexture.getWidth()/2, posybase + 558+YS);
					}

					if (maxcapit >= 10 && !serverpront){
						B_UP.render_T(280+XS, 530+YS,"+10",serverpront);
						B_DOWN.render_T(280+XS, 630+YS,"-10",serverpront);
					}
					
					B_LEFT.render_T(75+XS, 580+YS,std::to_string(mincapit),latest == mincapit);
					B_RIGHT.render_T(485+XS, 580+YS,std::to_string(maxcapit),latest == maxcapit);
				} else {
					VOX.render_VOX({posxbase + 185+XS, posybase + 570+YS, 200, 35 }, 50, 50, 50, 200);
					if (BD["com"]["nextdate"] == "Pelicula"){
						T_N.loadFromRenderedText(GOD.gFont3, "Reproducir...", { 255, 255, 255 });
						T_N.render(posxbase + 282+XS-T_N.getWidth()/2, posybase + 558+YS);
					} else {
						gTextTexture.loadFromRenderedText(GOD.gFont3, "Cargando...", { 255, 255, 255 });
						gTextTexture.render(posxbase + 282+XS-gTextTexture.getWidth()/2, posybase + 558+YS);
					}
					
				}
			}

			//Draw Footer Buttons
			int dist = 1095,posdist = 160;
			if(serverpront){
				B_A.render_T(dist, 680,"Ver Online");dist -= posdist;
				B_B.render_T(dist, 680,"Cerrar");dist -= posdist;
			} else {
				B_A.render_T(dist, 680,"Aceptar");dist -= posdist;
				B_B.render_T(dist, 680,"Atrás");dist -= posdist;
				B_X.render_T(dist, 680,"Descargar");dist -= posdist;
			}

			if(gFAV){
				FAV.render(1230, 70);
			} else {
				NFAV.render_T(1230, 70,"");
				B_Y.render_T(dist, 680,"Favorito");dist -= posdist;
			}
			
			if(!BD["DataBase"][KeyName]["Secuela"].empty()){
				std::string imagelocal=BD["DataBase"][KeyName]["Secuela"];
				imagelocal = KeyOfLink(imagelocal);
				imagelocal = rootdirectory+"DATA/"+imagelocal+".jpg";
				if(!serverpront){CheckImgNet(imagelocal);B_R.render_T(dist, 680,"Secuela");dist -= posdist;}
				GOD.Cover(imagelocal,160,457,"Secuela",120,GOD.BT_R);
			}
			if(!BD["DataBase"][KeyName]["Precuela"].empty()){
				std::string imagelocal=BD["DataBase"][KeyName]["Precuela"];
				imagelocal = KeyOfLink(imagelocal);
				imagelocal = rootdirectory+"DATA/"+imagelocal+".jpg";
				if(!serverpront){CheckImgNet(imagelocal);B_L.render_T(dist, 680,"Precuela");dist -= posdist;}
				GOD.Cover(imagelocal,10,457,"Precuela",120,GOD.BT_L);
			}

			break;
			}
			case programationsliderstate:
			case programationstate:	{
				if (!reloading&&BD["arrays"]["chapter"]["link"].size()>=1) {

					if(ongrid){
						USER.render(SCREEN_WIDTH - USER.getWidth()-1,1);
						if (GOD.fingermotion_LEFT & (GOD.TouchX > 1100)){
							GOD.fingermotion_LEFT = false;
							callmenuslide();
						}
						if (GOD.fingermotion_RIGHT & (GOD.TouchX > 850)){
							GOD.fingermotion_RIGHT = false;
							statenow=programationstate;
						}
						GOD.HR=200;GOD.HG=200;GOD.HB=200;
						if (preview)
						{
							GOD.ListCover(selectchapter,BD["arrays"]["chapter"],ongrid,Frames);
							if (isHandheld && statenow!=programationsliderstate){
								FAVB.render(SCREEN_WIDTH - USER.getWidth() - FAVB.getWidth() - 20, 1);
								HISB.render(SCREEN_WIDTH - USER.getWidth() - FAVB.getWidth() - BUS.getWidth() - 50, 1);
								BUSB.render(SCREEN_WIDTH - USER.getWidth() - FAVB.getWidth() - BUS.getWidth() - HISB.getWidth() - 70, 1);
							}
						}
						double angle = 0.0;
						static int Ticker=0;
						if (isChained){
							REC.TickerRotate(Ticker,0,360,2,isChained);
							angle=Ticker;
						} else if (Ticker > 0){
							REC.TickerRotate(Ticker,-25,360,5,isChained);
							angle=Ticker;
						}else if (Ticker < 0){
							Ticker=0;
						}
						
						if (porcentajebuffer > 0){
							gTextTexture.loadFromRenderedText(GOD.digifontC, std::to_string(porcentajebufferAll - porcentajebuffer), {50,50,50});
							gTextTexture.render(27 - (gTextTexture.getWidth()/2), 30);
						}
						REC.render(5, 15,NULL,angle);
					} else {
						GOD.ListClassic(selectchapter,BD["arrays"]["chapter"]);
						if (preview)
						{
							GOD.ListCover(selectchapter,BD["arrays"]["chapter"]);
						}
						B_UP.render_T(580, 5,"");
						B_DOWN.render_T(580, 630,"");
					}

					//Draw Header
					std::string VERCAT =  VERSION;
					std::string TYPEA =  "sdmc";
					#ifdef USENAND
						TYPEA =  "emmc";
					#endif
					gTextTexture.loadFromRenderedText(GOD.digifontC, (TYPEA+" (Ver "+VERCAT+") #KASTXUPALO").c_str(), {100,0,0});
					gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 3, 672);
					
					gTextTexture.loadFromRenderedText(GOD.gFont, "Recientes", {100,0,0});
					if(ongrid){
						gTextTexture.render(5, 1);
					}else {
						gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 5, 2);
						if (imgNumbuffer > 0){
							gTextTexture.loadFromRenderedText(GOD.gFont, "Imágenes: ("+std::to_string(imgNumbuffer)+"/30)", {0,100,0});
							gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 15, 22);
							//Heart.render(posxbase + 570, posybase + 3 + (imgNumbuffer-1) * 22);
						}
						if (porcentajebuffer > 0){
							gTextTexture.loadFromRenderedText(GOD.gFont, "Búfer: ("+std::to_string(porcentajebuffer)+"/"+std::to_string(porcentajebufferAll)+")", {0,100,0});
							gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 15, 22);
						}
					}
					if (statenow==programationsliderstate){
						
						{
							StatesList= {"Busqueda","Favoritos","History","Horario","Top Anime","AnimeFLV"};
							if(isDownloading){StatesList.push_back("Descargas");}

							int mwide = 60,XD=940,YD=120,W=1280-XD;
							VOX.render_VOX({XD,61, 1280, 608}, 220, 220, 220, 220);//draw area
							VOX.render_VOX({XD,61, W, 1}, 255, 255, 255, 235);//head line
							VOX.render_VOX({XD,668, W, 1}, 255, 255, 255, 235);//bottom line
							VOX.render_VOX({XD,61, 1, 607}, 255, 255, 255, 235);//line left
							gTextTexture.loadFromRenderedText(GOD.gFont5, "Menú Primario",textColor);
							gTextTexture.render(XD+20, 65);
							if (imgNumbuffer > 0){
								gTextTexture.loadFromRenderedText(GOD.gFont, "Imágenes: ("+std::to_string(imgNumbuffer)+"/30)", {0,100,0});
								gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 15, 70);
							}
							if (porcentajebuffer > 0){
								gTextTexture.loadFromRenderedText(GOD.gFont, "Búfer: ("+std::to_string(porcentajebuffer)+"/"+std::to_string(porcentajebufferAll)+")", {0,100,0});
								gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 15,  70);
							}
							if (porcentajebufferF > 0){
								gTextTexture.loadFromRenderedText(GOD.gFont, "BúferFav: ("+std::to_string(porcentajebufferF)+"/"+std::to_string(porcentajebufferFF)+")", {0,100,0});
								gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 15, 70);
							}
							int indexLsize = StatesList.size();
							int Ymaxsize = indexLsize*mwide;
							if(GOD.TouchX < 1280 && GOD.TouchY < Ymaxsize+YD && GOD.TouchY > YD && GOD.TouchX > XD+100){
								int sel=(GOD.TouchY-YD) / mwide;
								VOX.render_VOX({XD+80-10,YD+(sel*mwide)+5, W-100, mwide-5}, 0, 0, 255, 235);
								if (sel >= 0 && sel < indexLsize){
									selectelement = sel;
								}
							}

							for (int x = 0; x < indexLsize; x++) {
								if(x == 0){BUSB.render(XD+10, YD + (x * mwide)+5);}
								if(x == 1){FAVB.render(XD+10, YD + (x * mwide)+5);}
								if(x == 2){HISB.render(XD+10, YD + (x * mwide)+5);}
								if(x == 3){HISB.render(XD+10, YD + (x * mwide)+5);}
								if(x == 4){HISB.render(XD+10, YD + (x * mwide)+5);}
								if(x == 5){AFLV.render(XD+10, YD + (x * mwide)+5);}
								if(x == 6){AFLV.render(XD+10, YD + (x * mwide)+5);}

								if (x == selectelement){
									T_T.loadFromRenderedText(GOD.gFont6, StatesList[x], textWhite);
									VOX.render_VOX({XD+80-10,YD + (x * mwide)+5, W-100, T_T.getHeight()-5}, 50, 50, 50, 100);
									T_T.render(XD+80, YD + (x * mwide));
								} else {
									gTextTexture.loadFromRenderedText(GOD.gFont6, StatesList[x],textColor);
									gTextTexture.render(XD+80, YD + (x * mwide));
								}

								if (x < indexLsize-1){
									VOX.render_VOX({XD+80,YD + (x * mwide)+mwide+2, W-130, 1}, 255, 255, 255, 235);
								}
							}
						}

						//Draw footer buttons
						int dist = 1100,posdist = 170;
						B_A.render_T(dist, 680,"Aceptar");dist -= posdist;
						B_B.render_T(dist, 680,"Atrás");dist -= posdist;
						break;
					}
					//Draw footer buttons
					int dist = 1100,posdist = 170;
					B_A.render_T(dist, 680,"Aceptar");dist -= posdist;
					B_R.render_T(dist, 680,"Buscar");dist -= posdist;
					//B_L.render_T(dist, 680,"AnimeFLV");dist -= posdist;
					B_Y.render_T(dist, 680,"Menú");dist -= posdist;
					if(isDownloading) {B_X.render_T(dist, 680,"Descargas");dist -= posdist-10;}
					if (isHandheld){CLEAR.render_T(dist, 680,"Cache");dist -= posdist;}
					
				}
				else
				{
					std::string textpro="Cargando programación";
					if(imgNumbuffer>0){textpro+=" "+std::to_string(imgNumbuffer)+"/30";} else {textpro+="...";}
					GOD.PleaseWait(textpro,false);
				}

				break;
			}
			case searchstate:		{
				if (!reloadingsearch) {					
					int srchsize=BD["arrays"]["search"]["link"].size();
					if(ongridS){USER.render(SCREEN_WIDTH - USER.getWidth()-1,1);}
					if (srchsize > 0){
						//if (srchsize > 30) ongridS=false;
						if (!ongridS) GOD.ListClassic(searchchapter,BD["arrays"]["search"]);
						if (preview)
						{
							if (ongridS){
								VOX.TickerColor(GOD.HG,210,255, 200);
								GOD.HR=200;GOD.HB=200;
								GOD.ListCover(searchchapter,BD["arrays"]["search"],ongridS,Frames);
								BUS.render_T(5, 15,"");
								if (isHandheld){
									BUSB.render(SCREEN_WIDTH - USER.getWidth() - FAVB.getWidth() - BUS.getWidth() - 50, 1);
								}
							} else {
								GOD.ListCover(searchchapter,BD["arrays"]["search"]);
								B_UP.render_T(580, 5,"");
								B_DOWN.render_T(580, 630,"");
							}
						}
					}else {
						NOP.render_T(230, 355,"?");
						BD["searchtext"]="";
					}
					//Draw Header
					gTextTexture.loadFromRenderedText(GOD.gFont, "Resultados de Búsqueda:", {100,0,0});
					if(ongridS){
						int distan = gTextTexture.getWidth()+10;
						gTextTexture.render(5, 1);
						gTextTexture.loadFromRenderedText(GOD.gFont, BD["searchtext"], {0,0,0});
						VOX.render_VOX({distan-2,1, gTextTexture.getWidth()+4, gTextTexture.getHeight()}, 210, 210, 210, 155);
						//T_D.getWidth()+4, T_D.getHeight()
						gTextTexture.render(distan, 1);
					}else {
						gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 5, 2);
						gTextTexture.loadFromRenderedText(GOD.gFont, BD["searchtext"], {0,0,0});
						gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 15, 22);
					}
					{//Draw footer buttons
						int dist = 1100,posdist = 160;
						B_A.render_T(dist, 680,"Aceptar");dist -= posdist;
						B_B.render_T(dist, 680,"Atras");dist -= posdist;
						B_R.render_T(dist, 680,"Buscar");dist -= posdist;
					}
				}
				else
				{
					std::string added="";
					if(porcentajebufferF > 0){
						int persen = ((porcentajebufferF) * 100) / porcentajebufferFF;
						added=" ("+std::to_string(persen)+"%)";
					}
					GOD.PleaseWait("Cargando búsqueda..."+added,false);
					Frames=1;
				}
				break;
			}
			case favoritesstate:	{
				if(ongridF){USER.render(SCREEN_WIDTH - USER.getWidth()-1,1);}
				int favsize=BD["arrays"]["favorites"]["link"].size();
				if (favsize > 0){
					//if (favsize > 30) ongridF=false;
					if (!ongridF) GOD.ListClassic(favchapter,BD["arrays"]["favorites"]);
					if (preview)
					{
						if (ongridF){
							VOX.TickerColor(GOD.HR,210,255, 200);
							GOD.HG=200;GOD.HB=200;
							GOD.ListCover(favchapter,BD["arrays"]["favorites"],ongridF,Frames);
							FAV.render_T(5, 15,"");
						} else {
							GOD.ListCover(favchapter,BD["arrays"]["favorites"]);
							B_UP.render_T(580, 5,"");
							B_DOWN.render_T(580, 630,"");
						}
					}
				}
				
				//Draw Header
				gTextTexture.loadFromRenderedText(GOD.gFont, "Favoritos", {100,0,0});
				if(ongridF){
					gTextTexture.render(5, 1);
				}else {
					gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 5, 2);

					if (porcentajebufferF > 0){
						gTextTexture.loadFromRenderedText(GOD.gFont, "Búfer: ("+std::to_string(porcentajebufferF)+"/"+std::to_string(porcentajebufferFF)+")", {0,100,0});
						gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 30, 20);
					}
				}

				{//Draw footer buttons
					int dist = 1100,posdist = 180;
					B_A.render_T(dist, 680,"Aceptar");dist -= posdist;
					B_B.render_T(dist, 680,"Volver");dist -= posdist;
					if ((int)BD["arrays"]["favorites"]["link"].size() >= 1){
						B_X.render_T(dist, 680,"Borrar #"+std::to_string(favchapter+1));dist -= posdist;
					}else NOP.render_T(230, 355,"");
				}
			break;
			}
			case historystate: {
				if(ongrid){USER.render(SCREEN_WIDTH - USER.getWidth()-1,1);}
				//reverse vector
				json histf;
				histf["link"]=UD["history"];
				/*
				std::cout << "-- " << histf << std::endl;
				std::reverse(histf["link"].begin(), histf["link"].end());
				std::cout << "-- " << histf << std::endl;
				*/
				
				int histsize=histf["link"].size();
				if (histsize > 0){
					//if (favsize > 30) ongridF=false;
					if (!ongrid) GOD.ListClassic(histchapter,histf);
					if (preview)
					{
						if (ongrid){
							VOX.TickerColor(GOD.HG,100,200, 200);
							GOD.HR=200;GOD.HB=200;
							GOD.ListCover(histchapter,histf,ongrid,Frames);
							HIS.render_T(5, 15,"");
						} else {
							GOD.ListCover(histchapter,histf);
							B_UP.render_T(580, 5,"");
							B_DOWN.render_T(580, 630,"");
						}
					}
				} else NOP.render_T(230, 355,"");
				
				//Draw Header
				gTextTexture.loadFromRenderedText(GOD.gFont, "history", {100,0,0});
				if(ongrid){
					gTextTexture.render(5, 1);
				}else {
					gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 5, 2);
				}

				{//Draw footer buttons
					int dist = 1100,posdist = 180;
					B_A.render_T(dist, 680,"Aceptar");dist -= posdist;
					B_B.render_T(dist, 680,"Volver");dist -= posdist;
				}
			break;
			}
			case downloadstate:	{
				USER.render(SCREEN_WIDTH - USER.getWidth()-1,1);
				VOX.render_VOX({0,0, 1280, 60} ,200, 200, 200, 130);//Head
				VOX.render_VOX({16,65, 900, 162}, 210, 210, 210, 115);//Rectangle
				
				gTextTexture.loadFromRenderedText(GOD.gFont, "Descargando Actualmente:", textColor);
				gTextTexture.render(posxbase, posybase+15);
				
				gTextTexture.loadFromRenderedText(GOD.gFont4, DownTitle, textColor);
				VOX.render_VOX({17,65, gTextTexture.getWidth()+15, 45}, 210, 210, 210, 155);//Draw title back
				gTextTexture.render(posxbase, posybase + 60);

				if (serverenlace != "Error de descarga"){
					gTextTexture.loadFromRenderedText(GOD.gFontcapit, std::to_string(porcendown) + "\%", textColor);
					gTextTexture.render(posxbase + 280, posybase + 90);

					gTextTexture.loadFromRenderedText(GOD.gFont, "Peso estimado: " + std::to_string((int)(sizeestimated / 1000000)) + "mb.", textColor);
					gTextTexture.render(posxbase, posybase + 160);

					gTextTexture.loadFromRenderedText(GOD.gFont, "Usa el HomeBrew PPlay para reproducir el video.", textColor);
					gTextTexture.render(posxbase, posybase + 200);

					if (std::to_string(porcendown) == "100"&&!isDownloading) {
						//Render red filled quad
						VOX.render_VOX({ posxbase + 198, posybase + 500, 580, 50 }, 255, 255, 255, 195);
						gTextTexture.loadFromRenderedText(GOD.gFont3, "¡Descarga Completada! Revisa tu SD.", textColor);
						gTextTexture.render(posxbase + 200, posybase + 500);
						if(lcdoff){lcdoff=false; appletSetLcdBacklightOffEnabled(lcdoff);}
					 }else{
						gTextTexture.loadFromRenderedText(GOD.digifont, "Velocidad: " +speedD+" M/S", textColor);
						VOX.render_VOX({ posxbase, posybase + 180, gTextTexture.getWidth()+6, 20 }, 255, 255, 255, 145);
						gTextTexture.render(posxbase + 2, posybase + 180);
						if (isHandheld){
							SCREEN.render(1180, 65);
							B_ZR.render_T(550, 680,"Apagar Pantalla");
						}
					 }
				} else {
					porcendown=0;
				}
				
				gTextTexture.loadFromRenderedText(GOD.gFont, serverenlace.substr(0,300), {168,0,0});
				gTextTexture.render(posxbase , posybase + 220);
				
				static int tatic = 850;
				VOX.render_VOX({posxbase-5,posybase + 240 , tatic, ((int)BD["arrays"]["downloads"]["log"].size() * 22)+33}, 200, 200, 200, 105);
				gTextTexture.loadFromRenderedText(GOD.digifont, "Cola De Descarga :", textColor);
				gTextTexture.render(posxbase, posybase+240);
				for (u64 x = 0; x < BD["arrays"]["downloads"]["log"].size(); x++) {
					std::string descarga = BD["arrays"]["downloads"]["log"][x];
					//NameOfLink(descarga);
					SDL_Color txtColor = textColor;//{ 50, 50, 50 };
					
					if(descarga.substr(0,3) == "100") txtColor = { 0, 100, 0 };
					if(descarga.substr(0,3) == "Err") txtColor = { 150, 50, 50 };
					if(descarga.substr(0,3) == ">>>") {txtColor = { 0, 0, 0 }; replace(descarga,">>>>",">>"+std::to_string(porcendown)+"\%");}
					if(descarga.substr(0,3) == "htt") {txtColor = { 100, 100, 100}; NameOfLink(descarga);descarga="En Cola: "+descarga;}

					gTextTexture.loadFromRenderedText(GOD.digifont, descarga, txtColor);
					if (tatic < gTextTexture.getWidth()){tatic = gTextTexture.getWidth()+35;}
					gTextTexture.render(posxbase, posybase+260 + ((x) * 22));
					
				}
				
				if(isDownloading) B_X.render_T(800, 680,"Cancelar la descarga");
				B_B.render_T(1100, 680,"Volver");
			break;
			}
		}
		//global render
		if(isDownloading&& downloadstate != statenow){
			T_D.loadFromRenderedText(GOD.digifont, ""+DownTitle.substr(0,42)+"... ("+std::to_string(porcendown)+"\%)", {50,50,50});
			VOX.render_VOX({SCREEN_WIDTH - T_D.getWidth() - 2, 671-T_D.getHeight()+4, T_D.getWidth()+4, T_D.getHeight()-5}, 255, 255, 255, 180);
			T_D.render(SCREEN_WIDTH - T_D.getWidth() - 1, 671-T_D.getHeight());
		}
		if (AppletMode) GOD.PleaseWait("Esta App No funciona en Modo Applet. Pulsa R Al Abrir un Juego",false);
		
		//clock cicle 1s
		int maxt=100;
		static int net=maxt;
		static unsigned long long time2 = 0;
		if (onTimeC(1000,time2)){		
			if (!HasConnection()) {
				isConnected=false;
				if (net <= 0){quit=true;} else {net--;}
			} else {
				isConnected=true;
				net=maxt;
			}
			if (Frames>0){
				static int rest=0;
				printf("Frames %d - FPS: %d \r",Frames,Frames-rest);
				fflush(stdout);
				rest=Frames;
			}
		}
		if (!isConnected){
			gTextTexture.loadFromRenderedText(GOD.digifont, "Sin Internet, Cerrando: "+std::to_string(net), {255,0,0});
			VOX.render_VOX({SCREEN_WIDTH - gTextTexture.getWidth() - 8,0, gTextTexture.getWidth()+4, gTextTexture.getHeight()+2}, 0, 0, 0, 180);
			gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 5, 1 );
		}

		if(programationstate != statenow && isHandheld){BACK.render(SCREEN_WIDTH - USER.getWidth() - BACK.getWidth() - 30, 1);}
		B_P.render_T(140, 680,"Salir",quit);
		B_M.render_T(10, 680,"Música",(Mix_PausedMusic() == 1 || Mix_PlayingMusic() == 0));
		SDL_SetRenderDrawBlendMode(GOD.gRenderer, SDL_BLENDMODE_BLEND);//enable alpha blend
		
		//Update screen
		SDL_RenderPresent(GOD.gRenderer);
		
		if(!isLoaded){LoadImages();}

		//Display the list
		if (!quit&&!reloading&&!AppletMode&&Frames>2) {preview = true;}
		
		if (Frames>1000)Frames=0;
		if (Frames>0)Frames++;
	}
} catch(...){
	printf("Error Catched\n");
	GOD.PleaseWait("A ocurrido un error Critico la app se va a cerrar",true);
	std::cout << "com: " << BD["com"] << std::endl;
	write_DB(BD,rootdirectory+"DataBase.json.bak");
	quit=true;
}
	//Quit if loop break for no reason
	quit=true;
	cancelcurl=1;
	//clear allocate
	BD["com"] = "{}"_json;

	// write prettified JSON
	write_DB(BD,rootdirectory+"DataBase.json");
//	write_DB(UD,rootsave+"UserData.json");
	
	if (AppletMode){
		appletRequestLaunchApplication (0x05B9DB505ABBE000, NULL);
	} 

	if (NULL == capithread) {printf("capithread Not in use: %s\n", SDL_GetError());} else {printf("capithread in use: %s\n", SDL_GetError());SDL_WaitThread(capithread, NULL);}
	if (NULL == downloadthread) {printf("downloadthread Not in use: %s\n", SDL_GetError());} else {printf("downloadthread in use: %s\n", SDL_GetError());SDL_WaitThread(downloadthread, NULL);}
	if (NULL == prothread) {printf("prothread Not in use: %s\n", SDL_GetError());}else {printf("prothread in use: %s\n", SDL_GetError());SDL_WaitThread(prothread, NULL);}
	if (NULL == searchthread) {printf("searchthread Not in use: %s\n", SDL_GetError());} else {printf("searchthread in use: %s\n", SDL_GetError());SDL_WaitThread(searchthread,NULL);}
	
	

	//Free loaded images
	gTextTexture.free();
	Farest.free();
	Heart.free();
	
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
	BUS.free();
	REC.free();
	USER.free();
	NFAV.free();
	CLEAR.free();
	SCREEN.free();

	//Free resources and close SDL
	GOD.deint();
	accountExit();
	hidsysExit();
	socketExit();
	romfsExit();
#ifdef USENAND
	//unmount and commit
	fsdevCommitDevice("save");
	fsdevUnmountDevice("save");
	fsFsClose(&acc);

	fsdevCommitDevice("user");
	fsdevUnmountDevice("user");
	fsFsClose(&data);
#endif
	accountExit();
	//if (!isConnected) appletRequestToSleep();
	return 0;
}