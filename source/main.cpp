

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
	read_DB(BD,rootdirectory+"DataBase.json");
	BD["com"] = "{}"_json;
	if(!BD["USER"].empty()){
		UD=BD["USER"];
		BD.erase("USER");
	}
	read_DB(UD,rootdirectory+"UserData.json");
	
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
	REC.loadFromFile("romfs:/buttons/REC.png");
	BUS.loadFromFile("romfs:/buttons/BUS.png");
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
					} else {
						GOD.fingermotion_DOWN = false;
						GOD.fingermotion_UP = false;
					}
					GOD.fingermotion=true;
					GOD.TouchX = -1;
					GOD.TouchY = -1;
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
								std::string tempurl = BD["com"]["ActualLink"].get<std::string>() + std::to_string(latest) + "/";
								if(!onlinejkanimevideo(tempurl,arrayservers[selectserver])){
									arrayservers.erase(arrayservers.begin()+selectserver);
								} else {
									serverpront = false;
									UD["chapter"][KeyName]["latest"] = latest;
									
									std::string item=BD["com"]["ActualLink"].get<std::string>();
									int hsize = UD["history"].size();
									if (hsize > 56){UD["history"].erase(0);}//limit history
									if (hsize > 0){
										UD["history"].erase(std::remove(UD["history"].begin(), UD["history"].end(), item), UD["history"].end());
									}
									UD["history"].push_back(item);
									write_DB(UD,rootdirectory+"UserData.json");

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
							std::string namefav=KeyOfLink(BD["arrays"]["favorites"]["link"][favchapter]);
							if(!UD["chapter"][namefav]["Favorite"].empty()){
								UD["chapter"][namefav].erase("Favorite");
							}
							
							delFavorite(favchapter);
							if (favchapter > 0) favchapter--;
							getFavorite();
							statenow = favoritesstate;
							write_DB(UD,rootdirectory+"UserData.json");
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
								Frames=1;
							}
							break;
						case downloadstate:
							break;
						case chapterstate:
						{//AGREGAR A FAVORITOS
							if(!isFavorite(BD["com"]["ActualLink"])){
								UD["chapter"][KeyName]["Favorite"] = "true";
								write_DB(UD,rootdirectory+"UserData.json");

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
							if (!reloadingsearch)
							{
								if (BD["searchtext"].empty()){BD["searchtext"]="";}
								BD["searchtext"] = KeyboardCall("Buscar el Anime",BD["searchtext"]);
								if ((BD["searchtext"].get<std::string>()).length() > 0){
									searchchapter = 0;
									reloadingsearch = true;	 
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

						case favoritesstate:
							GOD.HandleList(favchapter, BD["arrays"]["favorites"]["link"].size(), e.jbutton.button, ongridF);
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

						case favoritesstate:
							GOD.HandleList(favchapter, BD["arrays"]["favorites"]["link"].size(), e.jbutton.button, ongridF);
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
			VOX.render_VOX({0,0, 1280, 60} ,200, 200, 200, 130);
			VOX.render_VOX({0,671, 1280, 50}, 210, 210, 210, 115);//Draw a rectagle to a nice view
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
				int XS=-50 , YS =0;
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
				if (maxcapit >= 0&&BD["com"]["nextdate"] != "Pelicula"){//draw caps Scroll
					VOX.render_VOX({posxbase + 70+XS, posybase + 571+YS, 420, 33 }, 50, 50, 50, 200);
					if (latest-2 >= mincapit) {
						gTextTexture.loadFromRenderedText(GOD.gFont3,  std::to_string(latest-2), textGray);
						gTextTexture.render(posxbase + 150 +XS-gTextTexture.getWidth()/2, posybase + 558+YS);
					}
					if (latest-1 >= mincapit) {
						gTextTexture.loadFromRenderedText(GOD.gFont3,  std::to_string(latest-1), textGray);
						gTextTexture.render(posxbase + 215+XS-gTextTexture.getWidth()/2, posybase + 558+YS);
					}
					
					if (serverpront){
						gTextTexture.loadFromRenderedText(GOD.gFont3, std::to_string(latest), { 255, 255, 255 });
						gTextTexture.render(posxbase + 280+XS-gTextTexture.getWidth()/2, posybase + 558+YS);
					} else {
						T_T.loadFromRenderedText(GOD.gFont3, std::to_string(latest), { 255, 255, 255 });
						T_T.render(posxbase + 280+XS-T_T.getWidth()/2, posybase + 558+YS);
					}

					if (latest+1 <= maxcapit) {
						gTextTexture.loadFromRenderedText(GOD.gFont3,  std::to_string(latest+1), textGray);
						gTextTexture.render(posxbase + 345+XS-gTextTexture.getWidth()/2, posybase + 558+YS);
					}
					if (latest+2 <= maxcapit) {
						gTextTexture.loadFromRenderedText(GOD.gFont3,  std::to_string(latest+2), textGray);
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
						T_T.loadFromRenderedText(GOD.gFont3, "Reproducir...", { 255, 255, 255 });
						T_T.render(posxbase + 282+XS-T_T.getWidth()/2, posybase + 558+YS);
					} else {
						gTextTexture.loadFromRenderedText(GOD.gFont3, "Cargando...", { 255, 255, 255 });
						gTextTexture.render(posxbase + 282+XS-gTextTexture.getWidth()/2, posybase + 558+YS);
					}
					
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

			if(gFAV){FAV.render_T(1230, 70,"");}
			else {B_Y.render_T(dist, 680,"Favorito");dist -= posdist;}
			
			if(!BD["DataBase"][KeyName]["Secuela"].empty()){
				std::string imagelocal=BD["DataBase"][KeyName]["Secuela"];
				imagelocal = KeyOfLink(imagelocal);
				imagelocal = rootdirectory+"DATA/"+imagelocal+".jpg";
				if(!serverpront){CheckImgNet(imagelocal);B_R.render_T(dist, 680,"Secuela");dist -= posdist;}
				GOD.Cover(imagelocal,660,457,"Secuela",120,GOD.BT_R);
			}
			if(!BD["DataBase"][KeyName]["Precuela"].empty()){
				std::string imagelocal=BD["DataBase"][KeyName]["Precuela"];
				imagelocal = KeyOfLink(imagelocal);
				imagelocal = rootdirectory+"DATA/"+imagelocal+".jpg";
				if(!serverpront){CheckImgNet(imagelocal);B_L.render_T(dist, 680,"Precuela");dist -= posdist;}
				GOD.Cover(imagelocal,520,457,"Precuela",120,GOD.BT_L);
			}

			break;
			}
			case programationstate:	{
				if (!reloading&&BD["arrays"]["chapter"]["link"].size()>=1) {
					VOX.render_VOX({0,671, 1280, 50}, 210, 210, 210, 115);//Draw a rectagle to a nice view

					if(ongrid){
						if (preview)
						{
							GOD.ListCover(selectchapter,BD["arrays"]["chapter"],ongrid,Frames);
						}
						REC.render_T(5, 15,"");
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
					#ifdef USENAND
						std::string TYPEA =  "emmc";
					#else
						std::string TYPEA =  "sdmc";
					#endif
					gTextTexture.loadFromRenderedText(GOD.digifontC, (TYPEA+" (Ver "+VERCAT+") #KASTXUPALO").c_str(), {100,0,0});
					gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 5, 670);
					
					gTextTexture.loadFromRenderedText(GOD.gFont, "Recientes", {100,0,0});
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
					std::string textpro="Cargando programación";
					if(imgNumbuffer>0){textpro+=" "+std::to_string(imgNumbuffer)+"/30";} else {textpro+="...";}
					GOD.PleaseWait(textpro,false);
				}
				break;
			}
			case searchstate:		{
				if (!reloadingsearch) {
					//Draw footer
					VOX.render_VOX({0,671, 1280, 50}, 210, 210, 210, 115);
					
					int srchsize=BD["arrays"]["search"]["link"].size();
					if (srchsize > 0){
						//if (srchsize > 30) ongridS=false;
						if (!ongridS) GOD.ListClassic(searchchapter,BD["arrays"]["search"]);
						if (preview)
						{
							if (ongridS){
								GOD.ListCover(searchchapter,BD["arrays"]["search"],ongridS,Frames);
								BUS.render_T(5, 15,"");
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
					gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 5, 2);
					{//Draw footer buttons
						int dist = 1100,posdist = 160;
						B_A.render_T(dist, 680,"Aceptar");dist -= posdist;
						B_B.render_T(dist, 680,"Atras");dist -= posdist;
						B_R.render_T(dist, 680,"Buscar");dist -= posdist;
					}
					gTextTexture.loadFromRenderedText(GOD.gFont, BD["searchtext"], {0,0,0});
					gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 15, 22);
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
				//Draw footer
				VOX.render_VOX({0,671, 1280, 50}, 210, 210, 210, 115);
				
				int favsize=BD["arrays"]["favorites"]["link"].size();
				if (favsize > 0){
					//if (favsize > 30) ongridF=false;
					if (!ongridF) GOD.ListClassic(favchapter,BD["arrays"]["favorites"]);
					if (preview)
					{
						if (ongridF){
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
				gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 5, 2);

				if (porcentajebufferF > 0){
					gTextTexture.loadFromRenderedText(GOD.gFont, "Búfer: ("+std::to_string(porcentajebufferF)+"/"+std::to_string(porcentajebufferFF)+")", {0,100,0});
					gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 30, 20);
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
			T_D.loadFromRenderedText(GOD.digifont, ""+DownTitle.substr(0,22)+"... ("+std::to_string(porcendown)+"\%)", {50,150,0});
			if (statenow == programationstate){
				het = porcentajebuffer > 0 ? T_D.getHeight()+22 : 20;
			}
			if (statenow == searchstate){
				het = 42;
			}
			if (statenow == favoritesstate){
				het = porcentajebufferF > 0 ? T_D.getHeight()+22 : 20;
			}
			if (statenow == chapterstate){
				het=10;
			}
			T_D.render(SCREEN_WIDTH - T_D.getWidth() - 8, het);
		}
		if (AppletMode) GOD.PleaseWait("Esta App No funciona en Modo Applet. Pulsa R Al Abrir un Juego",false);
		
		//clock cicle 1s
		int maxt=100;
		static int net=maxt;
		static int time2 = 0;
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
	std::cout << "com: " << BD["com"] << std::endl;
	write_DB(BD,rootdirectory+"DataBase.json.bak");
}
	cancelcurl=1;
	//clear allocate
	BD["com"] = "{}"_json;
//	BD["history"] = "{}"_json;

	// write prettified JSON
	write_DB(BD,rootdirectory+"DataBase.json");
//	write_DB(UD,rootdirectory+"UserData.json");

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