#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <switch.h>
#include <math.h> 
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <iostream>
#include <math.h>  
#include <Vector>

							
// Empty strings are invalid.
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

bool	isFileExist(const char *file)
{
	struct stat	st = {0};

	if (stat(file, &st) == -1) {
		return (false);
	}

	return (true);
}

//////////////////////////////////aquí empieza el pc.
//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
enum states {programationstate,downloadstate,chapterstate,searchstate};
int statenow = programationstate;
std::string  urltodownload = "";
int porcendown = 0;
int sizeestimated = 0;
std::string temporallink = "";
bool ahorro = false;
int cancelcurl = 0;
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

	//Creates image from font string
	bool loadFromRenderedText(TTF_Font *fuente,std::string textureText, SDL_Color textColor);

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

//Globally used font
TTF_Font *gFont = NULL;
TTF_Font *gFont2 = NULL;
TTF_Font *gFont3 = NULL;
//Rendered texture
LTexture gTextTexture;
LTexture Farest;
LTexture Heart;

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

bool LTexture::loadFromRenderedText(TTF_Font *fuente,std::string textureText, SDL_Color textColor)
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
	
	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;
	TTF_CloseFont(gFont2);
	gFont2 = NULL;
	TTF_CloseFont(gFont3);
	gFont3 = NULL;
	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	
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
	std::string directorydownload = "sdmc:/" + namedownload;
	std::cout << namedownload << std::endl;
	content = gethtml(enlacejk.c_str());
	int val1 = 0, val2 = 0;

	val1 = content.find("https://jkanime.net/jkrapid.php");
	if (val1 != -1) {
		val2 = content.find('"', val1);




		videourl = content.substr(val1, val2 - val1);
		replace(videourl, "\\", "");
		replace(videourl, "https://jkanime.net/jkrapid.php?u=", "https://www.rapidvideo.com/e/");
		videourl = videourl + "/";
		std::cout << videourl << std::endl;

		videourl = gethtml(videourl);
		val1 = videourl.find("720p");
		if (val1 != -1)
		{
			std::string temporal = "";
			val1 = videourl.rfind("https://", val1);
			val2 = videourl.find('"', val1);
			temporal = videourl.substr(val1, val2 - val1);
			std::cout << "intentando 720" << std::endl;
		}
		else {
			val1 = videourl.find("480p");
			if (val1 != -1)
			{
				std::string temporal = "";
				val1 = videourl.rfind("https://", val1);
				val2 = videourl.find('"', val1);
				temporal = videourl.substr(val1, val2 - val1);
				std::cout << "intentando 480" << std::endl;
			}
			else {
				std::string temporal = "";
				val1 = videourl.find("video/mp4");
				val1 = videourl.rfind("https://", val1);
				val2 = videourl.find('"', val1);
				temporal = videourl.substr(val1, val2 - val1);
				std::cout << "la minima" << std::endl;
			}

		}
		//ahorrotrue
		if (ahorro == true) {
			std::string temporal = "";
			val1 = videourl.find("video/mp4");
			val1 = videourl.rfind("https://", val1);
			val2 = videourl.find('"', val1);
			temporal = videourl.substr(val1, val2 - val1);
			std::cout << "la minima" << std::endl;
		}

		videourl = videourl.substr(val1, val2 - val1);

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
			downloadfile(videourl, directorydownload);
		}
	}
	return 0;
}


bool tienezero = false;
std::string capit(std::string b) {
	tienezero = false;


	std::string a = "";

	
		a = gethtml(b);
  //
	
	
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
std::vector<std::string> arraychapter;
std::vector<std::string> arraysearch;

int main(int argc, char **argv)

{	romfsInit();
	socketInitializeDefault();
	std::string content = gethtml("https://jkanime.net");
	
	int val1 = 1;
	int val2;
	int val0 = 0;
	int arrayselect = 0;
	int arraycount = 0;
	int searchcount = 0;
	int maxcapit = 1;
	int mincapit = 0;
	int capmore = 1;
	SDL_Thread* threadID = NULL;
	std::string temporal = "";
	
	while (val0 != -1) {
		val0 = content.find("play-button", val1);
		if (val0 == -1) { break; }

		val1 = 19+ content.find("play-button", val1);
		val2 = (content.find('"', val1));
		std::string gdrive = content.substr(val1, val2 - val1);


		arraychapter.push_back(gdrive);
		//std::cout << arraycount << ". " << gdrive << std::endl;
		temporal = temporal + gdrive + "\n";
		arraycount++;
		val1++;
	}
	
	printf(temporal.c_str());
	//Start up SDL and create window
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
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
			}
		}
	}
		

			int searchchapter = 0;
			int selectchapter = 0;
			int posxbase = 20;
			int posybase = 10;
			gFont = TTF_OpenFont("romfs:/lazy.ttf", 16);
			gFont2 = TTF_OpenFont("romfs:/lazy2.ttf", 150);
			gFont3 = TTF_OpenFont("romfs:/lazy2.ttf", 40);
			SDL_Color textColor = { 50, 50, 50 };
			
			Farest.loadFromFile("romfs:/texture.png");
			Heart.loadFromFile("romfs:/heart.png");
			//Main loop flag
			int quit = 0;

			//Event handler
			SDL_Event e;

		for (int i = 0; i < 2; i++) {
        if (SDL_JoystickOpen(i) == NULL) {
            SDL_Log("SDL_JoystickOpen: %s\n", SDL_GetError());
            SDL_Quit();
            return -1;
			}
		}
			//While application is running
			while (!quit)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) )
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
									if (searchchapter < searchcount - 1)
									{
										searchchapter++;

										std::cout << searchchapter << std::endl;
									}
								break;

								case programationstate:
								if (selectchapter < arraycount - 1)
								{
									selectchapter++;

									std::cout << selectchapter << std::endl;
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
							
							}
							
							break;

						case SDLK_UP:
							switch (statenow)
							{
							case programationstate:
								if (selectchapter > 0)
								{
									selectchapter--;
									std::cout << selectchapter << std::endl;
								}
								break;
							case chapterstate:
								if (capmore < maxcapit)
								{
									capmore = capmore + 10;
								}
								if (capmore >maxcapit)
								{
									capmore = maxcapit;
								}
								break;

							case searchstate:
								if (searchchapter > 0)
								{
									searchchapter--;
									std::cout << searchchapter << std::endl;
								}
								break;

							}
							break;
						case SDLK_a:
							
							switch (statenow)
							{
							case programationstate:

									{
								statenow = chapterstate;
								temporallink = arraychapter[selectchapter];
								
								int val1 = temporallink.find("/", 20);
								temporallink = temporallink.substr(0, val1 + 1);
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
								std::cout <<  maxcapit << std::endl;


									}
								break;

							case searchstate:
							{
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
								break;
								case chapterstate:
								statenow = downloadstate;
								cancelcurl = 0;
								urltodownload = temporallink + std::to_string(capmore) + "/";
								threadID = SDL_CreateThread(downloadjkanimevideo, "jkthread", (void*)NULL);
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
								statenow = programationstate;
								

								break;
							case searchstate:
								statenow = programationstate;
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

						case SDLK_r:
							switch (statenow)
							{
							case programationstate:
							    arraysearch.clear();
								statenow = searchstate;
							char *buf = (char*)malloc(256);
							
							strcpy(buf, Keyboard_GetText("Buscar Anime (3 letras minimo.)", ""));
							std::string tempbus(buf);
							replace(tempbus," ", "_");
							replace(tempbus,"!", "");
							replace(tempbus,";", "");
							 if (tempbus.length() >= 3){
								std::string content = gethtml("https://jkanime.net/buscar/" + tempbus + "/1/");
								 content = content + gethtml("https://jkanime.net/buscar/" + tempbus + "/2/");
								int val1 = 1;
								int val2;
								int val0 = 0;
								
								searchcount = 0;
							
							
							

								while (val0 != -1) {
									val0 = content.find("portada-title", val1);
									if (val0 == -1) { break; }

									val1 = 6 + content.find("href=", val0);
									val2 = (content.find('"', val1));
									std::string gdrive = content.substr(val1, val2 - val1);
									std::cout << searchcount << ". " << gdrive << std::endl;

									arraysearch.push_back(gdrive);
									
									searchcount++;
									val1++;
								}
							 }
							 else
							 {
								statenow = programationstate; 
							 }
								break;
							
							
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

									{
								statenow = chapterstate;
								temporallink = arraychapter[selectchapter];
								
								int val1 = temporallink.find("/", 20);
								temporallink = temporallink.substr(0, val1 + 1);
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
								std::cout <<  maxcapit << std::endl;


									}
								break;

							case searchstate:
							{
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
								break;
								case chapterstate:
								statenow = downloadstate;
								cancelcurl = 0;
								urltodownload = temporallink + std::to_string(capmore) + "/";
								threadID = SDL_CreateThread(downloadjkanimevideo, "jkthread", (void*)NULL);
								break;
							

							}
                        } else if (e.jbutton.button == 10) {
                            // (+) button down
							cancelcurl = 1;
                            quit = 1;
                        }else if (e.jbutton.button == 1) {
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
								statenow = programationstate;
								

								break;
							case searchstate:
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
						else if (e.jbutton.button == 7) {
                            // (R) button down
                           switch (statenow)
							{
							case programationstate:
							    arraysearch.clear();
								statenow = searchstate;
							char *buf = (char*)malloc(256);
							
							strcpy(buf, Keyboard_GetText("Buscar Anime (3 letras minimo.)", ""));
							std::string tempbus(buf);
							replace(tempbus," ", "_");
							replace(tempbus,"!", "");
							replace(tempbus,";", "");
							 if (tempbus.length() >= 3){
								std::string content = gethtml("https://jkanime.net/buscar/" + tempbus + "/1/");
								 content = content + gethtml("https://jkanime.net/buscar/" + tempbus + "/2/");
								int val1 = 1;
								int val2;
								int val0 = 0;
								
								searchcount = 0;
							
							
							

								while (val0 != -1) {
									val0 = content.find("portada-title", val1);
									if (val0 == -1) { break; }

									val1 = 6 + content.find("href=", val0);
									val2 = (content.find('"', val1));
									std::string gdrive = content.substr(val1, val2 - val1);
									std::cout << searchcount << ". " << gdrive << std::endl;

									arraysearch.push_back(gdrive);
									
									searchcount++;
									val1++;
								}
							 }
							 else
							 {
								statenow = programationstate; 
							 }
								break;
							
							
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
								if (selectchapter > 0)
								{
									selectchapter--;
									std::cout << selectchapter << std::endl;
								}
								break;
							case chapterstate:
								if (capmore < maxcapit)
								{
									capmore = capmore + 10;
								}
								if (capmore >maxcapit)
								{
									capmore = maxcapit;
								}
								break;

							case searchstate:
								if (searchchapter > 0)
								{
									searchchapter--;
									std::cout << searchchapter << std::endl;
								}
								break;

							}
                        }else if (e.jbutton.button == 19 || e.jbutton.button == 15 ) {
                            // (down) button down
                          switch (statenow)
								{
								case searchstate:
									if (searchchapter < searchcount - 1)
									{
										searchchapter++;

										std::cout << searchchapter << std::endl;
									}
								break;

								case programationstate:
								if (selectchapter < arraycount - 1)
								{
									selectchapter++;

									std::cout << selectchapter << std::endl;
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
							
							}
                        }
                    }
                    break;

                default:
                    break;
            }	
					
				}
				
				

					
				
				
				
			
				
					//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				//Render current frame
				//wallpaper
				Farest.render((0) , (0) );
				switch (statenow)
				{
				case chapterstate:
					
				{std::string temptext = temporallink;
					replace(temptext, "https://jkanime.net/", "");
					replace(temptext, "/", " ");
					replace(temptext, "-", " ");
					mayus(temptext); 
					gTextTexture.loadFromRenderedText(gFont3, temptext + ":", textColor);
					gTextTexture.render(posxbase, posybase);
					gTextTexture.loadFromRenderedText(gFont, "Por favor escoge un capitulo entre: " + std::to_string(mincapit) + " - " + std::to_string(maxcapit), textColor);
					gTextTexture.render(posxbase, posybase + 50);
						if (ahorro == true)
					{
						gTextTexture.loadFromRenderedText(gFont, "Presiona \"X\" para cambiar la calidad del video. (ACTUAL: MEDIA)", textColor);
						gTextTexture.render(posxbase, SCREEN_HEIGHT - 160);
					}
					else
					{
						gTextTexture.loadFromRenderedText(gFont, "Presiona \"X\" para cambiar la calidad del video. (ACTUAL: ALTA)", textColor);
						gTextTexture.render(posxbase, SCREEN_HEIGHT - 160);
					}
					gTextTexture.loadFromRenderedText(gFont, "(A veces la calidad ALTA y MEDIA son lo mismo.)", textColor);
					gTextTexture.render(posxbase, SCREEN_HEIGHT - 140);
					gTextTexture.loadFromRenderedText(gFont, "Presiona \"Left\" para restar 1, \"Down\" para restar 10. ", textColor);
					gTextTexture.render(posxbase, SCREEN_HEIGHT - 100);
					gTextTexture.loadFromRenderedText(gFont, "Presiona \"Right\" para sumar 1 y \"Up\" para sumar 10.", textColor);
					gTextTexture.render(posxbase, SCREEN_HEIGHT - 80);
					gTextTexture.loadFromRenderedText(gFont, "Presiona \"B\" para volver a la programacion.", textColor);
					gTextTexture.render(posxbase, SCREEN_HEIGHT - 60);
					gTextTexture.loadFromRenderedText(gFont, "Presiona \"A\" para empezar la descarga.", textColor);
					gTextTexture.render(posxbase, SCREEN_HEIGHT - 30);
					gTextTexture.loadFromRenderedText(gFont2,"Capitulo: " + std::to_string(capmore), textColor);
					gTextTexture.render(posxbase, posybase + 200);
				}

					break;


				case programationstate:
					for (int x = 0; x < arraycount; x++) {
						std::string temptext = arraychapter[x];
						replace(temptext, "https://jkanime.net/", "");
						replace(temptext, "/", " ");
						replace(temptext, "-", " ");
						mayus(temptext);
						if (x == selectchapter) {
							Heart.render(posxbase + 5, posybase + (x * 22));
							textColor = { 120, 120, 120 };

						}
						else
						{
							textColor = { 50, 50, 50 };
						}



						gTextTexture.loadFromRenderedText(gFont,temptext, textColor);

						if (x == selectchapter) {

							gTextTexture.render(posxbase + 30, posybase + (x * 22));

						}
						else
						{


							gTextTexture.render(posxbase, posybase + (x * 22));

						}
						
					}
					textColor = { 50, 50, 50 };
					gTextTexture.loadFromRenderedText(gFont, "Presiona \"A\" para Descargar y \"R\" para Buscar...", textColor);
					gTextTexture.render(posxbase, SCREEN_HEIGHT - 30);
				
					
					break;
				case searchstate:

					for (int x = 0; x < searchcount; x++) {
						std::string temptext = arraysearch[x];
						replace(temptext, "https://jkanime.net/", "");
						replace(temptext, "/", " ");
						replace(temptext, "-", " ");
						mayus(temptext);
						if (x == searchchapter) {
							Heart.render(posxbase + 5, posybase + (x * 22));
							textColor = { 120, 120, 120 };

						}
						else
						{
							textColor = { 50, 50, 50 };
						}



						gTextTexture.loadFromRenderedText(gFont, temptext, textColor);

						if (x == searchchapter) {

							gTextTexture.render(posxbase + 30, posybase + (x * 22));

						}
						else
						{


							gTextTexture.render(posxbase, posybase + (x * 22));

						}

					}
					textColor = { 50, 50, 50 };
					gTextTexture.loadFromRenderedText(gFont, "Presiona \"A\" para Descargar y \"B\" para volver...", textColor);
					gTextTexture.render(posxbase, SCREEN_HEIGHT - 30);

					break;
				case downloadstate:
					std::string temptext = urltodownload;
					replace(temptext, "https://jkanime.net/", "");
					replace(temptext, "/", " ");
					replace(temptext, "-", " ");
					mayus(temptext);
					gTextTexture.loadFromRenderedText(gFont,"Descargando actualmente:", textColor);
					gTextTexture.render(posxbase, posybase );
					gTextTexture.loadFromRenderedText(gFont, temptext, textColor);
					gTextTexture.render(posxbase , posybase + 20);
				
					gTextTexture.loadFromRenderedText(gFont2, std::to_string(porcendown) + "\%", textColor);
					gTextTexture.render(posxbase + 40, posybase + 40);
					gTextTexture.loadFromRenderedText(gFont, "Peso estimado: "+ std::to_string((int )(sizeestimated/1000000)) + "mb.", textColor);
					gTextTexture.render(posxbase + 100, posybase + 220);
					
					gTextTexture.loadFromRenderedText(gFont, "Usa el HomeBrew PPlay para reproducir el video.", textColor);
					gTextTexture.render(posxbase, posybase + 260);
					gTextTexture.loadFromRenderedText(gFont, "Para cancelar la descarga presiona \"B\" y \"+\" para salir.", textColor);
					gTextTexture.render(posxbase, SCREEN_HEIGHT - 50);

					if (std::to_string(porcendown) == "100") {
						//Render red filled quad
						SDL_Rect fillRect = { posxbase + 98, posybase + 400, 580, 50};
						SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
						SDL_RenderFillRect(gRenderer, &fillRect);
						gTextTexture.loadFromRenderedText(gFont3, "Descarga Completada! Revisa tu SD.", textColor);
						gTextTexture.render(posxbase + 100 , posybase + 400);
					}
					break;



				}
				
				
				
				//Update screen
				SDL_RenderPresent(gRenderer);
			
			
			}
			SDL_WaitThread(threadID, NULL);
	

	//Free resources and close SDL
	socketExit();
	romfsExit();
	close();

	return 0;
	
}
