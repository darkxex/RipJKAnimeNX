

#include "JKanime.hpp"

//use the nand of the switch
#ifdef USENAND
std::string rootdirectory = "user:/RipJKAnime_NX/";
std::string oldroot = "sdmc:/switch/RipJKAnime_NX/";
#else
std::string rootdirectory = "sdmc:/switch/RipJKAnime_NX/";
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
	printf("printf output now goes to nxlink server\n");
	struct stat st = { 0 };
	#ifdef USENAND
		//mount user
		FsFileSystem data;
		fsOpenBisFileSystem(&data, FsBisPartitionId_User, "");
		fsdevMountDevice("user", data);
	#endif
	AppletMode=GetAppletMode();
	//mkdir((rootdirectory+"Video").c_str(), 0777);
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

	// read a JSON file
	std::ifstream inf(rootdirectory+"DataBase.json");
	if(!inf.fail()){
		std::string tempjson="";
		for(int f = 0; !inf.eof(); f++)
		{
			string TempLine = "";
			getline(inf, TempLine);
			tempjson += TempLine;
		}
		inf.close();
		if(json::accept(tempjson))
		{
			//Parse and use the JSON data
			BD = json::parse(tempjson);
			BD["com"] = "{}"_json;
			std::cout  << "Json Readed..." << std::endl;
		}
	}
	//printf("-- %d %d \n",BT_0,BT_1);
	//std::cout  << BD << std::endl;
	
	SDL_Thread* prothread = NULL;
	SDL_Thread* searchthread = NULL;
	SDL_Thread* downloadthread = NULL;
	
	//Set main Thread get images and descriptions
	prothread = SDL_CreateThread(refrescarpro, "prothread", (void*)NULL);

	//set custom music 
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
	CLEAR.loadFromFile("romfs:/buttons/clear.png");
	SCREEN.loadFromFile("romfs:/buttons/screen.png");
	FAV.loadFromFile("romfs:/buttons/FAV.png");/////
	NOP.loadFromFile("romfs:/nop.png");


	SDL_Color textColor = { 50, 50, 50 };
	SDL_Color textWhite = { 255, 255, 255 };
	SDL_Color textGray = { 200, 200, 200 };

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
	//While application is running
	while (!quit)
	{
		bool GRIDC=true;
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
				if (statenow == programationstate && ongrid) {GRIDC=false;}
			
				if(GOD.TouchX > 55 && GOD.TouchX < 620 && statenow != chapterstate && GRIDC){
					//swipe down go up
					if(e.tfinger.dy * SCREEN_HEIGHT > 15)
					{
						e.jbutton.button = GOD.BT_DOWN;
					}
					//swipe up go down
					else if(e.tfinger.dy * SCREEN_HEIGHT < -15)
					{
						e.jbutton.button = GOD.BT_UP;
					} else {
						break;
					}
				SDL_Log("motion %f \n",e.tfinger.dy * SCREEN_HEIGHT);
				} else if(e.tfinger.dy * SCREEN_HEIGHT > 30 || e.tfinger.dy * SCREEN_HEIGHT < -30 || e.tfinger.dx * SCREEN_WIDTH > 30 || e.tfinger.dx * SCREEN_WIDTH < -30){
				SDL_Log("motion %f \n",e.tfinger.dy * SCREEN_HEIGHT);
					GOD.fingermotion=true;
					GOD.TouchX = -1;
					GOD.TouchY = -1;
					break;
				}else {
					GOD.fingermotion=false;
					break;
				}
			case SDL_FINGERUP:
			if (e.type == SDL_FINGERUP){
				if(lcdoff){
					lcdoff=false;
					appletSetLcdBacklightOffEnabled(lcdoff);
				} else{
					
					GOD.TouchX = e.tfinger.x * SCREEN_WIDTH;
					GOD.TouchY = e.tfinger.y * SCREEN_HEIGHT;
					GOD.fingerdown = false;
					e.jbutton.button=-1;
					if (GOD.MasKey >=0){
						if(GOD.MapT[GOD.WorKey].SP()){
							e.jbutton.button=GOD.MasKey;
						}
						GOD.WorKey="0";GOD.MasKey=-1;
					}
					else if (B_A.SP() || T_T.SP() ) e.jbutton.button = GOD.BT_A;
					else if (B_B.SP()) e.jbutton.button = GOD.BT_B;
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
					else if (SCREEN.SP()){lcdoff=true; appletSetLcdBacklightOffEnabled(lcdoff); }
					else if (CLEAR.SP()){
						GOD.PleaseWait("Borrando cache");
						BD["DataBase"] = "{}"_json;
						BD["arrays"]["chapter"] = "{}"_json;
						BD["latestchapter"] = "";
						fsdevDeleteDirectoryRecursively((rootdirectory+"DATA").c_str());
						cancelcurl = 1;
						preview = false;
						quit = true;
					}
					SDL_Log("ScreenX %d    ScreenY %d butt %d\n",GOD.TouchX, GOD.TouchY,e.jbutton.button);
				}
				GOD.TouchX = -1;
				GOD.TouchY = -1;
			}
			case SDL_JOYAXISMOTION:
			if (e.type == SDL_JOYAXISMOTION)
			{
				if (e.jaxis.axis == 1){
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
								std::string tempurl = BD["com"]["ActualLink"].get<std::string>() + std::to_string(capmore) + "/";
								if(!onlinejkanimevideo(tempurl,arrayservers[selectserver])){
									arrayservers.erase(arrayservers.begin()+selectserver);
								} else {
									serverpront = false;
									BD["DataBase"][KeyName]["capmore"] = capmore;
									//if (BD["history"].find(tempurl)){
									int hsize = BD["history"].size();
									if (hsize > 50){BD["history"].erase(0);}//limit history
									if (hsize > 0){
										if(BD["history"][hsize-1] != tempurl){
											BD["history"].push_back(tempurl);
										}
									} else {BD["history"].push_back(tempurl);}
								}
							} else {
								if (isConnected) serverpront = true;
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
							if(!BD["DataBase"][KeyName]["Precuela"].empty()&&!serverpront){
								capBuffer(BD["DataBase"][KeyName]["Precuela"]);
								gFAV = isFavorite(BD["com"]["ActualLink"]);
							}
						}
				
						if (statenow == programationstate)
						{
							if (e.jbutton.button == GOD.BT_ZL)
								WebBrowserCall(urlc,true);
							else 
								WebBrowserCall("https://animeflv.net",true);
						}
					}
					else if (e.jbutton.button == GOD.BT_ZR) {// (ZR) button down
						std::cout  << BD << std::endl;
						lcdoff = !lcdoff;
						appletSetLcdBacklightOffEnabled(lcdoff);
						
						switch (statenow)
						{
						case favoritesstate:/*
							//please don't do-it
							delFavorite();
							
							favchapter=0;
							statenow = programationstate;
							BD["arrays"]["favorites"]["link"].clear();*/
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
							}
							break;

						case favoritesstate:
							returnnow = toprogramation;
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
							urltodownload  = BD["com"]["ActualLink"].get<std::string>() + std::to_string(capmore) + "/";
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
							if (!reloading)
							{
								getFavorite();
								returnnow = tofavorite;
								statenow = favoritesstate;
							}
							break;
						case downloadstate:



							break;
						case chapterstate:
						{//AGREGAR A FAVORITOS
							if(!isFavorite(BD["com"]["ActualLink"])){
								std::ofstream outfile;
								outfile.open(rootdirectory+"favoritos.txt", std::ios_base::app); // append instead of overwrite
								outfile << BD["com"]["ActualLink"].get<std::string>();
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
					else if (e.jbutton.button == GOD.BT_R3) {// (R3) button down
						switch (statenow)
						{//only for test 
						case chapterstate:
							if(serverpront){
								arrayservers.push_back("test");
							} else {
								std::string tempurl = BD["com"]["ActualLink"].get<std::string>() + std::to_string(capmore) + "/";
								WebBrowserCall(tempurl);
							}
							break;
						case programationstate:
							ongrid = !ongrid;
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
								capBuffer(BD["DataBase"][KeyName]["Secuela"]);
								gFAV = isFavorite(BD["com"]["ActualLink"]);
							}
						break;
						case programationstate:
						case searchstate:
							if (!reloadingsearch)
							{
								if (BD["searchtext"].empty()){BD["searchtext"]="";}
								BD["searchtext"] = KeyboardCall("Buscar el Anime",BD["searchtext"]);
								if ((BD["searchtext"].get<std::string>()).length() > 0){
									searchchapter = 0;
									BD["arrays"]["search"]["link"].clear();
									BD["arrays"]["search"]["images"].clear();
									statenow = searchstate;
									returnnow = tosearch;
									searchthread = SDL_CreateThread(searchjk, "searchthread", (void*)NULL);
								}
							}
							break;

						}

					}
					else if (e.jbutton.button == GOD.BT_LEFT || e.jbutton.button == GOD.BT_LS_LEFT) {// (left) button down

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
						case programationstate:
							if (!reloading)
							{
								if(ongrid){
									if (selectchapter > 0)
									{
										selectchapter--;
									} else {
										selectchapter = BD["arrays"]["chapter"]["link"].size() - 1;
									}
								}

							}
							break;
						}
					}
					else if (e.jbutton.button == GOD.BT_RIGHT || e.jbutton.button == GOD.BT_LS_RIGHT) {// (right) button down

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
						case programationstate:
							if (!reloading)
							{
								if(ongrid){
									if (selectchapter < (int)BD["arrays"]["chapter"]["link"].size() - 1)
									{
										selectchapter++;
									} else {
										selectchapter = 0;
									}
								}
							}
							break;
						}
					}
					else if (e.jbutton.button == GOD.BT_UP || e.jbutton.button == GOD.BT_LS_UP) {// (up) button down

						switch (statenow)
						{
						case programationstate:
							if (!reloading)
							{
								if(ongrid){
									if (selectchapter > 0)
									{
										selectchapter-=10;
									} else {
										selectchapter = BD["arrays"]["chapter"]["link"].size() - 1;
									}
								} else {
									if (selectchapter > 0)
									{
										selectchapter--;
									} else {
										selectchapter = BD["arrays"]["chapter"]["link"].size() - 1;
									}
								}

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
							if (!reloadingsearch&&(BD["arrays"]["search"]["link"].size() >= 1))
							{
								if (searchchapter > 0)
								{
									searchchapter--;
								} else {
									searchchapter = BD["arrays"]["search"]["link"].size() - 1;
								}
							}
							break;

						case favoritesstate:
							if (favchapter > 0)
							{
								favchapter--;
							}
							else {
								favchapter = (int)BD["arrays"]["favorites"]["link"].size() - 1;
							}
							break;
						}
					}
					else if (e.jbutton.button == GOD.BT_DOWN || e.jbutton.button == GOD.BT_LS_DOWN) {// (down) button down
						switch (statenow)
						{
						case searchstate:
							if (!reloadingsearch&&(BD["arrays"]["search"]["link"].size() >= 1))
							{
								
								if (searchchapter < (int)BD["arrays"]["search"]["link"].size() - 1)
								{
									searchchapter++;
								} else {
									searchchapter = 0;
								}

							}
							break;

						case programationstate:
							if (!reloading)
							{
								if(ongrid){
									if (selectchapter < (int)BD["arrays"]["chapter"]["link"].size() - 1)
									{
										selectchapter+=10;
									} else {
										selectchapter = 0;
									}
								} else {
									if (selectchapter < (int)BD["arrays"]["chapter"]["link"].size() - 1)
									{
										selectchapter++;
									} else {
										selectchapter = 0;
									}
								}
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
								}
							}
							break;

						case favoritesstate:
							if (favchapter < (int)BD["arrays"]["favorites"]["link"].size() - 1)
							{
								favchapter++;
							} else {
								favchapter = 0;
							}
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
		
		//render states
		switch (statenow)
		{
			case chapterstate:		{
			//Draw a background to a nice view
			VOX.render_VOX({0,0, SCREEN_WIDTH, 670} ,170, 170, 170, 100);
			VOX.render_VOX({0,671, 1280, 50}, 210, 210, 210, 115);//Draw a rectagle to a nice view
			std::string temptext = BD["com"]["ActualLink"];
			NameOfLink(temptext);

			//warning , only display in sxos ToDo
			gTextTexture.loadFromRenderedText(GOD.gFont, "(*En SXOS desactiva Stealth Mode*)", textColor);
			gTextTexture.render(posxbase, 0 );
			
			{//draw back rectangle
				VOX.render_VOX({ SCREEN_WIDTH - 442,58, 404, 590}, 0, 0, 0, 200);
				//draw preview image
				GOD.Image(BD["com"]["ActualLink"],SCREEN_WIDTH - 440, 60,400, 550,GOD.BT_A);
			}	


			//draw Title
			gTextTexture.loadFromRenderedText(GOD.gFont3, temptext.substr(0,62)+ ":", textColor);
			gTextTexture.render(posxbase+10, posybase);

			{//draw description
				VOX.render_VOX({25,60, 770, 340}, 255, 255, 255, 100);
				static std::string rese_prot = "..";
				if (rese_prot != BD["com"]["sinopsis"]){//load texture on text change 
					T_R.loadFromRenderedTextWrap(GOD.gFont, BD["com"]["sinopsis"], textColor, 750);
					rese_prot = BD["com"]["sinopsis"];
				}
				T_R.render(posxbase+15, posybase + 65);

				gTextTexture.loadFromRenderedTextWrap(GOD.gFont, BD["com"]["Emitido"], textColor,750);
				gTextTexture.render(posxbase+15, posybase + 350-gTextTexture.getHeight());

				gTextTexture.loadFromRenderedTextWrap(GOD.gFont, BD["com"]["generos"], textColor,750);
				gTextTexture.render(posxbase+25, posybase + 380-gTextTexture.getHeight());
			}
			bool anend=false;
			int sizefix = 0;
			if (maxcapit >= 0){
				if (BD["com"]["nextdate"] == "Pelicula"){
					gTextTexture.loadFromRenderedText(GOD.gFont3, "Pelicula", { 250,250,250 });
					gTextTexture.render(posxbase + 820, posybase + 598);
				} else {
					if (BD["com"]["enemision"] == "true")
					{
						gTextTexture.loadFromRenderedText(GOD.gFont3, "En Emisión ", { 16,191,0 });
						gTextTexture.render(posxbase + 820, posybase + 598);
					} else {
						gTextTexture.loadFromRenderedText(GOD.gFont3, "Concluido", { 140,0,0 });
						gTextTexture.render(posxbase + 820, posybase + 598);
					}
					gTextTexture.loadFromRenderedText(GOD.gFont, BD["com"]["nextdate"], { 255,255,255 });
					gTextTexture.render(posxbase + 1020, posybase + 615);
				}
				int mwide = 35;//52
				int XD = 310;
				int YD = 582;
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

			//use this to move the element
			int XS=100 , YS =0;
			if(serverpront){
				if (anend){
					B_UP.render_T(280+XS, 530+YS-sizefix,"");
				}
				B_DOWN.render_T(280+XS, 630+YS,"");
			}
			if (maxcapit >= 0&&BD["com"]["nextdate"] != "Pelicula"){//draw caps Scroll
				VOX.render_VOX({posxbase + 70+XS, posybase + 571+YS, 420, 33 }, 50, 50, 50, 200);
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
				if (BD["com"]["nextdate"] == "Pelicula"){
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
			else {B_Y.render_T(dist, 680,"Favorito");dist -= posdist;}
			
			if(!serverpront){
				if(!BD["DataBase"][KeyName]["Secuela"].empty()){
					B_R.render_T(dist, 680,"Secuela");dist -= posdist;
				}
				if(!BD["DataBase"][KeyName]["Precuela"].empty()){
					B_L.render_T(dist, 680,"Precuela");dist -= posdist;
				}
			}
			
			
			break;
			}
			case programationstate:	{
				if (!reloading&&BD["arrays"]["chapter"]["link"].size()>=1) {
					VOX.render_VOX({0,671, 1280, 50}, 210, 210, 210, 115);//Draw a rectagle to a nice view

					if(ongrid){
						VOX.render_VOX({0,0, SCREEN_WIDTH, 670} ,170, 170, 170, 100);
						VOX.render_VOX({0,0, SCREEN_WIDTH, 60} ,200, 200, 200, 130);
						
						int chapsize = BD["arrays"]["chapter"]["link"].size() - 1;
						if (selectchapter < 0) {
							selectchapter = chapsize+selectchapter+0;
							//selectchapter++;
						} else if (selectchapter > chapsize) {
							selectchapter = selectchapter-chapsize;
							//selectchapter--;
						}
						if (preview)
						{
							for (int x = 0; x < (int)BD["arrays"]["chapter"]["link"].size(); x++) {
								if (Frames>0 && Frames < x) break;
								GOD.ListCover(x,selectchapter,BD["arrays"]["chapter"]["link"][x],ongrid);
								if(!BD["arrays"]["chapter"]["date"].empty()){
									if (x == selectchapter) {
										//draw Title
										gTextTexture.loadFromRenderedText(GOD.digifont, BD["arrays"]["chapter"]["date"][x], { 0, 0, 0 });
										gTextTexture.render(20, 37);
									}
								}
							}
						}
					} else {
						VOX.render_VOX({0,0, 620, 670}, 200, 200, 200, 115);//Draw a rectagle to a nice view
						if(GOD.TouchY < 670 && GOD.TouchX < 530 && GOD.TouchY > 5 && GOD.TouchX > 15){
							u32 sel=(GOD.TouchY*30/660);
							if (sel >= 0 && sel < BD["arrays"]["chapter"]["link"].size()){
								selectchapter = sel;
							} 
						}
						for (int x = 0; x < (int)BD["arrays"]["chapter"]["link"].size(); x++) {
							std::string temptext = BD["arrays"]["chapter"]["link"][x];
							temptext = temptext.substr(0,temptext.length()-1);
							NameOfLink(temptext);

							temptext = (temptext.substr(0,temptext.rfind(" ")).substr(0,53) + " " + temptext.substr(temptext.rfind(" ")) );

							if (x == selectchapter) {
								T_T.loadFromRenderedText(GOD.digifont, temptext, { 255,255,255 });
								VOX.render_VOX({posxbase-2,posybase + (x * 22), 590, T_T.getHeight()}, 0, 0, 0, 105);
								T_T.render(posxbase, posybase + (x * 22));

								Heart.render(posxbase - 18, posybase + 3 + (x * 22));
								
							}
							else
							{
								gTextTexture.loadFromRenderedText(GOD.digifont, temptext.substr(0,58), textColor);
								gTextTexture.render(posxbase, posybase + (x * 22));
							}
							
							if (preview)
							{
								GOD.ListCover(x,selectchapter,BD["arrays"]["chapter"]["link"][x]);
							}
						}
						B_UP.render_T(580, 5,"");
						B_DOWN.render_T(580, 630,"");
					}

					//Draw Header
					std::string VERCAT =  VERSION;
					#ifdef USENAND
						std::string TYPEA =  "emmc";
					#else
						std::string TYPEA =  "sdmc";
					#endif
					gTextTexture.loadFromRenderedText(GOD.gFont, (TYPEA+" (Ver "+VERCAT+") #KASTXUPALO").c_str(), {100,0,0});
					gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 5, 2);
					if (imgNumbuffer > 0){
						gTextTexture.loadFromRenderedText(GOD.gFont, "Imagenes: ("+std::to_string(imgNumbuffer)+"/30)", {0,100,0});
						gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 15, 22);
						//Heart.render(posxbase + 570, posybase + 3 + (imgNumbuffer-1) * 22);
					}
					if (porcentajebuffer > 0){
						gTextTexture.loadFromRenderedText(GOD.gFont, "Buffering: ("+std::to_string(porcentajebuffer)+"/30)", {0,100,0});
						gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 15, 22);
					}

					//Draw footer buttons
					int dist = 1100,posdist = 170;
					B_A.render_T(dist, 680,"Aceptar");dist -= posdist;
					B_R.render_T(dist, 680,"Buscar");dist -= posdist;
					B_L.render_T(dist, 680,"AnimeFLV");dist -= posdist;
					B_Y.render_T(dist, 680,"Favoritos");dist -= posdist;
					CLEAR.render_T(dist, 680,"Cache");dist -= posdist;
					if(isDownloading) {B_X.render_T(dist, 680,"Descargas");dist -= posdist;}
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
					gTextTexture.loadFromRenderedText(GOD.gFont, "Búsqueda", {100,0,0});
					gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 30, 20);

					if(GOD.TouchY < 670 && GOD.TouchX < 530 && GOD.TouchY > 1 && GOD.TouchX > 15){
						u32 sel=(GOD.TouchY*30/670);
						if (sel >= 0 && sel < BD["arrays"]["search"]["link"].size()){
							searchchapter = sel;
						}
					}
					int srchsize=BD["arrays"]["search"]["link"].size();
					if (srchsize >= 1){
						VOX.render_VOX({0,0, 620, 670}, 100, 100, 100, 115);
						VOX.render_VOX({0,671, 1280, 50}, 210, 210, 210, 115);//Draw a rectagle to a nice view

						int of = searchchapter < 30 ? 0 : searchchapter - 26;
						if (srchsize > 30) {
							gTextTexture.loadFromRenderedText(GOD.gFont, std::to_string(searchchapter+1)+"/"+std::to_string(BD["arrays"]["search"]["link"].size()), {0,0,0});
							gTextTexture.render(400, 690);
						}
						for (int x = of; x < srchsize; x++) {
							std::string temptext = BD["arrays"]["search"]["link"][x];
							NameOfLink(temptext);
							
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
						if (preview)
						{
							for (int x = 0; x < srchsize; x++) {
								GOD.ListCover(x,searchchapter,BD["arrays"]["search"]["link"][x]);
							}
						}
					}else {
						NOP.render_T(230, 355,"?");
						BD["searchtext"]="";
					}
					
					{//Draw footer buttons
						int dist = 1100,posdist = 160;
						B_A.render_T(dist, 680,"Aceptar");dist -= posdist;
						B_B.render_T(dist, 680,"Atras");dist -= posdist;
						B_R.render_T(dist, 680,"Buscar");dist -= posdist;
					}
					B_UP.render_T(580, 5,"");
					B_DOWN.render_T(580, 630,"");
				}
				else
				{
					GOD.PleaseWait("Cargando búsqueda... (" + std::to_string(BD["com"]["porcentajereload"].get<int>()) + "%)",false);
				}
				break;
			}
			case favoritesstate:	{
				VOX.render_VOX({0,671, 1280, 50}, 210, 210, 210, 115);//Draw a rectagle to a nice view
				//Draw Header
				gTextTexture.loadFromRenderedText(GOD.gFont, "Lista de Favoritos", {100,0,0});
				gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 30, 20);
				
				if(GOD.TouchY < 670 && GOD.TouchX < 530 && GOD.TouchY > 5 && GOD.TouchX > 15){
					u32 sel=(GOD.TouchY*30/660);
					if (sel >= 0 && sel < BD["arrays"]["favorites"]["link"].size()){
						favchapter = sel;
					}
				}

				int FavLsize = BD["arrays"]["favorites"]["link"].size();
				if (FavLsize >= 1 ){
					VOX.render_VOX({0,0, 620, 670}, 150, 150, 150, 115);
					if (FavLsize > 30) {
						gTextTexture.loadFromRenderedText(GOD.gFont, std::to_string(favchapter+1)+"/"+std::to_string(FavLsize), {0,0,0});
						gTextTexture.render(400, 690);
					}
					int of = favchapter < 30 ? 0 : favchapter - 26;
					for (int x = of; x < FavLsize; x++) {
						std::string temptext = BD["arrays"]["favorites"]["link"][x];
						NameOfLink(temptext);
						
						if (x == favchapter) {
							T_T.loadFromRenderedText(GOD.digifont, temptext.substr(0,58), { 255,255,255 });
							VOX.render_VOX({posxbase-2,posybase + ((x-of) * 22), 590, T_T.getHeight()}, 0, 0, 0, 105);
							T_T.render(posxbase, posybase + ((x-of) * 22));
							Heart.render(posxbase - 18, posybase + 3 + ((x-of) * 22));
						}
						else if((x-of) < 30)
						{
							gTextTexture.loadFromRenderedText(GOD.digifont, temptext.substr(0,58), textColor);
							gTextTexture.render(posxbase, posybase + ((x-of) * 22));
						}
					}
					for (int x = 0; x < FavLsize; x++) {
						GOD.ListCover(x,favchapter,BD["arrays"]["favorites"]["link"][x]);
					}
				}

				if (porcentajebufferF > 0){
					gTextTexture.loadFromRenderedText(GOD.gFont, "Buffering fav: ("+std::to_string(porcentajebufferF)+"/"+std::to_string(porcentajebufferFF)+")", {0,100,0});
					gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 30, 40);
				}

				{//Draw footer buttons
					int dist = 1100,posdist = 180;
					B_A.render_T(dist, 680,"Aceptar");dist -= posdist;
					B_B.render_T(dist, 680,"Volver");dist -= posdist;
					if ((int)BD["arrays"]["favorites"]["link"].size() >= 1){
						B_X.render_T(dist, 680,"Borrar #"+std::to_string(favchapter+1));dist -= posdist;
					}else NOP.render_T(230, 355,"");
					
					B_UP.render_T(580, 5,"");
					B_DOWN.render_T(580, 630,"");
				}
			break;
			}
			case downloadstate:		{
				VOX.render_VOX({16,5, 900, 282}, 210, 210, 210, 115);//Draw a rectagle to a nice view
				VOX.render_VOX({0,671, 1280, 50}, 210, 210, 210, 115);//Draw a rectagle to a nice view
				
				gTextTexture.loadFromRenderedText(GOD.gFont, "Descargando Actualmente:", textColor);
				gTextTexture.render(posxbase, posybase);
				
				gTextTexture.loadFromRenderedText(GOD.gFont3, DownTitle, textColor);
				VOX.render_VOX({17,35, gTextTexture.getWidth()+5, 45}, 210, 210, 210, 215);//Draw title back
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
						if(lcdoff){lcdoff=false; appletSetLcdBacklightOffEnabled(lcdoff);}
					 }else{
						gTextTexture.loadFromRenderedText(GOD.digifont, "Velocidad: " +speedD+" M/S", textColor);
						VOX.render_VOX({ posxbase + 120, posybase + 240, gTextTexture.getWidth()+15, 20 }, 255, 255, 255, 145);
						gTextTexture.render(posxbase + 130, posybase + 240);
						SCREEN.render(1150, 10);
						B_ZR.render_T(550, 680,"Apagar Pantalla");
					 }
				} else {
					porcendown=0;
				}
				
				VOX.render_VOX({posxbase-5,posybase + 300 , 750, ((int)BD["arrays"]["downloads"]["log"].size() * 22)+53}, 200, 200, 200, 105);
				gTextTexture.loadFromRenderedText(GOD.digifont, "Cola De Descarga::", textColor);
				gTextTexture.render(posxbase, posybase+310);
				for (u64 x = 0; x < BD["arrays"]["downloads"]["log"].size(); x++) {
					std::string descarga = BD["arrays"]["downloads"]["log"][x];
					NameOfLink(descarga);
					SDL_Color txtColor = textColor;//{ 50, 50, 50 };
					
					if(descarga.substr(0,3) == "100") txtColor = { 0, 100, 0 };
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
			T_D.loadFromRenderedText(GOD.digifont, "Downloading: "+DownTitle.substr(0,22)+"... ("+std::to_string(porcendown)+"\%)", {50,150,0});
			if (statenow == programationstate){
				het = porcentajebuffer > 0 ? T_D.getHeight()+22 : 20;
			}
			if (statenow == favoritesstate){
				het = porcentajebufferF > 0 ? T_D.getHeight()+42 : 40;
			}
			if (statenow == chapterstate){
				het=10;
			}
			T_D.render(SCREEN_WIDTH - T_D.getWidth() - 15, het);
		}
		if (AppletMode) GOD.PleaseWait("Esta App No funciona en Modo Applet. Pulsa R Al Abrir un Juego",false);
		
		//clock cicle 1s
		int maxt=100;
		static int net=maxt;
		if (onTimeC(1)){		
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
			VOX.render_VOX({SCREEN_WIDTH - gTextTexture.getWidth() - 8,1, gTextTexture.getWidth()+4, gTextTexture.getHeight()+2}, 0, 0, 0, 255);
			gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 5, 1 );
		}

		B_P.render_T(160, 680,"Salir",quit);
		B_M.render_T(10, 680,"Música",(Mix_PausedMusic() == 1 || Mix_PlayingMusic() == 0));
		SDL_SetRenderDrawBlendMode(GOD.gRenderer, SDL_BLENDMODE_BLEND);//enable alpha blend
		
		//Update screen
		SDL_RenderPresent(GOD.gRenderer);
		//Display the list
		if (!quit&&!reloading&&!AppletMode&&Frames>2) {preview = true;}
		
		if (Frames>1000)Frames=0;
		if (Frames>0)Frames++;
	}
} catch(...){
	printf("Error Catched\n");
}
	cancelcurl=1;
	//clear allocate
	BD["com"] = "{}"_json;
//
	// write prettified JSON
	write_DB(BD,rootdirectory+"DataBase.json");
	
	//appletEndBlockingHomeButton();
	
	if (AppletMode){
		appletRequestLaunchApplication (0x05B9DB505ABBE000, NULL);
	} 

	if (NULL == capithread) {
		printf("capithread Not in use: %s\n", SDL_GetError());
	}
	else {
		printf("capithread in use: %s\n", SDL_GetError());
		SDL_WaitThread(capithread, NULL);
	}
	if (NULL == downloadthread) {
		printf("downloadthread Not in use: %s\n", SDL_GetError());
	}
	else {
		printf("downloadthread in use: %s\n", SDL_GetError());
		SDL_WaitThread(downloadthread, NULL);
	}
	if (NULL == prothread) {
		printf("prothread Not in use: %s\n", SDL_GetError());
	}
	else {
		printf("prothread in use: %s\n", SDL_GetError());
		SDL_WaitThread(prothread, NULL);
	}
	if (NULL == searchthread) {
		printf("searchthread Not in use: %s\n", SDL_GetError());
	}
	else {
		printf("searchthread in use: %s\n", SDL_GetError());
		SDL_WaitThread(searchthread,NULL);
	}
	
	
	//Free resources and close SDL
	accountExit();
	hidsysExit();
	socketExit();
	romfsExit();

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

	GOD.deint();
#ifdef USENAND
	//unmount and commit
	fsdevCommitDevice("user");
	fsdevUnmountDevice("user");
	fsFsClose(&data);
#endif
	//if (!isConnected) appletRequestToSleep();
	return 0;
}