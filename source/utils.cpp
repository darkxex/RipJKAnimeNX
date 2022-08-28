
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
#include "utils.hpp"
#include "Networking.hpp"
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "applet.hpp"
#include "SDLWork.hpp"

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
vector<string> scrapElementAll(std::string content, std::string get,std::string delim,std::string addend){
	vector<string> res;
	std::string Element = "";
	if(content.length() <= 0)
	{return res;}

	int val1 = 0, val2 = 0;
	while (true) {
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
			val1 = val1 + Element.length();
			replace(Element, "\\", "");
			replace(Element, "\n", "");
			replace(Element, "<span>", "");
			replace(Element, "<h2>", "");
			replace(Element, "</h2>", "");
			//doit better
			replace(Element, "  ", " ");
			replace(Element, "  ", " ");
			replace(Element, "  ", " ");
			replace(Element, "  ", " ");
			replace(Element, "  ", " ");
			replace(Element, "  ", " ");


			res.push_back (Element+addend);
		} else {

			break;
		}
	}
//std::cout << "Vector Size: " << std::to_string(res.size()) << std::endl;
	return res;
}
vector<string> split (string s, string delimiter) {// for string delimiter
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	string token;
	vector<string> res;

	while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
		token = s.substr (pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		if(token.length() > 0)
			res.push_back (token);
	}
	string token2 = (s.substr (pos_start));
	if(token2.length() > 0)
		res.push_back (token2);
	return res;
}

bool erase(string file) {
	if(isFileExist(file)) {
		remove((file).c_str());
		return true;
	}
	return false;
}
bool isFileExist(std::string file){
	struct stat st = { 0 };

	if (stat(file.c_str(), &st) == -1) {
		return (false);
	}

	return (true);
}
void replace(std::string& subject, const std::string& search, const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}
void mayus(std::string &s){
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
void RemoveAccents(std::string& word){
        replace(word, "à","á");
        replace(word, "è","é");
        replace(word, "ì","í");
        replace(word, "ò","ó");
        replace(word, "ù","ú");
        replace(word, "À","Á");
        replace(word, "È","É");
        replace(word, "Ì","Í");
        replace(word, "Ò","Ó");
        replace(word, "Ù","Ú");
/*
        //std::cout << word << std::endl;
        replace(word, "á","a");
        replace(word, "é","e");
        replace(word, "í","i");
        replace(word, "ó","o");
        replace(word, "ú","u");
        replace(word, "ñ","n");

        replace(word, "Á","A");
        replace(word, "É","E");
        replace(word, "Í","I");
        replace(word, "Ó","O");
        replace(word, "Ú","U");
        replace(word, "Ñ","N");
        
        replace(word, "¡","");
        replace(word, "!","");
        replace(word, "?","");
        replace(word, "¿","");
 */
	replace(word, "&amp;","");
	replace(word, "#8230;","");
	replace(word, "”","");
	replace(word, "“","");
	replace(word, "\n","");


}
void touch(std::string route){
	std::ofstream outfile;
	outfile.open(route, std::ios_base::app);
	outfile.close();
}
bool copy_me(std::string origen, std::string destino) {
	if(isFileExist(origen))
	{
		cout << "# copy "<< origen << " >> " << destino;
		std::string tempdest = destino+".tmp";
		std::ifstream source(origen, std::ios::binary);
		std::ofstream dest(tempdest, std::ios::binary);
		dest << source.rdbuf();
		source.close();
		dest.close();
		remove(destino.c_str());
		rename (tempdest.c_str(), destino.c_str());
		if(isFileExist(destino)) cout << "OK" << endl; else cout << "KO" << endl;
		return true;
	}else{
		return false;
	}
	return false;
}
bool read_DB(json& base,std::string path){
	std::ifstream inf(path);
	if(!inf.fail()) {
		std::string tempjson="";
		for(int f = 0; !inf.eof(); f++)
		{
			string TempLine = "";
			getline(inf, TempLine);
			tempjson += TempLine;
		}
		inf.close();
		if(json::accept(tempjson))
		{
			//Parse and use the JSON data
			base = json::parse(tempjson);
			std::cout << "Json Readed... "<< path << std::endl;
			return true;
		}
	}
	return false;
}
bool write_DB(json base,std::string path){
	std::string pathtemp = path+".tmp";
	std::string pathback = path+".bck";
	std::string type = path.substr(0,4);
	std::stringstream strm;
	try{
		strm << std::setw(4) << base;
//		strm << base;
		std::string A = strm.str();

		std::ofstream otf(pathtemp);
		otf << A;
		otf.close();
		remove(path.c_str());
		rename(pathtemp.c_str(),path.c_str());

		//commit
		if (type == "save") user::commit();
		if (type == "emmc") emmc::commit();
	} catch(...) {
		LOG::E(14);
		std::cout << "Json: write Error... "<< path << std::endl;
		return false;
	}
	std::cout << "Json: writhen... "<< path << std::endl;
	return true;
}

void led_on(int inter){
	// Configure our supported input layout: a single player with standard controller styles

	Result rc=0;
	s32 i;
	s32 total_entries;
	HidsysUniquePadId unique_pad_ids[2]={0};
	HidsysNotificationLedPattern pattern;

	rc = hidsysInitialize();
	if (R_FAILED(rc)) {
		printf("hidsysInitialize(): 0x%x\n", rc);
	}
	// Scan the gamepad. This should be done once for each frame
	PadState pad;
	padInitializeDefault(&pad);
	padUpdate(&pad);

	// padGetButtonsDown returns the set of buttons that have been
	// newly pressed in this frame compared to the previous one
	//u64 kDown = padGetButtonsDown(&pad);

	switch(inter)
	{
	case 1:

		memset(&pattern, 0, sizeof(pattern));
		// Setup Breathing effect pattern data.
		pattern.baseMiniCycleDuration = 0x8;                             // 100ms.
		pattern.totalMiniCycles = 0x2;                                   // 3 mini cycles. Last one 12.5ms.
		pattern.totalFullCycles = 0x0;                                   // Repeat forever.
		pattern.startIntensity = 0x2;                                    // 13%.

		pattern.miniCycles[0].ledIntensity = 0xF;                        // 100%.
		pattern.miniCycles[0].transitionSteps = 0xF;                     // 15 steps. Transition time 1.5s.
		pattern.miniCycles[0].finalStepDuration = 0x0;                   // Forced 12.5ms.
		pattern.miniCycles[1].ledIntensity = 0x2;                        // 13%.
		pattern.miniCycles[1].transitionSteps = 0xF;                     // 15 steps. Transition time 1.5s.
		pattern.miniCycles[1].finalStepDuration = 0x0;                   // Forced 12.5ms.

		break;


	case 2:
		memset(&pattern, 0, sizeof(pattern));
		// Setup Heartbeat effect pattern data.
		pattern.baseMiniCycleDuration = 0x1;                             // 12.5ms.
		pattern.totalMiniCycles = 0xF;                                   // 16 mini cycles.
		pattern.totalFullCycles = 0x0;                                   // Repeat forever.
		pattern.startIntensity = 0x0;                                    // 0%.

		// First beat.
		pattern.miniCycles[0].ledIntensity = 0xF;                        // 100%.
		pattern.miniCycles[0].transitionSteps = 0xF;                     // 15 steps. Total 187.5ms.
		pattern.miniCycles[0].finalStepDuration = 0x0;                   // Forced 12.5ms.
		pattern.miniCycles[1].ledIntensity = 0x0;                        // 0%.
		pattern.miniCycles[1].transitionSteps = 0xF;                     // 15 steps. Total 187.5ms.
		pattern.miniCycles[1].finalStepDuration = 0x0;                   // Forced 12.5ms.

		// Second beat.
		pattern.miniCycles[2].ledIntensity = 0xF;
		pattern.miniCycles[2].transitionSteps = 0xF;
		pattern.miniCycles[2].finalStepDuration = 0x0;
		pattern.miniCycles[3].ledIntensity = 0x0;
		pattern.miniCycles[3].transitionSteps = 0xF;
		pattern.miniCycles[3].finalStepDuration = 0x0;

		// Led off wait time.
		for(i=2; i<4; i++) {
			pattern.miniCycles[i].ledIntensity = 0x0;                        // 0%.
			pattern.miniCycles[i].transitionSteps = 0xF;                     // 15 steps. Total 187.5ms.
			pattern.miniCycles[i].finalStepDuration = 0xF;                   // 187.5ms.
		}
		break;

	case 3:
		memset(&pattern, 0, sizeof(pattern));
		// Setup Beacon effect pattern data.
		pattern.baseMiniCycleDuration = 0x1;                             // 12.5ms.
		pattern.totalMiniCycles = 0xF;                                   // 16 mini cycles.
		pattern.totalFullCycles = 0x0;                                   // Repeat forever.
		pattern.startIntensity = 0x0;                                    // 0%.

		// First beat.
		pattern.miniCycles[0].ledIntensity = 0xF;                        // 100%.
		pattern.miniCycles[0].transitionSteps = 0xF;                     // 15 steps. Total 187.5ms.
		pattern.miniCycles[0].finalStepDuration = 0x0;                   // Forced 12.5ms.
		pattern.miniCycles[1].ledIntensity = 0x0;                        // 0%.
		pattern.miniCycles[1].transitionSteps = 0xF;                     // 15 steps. Total 187.5ms.
		pattern.miniCycles[1].finalStepDuration = 0x0;                   // Forced 12.5ms.

		// Led off wait time.
		for(i=4; i<4; i++) {
			pattern.miniCycles[i].ledIntensity = 0x0;                        // 0%.
			pattern.miniCycles[i].transitionSteps = 0xF;                     // 15 steps. Total 187.5ms.
			pattern.miniCycles[i].finalStepDuration = 0xF;                   // 187.5ms.
		}
		break;
	case 0:
	default:
		memset(&pattern, 0, sizeof(pattern));
		break;
	}

	total_entries = 0;
	memset(unique_pad_ids, 0, sizeof(unique_pad_ids));

	// Get the UniquePadIds for the specified controller, which will then be used with hidsysSetNotificationLedPattern*.
	// If you want to get the UniquePadIds for all controllers, you can use hidsysGetUniquePadIds instead.
	rc = hidsysGetUniquePadsFromNpad(padIsHandheld(&pad) ? HidNpadIdType_Handheld : HidNpadIdType_No1, unique_pad_ids, 2, &total_entries);
	//printf("hidsysGetUniquePadsFromNpad(): 0x%x", rc);
	if (!R_SUCCEEDED(rc)) printf(",error %d", total_entries);
	// printf("\n");

	if (R_SUCCEEDED(rc)) {
		for(i=0; i<total_entries; i++) { // System will skip sending the subcommand to controllers where this isn't available.
			// printf("[%d] = 0x%lx ", i, unique_pad_ids[i].id);

			// Attempt to use hidsysSetNotificationLedPatternWithTimeout first with a 2 second timeout, then fallback to hidsysSetNotificationLedPattern on failure. See hidsys.h for the requirements for using these.
			rc = hidsysSetNotificationLedPatternWithTimeout(&pattern, unique_pad_ids[i], 100000000000ULL);
			//printf("hidsysSetNotificationLedPatternWithTimeout(): 0x%x\n", rc);
			if (R_FAILED(rc)) {
				rc = hidsysSetNotificationLedPattern(&pattern, unique_pad_ids[i]);
				// printf("hidsysSetNotificationLedPattern(): 0x%x\n", rc);
			}
		}
	}

}

bool DoubleKill(vector<string>& data) {
	vector<string> chap = data;
	u64 sizecap = chap.size();
	if (sizecap == 0) {return false;}
	vector<string> next;
	u64 i=0;
	//for(i=0; i < sizecap; i++){
	while(chap.size() > 0 && !quit) {
		string anime = chap[i];
		int v2 = anime.find("/", 20);
		string serie = anime.substr(0, v2 + 1);
		chap = eraseVec(chap,serie);
		next.push_back(anime);
		sizecap = chap.size();
		//cout << " "+serie+" - size: "<< chap.size() << endl;
	}
	if (sizecap == next.size()) {return false;}
	data = next;
	return true;
}


std::vector<std::string> eraseVec(std::vector<std::string> array,std::string patther){
	std::vector<std::string> array2={};
	for (int x=0; x < (int)array.size(); x++) {
		std::string a=array[x];
		if (patther != a.substr(0,patther.length())) {
			array2.push_back(a);
		}
	}
	return array2;
}
bool onTimeC(u64 sec,u64& time8){
	struct timeval time_now {};
	gettimeofday(&time_now, nullptr);
	time_t msecs_time = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);

	u64 time1 = msecs_time;
//	if (time1<0){time1=-time1;}
	//static int time8 = msecs_time;
	if (time1 > time8+sec) {
		time8 = time1;
		return true;
	}
	return false;
}
bool inTimeN(u64 sec,int framesdelay){
	static json data;
	string name = to_string(sec);

	if (data[name]["frames"].is_null()) {
		data[name]["frames"]=0;
	}

	//frames delay
	//uncoment to debug
	if (framesdelay == 0 || data[name]["frames"].get<int>() > framesdelay) {
		data[name]["frames"]=0;

		struct timeval time_now {};
		gettimeofday(&time_now, nullptr);
		time_t msecs_time = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);


		if (data[name]["time"].is_null()) {
			data[name]["time"]=msecs_time;
		} else{
			u64 time1 = msecs_time;
			u64 time2 = data[name]["time"].get<u64>()+sec;
			//if (framesdelay != 0) std::cout << std::setw(4) << data << std::endl;//uncoment to debug

			if (time1 > time2) {
				data[name]["time"] = time1;
				//if (framesdelay != 0)std::cout << name << " true" << std::endl;//uncoment to debug
				return true;
			} else {
				//if (framesdelay != 0)std::cout << name << " false " << time2 << " - "<< time1 << " = " << time2-time1 << std::endl;//uncoment to debug
			}
		}
	}
	data[name]["frames"]= data[name]["frames"].get<int>()+1;
	return false;
}
void TickerName(int& color,int sec,int min,int max){
	static bool running=false;
	static bool Start=false;

	static u64 time2 = 0;
	static int increment = 5;
	if (color < 0) {
		color=0;
		running=false;
		Start=true;
	}
	if(!running) {
		//delay
		if (onTimeC(sec*increment,time2))
		{
			//cout << "<< "  << color << endl;
			if (color > 0 || Start) {
				color=0;
				increment=13;
				Start=false;
			} else {
				running=true;
			}
		}
	} else {
		//running time
		if (onTimeC(sec,time2))
		{
			//cout << ">> "  << color << endl;
			color+=1;
			if(color >= max) {
				color = max;
				increment=8;
				running=false;
			}
		}
	}


/*
                        color-=1;
                        if(color < min){
                                color=0;
                                running=false;
                        }

                } else {
 */
}
std::string string_to_hex(const std::string& in) {
	std::stringstream ss;

	ss << std::hex << std::setfill('0');
	for (size_t i = 0; in.length() > i; ++i) {
		ss << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(in[i]));
	}
	return ss.str();
}

void NameOfLink(std::string& word){
	replace(word, "https://jkanime.net/", "");
	word = word.substr(0, word.length() - 1);
	replace(word, "/", " #");
	replace(word, "-", " ");
	mayus(word);
}
std::string KeyOfLink(std::string word){
	int v2 = word.find("/", 20);
	if (v2) {
		word = word.substr(0, v2 + 1);
		replace(word, "https://jkanime.net/", "");
		replace(word, "/", "");
		replace(word, " ", "");
	}
	return word;
}
template <typename Map, typename F>
void map_erase_if(Map& m, F pred){
	for (typename Map::iterator i = m.begin();
	     (i = std::find_if(i, m.end(), pred)) != m.end();
	     m.erase(i++));
}

bool isset(json& data,string key){
	if (data[key].is_null()) {
		data.erase(key);
		return false;
	}
	return true;
}

json StreamDev(string data){//Esto es para obtener valores de un json por red y modificar tamaños de interfaz
    json base;
    //cout << "GET -" << data << "-" << std::endl;
    string content=Net::GET(data);//cout <<content << std::endl;
    
    base = "{\"X\":0,\"Y\":0,\"H\":0,\"W\":0,\"A\":0,\"B\":0,\"Z\":0}"_json;
    if(json::accept(content))
    {
        //Parse and use the JSON data
        base = json::parse(content);
        std::cout << "Json Readed Dev... "<< data << std::endl;
	}
    cout << std::setw(4) << base << std::endl;
    return base;
}

namespace LOG {
streambuf* stream_buffer_cout;
streambuf* stream_buffer_cin;
std::stringstream redirectStream;
fstream fileLog;
bool HasError = false;
int ErrorCode = 0;

bool Logs2File(){
    return (DInfo()["config"]["Logs2File"] == 1);
}

bool MLOG(){
    if (Logs2File()) return true;
    return (DInfo()["TID"] == "05B9DB505ABBE000");
}

void init(){
    if (MLOG()) {
        // Backup streambuffers of  cout
        stream_buffer_cout = cout.rdbuf();
        stream_buffer_cin = cin.rdbuf();
        if (Logs2File()){
            //Save logs to File
            Files();
        } else {
            //Save logs to memory
            Memory();
        }
        cout << "HEAD> < " << time(0) << " >" << endl;
        cout << std::setw(4) << DInfo() << std::endl;
    }
}
int E(int r){
    GOD.PlayEffect(GOD.aree);
    led_on(2);
    int result = 1;
    for(int i=1; i < r; i++) {
        result*=10;
    }
    //Redirect Logs to a file since a error occur.
    if (!HasError){
        Files();
        write_DB(AB,"sdmc:/AnimeBase.json.bak");
        write_DB(BD,"sdmc:/DataBase.json.bak");
        write_DB(UD,"sdmc:/UserData.json.bak");
    }
    ErrorCode+=result;
    return result;
}
int getErrorCode(){
    return ErrorCode;
}
void Memory(){
    if (MLOG()) {
        cout << "Log to Mem >" << endl;
        // Redirect cout to Mem
        cout.rdbuf(redirectStream.rdbuf());
    }
}
void Screen(){
    if (MLOG()) {
        // Redirect cout back to screen
        cout.rdbuf(stream_buffer_cout);
        cout << "Log to screen >" << endl;
    }
}
void Files(){
    if (MLOG()) {
        HasError = true;
        cout << "Log to file >" << endl;
        //Redirect Logs to this file
        fileLog.open("sdmc:/JK.log", ios::app);
        fileLog << redirectStream.str();//get log from memory
        cout.rdbuf(fileLog.rdbuf());
    }
}
void deinit(){
    if (MLOG()) {
        if (HasError){cout << "ErrorCode: " << ErrorCode << endl;}
        cout << "END> < " << time(0) << " >" << endl;
        Screen();
        //Save log File
        if (HasError){fileLog.close();}

        cout << redirectStream.str() << endl;//Print the logs on nxlink
        redirectStream.str("");
    }
}


}





