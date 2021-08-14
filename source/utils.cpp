
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
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "applet.hpp"
#include "SDLWork.hpp"

extern SDLB GOD;
extern std::string serverenlace;
using namespace std;

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

		//std::cout << Element << std::endl;
	}
return Element;
}
vector<string> scrapElementAll(std::string content, std::string get,std::string delim){
	vector<string> res;
	std::string Element = "";
	if(content.length() <= 0)
	{return res;}

	int val1 = 0, val2 = 0;
	while (true){
		val1 = content.find(get,val1);
		
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
			res.push_back (Element);
			val1 = val1 + Element.length();
		} else {
			if (res.size() == 0)
				res.push_back ("");
			break;
		}
	}
std::cout << "Vector Size: " << std::to_string(res.size()) << std::endl;
return res;
}

// for string delimiter
vector<string> split (string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

//Mixdrop link decode
std::string MD_s(std::string code){
	std::string decode="";
	for (int i=1; i<5;i++){
		decode = code;
		std::cout << "----------------------------------------- "+std::to_string(i) << std::endl;
		std::cout << decode << std::endl;
		std::string tempmedia = gethtml(decode);
		decode = scrapElement(tempmedia, "MDCore|","'");
		std::cout << decode << std::endl;
		if(decode.length())
		{
			std::vector<std::string> list = split (decode, "|");
			
			//get deliver and type
			std::string type;
			if (decode.find("|s|")){type="s";} else {type="a";}
			std::string dely = scrapElement(decode, "delivery");
			
			//get file name and key
			std::string vidname = "";
			std::string playkey = "";
			for (u64 i=0;i < list.size();i++){
				if ( (list[i]).length() == 32) vidname = list[i];
				if ( (list[i]).length() == 22) playkey = list[i];//some times get a split key,  fix that 
			}
			//get num keys
			int v1=0;
			v1 = decode.find("|16");
			std::string numkey = decode.substr(v1+1,10);
			
			v1 = decode.find("|16",v1+10);
			std::string numkey2 = decode.substr(v1+1,10);
			
			//bypass, if not get the key repeat the process
			if(!vidname.length()||!playkey.length()){decode="";continue;}

			//model
			decode = "https://"+type+"-"+dely+".mxdcontent.net/v/"+vidname+".mp4?s="+playkey+"&e="+numkey+"&_t="+numkey2;//.substr(1)
			std::cout << decode << std::endl;
			break;
		} else {
			decode = "";
			break;
		}
	}
return decode;
}
std::string Nozomi_Link(std::string Link){
	std::string codetemp;
	//Get FirstKey
	std::string FirstKey = gethtml(Link);
	codetemp = scrapElement(FirstKey,"name=\"data\" value=\"", "\"");
	replace(codetemp,"name=\"data\" value=\"","");
	FirstKey = codetemp;
	std::cout << "FirstKey: "<< FirstKey << std::endl;
	//Get SecondKey
	std::string data = "data=" + FirstKey;
	std::string SecondKey = gethtml("https://jkanime.net/gsplay/redirect_post.php",data,true);
	replace(SecondKey,"https://jkanime.net/gsplay/player.html#","");
	std::cout << "Secondkey: "<< SecondKey << std::endl;
	//Get ThirdKey
	std::string ThirdKey="";
	std::string second = "v=" + SecondKey;
	for (int i=1; i<5;i++){
		std::cout << "----------------------------------------- "+std::to_string(i) << std::endl;
		ThirdKey = gethtml("https://jkanime.net/gsplay/api.php",second);
		//decode json
		json data;
		if(json::accept(ThirdKey))
		{
			data = json::parse(ThirdKey);
			if (!data["file"].empty())
			{
				codetemp = data["file"];
				break;
			} else {
				SDL_Delay(1000);
			}
		}
	}
	ThirdKey = codetemp;
	std::cout << "ThirdKey: "<< ThirdKey << std::endl;
	return ThirdKey;
}
std::string Fembed_Link(std::string Link) {
	std::string codetemp = "";
	if (Link.length() > 0){
		replace(Link, "https://jkanime.net/jkfembed.php?u=", "https://www.fembed.com/api/source/");
		//std::cout << "enlace: " << Link << std::endl;
		
		//POST to api
		std::string videojson = gethtml(Link, "0");
		//std::cout << "Json720key: " << videojson << std::endl;
		
		//decode json
		json data;
		if(json::accept(videojson))
		{
			data = json::parse(videojson);
			if (!data["data"][1]["file"].empty())
				codetemp = data["data"][1]["file"];
		}
	}
	return codetemp;
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
	if (server == "Fembed 2.0") {
		videourl = scrapElement(content, "https://jkanime.net/jkfembed.php?u=");
		videourl = Fembed_Link(videourl);
	} else if (server == "Nozomi"){
		videourl = scrapElement(content,"https://jkanime.net/um2.php?");
		videourl = Nozomi_Link(videourl);
	} else if (server == "Okru"){
		videourl = scrapElement(content,"https://jkanime.net/jkokru.php?");
		replace(videourl, "https://jkanime.net/jkokru.php?u=", "https://ok.ru/videoembed/");
	} else if (server == "Desu"){
		videourl = scrapElement(content,"https://jkanime.net/um.php?");
	} else if (server == "Fembed"){
		videourl = scrapElement(content,"https://jkanime.net/jkfembed.php?u=");
		//replace(videourl, "https://jkanime.net/jkfembed.php?u=", "https://www.fembed.com/v/");
	} else if (server == "Xtreme S"){
		videourl = scrapElement(content,"https://jkanime.net/jk.php?");
		//replace(videourl, "https://jkanime.net/jk.php?u=", "https://jkanime.net/");
	} else if (server == "MixDrop"){
		videourl = scrapElement(content,"https://jkanime.net/jkvmixdrop.php?u=");
		replace(videourl, "https://jkanime.net/jkvmixdrop.php?u=", "https://mixdrop.co/e/");
		if(videourl.length())
		{
			videourl=MD_s(videourl);
		}
	} else if (server == "Local"){
		LoadNRO("sdmc:/switch/pplay/pplay.nro");
		return false;
	} else if (server == "Mega"){
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

bool linktodownoadjkanime(std::string urltodownload,std::string directorydownload) {
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

	videourl = scrapElement(content, "https://jkanime.net/jkfembed.php?u=");
	if (videourl.length())
	{
		videourl = Fembed_Link(videourl);
		if (videourl.length())
		{
			std::cout << videourl << std::endl;
			serverenlace = videourl;
			if (downloadfile(videourl, directorydownload)) return true;
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
bool write_DB(json base,std::string path){
	appletBeginBlockingHomeButton (0);
	std::ofstream otf(path);
	otf << std::setw(4) << base << std::endl;
	otf.close();
	appletEndBlockingHomeButton();
	std::cout << "Json: writhen..." << std::endl;

	return true;
}