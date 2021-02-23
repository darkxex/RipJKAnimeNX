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

extern LTexture gTextTexture;
extern TTF_Font *gFont;
extern SDL_Renderer* gRenderer;
LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
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
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
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
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
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
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
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
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
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

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

void DrawImageFile(SDL_Renderer* gRenderer,std::string path,int X, int Y,std::string Text){

		SDL_Surface* DrawImg;
		DrawImg = IMG_Load(path.c_str());
		int DW=0,DH=0;
		if (DrawImg == NULL)
		{
			printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
		}else{
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
		SDL_Texture* CLUST = SDL_CreateTextureFromSurface(gRenderer, DrawImg);

		SDL_Rect ImagetRect2 = {X, Y, DrawImg->w, DrawImg->h};
		SDL_RenderCopy(gRenderer, CLUST , NULL, &ImagetRect2);
		SDL_DestroyTexture(CLUST);
		DW=DrawImg->w;
		DH=DrawImg->h;
		}
		
		if (Text.length()){
			SDL_Color textColor = { 50, 50, 50 };
			gTextTexture.loadFromRenderedText(gFont, Text.c_str(), textColor);
			gTextTexture.render(X+DW+3, Y +(DH/3));
		}
		SDL_FreeSurface(DrawImg);
}

void DrawImageCover(SDL_Renderer* gRenderer,std::string path,int X, int Y,std::string Text,int HS){

		SDL_Surface* DrawImg;
		DrawImg = IMG_Load(path.c_str());
		int WS=0;
		
		if (DrawImg == NULL)
		{
			printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
		}else{
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
		SDL_Texture* CLUST = SDL_CreateTextureFromSurface(gRenderer, DrawImg);
			
		WS = (DrawImg->w * (HS * 1000 /DrawImg->h) /1000);
		
		
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 100);
		SDL_Rect HeaderRect = {X+5,Y+5, WS+6, HS+6};
		SDL_RenderFillRect(gRenderer, &HeaderRect);
		
		SDL_Rect ImagetRect2 = {X, Y, WS, HS};
		SDL_RenderCopy(gRenderer, CLUST , NULL, &ImagetRect2);
		SDL_DestroyTexture(CLUST);

		}

		
		if (Text.length()){
			SDL_Color textColor = { 50, 50, 50 };
			gTextTexture.loadFromRenderedText(gFont, Text.c_str(), textColor);
			gTextTexture.render(X+WS+3, Y +(HS/3));
		}
		SDL_FreeSurface(DrawImg);
}
