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
#include "SDLWork.hpp"
#include "utils.hpp"

extern SDLB GOD;
extern LTexture gTextTexture;
extern LTexture Farest;
extern std::string rootdirectory;

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
				if (isFileExist(rootdirectory+"wada.ogg")){
					gMusic = Mix_LoadMUS((rootdirectory+"wada.ogg").c_str());
				} else {
					gMusic = Mix_LoadMUS("romfs:/wada.ogg");
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

	B_O_F = TTF_OpenFont("romfs:/AF.ttf", 19);
	gFont = TTF_OpenFont("romfs:/lazy.ttf", 16);
	gFont2 = TTF_OpenFont("romfs:/lazy2.ttf", 150);
	gFont3 = TTF_OpenFont("romfs:/lazy2.ttf", 40);
	gFont4 = TTF_OpenFont("romfs:/lazy2.ttf", 30);
	gFont5 = TTF_OpenFont("romfs:/lazy2.ttf", 20);
	gFontcapit = TTF_OpenFont("romfs:/lazy2.ttf", 100);
	digifont = TTF_OpenFont("romfs:/digifont.otf", 16);
	digifontC = TTF_OpenFont("romfs:/digifont.otf", 11);

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
		path = "romfs:/nop.png";
	}
	if ( MapT.find(KeyImage) == MapT.end() ) {
		MapT[KeyImage].loadFromFileCustom(path.c_str(), H, W);
	}
	MapT[KeyImage].render(X, Y);
	if(MapT[KeyImage].SP()){WorKey=KeyImage;MasKey=key;}
}
void SDLB::PleaseWait(std::string text,bool render){
	if (render){
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
	if (render)	SDL_RenderPresent(gRenderer);
}
void SDLB::Cover(std::string path,int X, int Y,std::string Text,int WS,int key,bool selected){
//render images and map to memory for fast display
		std::string KeyImage=path.substr(25)+"-"+std::to_string(WS);
		std::string KeyText=Text+"-"+std::to_string(WS);

		if (!isFileExist(path)) {
			KeyImage="nop.png";
			KeyImage+="-"+std::to_string(WS);
			path = "romfs:/nop.png";
		}
		int sizeportraitx = 300;
		int sizeportraity =424;
		//make the math

		int HS = (sizeportraity * (WS * 10000 /sizeportraitx) /10000);

		if ( MapT.find(KeyImage) == MapT.end() ) {
			MapT[KeyImage].loadFromFileCustom(path.c_str(), HS, WS);
		}
		
		static int blue=0;
		if(selected){
			TikerColor(blue,1);
		} else blue=0;

		
		if (Text.length()){
			//text
			if ( MapT.find(KeyText) == MapT.end() ) {
				int kinsize =11;
				if (WS < 115){kinsize =7;Text=Text.substr(0,20);}//
				TTF_Font* customFont = TTF_OpenFont("romfs:/digifont.otf", kinsize);
				MapT[KeyText].loadFromRenderedTextWrap(customFont, Text, { 255,255,255 }, WS);
			}
			MapT[KeyImage].render_VOX({ X - 3, Y - 3 , WS + 6, HS + 6 + MapT[KeyText].getHeight()+10}, 0, 0, blue, 200);
			MapT[KeyText].render(X + 4, Y + 4+MapT[KeyImage].getHeight());
		} else {
			MapT[KeyImage].render_VOX({ X - 3, Y - 3 , X + 6, Y + 6}, 0, 0, blue, 200);
		}

		MapT[KeyImage].render(X, Y);
		if(MapT[KeyImage].SP()){WorKey=KeyImage;MasKey=key;}
}
void SDLB::Cover_idx(std::string path,int X, int Y,std::string Text,int WS,int index,int& select){
//render images and map to memory for fast display
	std::string KeyImage=path.substr(25)+"-"+std::to_string(WS);
	std::string KeyText=Text+"-"+std::to_string(WS);

	if (!isFileExist(path)) {
		KeyImage="nop.png";
		KeyImage+="-"+std::to_string(WS);
		path = "romfs:/nop.png";
	}
	int sizeportraitx = 300;
	int sizeportraity =424;
	//make the math

	int HS = (sizeportraity * (WS * 10000 /sizeportraitx) /10000);
	if ( MapT.find(KeyImage) == MapT.end() ) {
		MapT[KeyImage].loadFromFileCustom(path.c_str(), HS, WS);
	}
	if (Text.length()){
		//text
		if ( MapT.find(KeyText) == MapT.end() ) {
			int kinsize =11;
			if (WS < 115){kinsize =9;Text=Text.substr(0,19);}//
			TTF_Font* customFont = TTF_OpenFont("romfs:/digifont.otf", kinsize);
			MapT[KeyText].loadFromRenderedTextWrap(customFont, Text, { 255,255,255 }, WS);
		}
		MapT[KeyImage].render_VOX({ X - 3, Y - 3 , WS + 6, HS + 6 + MapT[KeyText].getHeight()+10}, 0, 0, 0, 200);
		MapT[KeyText].render(X + 4, Y + 4+MapT[KeyImage].getHeight());
	} else {
		MapT[KeyImage].render_VOX({ X - 3, Y - 3 , X + 6, Y + 6}, 0, 0, 0, 200);
	}

	MapT[KeyImage].render(X, Y);
		
	//How not to handle a touch input, wala, yolo
	static int findex = -1;
	if (findex >= 0 && !GOD.fingerdown){
		printf("relea %d to %d %s\n",select,index,KeyImage.c_str());
		select=findex;
		findex=-1;
	} else {
		if(MapT[KeyImage].SP()){
			printf("touch %d to %d %s\n",select,index,KeyImage.c_str());
			findex = index;
		} 
		if(fingermotion) {
			printf("reset %d to %d %s\n",select,index,KeyImage.c_str());
			findex=-1;
		}
	}
}
void SDLB::ListCover(int x,int& selectindex,std::string Link, bool ongrid){
	//This controll the image order and logic

	//Get the Cap Key
	replace(Link, "https://jkanime.net/", "");
	int val0 = Link.find("/");
	std::string imagelocal=Link.substr(0,val0);
	replace(Link, imagelocal, "");
	replace(Link, "/", "");

	//Cap Key to name 
	std::string TEXT=imagelocal;
	std::string TEXTH=imagelocal;
	replace(TEXT, "/", " ");
	replace(TEXT, "-", " ");
	if (Link.length() > 0) {
		TEXT=TEXT.substr(0,20)+" #"+Link;
		TEXTH+=" #"+Link;
	}
	mayus(TEXT);
	mayus(TEXTH);

	//Image of cap
	imagelocal = rootdirectory+"DATA/"+imagelocal+".jpg";
	
	//set the offset position of images
	static int MainOffSet=1,offset1=1,offset2=1,offset3=1;
	
	if (ongrid)
	{
		if (x == selectindex) {
			//draw Title
			gTextTexture.loadFromRenderedText(gFont4, TEXTH, { 0, 0, 0 });
			gTextTexture.render(20, 10);
		}

		if (x==0){MainOffSet=0;offset1=0; offset2=0;offset3=0;}
		int HO = 0;
		//Get Images , render Small
		if (x < 10){
			MainOffSet=offset1;
			HO = 70;
			offset1++;
		} else if (x < 20){
			HO = 270;
			MainOffSet=offset2;
			offset2++;
		} else if (x < 30){
			MainOffSet=offset3;
			HO = 470;
			offset3++;
		}
		if (x == selectindex) {
			Cover(imagelocal,10 +  (MainOffSet * 127)-5,HO-13,TEXT,120,BT_A,true);
		} else {
			Cover_idx(imagelocal,10 +  (MainOffSet * 127),HO,TEXT,113,x,selectindex);
		}
	} else {
		if (x==0){offset1 =1; offset2 =1;}
		//Get 4 Images before, render Small
		if ((x > selectindex-5) && (x < selectindex)){
			int comp=0;//this is to get closer to te main image
			if (selectindex < 5){
				comp = offset1+(4-selectindex);
			} else {
				comp = offset1;
			}
			Cover(imagelocal,600+  (comp * 30),  (comp * 22),TEXT,113,BT_UP);
			offset1++;
		}
		//Central Big image
		if (x == selectindex) {
			Cover(imagelocal,680+ 132,  132,TEXT,255,BT_A);
		}
		//Get 4 Images After, render small
		if ((x < selectindex+5) && (x > selectindex)){
			Cover(imagelocal,1030+ (offset2 * 30), 400 + (offset2 * 22),TEXT,113,BT_DOWN);
			offset2++;
		}
	}
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

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
	mX = 0;
	mY = 0;
	SelIns = -1;
	mark=true;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());remove(path.c_str());
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
bool LTexture::loadFromFileCustom(std::string path, int h, int w)
{
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
bool LTexture::loadFromRenderedText(TTF_Font *fuente, std::string textureText, SDL_Color textColor)
{
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
bool LTexture::loadFromRenderedTextWrap(TTF_Font *fuente, std::string textureText, SDL_Color textColor, Uint32 size)
{
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
void LTexture::free()
{
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
void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	if(SP()){setColor(77,166,255);}
	else setColor( 255, 255, 255);

	//Render to screen
	SDL_RenderCopyEx(GOD.gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
	//tactil stuff
	mX = x;mY = y;	SelIns = GOD.GenState;
}

void LTexture::render_T(int x, int y, std::string text, bool presed)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	
	if (presed ){setColor( 150, 150, 150);}
	else {setColor( 255, 255, 255);}
	
	if(SP()){setColor( 77,166,255);}
	
	//Render to screen
	SDL_RenderCopy(GOD.gRenderer, mTexture, NULL, &renderQuad);
	
	if (text.length()){
		SDL_Surface* textSurface = TTF_RenderText_Blended(GOD.B_O_F, text.c_str(), { 50, 50, 50 });
		if (textSurface == NULL)
		{
			printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		}
		else
		{
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
	mX = x;mY = y;	SelIns = GOD.GenState;
}

bool LTexture::render_AH(int x, int y, int w, int h, bool type)
{
		//tactil stuff
		mX = x;mY = y;	SelIns = GOD.GenState;
		static bool anend = false;
		static int delayp = 0;
		int sizeH = 0;
		int HP = h < 0 ? h *- 1 : h;
		if(type){
			if (delayp < HP &&!anend)
			{
				delayp+=30;
				sizeH = h < 0 ? delayp*-1 : delayp;
			} else {
				anend = true;
				sizeH = h;
				//delayp = h; 
			}
			{//SIDE
			SDL_SetRenderDrawColor(GOD.gRenderer, 0, 0, 0, 220);
			SDL_Rect HeaderRect = {x+w,y, 2, sizeH*-1};
			SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);
			
			SDL_SetRenderDrawColor(GOD.gRenderer, 0, 0, 0, 220);
			HeaderRect = {x-2,y, 2, sizeH*-1};
			SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);
			

			//TOP
			SDL_SetRenderDrawColor(GOD.gRenderer, 0, 0, 0, 220);
			HeaderRect = {x-2,y-2+sizeH*-1, w+2, 2};
			SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);
			}
			
			
			SDL_SetRenderDrawColor(GOD.gRenderer, 50, 50, 50, 200);
			SDL_Rect HeaderRect = {x,y+sizeH*-1, w, sizeH};
			SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);
			return anend;
		} else{
			if (anend){
				if (delayp <= 0)
				{
					delayp=0;
					anend = false;
				} else {
					delayp-=30;
					sizeH = h < 0 ? delayp*-1 : delayp;					

					{//SIDE
					SDL_SetRenderDrawColor(GOD.gRenderer, 0, 0, 0, 220);
					SDL_Rect HeaderRect = {x+w,y, 3, sizeH*-1};
					SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);
					
					SDL_SetRenderDrawColor(GOD.gRenderer, 0, 0, 0, 220);
					HeaderRect = {x-2,y, 2, sizeH*-1};
					SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);

					//TOP
					SDL_SetRenderDrawColor(GOD.gRenderer, 0, 0, 0, 220);
					HeaderRect = {x-2,y-2+sizeH*-1, w+2, 2};
					SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);
					}

					SDL_SetRenderDrawColor(GOD.gRenderer, 50, 50, 50, 200);
					SDL_Rect HeaderRect = {x,y+sizeH*-1, w, sizeH};
					SDL_RenderFillRect(GOD.gRenderer, &HeaderRect);
				}
			}
		}

return false;
}

void LTexture::render_VOX(SDL_Rect Form ,int R, int G, int B, int A)
{
	//Set rendering space and render to screen
	SDL_SetRenderDrawColor(GOD.gRenderer, R, G, B, A);
	SDL_RenderFillRect(GOD.gRenderer, &Form);
	//tactil stuff
	mX = Form.x; mY = Form.y;	SelIns = GOD.GenState;
}

bool LTexture::SP()
{
	//return on negative touch
	if (GOD.TouchX <= 0||GOD.TouchY <= 0||mWidth <= 0||mHeight <= 0) return false;
	if (SelIns != GOD.GenState) return false;
	if (!mark) return false;
	
	//check if touched
	if(GOD.TouchX > mX-3 && GOD.TouchX < mX + mWidth +3 && GOD.TouchY > mY-3 && GOD.TouchY < mY + mHeight +3){
		//printf("TouchX:%d  TouchY:%d\nB_X:%d  B_Y:%d\nB_W:%d  B_H:%d  \n",GOD.TouchX,GOD.TouchY,mX,mY,mWidth,mHeight);
		return true;
	}
return false;
}
//is press relesed
bool LTexture::SPr()
{
	static bool isRe = false;
	if (SP()){
		if (!isRe) isRe = true;
	} else if (isRe){
		isRe=false;
		if (!GOD.fingerdown) return true;
	}
return false;
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

int LTexture::getX()
{
	return mX;
}

int LTexture::getY()
{
	return mY;
}



