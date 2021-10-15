#include <string>
#include <math.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <vector>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <switch.h>
#include <thread>
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "applet.hpp"
#include "utils.hpp"
#include "Networking.hpp"
#include "Link.hpp"
#include "SDLWork.hpp"

//Private Link decoders
string MD_s(string Link){
	replace(Link, "https://jkanime.net/jkvmixdrop.php?u=", "https://mixdrop.co/e/");
	string decode="";
	for (int i=1; i<6; i++) {
		decode = Link;
		cout << "----------------------------------------- "+to_string(i) << endl;
		cout << decode << endl;
		string tempmedia = Net::GET(decode);
		decode = scrapElement(tempmedia, "MDCore|","'");
		cout << decode << endl;
		if(decode.length())
		{
			vector<string> list = split (decode, "|");

			//get deliver and type
			string type;
			if (decode.find("|s|")) {type="s";} else {type="a";}
			string dely = scrapElement(decode, "delivery");

			//get file name and key
			string vidname = "";
			string playkey = "";
			for (u64 i=0; i < list.size(); i++) {
				if ( (list[i]).length() == 32) vidname = list[i];
				if ( (list[i]).length() == 22) playkey = list[i]; //some times get a split key,  fix that
			}
			//get num keys
			int v1=0;
			v1 = decode.find("|16");
			string numkey = decode.substr(v1+1,10);

			v1 = decode.find("|16",v1+10);
			string numkey2 = decode.substr(v1+1,10);

			//bypass, if not get the key repeat the process
			if(!vidname.length()||!playkey.length()) {decode=""; continue;}

			//model
			decode = "https://"+type+"-"+dely+".mxdcontent.net/v/"+vidname+".mp4?s="+playkey+"&e="+numkey+"&_t="+numkey2;//.substr(1)
			cout << decode << endl;
			break;
		} else {
			decode = "";
			break;
		}
	}
	return decode;
}
string Nozomi_Link(string Link){
	string codetemp;
	//Get FirstKey
	string FirstKey = Net::GET(Link);
	codetemp = scrapElement(FirstKey,"name=\"data\" value=\"", "\"");
	replace(codetemp,"name=\"data\" value=\"","");
	FirstKey = codetemp;
	cout << "FirstKey: "<< FirstKey << endl;
	//Get SecondKey
	string data = "data=" + FirstKey;
	string SecondKey = Net::REDIRECT("https://jkanime.net/gsplay/redirect_post.php",data);
	sleep(3);
	replace(SecondKey,"https://jkanime.net/gsplay/player.html#","");
	replace(SecondKey,"/gsplay/player.html#","");
	cout << "Secondkey: "<< SecondKey << endl;
	//Get ThirdKey
	string ThirdKey="";
	string second = "v=" + SecondKey;
	for (int i=1; i<5; i++) {
		cout << "-----------------------------------------> "+to_string(i) << endl;
		codetemp = Net::POST("https://jkanime.net/gsplay/api.php",second);
		
		//decode json
		json data;
		if(json::accept(codetemp))
		{
			data = json::parse(codetemp);
			if (!data["file"].empty())
			{
				ThirdKey = data["file"];
				replace(ThirdKey,"\r","");

				break;
			} else {
				SDL_Delay(1000);
			}
		}
	}
	cout << "Link: "<< ThirdKey << "-" << endl;
	return ThirdKey;
}

string Nozomi_player(string Link){
	string codetemp;
	//Get FirstKey
	string FirstKey = Net::GET(Link);
	codetemp = scrapElement(FirstKey,"name=\"data\" value=\"", "\"");
	replace(codetemp,"name=\"data\" value=\"","");
	FirstKey = codetemp;
	cout << "FirstKey: "<< FirstKey << endl;
	//Get SecondKey
	string data = "data=" + FirstKey;
	string SecondKey = Net::REDIRECT("https://jkanime.net/gsplay/redirect_post.php",data);
	
	return SecondKey;
}
string Fembed_Link(string Link) {
	string codetemp = "";
	replace(Link, "https://jkanime.net/jkfembed.php?u=", "https://www.fembed.com/api/source/");
	
	cout << "enlace: " << Link << endl;

	//POST to api
	string videojson = Net::POST(Link);
	//cout << "Json720key: " << videojson << endl;

	//decode json
	json data;
	if(json::accept(videojson))
	{
		data = json::parse(videojson);
		int sz = data["data"].size()-1;
		cout << "num: " << sz << endl;
		if(sz > 0){
			if (!data["data"][sz]["file"].empty()){
				codetemp = data["data"][sz]["file"];
			}
		}
	}
	return codetemp;
}

//servers
std::vector<std::string> arrayserversbak = {
	"Nozomi","Fembed 2.0","MixDrop","Desu","Xtreme S","Okru"
};
std::vector<std::string> arrayservers = arrayserversbak;

bool onlinejkanimevideo(string onlineenlace,string server){
	bool autoplay = true;
	string videourl = "";
	string content = "";
	string tempcon = "";
	content = Net::GET(onlineenlace);
	if (server == "Fembed 2.0") {
		videourl = scrapElement(content, "https://jkanime.net/jkfembed.php?u=");
		if(videourl.length()){
			tempcon = Fembed_Link(videourl);
			if(tempcon.length()){videourl=tempcon;}
		}
	}
	if (server == "Nozomi") {
		videourl = scrapElement(content,"https://jkanime.net/um2.php?");
		if(videourl.length()){

			tempcon = Nozomi_player(videourl);

			if(tempcon.length()){videourl=tempcon;
			autoplay = false;
			}
		}
	}
	if (server == "MixDrop") {
		videourl = scrapElement(content,"https://jkanime.net/jkvmixdrop.php?u=");
		if(videourl.length()){
			tempcon=MD_s(videourl);
			if(tempcon.length()){videourl=tempcon;}
		}
	}
	if (server == "Okru") {
		videourl = scrapElement(content,"https://jkanime.net/jkokru.php?");
		replace(videourl, "https://jkanime.net/jkokru.php?u=", "https://ok.ru/videoembed/");
	}
	if (server == "Desu") {
		videourl = scrapElement(content,"https://jkanime.net/um.php?");
	}
	if (server == "Fembed") {
		videourl = scrapElement(content,"https://jkanime.net/jkfembed.php?u=");
	}
	if (server == "Xtreme S") {
		videourl = scrapElement(content,"https://jkanime.net/jk.php?");
	}
	if (server == "Mega") {
		videourl = scrapElement(content,"https://mega.nz/embed/");
	}
	cout << videourl << endl;
	if (videourl.length() != 0)
	{
		if (autoplay == true)
		WebBrowserCall(videourl);
		else
		WebBrowserforNozomi(videourl);
		videourl = "";
		return true;
	} else {
		cout << "Server no encontrado..." << onlineenlace << endl;
	}
	return false;
}
bool linktodownoadjkanime(string urltodownload,string directorydownload) {
	string videourl = "";
	string content = "";
	content = Net::GET(urltodownload);

	videourl = scrapElement(content, "https://jkanime.net/um2.php?");
	if(videourl.length())
	{
		videourl = Nozomi_Link(videourl);
		if(videourl.length())
		{
			cout << videourl << endl;
			serverenlace = videourl;
			if(Net::DOWNLOAD(videourl, directorydownload)) return true;
			if(cancelcurl == 1) return false;
		}
	}

	videourl = scrapElement(content, "https://jkanime.net/jkfembed.php?u=");
	if (videourl.length())
	{
		videourl = Fembed_Link(videourl);
		if (videourl.length())
		{
			cout << videourl << endl;
			serverenlace = videourl;
			if (Net::DOWNLOAD(videourl, directorydownload)) return true;
			if(cancelcurl == 1) return false;
		}
	}

	videourl = scrapElement(content, "https://www.mediafire.com/file/");
	if(videourl.length())
	{
		cout << videourl << endl;
		string tempmedia = Net::GET(videourl);
		videourl = scrapElement(tempmedia, "https://download");
		if(videourl.length())
		{
			replace(videourl, "\\", "");
			cout << videourl << endl;
			serverenlace = videourl;
			if(Net::DOWNLOAD(videourl, directorydownload)) return true;
			if(cancelcurl == 1) return false;
		}
	}

	videourl = scrapElement(content,"https://jkanime.net/jkvmixdrop.php?u=");
	if(videourl.length())
	{
		cout << videourl << endl;
		videourl=MD_s(videourl);
		if (videourl.length())
		{
			cout << videourl << endl;
			serverenlace = videourl;
			if(Net::DOWNLOAD(videourl, directorydownload)) return true;
			if(cancelcurl == 1) return false;
		}
	}

	videourl = scrapElement(content, "https://www24.zippyshare.com");
	if(videourl.length())
	{
		cout << videourl << endl;
		string tempmedia = Net::GET(videourl);
		videourl = scrapElement(tempmedia, "https://www24.zippyshare.com/d");
		if(videourl.length())
		{
			replace(videourl, "\\", "");
			cout << videourl << endl;
			serverenlace = videourl;
			if(Net::DOWNLOAD(videourl, directorydownload)) return true;
			if(cancelcurl == 1) return false;
		}
	}

	videourl = scrapElement(content, "https://jkanime.net/jk.php?");
	if(videourl.length())
	{
		replace(videourl, "\\", "");
		replace(videourl, "https://jkanime.net/jk.php?u=", "https://jkanime.net/");
		cout << videourl << endl;
		serverenlace = videourl;
		if(Net::DOWNLOAD(videourl, directorydownload)) return true;
		if(cancelcurl == 1) return false;
	}

	return false;
}
