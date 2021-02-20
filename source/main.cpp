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
extern u32 __nx_applet_exit_mode;

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

bool AppletMode=false;
bool isSXOS=false;
bool hasStealth=false;

#ifdef __SWITCH__
std::string favoritosdirectory = "sdmc:/favoritos.txt";
#else
std::string favoritosdirectory = "C:/respaldo2017/C++/test/Debug/favoritos.txt";
#endif // SWITCH
Mix_Music* gMusic = NULL;
std::ofstream outfile;
//Frees media and shuts down SDL
void close();

//make some includes to clean alittle the main
#include "SDLWork.hpp"
#include "utils.hpp"
#include "Request.hpp"


//MAIN INT
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

	
AppletType at = appletGetAppletType();
if (at != AppletType_Application && at != AppletType_SystemApplication) {AppletMode=true;}


//	int val1 = 1;
//	int val2;
//	int val0 = 0;
//	int val3, val4;
//	int arrayselect = 0;

//	int searchcount = 0;
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

if (AppletMode) {
			//Clear screen
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		//Render current frame
		//wallpaper
		Farest.render((0), (0));
		SDL_Rect fillRect = { 0, SCREEN_HEIGHT/2 - 25, 1280, 50 };
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

		SDL_RenderFillRect(gRenderer, &fillRect); 
		gTextTexture.loadFromRenderedText(gFont3, "Esta App No funciona en Modo Applet ", textColor);
		gTextTexture.render(SCREEN_WIDTH/2 - gTextTexture.getWidth()/2, SCREEN_HEIGHT/2 - gTextTexture.getHeight() / 2);
		SDL_RenderPresent(gRenderer);
		SDL_Delay(3000);
quit=1;
}
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
							if (searchchapter < (int)arraysearch.size() - 1)
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


							if (selectchapter < (int)arraychapter.size() - 1)
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
						if (favchapter < (int)arrayfavorites.size() - 1)
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
								if (str.find("jkanime"))
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
								if (str.find("jkanime"))
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
						// (+) button down close to home menu
						cancelcurl = 1;
						quit = 1;
					}
					else if (e.jbutton.button == 11) {
						// (-) button down
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
									if (str.find("jkanime"))
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
									if (str.find("jkanime"))
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
								favchapter = (int)arrayfavorites.size() - 1;
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
								if (searchchapter < (int)arraysearch.size() - 1)
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


								if (selectchapter < (int)arraychapter.size() - 1)
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
							if (favchapter < (int)arrayfavorites.size() - 1)
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
		{
		//Draw a bacground to a nice view
		SDL_SetRenderDrawColor(gRenderer, 170, 170, 170, 80);
		SDL_Rect HeaderRect = {0,0, 1280, 720};
		SDL_RenderFillRect(gRenderer, &HeaderRect);
		std::string temptext = temporallink;
		replace(temptext, "https://jkanime.net/", "");
		replace(temptext, "/", " ");
		replace(temptext, "-", " ");
		mayus(temptext);

		{//draw preview image
		SDL_Rect fillRect = { SCREEN_WIDTH - 462,SCREEN_HEIGHT / 2 - 352, 404, 595 };
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

		SDL_RenderFillRect(gRenderer, &fillRect); 
		TChapters.render(SCREEN_WIDTH - 460, SCREEN_HEIGHT / 2 - 350);
		
		//draw status
		gTextTexture.loadFromRenderedText(gFont3, temptext + ":", { 255, 255, 255 });
		gTextTexture.render(posxbase - 1, posybase - 1);
		gTextTexture.loadFromRenderedText(gFont3, temptext + ":", textColor);
		gTextTexture.render(posxbase, posybase);
		}

		//warning , only display in sxos ToDo
		gTextTexture.loadFromRenderedText(gFont, "(*En SXOS desactiva Stealth Mode*)", textColor);
		gTextTexture.render(posxbase, 0 );

		int XS=0 , YS =0;//use this to move the element
		{//draw caps Scroll
			SDL_Rect fillRect = {posxbase + 90+XS, posybase + 570+YS, 420, 35 };
			SDL_SetRenderDrawColor(gRenderer, 50, 50, 50, 80);
			SDL_RenderFillRect(gRenderer, &fillRect);
			if (capmore-2 > 0) {
				gTextTexture.loadFromRenderedText(gFont3,  std::to_string(capmore-2), textColor);
				gTextTexture.render(posxbase + 150 +XS, posybase + 558+YS);
			}
			if (capmore-1 > 0) {
				gTextTexture.loadFromRenderedText(gFont3,  std::to_string(capmore-1), textColor);
				gTextTexture.render(posxbase + 215+XS, posybase + 558+YS);
			}
			
			gTextTexture.loadFromRenderedText(gFont3, std::to_string(capmore), { 255, 255, 255 });
			gTextTexture.render(posxbase + 280+XS, posybase + 558+YS);

			if (capmore+1 <= maxcapit) {
				gTextTexture.loadFromRenderedText(gFont3,  std::to_string(capmore+1), textColor);
				gTextTexture.render(posxbase + 345+XS, posybase + 558+YS);
			}
			if (capmore+2 <= maxcapit) {
				gTextTexture.loadFromRenderedText(gFont3,  std::to_string(capmore+2), textColor);
				gTextTexture.render(posxbase + 410+XS, posybase + 558+YS);
			}

			if (maxcapit >= 10){
				DrawImageFile(gRenderer,"romfs:/buttons/UP.png",300+XS, 530+YS,"+10");
				DrawImageFile(gRenderer,"romfs:/buttons/DOWN.png",300+XS, 630+YS,"-10");
			}
			if (capmore > 1) DrawImageFile(gRenderer,"romfs:/buttons/LEFT.png",100+XS, 580+YS,std::to_string(mincapit));
			if (capmore < maxcapit)DrawImageFile(gRenderer,"romfs:/buttons/RIGHT.png",500+XS, 580+YS,std::to_string(maxcapit));
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
		
		//Draw Footer Buttons
		int dist = 1120,posdist = 130;
		DrawImageFile(gRenderer,"romfs:/buttons/A.png",dist, 680,"Ver Online");dist -= posdist;
		DrawImageFile(gRenderer,"romfs:/buttons/B.png",dist, 680,"Atras");dist -= posdist;
		DrawImageFile(gRenderer,"romfs:/buttons/L.png",dist, 680,"AnimeFLV");dist -= posdist;
		DrawImageFile(gRenderer,"romfs:/buttons/L.png",dist, 680,"AnimeFLV");dist -= posdist;
		if(!txtyase.length()){
			DrawImageFile(gRenderer,"romfs:/buttons/Y.png",dist, 680,"Favorito");dist -= posdist;
		}else {
			DrawImageFile(gRenderer,"romfs:/buttons/FAV.png",dist, 680,"Favorito");dist -= posdist;
		}
		DrawImageFile(gRenderer,"romfs:/buttons/MINUS.png",dist, 680,"Música");dist -= posdist;
		DrawImageFile(gRenderer,"romfs:/buttons/PLUS.png",dist, 680,"Salir");dist -= posdist;

		}

		break;


		case programationstate:
			if (reloading == false) {
				{//Draw a rectagle to a nice view
				SDL_SetRenderDrawColor(gRenderer, 200, 200, 200, 105);
				SDL_Rect HeaderRect = {0,0, 620, 670};
				SDL_RenderFillRect(gRenderer, &HeaderRect);}
				
				for (int x = 0; x < (int)arraychapter.size(); x++) {
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
					
					{int ajuX = -390, ajuY = -450;
					SDL_Rect fillRect = { xdistance + 18 +ajuX, ydistance + 8  + ajuY, sizeportraity + 4, sizeportraitx + 4};
					SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 200);

					SDL_RenderFillRect(gRenderer, &fillRect);
					TPreview.render(posxbase + xdistance + ajuX, posybase + ydistance + ajuY);
					}
				}
				
				//Draw footer buttons
				int dist = 1100,posdist = 160;;
				DrawImageFile(gRenderer,"romfs:/buttons/A.png",dist, 680,"Aceptar");dist -= posdist;
				DrawImageFile(gRenderer,"romfs:/buttons/R.png",dist, 680,"Buscar");dist -= posdist;
				DrawImageFile(gRenderer,"romfs:/buttons/L.png",dist, 680,"AnimeFLV");dist -= posdist;
				DrawImageFile(gRenderer,"romfs:/buttons/ZR.png",dist, 680,"Favoritos");dist -= posdist;
				DrawImageFile(gRenderer,"romfs:/buttons/MINUS.png",dist, 680,"Música");dist -= posdist;
				DrawImageFile(gRenderer,"romfs:/buttons/PLUS.png",dist, 680,"Salir");dist -= posdist;
				/*{
				SDL_Rect fillRect = { 0, SCREEN_HEIGHT - 35, 1280, 25 };
				SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

				SDL_RenderFillRect(gRenderer, &fillRect); 
				
				gTextTexture.loadFromRenderedText(gFont, "\"A\" para Confirmar - \"R\" para Buscar - \"L\" para AnimeFLV -  \"ZR\" para Favoritos.", textColor);
				gTextTexture.render(posxbase, SCREEN_HEIGHT - 30);
				}*/

			
				//Draw Header
				gTextTexture.loadFromRenderedText(gFont, "(Ver 1.8) #KASTXUPALO", {100,0,0});
				gTextTexture.render(SCREEN_WIDTH - gTextTexture.getWidth() - 30, 20);




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
				{//Draw a rectagle to a nice view
				SDL_SetRenderDrawColor(gRenderer, 200, 200, 100, 105);
				SDL_Rect HeaderRect = {0,0, 620, 670};
				SDL_RenderFillRect(gRenderer, &HeaderRect);}

				for (int x = 0; x < (int)arraysearch.size(); x++) {
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
					{int ajuX = -390, ajuY = -450;
					SDL_Rect fillRect = { xdistance + 18 +ajuX, ydistance + 8  + ajuY, sizeportraity + 4, sizeportraitx + 4};
					SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 200);

					SDL_RenderFillRect(gRenderer, &fillRect);
					TSearchPreview.render(posxbase + xdistance + ajuX, posybase + ydistance + ajuY);}
					
				}

				
				{//Draw footer buttons
				int dist = 1100,posdist = 160;
				DrawImageFile(gRenderer,"romfs:/buttons/A.png",dist, 680,"Aceptar");dist -= posdist;
				DrawImageFile(gRenderer,"romfs:/buttons/B.png",dist, 680,"Volver");dist -= posdist;
				DrawImageFile(gRenderer,"romfs:/buttons/MINUS.png",dist, 680,"Música");dist -= posdist;
				DrawImageFile(gRenderer,"romfs:/buttons/PLUS.png",dist, 680,"Salir");dist -= posdist;}
				
				break;

		case favoritesstate:
				{//Draw a rectagle to a nice view
				SDL_SetRenderDrawColor(gRenderer, 200, 100, 200, 105);
				SDL_Rect HeaderRect = {0,0, 620, 670};
				SDL_RenderFillRect(gRenderer, &HeaderRect);}
			for (int x = 0; x < (int)arrayfavorites.size(); x++) {
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

				{//Draw footer buttons
				int dist = 1100,posdist = 160;
				DrawImageFile(gRenderer,"romfs:/buttons/A.png",dist, 680,"Aceptar");dist -= posdist;
				DrawImageFile(gRenderer,"romfs:/buttons/B.png",dist, 680,"Volver");dist -= posdist;
				DrawImageFile(gRenderer,"romfs:/buttons/MINUS.png",dist, 680,"Música");dist -= posdist;
				DrawImageFile(gRenderer,"romfs:/buttons/PLUS.png",dist, 680,"Salir");dist -= posdist;}
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
		SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);//enable alpha blend
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
	__nx_applet_exit_mode = 1;
#endif // SWITCH



	close();

	return 0;

}