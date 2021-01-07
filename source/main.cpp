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


#ifdef __SWITCH__
#include <unistd.h>

SwkbdTextCheckResult Keyboard_ValidateText(char *string, size_t size) {
	if (strcmp(string, "") == 0) {
		strncpy(string, "The name cannot be empty.", size);
		return SwkbdTextCheckResult_Bad;
	}

	return SwkbdTextCheckResult_OK;
}

const char *Keyboard_GetText(const char *guide_text, const char *initial_text) {
	Result ret = 0;
	SwkbdConfig swkbd;
	static char input_string[256];

	if (R_FAILED(ret = swkbdCreate(&swkbd, 0))) {
		swkbdClose(&swkbd);
		return "";
	}

	swkbdConfigMakePresetDefault(&swkbd);

	if (strlen(guide_text) != 0)
		swkbdConfigSetGuideText(&swkbd, guide_text);

	if (strlen(initial_text) != 0)
		swkbdConfigSetInitialText(&swkbd, initial_text);

	swkbdConfigSetTextCheckCallback(&swkbd, Keyboard_ValidateText);

	if (R_FAILED(ret = swkbdShow(&swkbd, input_string, sizeof(input_string)))) {
		swkbdClose(&swkbd);
		return "";
	}

	swkbdClose(&swkbd);
	return input_string;
}
#endif 




bool	isFileExist(const char *file)
{
	struct stat	st = { 0 };

	if (stat(file, &st) == -1) {
		return (false);
	}

	return (true);
}

//////////////////////////////////aquí empieza el pc.
//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
enum states { programationstate, downloadstate, chapterstate, searchstate, favoritesstate };
int statenow = programationstate;
enum statesreturn { toprogramation, tosearch };
int returnnow = toprogramation;
std::string  urltodownload = "";
int porcendown = 0;
int sizeestimated = 0;
std::string temporallink = "";
bool ahorro = false;
int cancelcurl = 0;
bool preview = false;
int searchchapter = 0;
int favchapter = 0;
int selectchapter = 0;
bool fulldownloaded = false;
bool reloading = false;
bool reloadingsearch = false;
int porcentajereload = 0;
bool activatefirstimage = true;
bool activatefirstsearchimage = true;
std::string serverenlace = "";
std::string searchtext = "";
std::string tempimage = "";
std::string txtyase = "";
#ifdef __SWITCH__
std::string favoritosdirectory = "sdmc:/favoritos.txt";
#else
std::string favoritosdirectory = "C:/respaldo2017/C++/test/Debug/favoritos.txt";
#endif // SWITCH
Mix_Music* gMusic = NULL;
std::ofstream outfile;
//Texture wrapper class
class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);
	bool loadFromFileCustom(std::string path, int h, int w);
	//Creates image from font string
	bool loadFromRenderedText(TTF_Font *fuente, std::string textureText, SDL_Color textColor);
	//Creates image from font string
	bool loadFromRenderedTextWrap(TTF_Font *fuente, std::string textureText, SDL_Color textColor, Uint32 size);
	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;
#ifdef __SWITCH__
HidsysNotificationLedPattern blinkLedPattern(u8 times);
void blinkLed(u8 times);
#endif // ___SWITCH___


//Globally used font
TTF_Font *gFont = NULL;
TTF_Font* digifont = NULL;
TTF_Font *gFontcapit = NULL;
TTF_Font *gFont2 = NULL;
TTF_Font *gFont3 = NULL;
//Rendered texture
LTexture gTextTexture;
LTexture Farest;
LTexture Heart;
LTexture TChapters;
LTexture TPreview;
LTexture TSearchPreview;
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






void close()
{
	//Free loaded images
	gTextTexture.free();
	Farest.free();
	Heart.free();
	TPreview.free();
	TChapters.free();
	TSearchPreview.free();
	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;
	TTF_CloseFont(digifont);
	digifont = NULL;

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

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}


void replace(std::string& subject, const std::string& search,
	const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}


void mayus(std::string &s)
{
	bool cap = true;

	for (unsigned int i = 0; i <= s.length(); i++)
	{
		if (isalpha(s[i]) && cap == true)
		{
			s[i] = toupper(s[i]);
			cap = false;
		}
		else if (isspace(s[i]))
		{
			cap = true;
		}
	}
}
int progress_func(void* ptr, double TotalToDownload, double NowDownloaded,
	double TotalToUpload, double NowUploaded)
{


	// ensure that the file to be downloaded is not empty
	// because that would cause a division by zero error later on
	if (TotalToDownload <= 0.0) {
		return 0;
	}

	// how wide you want the progress meter to be
	int totaldotz = 20;
	double fractiondownloaded = NowDownloaded / TotalToDownload;
	// part of the progressmeter that's already "full"
	int dotz = round(fractiondownloaded * totaldotz);

	// create the "meter"
	int ii = 0;
	porcendown = fractiondownloaded * 100;
	sizeestimated = TotalToDownload;
	printf("%3.0f%% [", fractiondownloaded * 100);
	// part  that's full already
	for (; ii < dotz; ii++) {
		printf("=");
	}
	// remaining part (spaces)
	for (; ii < 20; ii++) {
		printf(" ");
	}
	// and back to line begin - do not forget the fflush to avoid output buffering problems!
	printf("]\r");
	fflush(stdout);
	// if you don't return 0, the transfer will be aborted - see the documentation
	if (cancelcurl == 1)
	{
		return 1;
	}

	return 0;

}

std::string gethtml(std::string enlace)
{

	CURL *curl;
	CURLcode res;
	std::string Buffer;

	curl = curl_easy_init();
	if (curl) {

		curl_easy_setopt(curl, CURLOPT_URL, enlace.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Buffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		return Buffer;
	}
}
void downloadfile(std::string enlace, std::string directorydown)
{

	CURL *curl;
	FILE *fp;
	CURLcode res;

	curl = curl_easy_init();
	if (curl) {

		fp = fopen(directorydown.c_str(), "wb");
		curl_easy_setopt(curl, CURLOPT_URL, enlace.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		// Install the callback function
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func);
		res = curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
		fclose(fp);
	}
}

int downloadjkanimevideo(void* data)
{
	std::string videourl = "";


	std::string content = "";
	std::string enlacejk = urltodownload;
	std::string namedownload = enlacejk;
	replace(namedownload, "https://jkanime.net/", "");
	replace(namedownload, "-", " ");
	replace(namedownload, "/", " ");
	namedownload = namedownload.substr(0, namedownload.length() - 1);
	mayus(namedownload);
	namedownload = namedownload + ".mp4";
#ifdef __SWITCH__
	std::string directorydownload = "sdmc:/" + namedownload;
#else
	std::string directorydownload = "C:\\respaldo2017\\C++\\test\\Debug\\" + namedownload;
#endif // SWITCH


	std::cout << namedownload << std::endl;
	content = gethtml(enlacejk.c_str());
	int val1 = 0, val2 = 0;

	val1 = content.find("https://www.mediafire.com/file/");
	if (val1 != -1)
	{
		val2 = content.find('"', val1);
		
		videourl = content.substr(val1, val2 - val1);
		replace(videourl, "\\", "");
		std::string tempmedia = gethtml(videourl);
		val1 = tempmedia.find("https://download");
		if (val1 != -1)
		{
			val2 = tempmedia.find('"', val1);

			videourl = tempmedia.substr(val1, val2 - val1);
			replace(videourl, "\\", "");
			std::cout << videourl << std::endl;
			serverenlace = videourl;
			downloadfile(videourl, directorydownload);

		}
		else
		{
			val1 = content.find("https://jkanime.net/jk.php?");
			if (val1 != -1)
			{
				val2 = content.find('"', val1);

				videourl = content.substr(val1, val2 - val1);
				replace(videourl, "\\", "");
				replace(videourl, "https://jkanime.net/jk.php?u=", "https://jkanime.net/");

				std::cout << videourl << std::endl;
				serverenlace = videourl;
				downloadfile(videourl, directorydownload);
			}
		}
		
	
	}
	else

	{
		val1 = content.find("https://jkanime.net/jk.php?");
	if (val1 != -1)
	{
		val2 = content.find('"', val1);

		videourl = content.substr(val1, val2 - val1);
		replace(videourl, "\\", "");
		replace(videourl, "https://jkanime.net/jk.php?u=", "https://jkanime.net/");

		std::cout << videourl << std::endl;
		serverenlace = videourl;
		downloadfile(videourl, directorydownload);
	}
	}
	
	return 0;
}
void onlinejkanimevideo(std::string onlineenlace)
{
#ifdef __SWITCH__
	Result rc = 0;
#endif //  SWITCH


	std::string videourl = "";
	std::string content = "";


	std::string enlacejk = onlineenlace;

	content = gethtml(enlacejk.c_str());
	int val1 = 0, val2 = 0;

	val1 = content.find("https://jkanime.net/um.php?");
	if (val1 != -1)
	{
		val2 = content.find('"', val1);

		videourl = content.substr(val1, val2 - val1);
		replace(videourl, "\\", "");


		std::cout << videourl << std::endl;



	}
	else
	{
		val1 = content.find("https://jkanime.net/jk.php?");
		if (val1 != -1)
		{
			val2 = content.find('"', val1);

			videourl = content.substr(val1, val2 - val1);
			replace(videourl, "\\", "");


			std::cout << videourl << std::endl;

		}
	}
#ifdef __SWITCH__
	WebCommonConfig config;
	WebCommonReply reply;
	WebExitReason exitReason = (WebExitReason)0;

	// Create the config. There's a number of web*Create() funcs, see libnx web.h.
	// webPageCreate/webNewsCreate requires running under a host title which has HtmlDocument content, when the title is an Application. When the title is an Application when using webPageCreate/webNewsCreate, and webConfigSetWhitelist is not used, the whitelist will be loaded from the content. Atmosphère hbl_html can be used to handle this.
	rc = webPageCreate(&config, videourl.c_str());
	printf("webPageCreate(): 0x%x\n", rc);

	if (R_SUCCEEDED(rc)) {
		// At this point you can use any webConfigSet* funcs you want.

		rc = webConfigSetWhitelist(&config, "^http*"); // Set the whitelist, adjust as needed. If you're only using a single domain, you could remove this and use webNewsCreate for the above (see web.h for webNewsCreate).
		printf("webConfigSetWhitelist(): 0x%x\n", rc);

		if (R_SUCCEEDED(rc)) { // Launch the applet and wait for it to exit.
			printf("Running webConfigShow...\n");
			rc = webConfigShow(&config, &reply); // If you don't use reply you can pass NULL for it.
			printf("webConfigShow(): 0x%x\n", rc);
		}

		if (R_SUCCEEDED(rc)) { // Normally you can ignore exitReason.
			rc = webReplyGetExitReason(&reply, &exitReason);
			printf("webReplyGetExitReason(): 0x%x", rc);
			if (R_SUCCEEDED(rc)) printf(", 0x%x", exitReason);
			printf("\n");
		}
	}
#endif //  SWITCH

}

bool tienezero = false;
std::string rese = "";
bool enemision = false;
std::string capit(std::string b) {
	tienezero = false;


	std::string a = "";


	a = gethtml(b);
	//
	int re1, re2;
	re1 = a.find("Sinopsis: </strong>") + 19;
	re2 = a.find("</p>", re1);

	std::string terese = a.substr(re1, re2 - re1);
	replace(terese, "<br/>", "");
	rese = terese;
	std::cout << rese << std::endl;
	if (a.find("<b>En emision</b>") != -1)
	{
		enemision = true;
	}
	else
	{
		enemision = false;
	}

	int val0, val1, val2, val3;

	int zero1, zero2;
	std::string zerocontainer = "";
	std::string zerocontainer2 = "";
	zero1 = a.rfind("ajax/pagination_episodes/");
	zero2 = a.find("'", zero1);
	zerocontainer = "https://www.jkanime.net/" + a.substr(zero1, zero2 - zero1) + "/1/";
	std::cout << zerocontainer << std::endl;

	zerocontainer2 = gethtml(zerocontainer);
	int tempzero = zerocontainer2.find("\"0\"");
	if (tempzero != -1) {

		std::cout << "Si contiene" << std::endl;
		tienezero = true;
	}
	else {

		std::cout << "no contiene" << std::endl;
		tienezero = false;
	}




	val0 = a.rfind("href=\"#pag");

	if (val0 != -1) {

		val1 = a.find(">", val0);
		val1 = val1 + 1;
		val2 = a.find("<", val1);

		std::string urlx;


		urlx = (a.substr(val1, val2 - val1));
		val3 = urlx.find(" - ") + 3;
		urlx = urlx.substr(val3);

		return (urlx);
	}

	else

	{
		return "1";
	}


};
std::vector<std::string> arrayimages;
std::vector<std::string> arraychapter;
std::vector<std::string> arraysearch;
std::vector<std::string> arrayfavorites;
std::vector<std::string> arraysearchimages;

int sizeportraity = 180;
int sizeportraitx = 225;
int xdistance = 1000;
int ydistance = 448;
void callimage()
{
#ifdef __SWITCH__
	std::string directorydownloadimage = "sdmc:/RipJKAnime/";
	directorydownloadimage.append(std::to_string(selectchapter));
	directorydownloadimage.append(".jpg");
#else
	std::string directorydownloadimage = "C:/respaldo2017/C++/test/Debug/RipJKAnime/";
	directorydownloadimage.append(std::to_string(selectchapter));
	directorydownloadimage.append(".jpg");
#endif // SWITCH
	TPreview.loadFromFileCustom(directorydownloadimage.c_str(), sizeportraitx, sizeportraity);
	tempimage = directorydownloadimage;
}
void callimagesearch()
{
	
#ifdef __SWITCH__
		std::string directorydownloadimage = "sdmc:/RipJKAnime/s";
		directorydownloadimage.append(std::to_string(searchchapter));
		directorydownloadimage.append(".jpg");
#else
		std::string directorydownloadimage = "C:/respaldo2017/C++/test/Debug/RipJKAnime/s";
		directorydownloadimage.append(std::to_string(searchchapter));
		directorydownloadimage.append(".jpg");
#endif // SWITCH

		
		TSearchPreview.loadFromFileCustom(directorydownloadimage.c_str(), sizeportraitx, sizeportraity);
		tempimage = directorydownloadimage;
	
}
int refrescarpro(void* data)
{
	activatefirstimage = true;
	reloading = true;
	porcentajereload = 0;
	int val1 = 1;
	int val2;
	int val3, val4;
	int val0 = 0;
	std::string temporal = "";
	std::string content = gethtml("https://jkanime.net");
	while (val0 != -1) {
		val0 = content.find("play-button", val1);
		if (val0 == -1) { break; }

		val1 = 19 + content.find("play-button", val1);
		val2 = (content.find('"', val1));
		std::string gdrive = content.substr(val1, val2 - val1);

		arraychapter.push_back(gdrive);
		val3 = content.rfind("<img src=", val2) + 10;
		val4 = content.find('"', val3);
		std::string gpreview = content.substr(val3, val4 - val3);
		arrayimages.push_back(gpreview);
		//std::cout << arraycount << ". " << gdrive << std::endl;
		temporal = temporal + gdrive + "\n";
		temporal = temporal + gpreview + "\n";
		
		val1++;

	}
	printf(temporal.c_str());

	CURL *curl;
	FILE *fp;
	CURLcode res;

	for (int x = 0; x < arrayimages.size(); x++)
	{

	curl = curl_easy_init();
	if (curl) {
		std::string tempima = arrayimages[x];
		std::cout << tempima << selectchapter << std::endl;
#ifdef __SWITCH__
		std::string directorydownloadimage = "sdmc:/RipJKAnime/";
		directorydownloadimage.append(std::to_string(x));
		directorydownloadimage.append(".jpg");
#else
		std::string directorydownloadimage = "C:/respaldo2017/C++/test/Debug/RipJKAnime/" ;
		directorydownloadimage.append(std::to_string(x));
		directorydownloadimage.append(".jpg");
#endif // SWITCH

		fp = fopen(directorydownloadimage.c_str(), "wb");
		curl_easy_setopt(curl, CURLOPT_URL, tempima.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
		fclose(fp);
     
		
	}
	porcentajereload = ((x+1) * 100) / arrayimages.size();
	}
	
	reloading = false;
	preview = true;

	return 0;
}


int searchjk(void* data)
{
	porcentajereload = 0;
	activatefirstsearchimage = true;
	reloadingsearch = true;
	int val1 = 1;
	int val2;
	int val0 = 0;
	int val3, val4;
	int arrayselect = 0;

	
#ifndef __SWITCH__
	searchtext = "h2o";
#endif 

	replace(searchtext, " ", "_");
	replace(searchtext, "!", "");
	replace(searchtext, ";", "");
	if (searchtext.length() >= 3) {
		std::string content = gethtml("https://jkanime.net/buscar/" + searchtext + "/1/");
		content = content + gethtml("https://jkanime.net/buscar/" + searchtext + "/2/");
		int val1 = 1;
		int val2;
		int val0 = 0;

		




		while (val0 != -1) {
			val0 = content.find("portada-title", val1);
			if (val0 == -1) { break; }

			val1 = 6 + content.find("href=", val0);
			val2 = (content.find('"', val1));
			std::string gdrive = content.substr(val1, val2 - val1);
		

			arraysearch.push_back(gdrive);
			val3 = content.find("<img src=", val2) + 10;
			val4 = content.find('"', val3);
			std::string gsearchpreview = content.substr(val3, val4 - val3);
			arraysearchimages.push_back(gsearchpreview);
			std::cout << gsearchpreview << std::endl;
			
			val1++;
		}
		
		for (int x = 0; x < arraysearchimages.size(); x++) {
			CURL *curl;
			FILE *fp;
			CURLcode res;


			curl = curl_easy_init();
			if (curl) {
				std::string tempima = arraysearchimages[x];
				std::cout << tempima << selectchapter << std::endl;
#ifdef __SWITCH__
				std::string directorydownloadimage = "sdmc:/RipJKAnime/s";
				directorydownloadimage.append(std::to_string(x));
				directorydownloadimage.append(".jpg");
#else
				std::string directorydownloadimage = "C:/respaldo2017/C++/test/Debug/RipJKAnime/s";
				directorydownloadimage.append(std::to_string(x));
				directorydownloadimage.append(".jpg");
#endif // SWITCH

				fp = fopen(directorydownloadimage.c_str(), "wb");
				curl_easy_setopt(curl, CURLOPT_URL, tempima.c_str());
				curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36");
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
				curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
				res = curl_easy_perform(curl);
				/* always cleanup */
				curl_easy_cleanup(curl);
				fclose(fp);

				
			}

			porcentajereload = ((x + 1) * 100) / arraysearchimages.size();
		}
	}
	else
	{
		statenow = programationstate;
		returnnow = toprogramation;
	}
	reloadingsearch = false;
	return 0;
}
int main(int argc, char **argv)

{
#ifdef __SWITCH__
	romfsInit();
	socketInitializeDefault();
	//nxlinkStdio();
	struct stat st = { 0 };

	if (stat("sdmc:/RipJKAnime", &st) == -1) {
		mkdir("sdmc:/RipJKAnime", 0777);
	}
#endif 

	
	

	int val1 = 1;
	int val2;
	int val0 = 0;
	int val3, val4;
	int arrayselect = 0;

	int searchcount = 0;
	int maxcapit = 1;
	int mincapit = 0;
	int capmore = 1;
	SDL_Thread* threadID = NULL;
	SDL_Thread* prothread = NULL;
	SDL_Thread* searchthread = NULL;
	std::string temporal = "";
	
	/*std::string content = gethtml("https://jkanime.net");
	while (val0 != -1) {
		val0 = content.find("play-button", val1);
		if (val0 == -1) { break; }

		val1 = 19 + content.find("play-button", val1);
		val2 = (content.find('"', val1));
		std::string gdrive = content.substr(val1, val2 - val1);

		arraychapter.push_back(gdrive);
		val3 = content.rfind("<img src=", val2) + 10;
		val4 = content.find('"', val3);
		std::string gpreview = content.substr(val3, val4 - val3);
		arrayimages.push_back(gpreview);
		//std::cout << arraycount << ". " << gdrive << std::endl;
		temporal = temporal + gdrive + "\n";
		temporal = temporal + gpreview + "\n";

		val1++;
	}

	printf(temporal.c_str());
	*/
	prothread = SDL_CreateThread(refrescarpro, "prothread", (void*)NULL);
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
#ifdef __SWITCH__
		gWindow = SDL_CreateWindow("sdl2_gles2", 0, 0, 1280, 720, 0);
#else
		gWindow = SDL_CreateWindow("RipJKNX", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
#endif // SWITCH


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
#ifdef __SWITCH__
				gMusic = Mix_LoadMUS("romfs:/wada.ogg");
#else
				gMusic = Mix_LoadMUS("C:/respaldo2017/C++/test/Debug/wada.ogg");
#endif // SWITCH
				
				if (gMusic == NULL)
				{
					printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
					
				}
				if (Mix_PlayingMusic() == 0)
				{
					//Play the music
					Mix_PlayMusic(gMusic, -1);
				}
			}
		}
	}



	int posxbase = 20;
	int posybase = 10;

#ifdef __SWITCH__

	gFont = TTF_OpenFont("romfs:/lazy.ttf", 16);
	gFont2 = TTF_OpenFont("romfs:/lazy2.ttf", 150);
	gFontcapit = TTF_OpenFont("romfs:/lazy2.ttf", 100);
	gFont3 = TTF_OpenFont("romfs:/lazy2.ttf", 40);
	digifont = TTF_OpenFont("romfs:/digifont.otf", 16);
	Farest.loadFromFile("romfs:/texture.png");
	Heart.loadFromFile("romfs:/heart.png");

#else
	gFont = TTF_OpenFont("C:\\respaldo2017\\C++\\test\\Debug\\lazy.ttf", 16);
	digifont = TTF_OpenFont("C:\\respaldo2017\\C++\\test\\Debug\\digifont.otf", 16);
	gFont2 = TTF_OpenFont("C:\\respaldo2017\\C++\\test\\Debug\\lazy2.ttf", 150);
	gFontcapit = TTF_OpenFont("C:\\respaldo2017\\C++\\test\\Debug\\lazy2.ttf", 100);
	gFont3 = TTF_OpenFont("C:\\respaldo2017\\C++\\test\\Debug\\lazy2.ttf", 40);

	Farest.loadFromFile("C:\\respaldo2017\\C++\\test\\Debug\\texture.png");
	Heart.loadFromFile("C:\\respaldo2017\\C++\\test\\Debug\\heart.png");


#endif // SWITCH

	SDL_Color textColor = { 50, 50, 50 };

	// execute first


	//Main loop flag
	int quit = 0;

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
				quit = 1;
			}
			//User presses a key
			else if (e.type == SDL_KEYDOWN)
			{
				//Select surfaces based on key press
				switch (e.key.keysym.sym)
				{
				case SDLK_DOWN:
					switch (statenow)
					{
					case searchstate:
						if (reloadingsearch == false)
						{
							TSearchPreview.free();
							if (searchchapter < arraysearch.size() - 1)
							{
								searchchapter++;

								std::cout << searchchapter << std::endl;
							}
							else {
								searchchapter = 0;
							}
							callimagesearch();
						
						}
						break;

					case programationstate:
						if (reloading == false)
						{
							TPreview.free();


							if (selectchapter < arraychapter.size() - 1)
							{
								selectchapter++;

								std::cout << selectchapter << std::endl;
							}
							else {
								selectchapter = 0;
							}

							callimage();
							
						}
						break;

					case chapterstate:

						if (capmore > 10)
						{
							capmore = capmore - 10;
						}
						if (capmore < mincapit)
						{
							capmore = mincapit;
						}
						break;

					case favoritesstate:
						if (favchapter < arrayfavorites.size() - 1)
						{
							favchapter++;

							std::cout << favchapter << std::endl;
						}
						else {
							favchapter = 0;
						}
						break;

					}

					break;

				case SDLK_UP:
					switch (statenow)
					{
					case programationstate:
						if (reloading == false)
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
							callimage();
							
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
						if (reloadingsearch == false)
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
							callimagesearch();
							
						}
						break;

					case favoritesstate:
						
						if (favchapter > 0)
						{
							favchapter--;
							std::cout << favchapter << std::endl;
						}
						else {
							favchapter = arrayfavorites.size() - 1;
						}
						break;

					}
					break;
				case SDLK_a:

					switch (statenow)
					{
					case programationstate:

					{if (reloading == false)
					{
						TChapters.free();
						TChapters.loadFromFileCustom(tempimage, 550, 400);
						statenow = chapterstate;
						temporallink = arraychapter[selectchapter];

						int val1 = temporallink.find("/", 20);
						temporallink = temporallink.substr(0, val1 + 1);
						std::cout <<"q wea es esto?"<< temporallink << std::endl;
						maxcapit = atoi(capit(temporallink).c_str());
						if (tienezero == true) {
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

					}
					}
					break;

					case searchstate:
					{
						if (reloadingsearch == false)
						{
							TChapters.free();
							TChapters.loadFromFileCustom(tempimage, 550, 400);
							statenow = chapterstate;
							temporallink = arraysearch[searchchapter];

							std::cout << temporallink << std::endl;
							maxcapit = atoi(capit(temporallink).c_str());
							if (tienezero == true) {
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
						}

					}
					break;

					case favoritesstate:
					{
						
							TChapters.free();
							TChapters.loadFromFileCustom(tempimage, 550, 400);
							statenow = chapterstate;
							temporallink = arrayfavorites[favchapter];

							std::cout << temporallink << std::endl;
							maxcapit = atoi(capit(temporallink).c_str());
							if (tienezero == true) {
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
						

					}
					break;
					case chapterstate:
						std::string tempurl = temporallink + std::to_string(capmore) + "/";
						onlinejkanimevideo(tempurl);
						break;


					}
					break;
				case SDLK_b:

					switch (statenow)
					{
					case programationstate:


						break;
					case downloadstate:
						cancelcurl = 1;
						statenow = chapterstate;


						break;
					case chapterstate:
						cancelcurl = 1;
						txtyase = "";
						switch (returnnow)
						{
						case toprogramation:
							statenow = programationstate;
							break;
						case tosearch:
							statenow = searchstate;
							break;
						}

						break;
					case searchstate:
						if (reloadingsearch == false)
						{
							
							returnnow = toprogramation;
							statenow = programationstate;
							TSearchPreview.free();
						}
						break;

					case favoritesstate:
						statenow = programationstate;
						break;


					}
					break;
				case SDLK_y:

					switch (statenow)
					{
					case programationstate:


						break;
					case downloadstate:



						break;
					case chapterstate:
					{//AGREGAR A FAVORITOS

						outfile.open(favoritosdirectory, std::ios_base::app); // append instead of overwrite
						outfile << temporallink;
						outfile << "\n";
						outfile.close();
						txtyase = "(Ya se agregó)";
					}

					break;
					case favoritesstate:

						std::ofstream ofs(favoritosdirectory, std::ofstream::trunc);

						ofs << "";

						ofs.close();
						
						if (reloading == false)
						{
							arrayfavorites.clear();
							statenow = favoritesstate;
							std::string temp;
							std::ifstream infile;

							std::ifstream file(favoritosdirectory);
							std::string str;
							while (std::getline(file, str)) {
								std::cout << str << "\n";
								if (str.find("jkanime") != -1)
								{
									arrayfavorites.push_back(str);
								}
							}
							file.close();


						}
					       break;




					}
					break;
				case SDLK_l:

					switch (statenow)
					{
					case programationstate:

						
						break;
					case downloadstate:



						break;
					case chapterstate:


						break;
					case searchstate:


						break;

					}
					break;
				case SDLK_f:

					switch (statenow)
					{
					case programationstate:
						if (reloading == false)
						{
							arrayfavorites.clear();
							statenow = favoritesstate;
							std::string temp;
							std::ifstream infile;

							std::ifstream file(favoritosdirectory);
							std::string str;
							while (std::getline(file, str)) {
								std::cout << str << "\n";
								if (str.find("jkanime") != -1)
								{
									arrayfavorites.push_back(str);
								}
							}
							file.close();
							

						}

						break;
					case downloadstate:



						break;
					case chapterstate:


						break;
					case searchstate:


						break;
					case favoritesstate:

						statenow = programationstate;
							break;

					}
					break;
				case SDLK_z:

					switch (statenow)
					{
					case programationstate:
						//refrescar();



					{
						
							preview = true;
							TPreview.free();
							callimage();

						
						

					}
					break;
					case downloadstate:



						break;
					case chapterstate:


						break;
					case searchstate:
						
							preview = true;
							TSearchPreview.free();
							callimagesearch();

					

						break;


					}
					break;
				case SDLK_x:

					switch (statenow)
					{
					case programationstate:


						break;
					case downloadstate:


						break;
					case chapterstate:

						if (ahorro == true)
						{
							ahorro = false;
						}
						else
						{
							ahorro = true;
						}



						break;
					case searchstate:

						break;


					}
					break;
				case SDLK_LEFT:
					switch (statenow)
					{
					case downloadstate:


						break;

					case chapterstate:
						if (capmore > mincapit)
						{
							capmore = capmore - 1;
						}
						if (capmore < mincapit)
						{
							capmore = mincapit;
						}
						break;
					}

					break;
				case SDLK_p:
					if (Mix_PlayingMusic() == 0)
					{
						//Play the music
						Mix_PlayMusic(gMusic, -1);
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
					break;
				case SDLK_r:
					switch (statenow)
					{
					case programationstate:
						if (reloadingsearch == false)
						{
							searchchapter = 0;

							TSearchPreview.free();
							arraysearch.clear();
							arraysearchimages.clear();
							statenow = searchstate;
							returnnow = tosearch;
							char *buf = (char*)malloc(256);
#ifdef __SWITCH__
							strcpy(buf, Keyboard_GetText("Buscar Anime (3 letras minimo.)", ""));
#endif // SWITCH


							std::string tempbus(buf);
							searchtext = tempbus;

							searchthread = SDL_CreateThread(searchjk, "searchthread", (void*)NULL);

							break;


						}
					}

					break;

				case SDLK_RIGHT:

					switch (statenow)
					{
					case programationstate:

						break;
					case downloadstate:


						break;
					case chapterstate:
						if (capmore < maxcapit)
						{
							capmore = capmore + 1;
						}
						if (capmore > maxcapit)
						{
							capmore = maxcapit;
						}
						break;

					}


					break;

				default:

					break;
				}
			}
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
					if (e.jbutton.button == 0) {
						// (A) button down
						switch (statenow)
						{
						case programationstate:

						{if (reloading == false)
						{
							TChapters.free();
							TChapters.loadFromFileCustom(tempimage, 550, 400);
							statenow = chapterstate;
							temporallink = arraychapter[selectchapter];

							int val1 = temporallink.find("/", 20);
							temporallink = temporallink.substr(0, val1 + 1);
							std::cout << "q wea es esto?" << temporallink << std::endl;
							maxcapit = atoi(capit(temporallink).c_str());
							if (tienezero == true) {
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

						}
						}
						break;

						case searchstate:
						{
							if (reloadingsearch == false)
							{
								TChapters.free();
								TChapters.loadFromFileCustom(tempimage, 550, 400);
								statenow = chapterstate;
								temporallink = arraysearch[searchchapter];

								std::cout << temporallink << std::endl;
								maxcapit = atoi(capit(temporallink).c_str());
								if (tienezero == true) {
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
							}

						}
						break;

						case favoritesstate:
						{

							TChapters.free();
							TChapters.loadFromFileCustom(tempimage, 550, 400);
							statenow = chapterstate;
							temporallink = arrayfavorites[favchapter];

							std::cout << temporallink << std::endl;
							maxcapit = atoi(capit(temporallink).c_str());
							if (tienezero == true) {
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


						}
						break;
						case chapterstate:
							std::string tempurl = temporallink + std::to_string(capmore) + "/";
							onlinejkanimevideo(tempurl);
							break;


						}
					}
					else if (e.jbutton.button == 10) {
						// (+) button down
						//cancelcurl = 1;
						//quit = 1;
						if (Mix_PlayingMusic() == 0)
						{
							//Play the music
							Mix_PlayMusic(gMusic, -1);
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
					else if (e.jbutton.button == 6) {
						// (L) button down
#ifdef __SWITCH__
						Result rc = 0;
#endif //  SWITCH
#ifdef __SWITCH__
						WebCommonConfig config;
						WebCommonReply reply;
						WebExitReason exitReason = (WebExitReason)0;

						// Create the config. There's a number of web*Create() funcs, see libnx web.h.
						// webPageCreate/webNewsCreate requires running under a host title which has HtmlDocument content, when the title is an Application. When the title is an Application when using webPageCreate/webNewsCreate, and webConfigSetWhitelist is not used, the whitelist will be loaded from the content. Atmosphère hbl_html can be used to handle this.
						rc = webPageCreate(&config, "https://m.animeflv.net");
						printf("webPageCreate(): 0x%x\n", rc);

						if (R_SUCCEEDED(rc)) {
							// At this point you can use any webConfigSet* funcs you want.

							rc = webConfigSetWhitelist(&config, "^http*"); // Set the whitelist, adjust as needed. If you're only using a single domain, you could remove this and use webNewsCreate for the above (see web.h for webNewsCreate).
							printf("webConfigSetWhitelist(): 0x%x\n", rc);

							if (R_SUCCEEDED(rc)) { // Launch the applet and wait for it to exit.
								printf("Running webConfigShow...\n");
								rc = webConfigShow(&config, &reply); // If you don't use reply you can pass NULL for it.
								printf("webConfigShow(): 0x%x\n", rc);
							}

							if (R_SUCCEEDED(rc)) { // Normally you can ignore exitReason.
								rc = webReplyGetExitReason(&reply, &exitReason);
								printf("webReplyGetExitReason(): 0x%x", rc);
								if (R_SUCCEEDED(rc)) printf(", 0x%x", exitReason);
								printf("\n");
							}
						}
#endif //  SWITCH


					}
					else if (e.jbutton.button == 9) {
						// (ZR) button down
						switch (statenow)
						{
						case programationstate:
							if (reloading == false)
							{
								arrayfavorites.clear();
								statenow = favoritesstate;
								std::string temp;
								std::ifstream infile;

								std::ifstream file(favoritosdirectory);
								std::string str;
								while (std::getline(file, str)) {
									std::cout << str << "\n";
									if (str.find("jkanime") != -1)
									{
										arrayfavorites.push_back(str);
									}
								}
								file.close();


							}

							break;
						case downloadstate:



							break;
						case chapterstate:


							break;
						case searchstate:


							break;
						case favoritesstate:

							statenow = programationstate;
							break;

						}
					}
					else if (e.jbutton.button == 1) {
						// (B) button down
						switch (statenow)
						{
						case programationstate:


							break;
						case downloadstate:
							cancelcurl = 1;
							statenow = chapterstate;


							break;
						case chapterstate:
							cancelcurl = 1;
							txtyase = "";
							switch (returnnow)
							{
							case toprogramation:
								statenow = programationstate;
								break;
							case tosearch:
								statenow = searchstate;
								break;
							}

							break;
						case searchstate:
							if (reloadingsearch == false)
							{

								returnnow = toprogramation;
								statenow = programationstate;
								TSearchPreview.free();
							}
							break;

						case favoritesstate:
							statenow = programationstate;
							break;


						}
					}
					else if (e.jbutton.button == 2) {
						// (X) button down

						switch (statenow)
						{
						case programationstate:


							break;
						case downloadstate:


							break;
						case chapterstate:

							if (ahorro == true)
							{
								ahorro = false;
							}
							else
							{
								ahorro = true;
							}



							break;
						case searchstate:

							break;


						}
					}
					else if (e.jbutton.button == 3) {
						// (Y) button down

						switch (statenow)
						{
						case programationstate:


							break;
						case downloadstate:



							break;
						case chapterstate:
						{//AGREGAR A FAVORITOS

							outfile.open(favoritosdirectory, std::ios_base::app); // append instead of overwrite
							outfile << temporallink;
							outfile << "\n";
							outfile.close();
							txtyase = "(Ya se agregó)";
						}

						break;
						case favoritesstate:

							std::ofstream ofs(favoritosdirectory, std::ofstream::trunc);

							ofs << "";

							ofs.close();

							if (reloading == false)
							{
								arrayfavorites.clear();
								statenow = favoritesstate;
								std::string temp;
								std::ifstream infile;

								std::ifstream file(favoritosdirectory);
								std::string str;
								while (std::getline(file, str)) {
									std::cout << str << "\n";
									if (str.find("jkanime") != -1)
									{
										arrayfavorites.push_back(str);
									}
								}
								file.close();


							}
							break;




						}
					}
					else if (e.jbutton.button == 7) {
						// (R) button down
						switch (statenow)
						{
						case programationstate:
							if (reloadingsearch == false)
							{
#ifdef __SWITCH__
								//blinkLed(1);//LED
#endif // __SWITCH__
								searchchapter = 0;
								TSearchPreview.free();
								arraysearch.clear();
								arraysearchimages.clear();
								statenow = searchstate;
								returnnow = tosearch;
								char *buf = (char*)malloc(256);
#ifdef __SWITCH__
								strcpy(buf, Keyboard_GetText("Buscar Anime (3 letras minimo.)", ""));
#endif // SWITCH


								std::string tempbus(buf);
								searchtext = tempbus;

								searchthread = SDL_CreateThread(searchjk, "searchthread", (void*)NULL);

								break;


							}

						}

					}
					else if (e.jbutton.button == 12 || e.jbutton.button == 16) {
						// (left) button down
						switch (statenow)
						{
						case downloadstate:


							break;

						case chapterstate:
							if (capmore > mincapit)
							{
								capmore = capmore - 1;
							}
							if (capmore < mincapit)
							{
								capmore = mincapit;
							}
							break;
						}
					}
					else if (e.jbutton.button == 14 || e.jbutton.button == 18) {
						// (right) button down
						switch (statenow)
						{
						case programationstate:

							break;
						case downloadstate:


							break;
						case chapterstate:
							if (capmore < maxcapit)
							{
								capmore = capmore + 1;
							}
							if (capmore > maxcapit)
							{
								capmore = maxcapit;
							}
							break;

						}
					}
					else if (e.jbutton.button == 17 || e.jbutton.button == 13) {
						// (up) button down
						switch (statenow)
						{
						case programationstate:
							if (reloading == false)
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
								callimage();

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
							if (reloadingsearch == false)
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
								callimagesearch();

							}
							break;

						case favoritesstate:

							if (favchapter > 0)
							{
								favchapter--;
								std::cout << favchapter << std::endl;
							}
							else {
								favchapter = arrayfavorites.size() - 1;
							}
							break;

						}
					}
					else if (e.jbutton.button == 19 || e.jbutton.button == 15) {
						// (down) button down
						switch (statenow)
						{
						case searchstate:
							if (reloadingsearch == false)
							{
								TSearchPreview.free();
								if (searchchapter < arraysearch.size() - 1)
								{
									searchchapter++;

									std::cout << searchchapter << std::endl;
								}
								else {
									searchchapter = 0;
								}
								callimagesearch();

							}
							break;

						case programationstate:
							if (reloading == false)
							{
								TPreview.free();


								if (selectchapter < arraychapter.size() - 1)
								{
									selectchapter++;

									std::cout << selectchapter << std::endl;
								}
								else {
									selectchapter = 0;
								}

								callimage();

							}
							break;

						case chapterstate:

							if (capmore > 10)
							{
								capmore = capmore - 10;
							}
							if (capmore < mincapit)
							{
								capmore = mincapit;
							}
							break;

						case favoritesstate:
							if (favchapter < arrayfavorites.size() - 1)
							{
								favchapter++;

								std::cout << favchapter << std::endl;
							}
							else {
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
#endif // SWITCH

		}









		//Clear screen
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		//Render current frame
		//wallpaper
		Farest.render((0), (0));
		switch (statenow)
		{
		case chapterstate:

		{std::string temptext = temporallink;
		replace(temptext, "https://jkanime.net/", "");
		replace(temptext, "/", " ");
		replace(temptext, "-", " ");
		mayus(temptext);
		gTextTexture.loadFromRenderedText(gFont3, temptext + ":", { 255, 255, 255 });
		gTextTexture.render(posxbase - 1, posybase - 1);
		gTextTexture.loadFromRenderedText(gFont3, temptext + ":", textColor);
		gTextTexture.render(posxbase, posybase);
		gTextTexture.loadFromRenderedText(gFont, "Por favor escoge un capítulo entre: " + std::to_string(mincapit) + " - " + std::to_string(maxcapit), { 255, 255, 255 });
		gTextTexture.render(posxbase + 800, posybase + 589);
		gTextTexture.loadFromRenderedText(gFont, "Por favor escoge un capítulo entre: " + std::to_string(mincapit) + " - " + std::to_string(maxcapit), textColor);
		gTextTexture.render(posxbase + 800, posybase + 590);
		
		SDL_Rect fillRect = { posxbase - 5, SCREEN_HEIGHT - 202, 320, 20 };
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		SDL_RenderFillRect(gRenderer, &fillRect);
		
		
		gTextTexture.loadFromRenderedText(gFont, "(*En SXOS desactiva Stealth Mode*)", textColor);
		gTextTexture.render(posxbase, SCREEN_HEIGHT - 200);
		gTextTexture.loadFromRenderedText(gFont, "\"Y\" para agregar a Favoritos. " + txtyase, textColor);
		gTextTexture.render(posxbase, SCREEN_HEIGHT - 120);
		gTextTexture.loadFromRenderedText(gFont, "\"Left\" para restar 1, \"Down\" para restar 10. ", textColor);
		gTextTexture.render(posxbase, SCREEN_HEIGHT - 100);
		gTextTexture.loadFromRenderedText(gFont, "\"Right\" para sumar 1 y \"Up\" para sumar 10.", textColor);
		gTextTexture.render(posxbase, SCREEN_HEIGHT - 80);
		gTextTexture.loadFromRenderedText(gFont, "\"B\" para volver a la programación.", textColor);
		gTextTexture.render(posxbase, SCREEN_HEIGHT - 60);
		gTextTexture.loadFromRenderedText(gFont, "\"A\" para ver Online.", textColor);
		gTextTexture.render(posxbase, SCREEN_HEIGHT - 30);
		gTextTexture.loadFromRenderedText(gFontcapit, "Capítulo: " + std::to_string(capmore), { 255, 255, 255 });
		gTextTexture.render(posxbase + 649, posybase + 589);
		gTextTexture.loadFromRenderedText(gFontcapit, "Capítulo: " + std::to_string(capmore), textColor);
		gTextTexture.render(posxbase + 650, posybase + 590);
		
		{SDL_Rect fillRect = { SCREEN_WIDTH - 462,SCREEN_HEIGHT / 2 - 352, 404, 554 };
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

		SDL_RenderFillRect(gRenderer, &fillRect); 
		TChapters.render(SCREEN_WIDTH - 460, SCREEN_HEIGHT / 2 - 350); 
		}
		
		if (enemision == true)
		{
			gTextTexture.loadFromRenderedText(gFont3, "Estado: En Emisión", {0,0,0});
			gTextTexture.render(posxbase + 800, posybase + 547);
			gTextTexture.loadFromRenderedText(gFont3, "Estado: En Emisión", { 16,191,0 });
			gTextTexture.render(posxbase + 800, posybase + 548);
		}
		else
		{
			gTextTexture.loadFromRenderedText(gFont3, "Estado: Concluido", {0,0,0});
			gTextTexture.render(posxbase + 800, posybase + 547);
			gTextTexture.loadFromRenderedText(gFont3, "Estado: Concluido", { 140,0,0 });
			gTextTexture.render(posxbase + 800, posybase + 548);
		}
		
		gTextTexture.loadFromRenderedTextWrap(gFont, rese, { 255, 255, 255 }, 750);
		gTextTexture.render(posxbase - 1, posybase + 54);
		gTextTexture.loadFromRenderedTextWrap(gFont, rese, textColor, 750);
		gTextTexture.render(posxbase, posybase + 55);
		}

		break;


		case programationstate:
			if (reloading == false) {

				for (int x = 0; x < arraychapter.size(); x++) {
					std::string temptext = arraychapter[x];
					replace(temptext, "https://jkanime.net/", "");
					replace(temptext, "/", " ");
					replace(temptext, "-", " ");
					mayus(temptext);



					if (x == selectchapter) {
						gTextTexture.loadFromRenderedText(digifont, temptext, { 3,96,153 });
						gTextTexture.render(posxbase + 30, posybase + (x * 22));


						Heart.render(posxbase + 10, posybase +5 + (x * 22));
						Heart.render(posxbase + gTextTexture.getWidth() + 25, posybase +5 + (x * 22));


					}
					else
					{

						gTextTexture.loadFromRenderedText(digifont, temptext, textColor);
						gTextTexture.render(posxbase, posybase + (x * 22));

					}

				}
				if (activatefirstimage == true)
				{
					TPreview.free();
					callimage();
					activatefirstimage = false;
				}
				if (preview == true)
				{
					{SDL_Rect fillRect = { xdistance + 18, ydistance + 8, sizeportraity + 4, sizeportraitx + 4 };
					SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

					SDL_RenderFillRect(gRenderer, &fillRect);
					TPreview.render(posxbase + xdistance, posybase + ydistance);
					}
				}

				{SDL_Rect fillRect = { 0, SCREEN_HEIGHT - 35, 1280, 25 };
				SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

				SDL_RenderFillRect(gRenderer, &fillRect); }

			
				gTextTexture.loadFromRenderedText(gFont, "\"A\" para Confirmar - \"R\" para Buscar - \"L\" para AnimeFLV -  \"ZR\" para Favoritos.", textColor);
				gTextTexture.render(posxbase, SCREEN_HEIGHT - 30);
				
				gTextTexture.loadFromRenderedText(gFont, "(Ver 1.8) #KASTXUPALO", {100,0,0});
				gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 30, SCREEN_HEIGHT - 30);




			}
			else
			{
				{SDL_Rect fillRect = { 0, SCREEN_HEIGHT/2 - 25, 1280, 50 };
				SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

				SDL_RenderFillRect(gRenderer, &fillRect); }

				
				gTextTexture.loadFromRenderedText(gFont3, "Cargando programación... (" + std::to_string(porcentajereload) + "%)", textColor);
				gTextTexture.render(SCREEN_WIDTH/2 - gTextTexture.getWidth()/2, SCREEN_HEIGHT/2 - gTextTexture.getHeight() / 2);
			}
			break;
		case searchstate:
			if (reloadingsearch == false) {
				for (int x = 0; x < arraysearch.size(); x++) {
					std::string temptext = arraysearch[x];
				
					replace(temptext, "https://jkanime.net/", "");
					replace(temptext, "/", " ");
					replace(temptext, "-", " ");
					mayus(temptext);
					if (x == searchchapter) {
						gTextTexture.loadFromRenderedText(digifont, temptext, { 3,96,153 });
						gTextTexture.render(posxbase + 30, posybase + (x * 22));


						Heart.render(posxbase + 10, posybase + 5 + (x * 22));
						Heart.render(posxbase + gTextTexture.getWidth() + 25, posybase + 5 + (x * 22));


					}
					else
					{

						gTextTexture.loadFromRenderedText(digifont, temptext, textColor);
						gTextTexture.render(posxbase, posybase + (x * 22));

					}

				}
				

				if (activatefirstsearchimage == true)
				{
					
					TSearchPreview.free();
					callimagesearch();
					activatefirstsearchimage = false;
				}
				if (preview == true)
				{
					{SDL_Rect fillRect = { xdistance + 18, ydistance + 8, sizeportraity + 4, sizeportraitx + 4 };
					SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

					SDL_RenderFillRect(gRenderer, &fillRect);
					TSearchPreview.render(posxbase + xdistance, posybase + ydistance);
					}
				}

				{SDL_Rect fillRect = { 0, SCREEN_HEIGHT - 35, 1280, 25 };
				SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

				SDL_RenderFillRect(gRenderer, &fillRect); }
				gTextTexture.loadFromRenderedText(gFont, "\"A\" para Continuar - \"B\" para volver", textColor);
				gTextTexture.render(posxbase, SCREEN_HEIGHT - 30);

				break;

		case favoritesstate:
			for (int x = 0; x < arrayfavorites.size(); x++) {
				std::string temptext = arrayfavorites[x];

				replace(temptext, "https://jkanime.net/", "");
				replace(temptext, "/", " ");
				replace(temptext, "-", " ");
				mayus(temptext);
				if (x == favchapter) {
					gTextTexture.loadFromRenderedText(digifont, temptext, { 3,96,153 });
					gTextTexture.render(posxbase + 30, posybase + (x * 22));


					Heart.render(posxbase + 10, posybase + 5 + (x * 22));
					Heart.render(posxbase + gTextTexture.getWidth() + 25, posybase + 5 + (x * 22));


				}
				else
				{

					gTextTexture.loadFromRenderedText(digifont, temptext, textColor);
					gTextTexture.render(posxbase, posybase + (x * 22));

				}


			}

			{SDL_Rect fillRect = { 0, SCREEN_HEIGHT - 35, 1280, 25 };
			SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

			SDL_RenderFillRect(gRenderer, &fillRect); }
			gTextTexture.loadFromRenderedText(gFont, "\"A\" para Continuar - \"B\" para volver - \"Y\" para borrar TODOS los Favoritos (¡CUIDADO!)", textColor);
			gTextTexture.render(posxbase, SCREEN_HEIGHT - 30);
			break;
		case downloadstate:
			std::string temptext = urltodownload;
			replace(temptext, "https://jkanime.net/", "");
			replace(temptext, "/", " ");
			replace(temptext, "-", " ");
			mayus(temptext);
			gTextTexture.loadFromRenderedText(gFont, "Descargando actualmente:", textColor);
			gTextTexture.render(posxbase, posybase);
			gTextTexture.loadFromRenderedText(gFont3, temptext, textColor);
			gTextTexture.render(posxbase, posybase + 20);

			gTextTexture.loadFromRenderedText(gFont2, std::to_string(porcendown) + "\%", textColor);
			gTextTexture.render(posxbase + 40, posybase + 40);
			gTextTexture.loadFromRenderedText(gFont, "Peso estimado: " + std::to_string((int)(sizeestimated / 1000000)) + "mb.", textColor);
			gTextTexture.render(posxbase + 100, posybase + 220);


			gTextTexture.loadFromRenderedText(gFont, serverenlace, {168,0,0});
			gTextTexture.render(posxbase , posybase + 280);
			gTextTexture.loadFromRenderedText(gFont, "Usa el HomeBrew PPlay para reproducir el video.", textColor);
			gTextTexture.render(posxbase, posybase + 260);
			gTextTexture.loadFromRenderedText(gFont, "Cancelar la descarga \"B\" - \"+\" para salir", textColor);
			gTextTexture.render(posxbase, SCREEN_HEIGHT - 50);

			if (std::to_string(porcendown) == "100") {


#ifdef __SWITCH__
				if (fulldownloaded == false)
				{
					//blinkLed(6);//LED
					fulldownloaded = true;
				}
#endif // __SWITCH__


				//Render red filled quad
				SDL_Rect fillRect = { posxbase + 98, posybase + 400, 580, 50 };
				SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
				SDL_RenderFillRect(gRenderer, &fillRect);
				gTextTexture.loadFromRenderedText(gFont3, "¡Descarga Completada! Revisa tu SD.", textColor);
				gTextTexture.render(posxbase + 100, posybase + 400);
			}
			}
				else
			{
				{SDL_Rect fillRect = { 0, SCREEN_HEIGHT / 2 - 25, 1280, 50 };
				SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

				SDL_RenderFillRect(gRenderer, &fillRect); }

				textColor = { 50, 50, 50 };
				gTextTexture.loadFromRenderedText(gFont3, "Cargando búsqueda... (" + std::to_string(porcentajereload) + "%)", textColor);
				gTextTexture.render(SCREEN_WIDTH / 2 - gTextTexture.getWidth() / 2, SCREEN_HEIGHT / 2 - gTextTexture.getHeight() / 2);
			}

			break;



		}



		//Update screen
		SDL_RenderPresent(gRenderer);


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
	hidsysExit();
	socketExit();
	romfsExit();
#endif // SWITCH



	close();

	return 0;

}