#include <iostream>
#include <string>
#include <math.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <iostream>
#include <Vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <switch.h>
#include <thread>
#include "utils.hpp"
#include "Networking.hpp"
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "applet.hpp"
#include "SDLWork.hpp"
extern SDLB GOD;

std::string scrapElement(std::string content, std::string get){
	int val1 = 0, val2 = 0;
	std::string Element = "";
	val1 = content.find(get);
	if (val1 != -1)
	{
		std::string elmetTMP = content.substr(val1-1, 1);
		replace(elmetTMP, ">","<");
		replace(elmetTMP, "{","}");
		replace(elmetTMP, "[","]");
		replace(elmetTMP, "(",")");
		val2 = content.find(elmetTMP, val1);

		Element = content.substr(val1, val2 - val1);
		replace(Element, "\\", "");

		std::cout << Element << std::endl;
	}
return Element;
}

bool onlinejkanimevideo(std::string onlineenlace,std::string server)
{
	std::string text = "Cargando... "+onlineenlace.substr(0,62)+"... desde "+server;
	
	replace(text,"https://jkanime.net/","");
	replace(text,"-"," ");
	replace(text,"/","");
	GOD.PleaseWait(text+"...");
	std::string videourl = "";
	std::string content = "";
	content = gethtml(onlineenlace);
	
	if (server == "Okru"){
		videourl = scrapElement(content,"https://jkanime.net/jkokru.php?");
		replace(videourl, "https://jkanime.net/jkokru.php?u=", "https://ok.ru/videoembed/");
	}
	if (server == "Desu"){
		videourl = scrapElement(content,"https://jkanime.net/um.php?");
	}
	if (server == "Fembed"){
		videourl = scrapElement(content,"https://jkanime.net/jkfembed.php?u=");
		replace(videourl, "https://jkanime.net/jkfembed.php?u=", "https://www.fembed.com/v/");
	}
	if (server == "Xtreme S"){
		videourl = scrapElement(content,"https://jkanime.net/jk.php?");
	}
	if (server == "MixDrop"){
		videourl = scrapElement(content,"https://jkanime.net/jkvmixdrop.php?u=");
		replace(videourl, "https://jkanime.net/jkvmixdrop.php?u=", "https://mixdrop.co/e/");
	}
	if (server == "Nozomi"){
		videourl = scrapElement(content,"https://jkanime.net/um2.php?");
	}
	if (server == "Mega"){
		videourl = scrapElement(content,"https://mega.nz/embed/");
	}

#ifdef __SWITCH__
if (videourl.length() != 0)
{
	WebBrowserCall(videourl);
	return true;
} else {
	std::cout << "Server no encontrado..." << onlineenlace << std::endl;
}
#endif //  SWITCH
return false;
}

bool isFileExist(std::string file)
{
	struct stat	st = { 0 };

	if (stat(file.c_str(), &st) == -1) {
		return (false);
	}

	return (true);
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

void touch(std::string route)
{
	std::ofstream outfile;
	outfile.open(route, std::ios_base::app);
	outfile.close();
}