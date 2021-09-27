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
#include "utils.hpp"
#include "SDLWork.hpp"

extern SDLB GOD;
extern LTexture gTextTexture;
extern LTexture Farest;
extern LTexture VOX;
extern LTexture T_T;
extern LTexture Heart;
extern int Frames;

extern std::string rootdirectory;

//Grafics and logic
void SDLB::intA(){
	json base;
	read_DB(base,"romfs:/NOR");
	AppVer=base["V"];
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
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

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

				//Load music
				if (isFileExist(rootdirectory+"wada.ogg")) {
					gMusic = Mix_LoadMUS((rootdirectory+"wada.ogg").c_str());
				} else {
					gMusic = Mix_LoadMUS("romfs:/audio/wada.ogg");
				}

				if (gMusic == NULL)
				{
					printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());

				}
				if (Mix_PlayingMusic() == 0)
				{
					//Play the music
					if(isFileExist(rootdirectory+"play"))
						Mix_PlayMusic(gMusic, -1);
				}
			}
		}
	}

	B_O_F = TTF_OpenFont("romfs:/fonts/AF.ttf", 19);
	digifont2 = TTF_OpenFont("romfs:/fonts/AF.ttf", 35);
	Arista = TTF_OpenFont("romfs:/fonts/Arista.ttf", 27);

	//B_O_F = TTF_OpenFont("romfs:/fonts/Comic.ttf", 19);
	gFont = TTF_OpenFont("romfs:/fonts/Comic.ttf", 16);
	gFont2 = TTF_OpenFont("romfs:/fonts/Arista.ttf", 150);
	gFont3 = TTF_OpenFont("romfs:/fonts/Arista.ttf", 40);
	gFont4 = TTF_OpenFont("romfs:/fonts/Arista.ttf", 30);
	gFont5 = TTF_OpenFont("romfs:/fonts/Arista.ttf", 20);
	gFont6 = TTF_OpenFont("romfs:/fonts/Arista.ttf", 50);
	gFontcapit = TTF_OpenFont("romfs:/fonts/Arista.ttf", 100);
	digifont = TTF_OpenFont("romfs:/fonts/digifont.otf", 16);
	digifontC = TTF_OpenFont("romfs:/fonts/digifont.otf", 9);

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
	std::string KeyImage=path.substr(25)+"-"+std::to_string(W)+"x"+std::to_string(H);
	if (!isFileExist(path)) {
		KeyImage="nop.png";
		KeyImage+="-"+std::to_string(W)+"x"+std::to_string(H);
		path = "romfs:/img/nop.png";
	}

	if ( MapT.find(KeyImage) == MapT.end()) {
		MapT[KeyImage].loadFromFileCustom(path.c_str(), H, W);
	} else if (MapT[KeyImage].getHeight() < H) {
		MapT[KeyImage].loadFromFileCustom(path.c_str(), H, W);
	}

	MapT[KeyImage].render(X, Y);
	if(key >= 0) {
		if(MapT[KeyImage].SP()) {WorKey=KeyImage; MasKey=key;}
	}
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

	gTextTexture.loadFromRenderedText(gFont3, text.c_str(), { 50, 50, 50 });
	gTextTexture.render(1280/2 - gTextTexture.getWidth()/2, 720/2 - gTextTexture.getHeight() / 2);
	if (render) SDL_RenderPresent(gRenderer);
}
void SDLB::Cover(std::string path,int X, int Y,std::string Text,int WS,int key,bool selected){
//render images and map to memory for fast display
	std::string KeyImage=path.substr(25)+"-"+std::to_string(WS);
	if (!isFileExist(path)) {
		KeyImage="nop.png";
		KeyImage+="-"+std::to_string(WS);
		path = "romfs:/img/nop.png";
	}

	int sizeportraitx = 300;
	int sizeportraity =424;

	int HS = (sizeportraity * (WS * 10000 /sizeportraitx) /10000);
	if ( MapT.find(KeyImage) == MapT.end()) {
		MapT[KeyImage].loadFromFileCustom(path.c_str(), HS, WS);
	} else if (MapT[KeyImage].getHeight() < 10) {
		MapT[KeyImage].loadFromFileCustom(path.c_str(), HS, WS);
	}

	//make the math
	static int blue=255;
	if(selected) {
		MapT[KeyImage].TickerColor(blue,150,250);
		WS=120;
		HS = (sizeportraity * (WS * 10000 /sizeportraitx) /10000);
		MapT[KeyImage].offW=WS;
		MapT[KeyImage].offH=HS;
	} else blue=0;

	std::string KeyText=Text+"-"+std::to_string(WS);
	std::string KeyTextH=Text+"-"+std::to_string(WS)+"-Head";

	if (Text.length()) {
		if ( MapT.find(KeyText) == MapT.end() ) {
			std::string numhe=scrapElement(Text, "#");
			if (numhe.length() > 0) {
				replace(Text, numhe, "");
				replace(numhe, " #", ""); replace(numhe, "#", "");
				TTF_Font* customFont = TTF_OpenFont("romfs:/fonts/digifont.otf", 11);
				MapT[KeyTextH].loadFromRenderedText(customFont, numhe, { 255,255,255 });
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
	if(MapT[KeyImage].SP()) {WorKey=KeyImage; MasKey=key;}
	if(selected) {
		MapT[KeyImage].offW=0;
		MapT[KeyImage].offH=0;
	}
}
void SDLB::Cover_idx(std::string path,int X, int Y,std::string Text,int WS,int index,int& select,bool render){
//render images and map to memory for fast display
	std::string KeyImage=path.substr(25)+"-"+std::to_string(WS);
	std::string KeyText=Text+"-"+std::to_string(WS);
	std::string KeyTextH=Text+"-"+std::to_string(WS)+"-Head";

	if (!isFileExist(path)) {
		KeyImage="SuperPro.jpg";
		KeyImage+="-"+std::to_string(WS);
		path = "romfs:/img/SuperPro.jpg";
	}
	int sizeportraitx = 300;
	int sizeportraity =424;
	//make the math

	int HS = (sizeportraity * (WS * 10000 /sizeportraitx) /10000);
	if ( MapT.find(KeyImage) == MapT.end()) {
		MapT[KeyImage].loadFromFileCustom(path.c_str(), HS, WS);
	} else if (MapT[KeyImage].getHeight() < HS) {
		MapT[KeyImage].loadFromFileCustom(path.c_str(), HS, WS);
	}
	if (Text.length()) {
		if ( MapT.find(KeyText) == MapT.end() ) {
			std::string numhe=scrapElement(Text, "#");
			if (numhe.length() > 0) {
				replace(Text, numhe, "");
				replace(numhe, " #", ""); replace(numhe, "#", "");
				TTF_Font* customFont = TTF_OpenFont("romfs:/fonts/digifont.otf", 11);
				MapT[KeyTextH].loadFromRenderedText(customFont, numhe, { 255,255,255 });
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

	//How not to handle a touch input, wala, yolo
	static int findex = -1;
	if (findex >= 0 && !fingerdown) {
		//printf("relea %d to %d %s\n",select,index,KeyImage.c_str());
		select=findex;
		findex=-1;
	} else {
		if(MapT[KeyImage].SP()) {
			//printf("touch %d to %d %s\n",select,index,KeyImage.c_str());
			findex = index;
		}
		if(fingermotion&&fingerdown) {
			//printf("reset %d to %d %s\n",select,index,KeyImage.c_str());
			findex=-1;
		}
	}
}
void SDLB::ListCover(int& selectindex,json Jlinks, bool ongrid,int limit){
	std::vector<std::string> vec = Jlinks["link"];
	int JlinksSize=vec.size();
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
				selectindex = chapsize+selectindex;
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
				std::string KeyImage=imagelocal.substr(25)+"-"+std::to_string(nosel);
				if ( MapT.find(KeyImage) == MapT.end()) {
					if (limit < x) break;
				}
			}

			if (x == selectindex) {
				//draw Title
				gTextTexture.loadFromRenderedText(gFont4, TEXTH, { 0, 0, 0 });
				gTextTexture.render(50, 9);
				if(!Jlinks["date"].empty()) {
					//draw Title
					int datesize = Jlinks["date"].size()-1;
					if (x+outof < datesize) {
						gTextTexture.loadFromRenderedText(GOD.digifont, Jlinks["date"][x+outof], { 0, 0, 0 });
						gTextTexture.render(50, 37);
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
			if (x == selectindex) {
				Cover(imagelocal,10 +  (MainOffSet * 127)-4,HO-13,TEXT,issel,BT_A,true);
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
			gTextTexture.loadFromRenderedText(gFont, std::to_string(selectindex+1)+"/"+std::to_string(indexLsize), {0,0,0});
			gTextTexture.render(400, 690);
		}
		int of = selectindex < 30 ? 0 : selectindex - 26;
		for (int x = of; x < indexLsize; x++) {
			std::string temptext = Jlinks["link"][x];
			NameOfLink(temptext);

			if (x == selectindex) {
				T_T.loadFromRenderedText(digifont, temptext.substr(0,58), { 255,255,255 });
				VOX.render_VOX({20-2,10 + ((x-of) * 22), 590, T_T.getHeight()}, 0, 0, 0, 105);
				T_T.render(20, 10 + ((x-of) * 22));
				Heart.render(20 - 18, 10 + 3 + ((x-of) * 22));
			}
			else if((x-of) < 30)
			{
				gTextTexture.loadFromRenderedText(digifont, temptext.substr(0,58), { 50, 50, 50 });
				gTextTexture.render(20, 10 + ((x-of) * 22));
			}
		}
	}
}
void SDLB::HandleList(int& selectchapter, int allsize, int key,bool ongrid){
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
	TTF_CloseFont(gFont);
	gFont = NULL;
	TTF_CloseFont(digifont);
	digifont = NULL;
	digifontC = NULL;

	TTF_CloseFont(gFontcapit);
	gFontcapit = NULL;
	TTF_CloseFont(gFont2);
	gFont2 = NULL;
	TTF_CloseFont(gFont3);
	gFont3 = NULL;
	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	//Free the music
	Mix_FreeMusic(gMusic);
	gMusic = NULL;
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
	SelIns = -1;
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

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Blended(fuente, textureText.c_str(), textColor);

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
		SelIns = -1;
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
void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip){
	//texture boom
	TickerScale();
	int offtikx2=offtik*2;

	//Set rendering space and render to screen

	int TextureW=offW > 0 ? offW : mWidth;
	int TextureH=offH > 0 ? offH : mHeight;

	SDL_Rect renderQuad = { x-offtik, y-offtik, TextureW+offtikx2, TextureH+offtikx2};
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
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
	mX = x; mY = y;  SelIns = GOD.GenState;
}
int LTexture::render_T(int x, int y, std::string text, bool presed){
	int TMPW = 0;
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	if (presed ) {setColor( 150, 150, 150);}
	else {setColor( 255, 255, 255);}

	if(SP()) {setColor( 77,166,255);}

	//Render to screen
	SDL_RenderCopy(GOD.gRenderer, mTexture, NULL, &renderQuad);
	if (text.length()) {
		SDL_Surface* textSurface = TTF_RenderText_Blended(GOD.B_O_F, text.c_str(), { 50, 50, 50 });
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
	mX = x; mY = y;  SelIns = GOD.GenState;
	return mWidth + TMPW;
}
bool LTexture::render_AH(int x, int y, int w, int h, bool type){
	//tactil stuff
	mX = x; mY = y;  SelIns = GOD.GenState;
	static bool anend = false;
	static int delayp = 0;
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
		{        //SIDE
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

				{        //SIDE
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
	mX = Form.x; mY = Form.y;       SelIns = GOD.GenState;
}
bool LTexture::SP(){
	//return on negative touch
	if (GOD.TouchX <= 0||GOD.TouchY <= 0||mWidth <= 0||mHeight <= 0) return false;
	if (SelIns != GOD.GenState) return false;
	if (!mark) return false;

	//check if touched
	if(GOD.TouchX > mX-3 && GOD.TouchX < mX + mWidth +3 && GOD.TouchY > mY-3 && GOD.TouchY < mY + mHeight +3) {
		//printf("TouchX:%d  TouchY:%d\nB_X:%d  B_Y:%d\nB_W:%d  B_H:%d  \n",GOD.TouchX,GOD.TouchY,mX,mY,mWidth,mHeight);
		return true;
	}
	return false;
}
//is press relesed
bool LTexture::SPr(){
	static bool isRe = false;
	if (SP()) {
		if (!isRe) isRe = true;
	} else if (isRe) {
		isRe=false;
		if (!GOD.fingerdown) return true;
	}
	return false;
}
int LTexture::getWidth(){
	return offW > 0 ? offW : mWidth;
}
int LTexture::getHeight(){
	return offH > 0 ? offH : mHeight;
}
int LTexture::getX(){
	return mX;
}
int LTexture::getY(){
	return mY;
}



