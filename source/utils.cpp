
#include <string>
#include <math.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <Vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <switch.h>
#include <thread>
#include "utils.hpp"
#include "Networking.hpp"
#include "NozomiHacked.hpp"
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "applet.hpp"
#include "SDLWork.hpp"

extern SDLB GOD;
extern std::string serverenlace;

std::string scrapElement(std::string content, std::string get,std::string delim){
	std::string Element = "";
	if(content.length() <= 0)
	{return Element;}
	
	int val1 = 0, val2 = 0;
	val1 = content.find(get);
	
	if (val1 != -1)
	{
		std::string elmetTMP;
		if(delim.length())
			elmetTMP = delim;
		else{
			elmetTMP = content.substr(val1-1, 1);
			replace(elmetTMP, ">","<");
			replace(elmetTMP, "{","}");
			replace(elmetTMP, "[","]");
			replace(elmetTMP, "(",")");
		}
		val2 = content.find(elmetTMP, val1+get.length()+1);

		Element = content.substr(val1, val2 - val1);
		replace(Element, "\\", "");

		std::cout << Element << std::endl;
	}
return Element;
}

std::string MD_s(std::string code){
std::string decode;
while (true){
	decode = code;
	std::cout << decode << std::endl;
	std::string tempmedia = gethtml(decode);
	decode = scrapElement(tempmedia, "MDCore|","'");
	if(decode.length())
	{
		//clean  int val0 =0;
		std::string E = scrapElement(decode, "|161","|");
		replace(decode, "MDCore||s|", "https://s-|");
		replace(decode, "MDCore|s|", "https://s-|");
		replace(decode, "MDCore||", "https://a-|");
		replace(decode, "||s|", "|");
		replace(decode, "|s|", "|");
		replace(decode, "||", "|");
		replace(decode, "||", "|");
		replace(decode, "|vfile|vserver|remotesub|chromeInject|poster", "");
		replace(decode, "|thumbs", "");
		replace(decode, "|jpg", "");
		replace(decode, "|furl", "");
		replace(decode, "|wurl", "");
		
		replace(decode, "|M", "");
		replace(decode, "|nLhMBA", "");
		
		replace(decode, "|v", "");
		replace(decode, "|_t|16", "&e=16");
		replace(decode, "|mp4", "");
		replace(decode, "|net", "");
		replace(decode, "|jmkBVb", "");
		replace(decode, "|mxdcontent", "");
		replace(decode, "|referrer|", ".mp4?s=");
		if (E.length() > 3) replace(decode, E, "&e="+E.substr(1));

		//scrap important elements
		std::string dely = scrapElement(decode, "delivery");
		
		replace(decode, "|"+dely+"|", ""+dely+".mxdcontent.net/v/");
		replace(decode, "|16", "&_t=16");
		
		if(decode.find("|") > 0 && decode.rfind("|") < decode.length()){continue;}
		std::cout << decode << std::endl;
		break;
	} else {
		decode = "";
		break;
	}
}
return decode;
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
	std::string tempcon = "";
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
		//replace(videourl, "https://jkanime.net/jkfembed.php?u=", "https://www.fembed.com/v/");
	}
	if (server == "Xtreme S"){
		videourl = scrapElement(content,"https://jkanime.net/jk.php?");
		//replace(videourl, "https://jkanime.net/jk.php?u=", "https://jkanime.net/");
	}
	if (server == "MixDrop"){
		videourl = scrapElement(content,"https://jkanime.net/jkvmixdrop.php?u=");
		replace(videourl, "https://jkanime.net/jkvmixdrop.php?u=", "https://mixdrop.co/e/");
		if(videourl.length())
		{
			videourl=MD_s(videourl);
			std::cout << videourl << std::endl;
		}
	}
	if (server == "Nozomi"){
		videourl = scrapElement(content,"https://jkanime.net/um2.php?");
		videourl = Nozomi_Link(videourl);
	}
	if (server == "Mega"){
		videourl = scrapElement(content,"https://mega.nz/embed/");
	}
	std::cout << videourl << std::endl;
#ifdef __SWITCH__
if (videourl.length() != 0)
{
	WebBrowserCall(videourl);
	videourl = "";
	return true;
} else {
	std::cout << "Server no encontrado..." << onlineenlace << std::endl;
}
#endif //  SWITCH
return false;
}

bool linktodownoadjkanime(std::string urltodownload,std::string directorydownload)
{
	std::string videourl = "";
	std::string content = "";
	content = gethtml(urltodownload);



	videourl = scrapElement(content, "https://jkanime.net/um2.php?");
	if(videourl.length())
	{
		videourl = Nozomi_Link(videourl);
		if(videourl.length())
		{
			std::cout << videourl << std::endl;
			serverenlace = videourl;
			if(downloadfile(videourl, directorydownload)) return true;
		}
	}

	videourl = scrapElement(content, "https://www.mediafire.com/file/");
	if(videourl.length())
	{
		std::cout << videourl << std::endl;
		std::string tempmedia = gethtml(videourl);
		videourl = scrapElement(tempmedia, "https://download");
		if(videourl.length())
		{
			replace(videourl, "\\", "");
			std::cout << videourl << std::endl;
			serverenlace = videourl;
			if(downloadfile(videourl, directorydownload)) return true;
		}
	}

	videourl = scrapElement(content, "https://www24.zippyshare.com");
	if(videourl.length())
	{
		std::cout << videourl << std::endl;
		std::string tempmedia = gethtml(videourl);
		videourl = scrapElement(tempmedia, "https://www24.zippyshare.com/d");
		if(videourl.length())
		{
			replace(videourl, "\\", "");
			std::cout << videourl << std::endl;
			serverenlace = videourl;
			if(downloadfile(videourl, directorydownload)) return true;
		}
	}

	videourl = scrapElement(content, "https://jkanime.net/jk.php?");
	if(videourl.length())
	{
		replace(videourl, "\\", "");
		replace(videourl, "https://jkanime.net/jk.php?u=", "https://jkanime.net/");
		std::cout << videourl << std::endl;
		serverenlace = videourl;
		if(downloadfile(videourl, directorydownload)) return true;
	}

	videourl = scrapElement(content,"https://jkanime.net/jkvmixdrop.php?u=");
	if(videourl.length())
	{
		replace(videourl, "https://jkanime.net/jkvmixdrop.php?u=", "https://mixdrop.co/e/");
		std::cout << videourl << std::endl;			
		videourl=MD_s(videourl);
		std::cout << videourl << std::endl;
		serverenlace = videourl;
		if(downloadfile(videourl, directorydownload)) return true;
	}
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

bool copy_me(std::string origen, std::string destino) {
    if(isFileExist(origen))
	{
		printf("%s a %s\n",origen.c_str(),destino.c_str());
		std::string tempdest = destino+".tmp";
		std::ifstream source(origen, std::ios::binary);
		std::ofstream dest(tempdest, std::ios::binary);
		dest << source.rdbuf();
		source.close();
		dest.close();
		remove(destino.c_str());
		rename (tempdest.c_str(), destino.c_str());
		if(isFileExist(destino)) printf("%s a %s OK\n\n",origen.c_str(),destino.c_str());
		return true;
	}else{
		return false;
	}
return false;
}