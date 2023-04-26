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
#include <filesystem>
#include <math.h>
#include "utils.hpp"
#include "SDLWork.hpp"
#include "applet.hpp"

//main SLD funct (Grafics On Display == GOD)
SDLB GOD;

LTexture TextBuffer;

//Grafics and logic
void SDLB::intA(){
	//Start up SDL and create window
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());

	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("sdl2_gles2", 0, 0, 1280, 720, 0);

		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());

		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());

			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
				SDL_RenderClear(gRenderer);

				//enable alpha blend
				SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());

				}

				//Initialize SDL_ttf
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());

				}
				//Initialize SDL_mixer
				if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
				{
					printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());

				}
				//Load sound effects
				aree = Mix_LoadWAV( "romfs:/audio/are.ogg" );
				if( aree == NULL)
				{
					printf( "Failed to load are.ogg sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
				}
				proc = Mix_LoadWAV( "romfs:/audio/processing.ogg" );
				if( proc == NULL)
				{
					printf( "Failed to load processing.ogg sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
				}
				gtemp = Mix_LoadWAV( "romfs:/audio/processing.ogg" );
				if(gtemp == NULL)
				{
					printf( "Failed to load processing.ogg sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
				}
			}
		}
	}

	NSM = TTF_OpenFont("romfs:/fonts/NintendoStandard.ttf", 22);
	//AF_19 = TTF_OpenFont("romfs:/fonts/AF.ttf", 19);
	AF_35 = TTF_OpenFont("romfs:/fonts/AF.ttf", 35);
	//Arista_27 = TTF_OpenFont("romfs:/fonts/Arista.ttf", 27);

	//Comic_19 = TTF_OpenFont("romfs:/fonts/Comic.ttf", 19);
	Comic_16 = TTF_OpenFont("romfs:/fonts/Comic.ttf", 16);
	Arista_150 = TTF_OpenFont("romfs:/fonts/Arista.ttf", 150);
	Arista_40 = TTF_OpenFont("romfs:/fonts/Arista.ttf", 40);
	Arista_30 = TTF_OpenFont("romfs:/fonts/Arista.ttf", 30);
	Arista_20 = TTF_OpenFont("romfs:/fonts/Arista.ttf", 20);
	//Arista_50 = TTF_OpenFont("romfs:/fonts/Arista.ttf", 50);
	Arista_100 = TTF_OpenFont("romfs:/fonts/Arista.ttf", 100);
	digi_9 = TTF_OpenFont("romfs:/fonts/digifont.otf", 9);
	digi_11 = TTF_OpenFont("romfs:/fonts/digifont.otf", 11);
	digi_16 = TTF_OpenFont("romfs:/fonts/digifont.otf", 16);
    //digi_20 = TTF_OpenFont("romfs:/fonts/digifont.otf", 20);
    digi_40 = TTF_OpenFont("romfs:/fonts/digifont.otf", 40);
    //digi_50 = TTF_OpenFont("romfs:/fonts/digifont.otf", 50);
}
bool SDLB::JKMainLoop(){
	try{
		//Main Pool events
		if (PlayF) {
			if (gtemp == NULL)
				cout << "Play-Effect is null" << endl;
			else
				Mix_PlayChannel(-1, gtemp, 0);
			PlayF=false;
		}

		if (ReloadSkin) {loadSkin(); ReloadSkin=false;}

		//Update screen
		SDL_RenderPresent(gRenderer);
		GOD.FrameState++;
	}catch(...) {
		LOG::E(12);
		cout << "- Error Catched Main Pool" << endl;
	}
	//if get false once then collapse
	static bool killer = true;
	if (killer) {
		killer=!quit && appletMainLoop();
	}
	return killer;
}
bool SDLB::PlayEffect(Mix_Chunk* efect){
	try{
		memcpy(gtemp, efect, sizeof(Mix_Chunk));
		PlayF = true;

		return true;
	}catch(...) {
		LOG::E(13);
		cout << "- Error Play Effect" << endl;
	}
	return false;
}
void SDLB::SwapMusic(bool swap){
	if (gMusic == NULL)
	{
		printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
		return;
	}

	if (Mix_PlayingMusic() == 0)
	{
		//Play the music
		Mix_PlayMusic(gMusic, -1);
		UD["Themes"]["play"]=1;
	}
	//If music is being played
	else if (swap)
	{
		//If the music is paused
		if (Mix_PausedMusic() == 1)
		{
			//Resume the music
			Mix_ResumeMusic();
			UD["Themes"]["play"]=1;
		}
		//If the music is playing
		else
		{
			//Pause the music
			Mix_PauseMusic();
			UD["Themes"]["play"]=0;
		}
	}
}

string defcord = "romfs:/theme/default";
string SkinMaster = defcord;
int them = 1;
string theme(string file){
	if (SkinMaster == defcord) {
		if (isFileExist(rootdirectory+file)) {
			return rootdirectory+file;
		}
	}
	if (!isFileExist(SkinMaster+file)) {
		return defcord+file;
	}
	return SkinMaster+file;
}
string NameOfTheme(string path){
	u64 v2 = path.rfind("/");
	if (v2 != string::npos) {
		path = path.substr(v2 + 1);
	}
	return path;
}
void SDLB::loadSkin(string img){
	if (UD["Themes"]["use"].is_null()) {
		UD["Themes"]["use"]=defcord;
	}
	//get Music State
	if(UD["Themes"]["play"].is_null()) {
		UD["Themes"]["play"]=0;
	}

	SkinMaster = UD["Themes"]["use"];
	UD["Themes"]["name"] = NameOfTheme(SkinMaster);
	cout << "# Theme: " << UD["Themes"]["use"] << std::endl;

	//images
	Farest.free();
	Farest.loadFromFile(theme("/background.jpg"));
	Heart.free();
	Heart.loadFromFile(theme("/heart.png"));


	//Load music
	Mix_FreeMusic(gMusic);
	gMusic = Mix_LoadMUS(theme("/music.ogg").c_str());
	if(UD["Themes"]["play"] == 1) {
		SwapMusic(false);
	}
}
void SDLB::setSkin(string path){
    if (UD["Themes"]["use"].get<string>() != path){
        UD["Themes"]["use"] = path;
        ReloadSkin=true;
    }
}
void SDLB::SkinInit(string path,bool add) {
    try {
        if (!add){
            BD["Themes"]["files"] = "[]"_json;
            BD["Themes"]["files"].push_back(defcord);
        }
        if(isFileExist(path)){
            for (const auto & entry : std::filesystem::directory_iterator(path)){
                if (entry.is_directory()){
                    if (UD["Themes"]["use"].get<string>() == entry.path()){
                       them = BD["Themes"]["files"].size()+1;
                    }
                    BD["Themes"]["files"].push_back(entry.path());
                    std::cout << entry.path() << std::endl;
                }
            }
        }
	} catch(...) {
		cout << "- Error Catched, SkinInit" <<endl;
	}
}
void SDLB::selectskin(string val) {
    try {
        //next theme
        int allp = BD["Themes"]["files"].size()-1;
        if (them > allp) them=0;
        GOD.setSkin(BD["Themes"]["files"][them]);
        them++;
	} catch(...) {
		cout << "- Error Catched, loadSkin" <<endl;
	}
}
bool SDLB::Confirm(std::string text,bool okonly,int type){

	LTexture NAG,IMG;
	//Print Menu once
	VOX.render_VOX({0,0,SCREEN_WIDTH,SCREEN_HEIGHT}, 0, 0, 0, 150);

	int NW=500,NH=210,BR = 2;//Ancho y largo
    int NX=(SCREEN_WIDTH-NW) /2, NY=(SCREEN_HEIGHT-NH)/2;//Centrado al medio

	T_T.loadFromRenderedTextWrap(Arista_40, text, { 50, 50, 50 },NW-20);

	//Draw black back
	VOX.render_VOX({NX-BR,NY-BR,NW+(BR*2),NH+(BR*2)}, 50, 50, 50, 200);
	//Draw Icon
	std::string image = "";
	switch(type) {
	case 1:
		image = "romfs:/img/pop/ques.png";
		break;
	case 2:
		image = "romfs:/img/pop/info.png";
		break;
	case 3:
		image = "romfs:/img/pop/error.png";
		break;
	}

	if (image.length()>0) {
		IMG.loadFromFileCustom(image, 100,100);
		IMG.render(NX+10, NY+10);
	}
	//Draw withe front
	NAG.render_VOX({NX,NY,NW,NH}, 200, 200, 200, 200);//
	T_T.render(NX+(NW/2)-(T_T.getWidth()/2), NY+((NH/4*3)/2)-(T_T.getHeight()/2));

	VOX.render_VOX({NX,NY+(NH/4*3),NW,BR}, 50, 50, 50, 200);
	if(!okonly) {
		VOX.render_VOX({NX+(NW/2),NY+(NH/4*3),BR,(NH/4)+2}, 50, 50, 50, 200);
		B_B.render_T(NX+(NW/4)-45, NY+(NH/4*3)+((NH/4)/2)-(B_B.getHeight()/2),"NO");
		B_A.render_T(NX+(NW/4*3)-45, NY+(NH/4*3)+((NH/4)/2)-(B_A.getHeight()/2),"SI");
	} else {
		B_A.render_T(NX+(NW/2)-55, NY+(NH/4*3)+((NH/4)/2)-(B_A.getHeight()/2),"Aceptar");
	}

	bool breaker = true;
	bool result = false;

	//Event handler
	SDL_Event e;
	//While application is running
	while (JKMainLoop()&breaker)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e))
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				cancelcurl = 1;
				quit = true;
				cout << "Saliendo NAG" << endl;
			}
			GOD.GenState = statenow;
			switch (e.type) {
			case SDL_FINGERDOWN:
				GOD.TouchX = e.tfinger.x * SCREEN_WIDTH;
				GOD.TouchY = e.tfinger.y * SCREEN_HEIGHT;
				break;
			case SDL_FINGERUP:
				e.jbutton.button = BT_B;
				if (GOD.MapT["EXIT"].SP()) e.jbutton.button = BT_P;
				else if (GOD.MapT["MUSIC"].SP()) e.jbutton.button = BT_M;
				else if (B_A.SP()) {e.jbutton.button = BT_A;}
				else if (B_B.SP()) {e.jbutton.button = BT_B;}
				else if (NAG.SP()) {e.jbutton.button = -1;}
				GOD.TouchX = -1;
				GOD.TouchY = -1;
				GOD.TouchXU = -1;
				GOD.TouchYU = -1;
			case SDL_JOYBUTTONDOWN:
				if (e.jbutton.which == 0) {
					if (e.jbutton.button == BT_P) {        // (+) button down close to home menu
						cancelcurl = 1;
						quit = true;
						result = false;
						breaker = false;
						break;
					}
					if (e.jbutton.button == BT_M) {        // (-) button down
						SwapMusic();
						result = false;
						breaker = false;
						break;
					}
					if (e.jbutton.button == BT_A) {        // (A) button down
						result = true;
						breaker = false;
						break;
					}
					if (e.jbutton.button == BT_B) {        // (B) button down
						result = false;
						breaker = false;
						break;
					}
				}
				break;
			default:
				break;
			}
		}
	}
	GOD.TouchX = -1;
	GOD.TouchY = -1;
	GOD.TouchXU = -1;
	GOD.TouchYU = -1;
	return result;
}
void SDLB::PleaseWait(std::string text,bool render){
	if (render) {
		//Clear screen
		SDL_SetRenderDrawColor(gRenderer, 0x55, 0x55, 0x55, 0xFF);
		SDL_RenderClear(gRenderer);

		//wallpaper
		Farest.render((0), (0));
	}

	SDL_Rect fillRect = { 0, 720/2 - 25, 1280, 50 };
	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
	SDL_RenderFillRect(gRenderer, &fillRect);

	TextBuffer.loadFromRenderedText(Arista_40, text.c_str(), { 50, 50, 50 });
	TextBuffer.render(1280/2 - TextBuffer.getWidth()/2, 720/2 - TextBuffer.getHeight() / 2);
	if (render) SDL_RenderPresent(gRenderer);
}
void SDLB::Image(std::string path,int X, int Y,int W, int H,int key){
//render images and map to memory for fast display
	//Image of cap
	int val0 = path.find("https://jkanime.net/");
	if (val0 != -1) {
		replace(path, "https://jkanime.net/", "");
		int val1 = path.find("/");
		path=path.substr(0,val1);
		path = rootdirectory+"DATA/"+path+".jpg";
	}
	std::string KeyImage=path.substr(path.rfind("/")+1);
	if (!isFileExist(path)) {
		KeyImage="nop.png";
		path = "romfs:/img/nop.png";
	}


	if (MapT.find(KeyImage) == MapT.end() || MapT[KeyImage].isZero()) {
		cout << "Loaded to mem :"+KeyImage+"<--"<< endl;
		MapT[KeyImage].loadFromFile(path.c_str());
	}
	MapT[KeyImage].ScaleA(H,W);

	MapT[KeyImage].render(X, Y);
	if(key >= 0) {
		if(MapT[KeyImage].SP()) {WorKey=KeyImage; MasKey=key;}
	}
}
void SDLB::Cover(std::string path,int X, int Y,std::string Text,int WS,int key,int selected){
//render images and map to memory for fast display
	std::string KeyImage=path.substr(path.rfind("/")+1);
	if (!isFileExist(path)) {
		KeyImage="nop.png";
		path = "romfs:/img/nop.png";
	}

	int sizeportraitx = 300;
	int sizeportraity =424;

	int HS = (sizeportraity * (WS * 10000 /sizeportraitx) /10000);
	if (MapT.find(KeyImage) == MapT.end() || MapT[KeyImage].isZero()) {
		MapT[KeyImage].loadFromFile(path.c_str());
	}

	//make the math
	static int blue=255;
	if(selected > 0) {
		MapT[KeyImage].TickerColor(blue,150,250);
		WS=WS+selected;
		HS = (sizeportraity * (WS * 10000 /sizeportraitx) /10000);
	} else{
		blue=0;
	}
	MapT[KeyImage].ScaleA(HS,WS);

	std::string KeyText=Text+"-"+std::to_string(WS);
	std::string KeyTextH=Text+"-"+std::to_string(WS)+"-Head";

	if (Text.length()) {
		if ( MapT.find(KeyText) == MapT.end() ) {
			std::string numhe=scrapElement(Text, "#");
			if (numhe.length() > 0) {
				replace(Text, numhe, "");
				replace(numhe, " #", ""); replace(numhe, "#", "");
				MapT[KeyTextH].loadFromRenderedText(digi_11, numhe, { 255,255,255 });
			}

			int kinsize =11;
			if(!selected) {if (WS < 115) {kinsize =7; Text=Text.substr(0,20);} }//
			TTF_Font* customFont = TTF_OpenFont("romfs:/fonts/digifont.otf", kinsize);
			MapT[KeyText].loadFromRenderedTextWrap(customFont, Text, { 255,255,255 }, WS);
		}
		MapT[KeyImage].render_VOX({ X - 3, Y - 3, WS + 6, HS + 6 + MapT[KeyText].getHeight()+8}, 0, 0, blue, 200);
		MapT[KeyText].render(X + 2, Y + 4+MapT[KeyImage].getHeight());
	} else {
		MapT[KeyImage].render_VOX({ X - 3, Y - 3, X + 6, Y + 6}, 0, 0, blue, 200);
	}

	MapT[KeyImage].render(X, Y);
	if (MapT.find(KeyTextH) != MapT.end()) {
		VOX.render_VOX({ X + MapT[KeyImage].getWidth() - MapT[KeyTextH].getWidth() - 4, Y, MapT[KeyTextH].getWidth() + 4, MapT[KeyTextH].getHeight()}, 0, 0, 0, 200);
		MapT[KeyTextH].render(X + MapT[KeyImage].getWidth() - MapT[KeyTextH].getWidth() -2, Y);
	}
	if (GenState == menu_s) {return;}
	if(key >= 0) {
		if(MapT[KeyImage].SP()) {WorKey=KeyImage; MasKey=key;}
	}
}
void SDLB::Cover_idx(std::string path,int X, int Y,std::string Text,int WS,int index,int& select,bool render){
//render images and map to memory for fast display
	std::string KeyImage=path.substr(path.rfind("/")+1);
	std::string KeyText=Text;
	std::string KeyTextH=Text+"-Head";

	if (!isFileExist(path)) {
		KeyImage="SuperPro.jpg";
		path = "romfs:/img/SuperPro.jpg";
	}
	int sizeportraitx = 300;
	int sizeportraity =424;
	//make the math
	bool needload = false;


	if ( MapT.find(KeyImage) == MapT.end() || MapT[KeyImage].isZero()) {
		needload = true;
	}

	static int HS=0;
	if (needload || HS==0) {HS = (sizeportraity * (WS * 10000 /sizeportraitx) /10000);}
	if (needload) {
		MapT[KeyImage].loadFromFile(path.c_str());
	}
	MapT[KeyImage].ScaleA(HS,WS);


	if (Text.length()) {
		if ( MapT.find(KeyText) == MapT.end() ) {
			std::string numhe=scrapElement(Text, "#");
			if (numhe.length() > 0) {
				replace(Text, numhe, "");
				replace(numhe, " #", ""); replace(numhe, "#", "");
				MapT[KeyTextH].loadFromRenderedText(digi_11, numhe, { 255,255,255 });
			}
			int kinsize =11;
			if (WS < 115) {kinsize =10; Text=Text.substr(0,16);}//
			TTF_Font* customFont = TTF_OpenFont("romfs:/fonts/digifont.otf", kinsize);
			MapT[KeyText].loadFromRenderedTextWrap(customFont, Text, { 255,255,255 }, WS);
		}
	}
	if (!render) {return;}
	if (Text.length()) {
		MapT[KeyImage].render_VOX({ X - 3, Y - 3, WS + 6, HS + 6 + MapT[KeyText].getHeight()+7}, 0, 0, 0, 200);
		MapT[KeyText].render(X + 2, Y + 5+MapT[KeyImage].getHeight());
	} else {
		MapT[KeyImage].render_VOX({ X - 3, Y - 3, X + 6, Y + 6}, 0, 0, 0, 200);
	}

	MapT[KeyImage].render(X, Y);
	if (MapT.find(KeyTextH) != MapT.end()) {
		VOX.render_VOX({ X + MapT[KeyImage].getWidth() - MapT[KeyTextH].getWidth() - 4, Y, MapT[KeyTextH].getWidth() + 4, MapT[KeyTextH].getHeight()}, 0, 0, 0, 200);
		MapT[KeyTextH].render(X + MapT[KeyImage].getWidth() - MapT[KeyTextH].getWidth() -2, Y);
	}

	if (GenState == menu_s) {return;}
	if(MapT[KeyImage].SPr()) {
		select = index;
	}

	/*
	   //How not to handle a touch input, wala, yolo
	   static int findex = -1;
	   if (findex >= 0 && !fingerdown) {
	        //printf("relea %d to %d %s\n",select,index,KeyImage.c_str());
	        select=findex;
	        findex=-1;
	   } else {
	        if(MapT[KeyImage].SPr()) {
	                //printf("touch %d to %d %s\n",select,index,KeyImage.c_str());
	                findex = index;
	        }
	        if(fingermotion&&fingerdown) {
	                //printf("reset %d to %d %s\n",select,index,KeyImage.c_str());
	                findex=-1;
	        }
	   }
	 */
}
void SDLB::ListCover(int& selectindex,json Jlinks, bool ongrid,int limit){
	std::vector<std::string> vec = Jlinks["link"];
	int AbsoluteSize=vec.size();
	int JlinksSize=AbsoluteSize;
	static int statte = GenState;
	if (statte != GenState) {statte = GenState; outof=0;}
	if(JlinksSize < 30) {outof=0;}

	JlinksSize=vec.size();
	if(ongrid) {
		if(JlinksSize > 30) {
			//Touch Handle
			if(fingermotion) {
				if(fingermotion_DOWN) {
					if(selectindex > 9 && outof>0) {
						selectindex-=10;
						outof-=10;
					}
					fingermotion_DOWN = false;
				}
				if(fingermotion_UP) {
					selectindex+= 10;
					if(selectindex < JlinksSize-1) {
						outof+=10;
					} else selectindex=JlinksSize-1;
					fingermotion_UP = false;
				}
			}
		}

		//select, bound fiscals
		int chapsize = JlinksSize - 1;
		static int preval=0;
		if (chapsize+outof < 29) {
			if (selectindex < 0) {
				selectindex=preval;
			} else if (selectindex > chapsize) {
				selectindex=chapsize;
			}
		} else {
			if (selectindex < 0) {
				selectindex = 0;
			} else if (selectindex > chapsize) {
				if (selectindex < chapsize+10&&JlinksSize > 30) {
					selectindex=chapsize;
				} else {
					selectindex = selectindex-chapsize;
					outof=0;
				}
			}
		}
		preval=selectindex;
		//normal boxes
		VOX.render_VOX({0,0, 1280, 670},170, 170, 170, 100);
		//std::cout << HR << "-" << HG << "-" << HB << std::endl;
		VOX.render_VOX({0,0, 1280, 60},HR, HG, HB, 130);

		//Calculate Offset of Grid
		if(JlinksSize > 30) {
			int outofMax=JlinksSize; outofMax+=9; outofMax=outofMax/10; outofMax=outofMax*10; outofMax-=30;
			if (outof > outofMax) outof = outofMax;
			//printf("part: %d - total: %d - Offset: %d - max offset: %d\r",selectindex,JlinksSize-1,outof,outofMax);fflush(stdout);
			ScrollBarDraw(1272, 65, 600, 5, outofMax/10+1, outof/10,false);
			//ScrollBarDraw(1272, 62, 600, 5, JlinksSize+outof, selectindex+1+outof,true);
			if(selectindex > 29+outof) {
				for (int x = 29; x-10 < selectindex; x+=10) {
					outof=x-29;
				}
			} else if (selectindex < outof) {
				outof-=10;
				if (outof<0) {outof=0;}
			}
			if (outof>0) {
				vec.erase(vec.begin(), vec.begin()+outof);
				selectindex-=outof;
			}
			JlinksSize=vec.size();
		}
	}
/*
        //reframe
        static int mateof = outof;
        if (outof != mateof){
                mateof=outof;
                if (Frames > 30){
                        Frames=19;
                }
        }
 */
	for (int x = 0; x < JlinksSize; x++) {
		//This controll the image order and logic
		std::string Link=vec[x];
		//Get the Cap Key
		replace(Link, "https://jkanime.net/", "");
		int val0 = Link.find("/");
		std::string imagelocal=Link.substr(0,val0);
		replace(Link, imagelocal, "");
		replace(Link, "/", "");

		//Cap Key to name
		std::string TEXT=imagelocal;
		replace(TEXT, "/", " ");
		replace(TEXT, "-", " ");
		mayus(TEXT);
		std::string TEXTH=TEXT.substr(0,62);
		int offsettval=0;
		if (x == selectindex) {
			static int take=selectindex;
			static int laof=-1;
			if (selectindex != take) {
				take = selectindex;
				laof=-1;
			}
			if (TEXT.length() > 25) {
				TickerName(laof,200,0,TEXT.length()-25);
				offsettval=laof;
			}
		}
		//cout << ">>>>> "  << TEXT.length() <<  ">>>>> "  << offsettval <<  ">>>>> "  << laof << endl;

		if (Link.length() > 0) {
			TEXT=TEXT.substr(offsettval,25)+" #"+Link;
			TEXTH+=" #"+Link;
		} else {
			TEXT=TEXT.substr(offsettval,25);
		}

		//Image of cap
		imagelocal = rootdirectory+"DATA/"+imagelocal+".jpg";

		//set the offset position of images
		static int MainOffSet=1,offset1=1,offset2=1,offset3=1;
		int nosel=113,issel=113;
		if (ongrid)
		{
			//Grid Animation
			if (limit>0) {
				std::string KeyImage=imagelocal.substr(25);
				if ( MapT.find(KeyImage) == MapT.end()) {
					if (limit < x) break;
				}
			}

			if (x == selectindex) {
				//draw Title
				TextBuffer.loadFromRenderedText(Arista_30, TEXTH, { 0, 0, 0 });
				TextBuffer.render(50, 9);
				if(!Jlinks["date"].is_null()) {
					//draw Title
					int datesize = Jlinks["date"].size()-1;
					if (x+outof < datesize) {
						TextBuffer.loadFromRenderedText(digi_16, Jlinks["date"][x+outof], { 0, 0, 0 });
						TextBuffer.render(50, 37);
					}
				}
			}

			if (x==0) {MainOffSet=0; offset1=0; offset2=0; offset3=0;}
			int HO = 0;
			//Get Images , render Small
			if (x < 10) {
				MainOffSet=offset1;
				HO = 70;
				offset1++;
			} else if (x < 20) {
				HO = 270;
				MainOffSet=offset2;
				offset2++;
			} else if (x < 30) {
				MainOffSet=offset3;
				HO = 470;
				offset3++;
			}
			if (GenState == menu_s) {
				//if (x < 6) continue;
				//if (x > 9 && x < 16) continue;
			}
			if ((AbsoluteSize < Frames)&&(x > 30)) break;
			if (x == selectindex) {
				Cover(imagelocal,10 +  (MainOffSet * 127)-4,HO-13,TEXT,issel,BT_A,7);
			} else {
				selectindex+=outof;
				//if(x >= 30) break;
				//if(x < 30) break;
				Cover_idx(imagelocal,10 +  (MainOffSet * 127),HO,TEXT,nosel,x+outof,selectindex,(x < 30));
				selectindex-=outof;

			}
		} else {
			if (x==0) {offset1 =1; offset2 =1;}
			//Get 4 Images before, render Small
			if ((x > selectindex-5) && (x < selectindex)) {
				int comp=0;//this is to get closer to te main image
				if (selectindex < 5) {
					comp = offset1+(4-selectindex);
				} else {
					comp = offset1;
				}
				Cover(imagelocal,600+  (comp * 30),  (comp * 22),TEXT,nosel,BT_UP);
				offset1++;
			}
			//Central Big image
			if (x == selectindex) {
				Cover(imagelocal,680+ 132,  132,TEXT,255,BT_A);
			}
			//Get 4 Images After, render small
			if ((x < selectindex+5) && (x > selectindex)) {
				Cover(imagelocal,1030+ (offset2 * 30), 400 + (offset2 * 22),TEXT,nosel,BT_DOWN);
				offset2++;
			}
		}
	}
	if(ongrid) {
		selectindex+=outof;
	}
}
void SDLB::ListClassic(int& selectindex,json Jlinks) {
	int indexLsize = Jlinks["link"].size();
	if(TouchY < 670 && TouchX < 530 && TouchY > 5 && TouchX > 15) {
		int sel=(TouchY*30/660);
		if (sel >= 0 && sel < indexLsize) {
			selectindex = sel;
		}
	}
	if (indexLsize > 0 ) {
		VOX.render_VOX({0,0, 620, 670}, 150, 150, 150, 115);
		if (indexLsize > 30) {
			TextBuffer.loadFromRenderedText(Comic_16, std::to_string(selectindex+1)+"/"+std::to_string(indexLsize), {0,0,0});
			TextBuffer.render(400, 690);
		}
		int of = selectindex < 30 ? 0 : selectindex - 26;
		for (int x = of; x < indexLsize; x++) {
			std::string temptext = Jlinks["link"][x];
			NameOfLink(temptext);

			if (x == selectindex) {
				T_T.loadFromRenderedText(digi_16, temptext.substr(0,58), { 255,255,255 });
				VOX.render_VOX({20-2,10 + ((x-of) * 22), 590, T_T.getHeight()}, 0, 0, 0, 105);
				T_T.render(20, 10 + ((x-of) * 22));
				Heart.render(20 - 18, 10 + 3 + ((x-of) * 22));
			}
			else if((x-of) < 30)
			{
				TextBuffer.loadFromRenderedText(digi_16, temptext.substr(0,58), { 50, 50, 50 });
				TextBuffer.render(20, 10 + ((x-of) * 22));
			}
		}
	}
}
void SDLB::HandleList(int& selectchapter, int allsize, int key,bool ongrid){
	if (allsize <=0) return;
	allsize--;
//	std::cout << selectchapter << " - " << allsize << std::endl;
	switch(key) {
	case BT_LEFT:
	case BT_LS_LEFT:
		if(ongrid) {
			if (selectchapter > 0)
			{
				selectchapter--;
			} else {
				selectchapter = allsize;
			}
		}
		break;
	case BT_RIGHT:
	case BT_LS_RIGHT:
		if(ongrid) {
			if (selectchapter < allsize)
			{
				selectchapter++;
			} else {
				selectchapter = 0;
			}
		}
		break;
	case BT_UP:
	case BT_LS_UP: {
		if(ongrid) {
			if (selectchapter > 0)
			{
				selectchapter-=10;
			} else {
				selectchapter = allsize;
			}
		} else {
			if (selectchapter > 0)
			{
				selectchapter--;
			} else {
				selectchapter = allsize;
			}
		}
		break;
	}
	case BT_DOWN:
	case BT_LS_DOWN: {
		if(ongrid) {
			if (selectchapter < allsize)
			{
				selectchapter+=10;
			} else {
				selectchapter = 0;
			}
		} else {
			if (selectchapter < allsize)
			{
				selectchapter++;
			} else {
				selectchapter = 0;
			}
		}
		break;
	}
	}
//	std::cout << selectchapter << " - " << allsize << std::endl;
}
void SDLB::ScrollBarDraw(int X, int Y,int H,int W, int Total, int Select,bool ongrid){
	if(ongrid) {
		int part=Select;
		int all=Total;
		part+=9;
		part=part/10;
		part--;
		all+=9;
		all=all/10;
		Select=part;
		Total=all;
		//printf("part: %d - total: %d \r",part,all);fflush(stdout);
	}

	//split the distance in peaces and get exact values
	double Texact = Total;
	double Bar = H/Texact;

	//Get approximate values, 1 pixel of error
	int BarSize = Bar + 1;       //approximate Size
	int BarPos  = Bar*Select; //approximate Position

	//Draw the ScrollBar
	VOX.render_VOX({X-1,64, W+2, 604}, 50, 50, 50, 255);//draw area
//	VOX.render_VOX({X+1,Y+BarPos+1, W,BarSize} ,50, 50, 50, 240);
	VOX.render_VOX({X,Y+BarPos, W, BarSize},255, 255, 255, 240);
}
void SDLB::deint(){
	//Clear Texture Map
	MapT.clear();
	//Free global font
	TTF_CloseFont(Comic_16);
	Comic_16 = NULL;
	TTF_CloseFont(digi_16);
	digi_16 = NULL;
	digi_9 = NULL;

	TTF_CloseFont(Arista_100);
	Arista_100 = NULL;
	TTF_CloseFont(Arista_150);
	Arista_150 = NULL;
	TTF_CloseFont(Arista_40);
	Arista_40 = NULL;
	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	//Free the music
	Mix_FreeMusic(gMusic);
	gMusic = NULL;
	aree = NULL;
	proc = NULL;
	Mix_Quit();
	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

}

//Textures manager
LTexture::LTexture(){
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
	mX = 0;
	mY = 0;
	mark=true;
}
LTexture::~LTexture(){
	//Deallocate
	free();
}
void LTexture::TickerColor(int& color,int min,int max,unsigned long long sec){
	if (onTimeC(sec,time2) || sec==0) {
		int fcolor=color;
		if (reverse) {
			fcolor-=1;
			if(fcolor < min) {
				fcolor=min;
				reverse=false;
			}
		} else {
			fcolor+=5;
			if(fcolor > max) {
				fcolor=max;
				reverse=true;
			}
		}
		color=fcolor;
	}
}
void LTexture::TickerRotate(int& angle,int min,int max, int addangle,bool clock){
	int fangle=angle;
	if(clock) {
		fangle+=addangle;
		if(fangle > max) {
			fangle=fangle-max;
		}
	} else {
		fangle-=addangle;
		if(fangle < min) {
			fangle=max;
		}
	}

	angle=fangle;
}
void LTexture::TickerBomb(int sizescale){
	reverse=false;
	offboom_min=sizescale;
	offboom=sizescale+1;
}
void LTexture::TickerScale(){
	if (offboom > offboom_min) {
		TickerColor(offboom,offboom_min,offboom_size);
		offtik=offboom;
	}
	if (offboom < offboom_min) {
		offtik=offboom_min;
		offboom=offboom_min;
	}
}
bool LTexture::loadFromFile(std::string path){
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError()); remove(path.c_str());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(GOD.gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}
bool LTexture::loadFromFileCustom(std::string path, int h, int w){
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
		if(std::string(IMG_GetError()) == "Unsupported image format") remove(path.c_str());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(GOD.gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = w;
			mHeight = h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}
bool LTexture::loadFromRenderedText(TTF_Font *fuente, std::string textureText, SDL_Color textColor){
	//Get rid of preexisting texture
	free();
	if(textureText.length() == 0) textureText=" ";
	//Render text surface
	SDL_Surface* textSurface = TTF_RenderUTF8_Blended(fuente, textureText.c_str(), textColor);

	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(GOD.gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}

	//Return success
	return mTexture != NULL;
}
bool LTexture::loadFromRenderedTextWrap(TTF_Font *fuente, std::string textureText, SDL_Color textColor, Uint32 size){
	//Get rid of preexisting texture
	free();
	if(textureText.length() == 0) textureText=" ";
	//Render text surface
	SDL_Surface* textSurface = TTF_RenderUTF8_Blended_Wrapped(fuente, textureText.c_str(), textColor, size);

	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(GOD.gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}

	//Return success
	return mTexture != NULL;
}
void LTexture::free(){
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
		mX = 0;
		mY = 0;
		offW = 0;
		offH = 0;
	}
}
void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue){
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}
void LTexture::setBlendMode(SDL_BlendMode blending){
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}
void LTexture::setAlpha(Uint8 alpha){
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}
void LTexture::ScaleA(int H, int W){//Absolute
	offW = W;
	offH = H;
}
void LTexture::ScaleR(int H, int W){//relative
	offW = mWidth+W;
	offH = mHeight+H;
}
void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip){
	//texture boom
	TickerScale();
	int offtikx2=offtik*2;

	//Set rendering Overwrite
	int TextureW=offW > 0 ? offW : mWidth;
	int TextureH=offH > 0 ? offH : mHeight;

	SDL_Rect renderQuad = { x-offtik, y-offtik, TextureW+offtikx2, TextureH+offtikx2};
	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	if(SP()) {setColor(77,166,255);}
	else setColor( 255, 255, 255);

	//Render to screen
	SDL_RenderCopyEx(GOD.gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
	//tactil stuff
	mX = x; mY = y;  SelIns = GOD.FrameState; Selstate = GOD.GenState;
}
int LTexture::render_T(int x, int y, std::string text, bool presed){
	int TMPW = 0;
	if(text.length() == 0) text=" ";
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	if (presed ) {setColor( 150, 150, 150);}
	else {setColor( 255, 255, 255);}

	if(SP()) {setColor( 77,166,255);}

	//Render to screen
	SDL_RenderCopy(GOD.gRenderer, mTexture, NULL, &renderQuad);
	if (text.length()) {
		SDL_Surface* textSurface = TTF_RenderUTF8_Blended(GOD.NSM, text.c_str(), { 50, 50, 50 });
		if (textSurface == NULL)
		{
			printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		}
		else
		{
			TMPW=textSurface->w;
			SDL_Rect TextRect = {x+mWidth+3, y+(mHeight/2)-(textSurface->h/2), textSurface->w, textSurface->h};
			//Create texture from surface pixels
			SDL_Texture* TextureT = SDL_CreateTextureFromSurface(GOD.gRenderer, textSurface);
			//Render to screen
			SDL_RenderCopy(GOD.gRenderer, TextureT, NULL, &TextRect);
			SDL_DestroyTexture(TextureT);
		}
		SDL_FreeSurface(textSurface);
	}
	//tactil stuff
	mX = x; mY = y;  SelIns = GOD.FrameState; Selstate = GOD.GenState;
	return mWidth + TMPW;
}
bool LTexture::render_AH(int x, int y, int w, int h, bool type){
	//tactil stuff
	mX = x; mY = y;
	SelIns = GOD.FrameState;
	Selstate = GOD.GenState;
	int sizeH = 0;
	int HP = h < 0 ? h * -1 : h;
	if(type) {
		if (delayp < HP &&!anend)
		{
			delayp+=30;
			sizeH = h < 0 ? delayp* -1 : delayp;
		} else {
			anend = true;
			sizeH = h;
			//delayp = h;
		}
		{
			//SIDE
			SDL_SetRenderDrawColor(GOD.gRenderer, 0, 0, 0, 220);
			SDL_Rect HeaderRect = {x+w,y, 2, sizeH*-1};
			SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);

			SDL_SetRenderDrawColor(GOD.gRenderer, 0, 0, 0, 220);
			HeaderRect = {x-2,y, 2, sizeH*-1};
			SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);


			//TOP
			SDL_SetRenderDrawColor(GOD.gRenderer, 0, 0, 0, 220);
			HeaderRect = {x-2,y-2+sizeH* -1, w+2, 2};
			SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);
		}


		SDL_SetRenderDrawColor(GOD.gRenderer, 50, 50, 50, 200);
		SDL_Rect HeaderRect = {x,y+sizeH* -1, w, sizeH};
		SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);
		return anend;
	} else{
		if (anend) {
			if (delayp <= 0)
			{
				delayp=0;
				anend = false;
			} else {
				delayp-=30;
				sizeH = h < 0 ? delayp* -1 : delayp;

				{   //SIDE
					SDL_SetRenderDrawColor(GOD.gRenderer, 0, 0, 0, 220);
					SDL_Rect HeaderRect = {x+w,y, 3, sizeH*-1};
					SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);

					SDL_SetRenderDrawColor(GOD.gRenderer, 0, 0, 0, 220);
					HeaderRect = {x-2,y, 2, sizeH*-1};
					SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);

					//TOP
					SDL_SetRenderDrawColor(GOD.gRenderer, 0, 0, 0, 220);
					HeaderRect = {x-2,y-2+sizeH* -1, w+2, 2};
					SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);
				}

				SDL_SetRenderDrawColor(GOD.gRenderer, 50, 50, 50, 200);
				SDL_Rect HeaderRect = {x,y+sizeH* -1, w, sizeH};
				SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);
			}
		}
	}

	return false;
}
void LTexture::render_VOX(SDL_Rect Form,int R, int G, int B, int A){
	//Set rendering space and render to screen
	SDL_SetRenderDrawColor(GOD.gRenderer, R, G, B, A);
	SDL_RenderFillRect(GOD.gRenderer, &Form);
	//tactil stuff
	mX = Form.x; mY = Form.y; mWidth = Form.w; mHeight = Form.h;
	SelIns = GOD.FrameState;
	Selstate = GOD.GenState;
}
bool LTexture::SP(){
	//return on negative touch
	if (GOD.TouchX < 0||GOD.TouchY < 0||getWidth() < 0||getHeight() < 0) return false;

	//printf("State %d : %d \n",Selstate,GOD.GenState);
//	if (SelIns == GOD.FrameState-1) return false;
	if (Selstate != GOD.GenState) return false;
	if (!mark) return false;

	//check if touched
	if(GOD.TouchX > mX-3 && GOD.TouchX < mX + getWidth() +3 && GOD.TouchY > mY-3 && GOD.TouchY < mY + getHeight() +3) {
		printf("Frame %d : %d \n",SelIns,GOD.FrameState);
		//printf("TouchX:%d  TouchY:%d\nB_X:%d  B_Y:%d\nB_W:%d  B_H:%d  \n",GOD.TouchX,GOD.TouchY,mX,mY,mWidth,mHeight);
		return true;
	}
	return false;
}
//is press relesed
bool LTexture::SPr(){
	//return on negative touch
	if (SelIns != GOD.FrameState) return false;
	if (Selstate != GOD.GenState) return false;
	if (!mark) return false;
	//check if touched
	if(GOD.TouchXU > mX-3 && GOD.TouchXU < mX + getWidth() +3 && GOD.TouchYU > mY-3 && GOD.TouchYU < mY + getHeight() +3) {
		printf("FrameUP %d : %d \n",SelIns,GOD.FrameState);
		GOD.TouchXU=-1;
		GOD.TouchYU=-1;
		//printf("TouchX:%d  TouchY:%d\nB_X:%d  B_Y:%d\nB_W:%d  B_H:%d  \n",GOD.TouchX,GOD.TouchY,mX,mY,mWidth,mHeight);
		return true;
	}
	return false;
}
int LTexture::getWidth(){
	return offW > 0 ? offW : mWidth;
}
int LTexture::getHeight(){
	return offH > 0 ? offH : mHeight;
}
int LTexture::getWidthR(){
	return mWidth;
}
int LTexture::getHeightR(){
	return mHeight;
}
int LTexture::getX(){
	return mX;
}
int LTexture::getY(){
	return mY;
}
bool LTexture::isZero(){
	return !(mHeight > 1 && mWidth > 1 && mTexture != NULL);
}



