#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <SDL_mixer.h>
#include <errno.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <iostream>
#include<ctime>
#include <iomanip>
#include <math.h>
#include <Vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <thread>

void callimage(int pos,std::vector<std::string> imageV);

int downloadjkanimevideo(void* data);

int refrescarpro(void* data);
int MKcapitBuffer();
int MKfavimgfix(bool images);

int searchjk(void* data);

void PushDirBuffer(std::string a,std::string name);
int capit(void* data);
int capBuffer (std::string Tlink);

void addFavorite(std::string text);
void getFavorite();
bool isFavorite(std::string fav);
void delFavorite(int inst = -1);




