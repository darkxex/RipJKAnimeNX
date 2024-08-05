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
//servers
json Servers;
//json ServersTMP;
std::vector<std::string> arrayserversbak = {
	"MixDrop 2.0","Desu","Nozomi","Xtreme S","Okru","MAS...","JKAnime"
};
bool white=false;

std::vector<std::string> arrayservers = arrayserversbak;
const string JkURL = "https://jkanime.net/";

bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>

// Función para convertir un número en base 36 a base 10
int base36ToInt(const std::string base36) {
    std::cout << ">>>: " << base36 << std::endl;

    return std::stoi(base36, nullptr, 36);
}

// Función para desofuscar el código usando el mapeo dado
std::string deobfuscate(const std::string& encoded, const std::vector<std::string>& map) {
    std::string result = encoded;
    int i = 0;
    // Encontrar todas las coincidencias de \b\w+\b (palabras) y reemplazarlas
    std::regex wordRegex("\\b\\w+\\b");
    std::smatch match;
    while (std::regex_search(result, match, wordRegex)) {
        i++;
        if (i > 100) break;
        std::string word = match.str();
        int index = base36ToInt(word);
        if (index < map.size()) {
            result.replace(match.position(), word.length(), map[index]);
            std::cout << "<<<<<: " << result << std::endl;

        }
    }

    return result;
}

int defuscate_mixdrop() {
    // Mapeo del código JavaScript ofuscado
    std::vector<std::string> map = {
        "MDCore", "", "s", "delivery31", "mxcontent", "mp4", "net", "z19zw4vzcgrd7nl",
        "referrer", "Xhog", "thumbs", "jpg", "furl", "wurl", "", "v2", "_zrWPlbeT2h0NWipW",
        "_t", "1722912076", "1722896923", "vfile", "7d41a99fbee433ab6b5d75e7c171e70c",
        "vserver", "remotesub", "chromeInject", "adTagUrl", "poster"
    };

    // Cadena codificada que representa el código ofuscado
    std::string encoded = "0.q=\"//2-3.4.6/a/7.b\";0.c=\" \";0.d=\"//2-3.4.6/f/7.5?2=g-9&e=i&h=j\";"
                          "0.k=\"l.5\";0.m=\"2-3\";0.n=\"\";0.o=1;0.p=\"\";0.8=\"\";";

    // Desofuscar y mostrar el código
    std::string deobfuscatedCode = deobfuscate(encoded, map);
    std::cout << "Código desofuscado:\n" << deobfuscatedCode << std::endl;

    return 0;
}




//Private Link decoders
string MixDrop_Link(string Link){
    //defuscate_mixdrop();
    //return "";
	replace(Link, "https://jkanime.net/jkvmixdrop.php?u=", "https://mixdrop.co/e/");
    //https://s-delivery34.mxdcontent.net/v/667ce306d2f8dbdea39e15dbd0344781.mp4?s=MezX-H8reQFQ-cLwRwehFg&e=1708498435&_t=1708480280
    
    
    /*
    
    MDCore.ref = "z19zw4vzcgrd7nl";
    MDCore.sublangs = {"ar":"Arabic","bg":"Bulgarian","bs":"Bosnian","hr":"Croatian","cz":"Czech","dk":"Danish","en":"English","fi":"Finish","fr":"French","de":"German","in":"Hindi","it":"Italian","id":"Indonesian","jp":"Japanese","my":"Malaysian","no":"Norwegian","pl":"Polish","pt":"Portuguese","ru":"Russian","ro":"Romanian","rs":"Serbian","sk":"Slovak","es":"Spanish","se":"Swedish","tr":"Turkish","ud":"Urdu"};
    eval(function(p,a,c,k,e,d){e=function(c){return c.toString(36)};if(!''.replace(/^/,String)){while(c--){d[c.toString(a)]=k[c]||c.toString(a)}k=[function(e){return d[e]}];e=function(){return'\\w+'};c=1};while(c--){if(k[c]){p=p.replace(new RegExp('\\b'+e(c)+'\\b','g'),k[c])}}return p}('0.q="//2-3.4.6/a/7.b";0.c=" ";0.d="//2-3.4.6/f/7.5?2=g-9&e=i&h=j";0.k="l.5";0.m="2-3";0.n="";0.o=1;0.p="";0.8="";',27,27,'MDCore||s|delivery31|mxcontent|mp4|net|z19zw4vzcgrd7nl|referrer|Xhog|thumbs|jpg|furl|wurl||v2|_zrWPlbeT2h0NWipW|_t|1722912076|1722896923|vfile|7d41a99fbee433ab6b5d75e7c171e70c|vserver|remotesub|chromeInject|adTagUrl|poster'.split('|'),0,{}))

    MDCore.q = "//s-delivery31.mxcontent.net/a/z19zw4vzcgrd7nl.jpg";
    MDCore.c = " ";
    MDCore.d = "//s-delivery31.mxcontent.net/v2/z19zw4vzcgrd7nl.mp4?s=g-9&e=1722912076&h=1722896923";
    MDCore.k = "vfile.mp4";
    MDCore.m = "s-delivery31";
    MDCore.n = "";
    MDCore.o = 1;
    MDCore.p = "";
    MDCore.referrer = "";

*/
	string decode="";
	for (int i=1; i<7; i++) {
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
			string numkey = "";
			string numkey2 = "";
            int vidnamelen = 32;
            string vertype = "v";
            if (decode.find("|v2|")) 
            {
                vertype="v2";
                vidnamelen = 15;
                //nl80orpgtlwoq3k
            }

            
			for (u64 i=0; i < list.size(); i++) {
				if ( (list[i]).length() == vidnamelen) vidname = list[i];
				if ( (list[i]).length() == 22) playkey = list[i]; //some times get a split key,  fix that, no e podido, no lo voy hacer

                //encuentra las llaves numéricas
                if (numkey == ""){
                    if ((list[i]).length() == 10 && is_number(list[i])) numkey = list[i];
                } else {
                    if ((list[i]).length() == 10 && is_number(list[i])) numkey2 = list[i];
                }
			}

			//bypass, if not get the key repeat the process
			if(!vidname.length()||!playkey.length()) {decode=""; continue;}

			//model
			decode = "https://"+type+"-"+dely+".mxcontent.net/"+vertype+"/"+vidname+".mp4?s="+playkey+"&e="+numkey+"&_t="+numkey2;//.substr(1)
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
			if (!data["file"].is_null())
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
	cout << "GET: "<< FirstKey << endl;
	codetemp = scrapElement(FirstKey,"name=\"data\" value=\"", "\"");
	replace(codetemp,"name=\"data\" value=\"","");
	FirstKey = codetemp;
	cout << "FirstKey: "<< FirstKey << endl;
	//Get SecondKey
	string data = "data=" + FirstKey;
	string SecondKey = Net::REDIRECT("https://jkanime.net/gsplay/redirect_post.php",data);
	//Use CustomPlayer
	//replace(SecondKey,"https://jkanime.net/gsplay/player.html#","https://darkxex.github.io/RipJKAnimeNX/player.html#");
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
		if(sz > 0) {
			if (!data["data"][sz]["file"].is_null()) {
				codetemp = data["data"][sz]["file"];
			}
		}
	}
	return codetemp;
}
string StreamWish_Link(string Link){
    string API = "";//PremiumAppKEY
    string FC = "";
    
    string tempcon = "";
    string LinkTemp = "https://api.streamwish.com/api/file/direct_link?key="+API+"&file_code="+FC;
    tempcon = Net::GET(Link);

    return tempcon;
}

bool OneMORE(string content,bool add = true){
    string tempcon = "";
    try{
        //verificar si no existe
        if(!isset(BD["com"]["servers"][KeyName],to_string(latest))){
            
            //Obtener Data incrustada
            tempcon = scrapElement(content, "var servers = [","]");

            //Obtener info de la Data
            cout << "Mas Servers." << tempcon << endl;
            //tempcon = Net::GET(tempcon);
            tempcon = scrapElement(tempcon, "[","]") + "]";
            cout << "Mas Servers.." << tempcon << endl;
            
            json ServersTMP;

            //destasar el json
            if(!json2ob (tempcon,ServersTMP)){
                return false;
            }
            for (auto& [key, value] : ServersTMP.items()) {
                //std::cout << key << " : " << value["server"] << "\n";
                std::cout << value["server"] << "::" << value["slug"] << endl;;
                string sourcename = string("_")+value["server"].get<string>();
                      
                BD["com"]["servers"][KeyName][to_string(latest)][sourcename] = value["slug"].get<string>();
            }
            Servers = BD["com"]["servers"][KeyName][to_string(latest)];
       } else {
            Servers = BD["com"]["servers"][KeyName][to_string(latest)];
        }
                        // even easier with structured bindings (C++17)
        if(add){
            for (auto& [key, value] : Servers.items()) {
                arrayservers.push_back(key);
            }
        }
        cout << std::setw(4) << BD["com"]["servers"][KeyName][to_string(latest)] << std::endl;
    } catch(...) {
        std::cout << "Error cap server... " << std::endl;
        return false;
    }
    return true;
}


// Public Func
bool onlinejkanimevideo(string onlineenlace,string server){
	if (!Net::HasConnection()) {return false;}
	string videourl = "";
	string content = "";
	string tempcon = "";
    
    if(server.at(0) == '_')
    {
        std::cout << Servers << endl;
        //https://jkanime.net/c3.php?u=7bc30453dVj8i&s=voe
        string Videored = string("https://c4.jkdesu.com/e/") + Servers[server].get<string>();
        videourl = Net::REDIRECT(Videored,"");
        if(videourl.length()) {
            videourl = "https://stardustcfw.github.io/player.html#" + string_to_hex(videourl);
        }
        //replace(videourl, "mdfx9dc8n.net", "mixdrop.ag");
        white=true;
    }
    
    
    //abrir Web para ver capitulo
	if (server == "JKAnime") {
		videourl = onlineenlace;
        white=true;
	} else if (videourl == ""){
        content = Net::GET(onlineenlace);
    }

	if (server == "MAS...") {
		arrayservers.erase(arrayservers.begin()+arrayservers.size());
        OneMORE(content);
        arrayservers.push_back("JKAnime");
	}
	if (server == "Fembed 2.0") {
		videourl = scrapElement(content, "jkfembed.php?u=","\"");
		if(videourl.length()) {
			tempcon = Fembed_Link(JkURL + videourl);
			if(tempcon.length()) {videourl=tempcon;}
		}
	}
	if (server == "Nozomi") {
		videourl = scrapElement(content,"um2.php?","\"");
		if(videourl.length()) {
			tempcon = Nozomi_player(JkURL + videourl);
			if(tempcon.length()) {videourl=tempcon;}
		}
	}
	if (server == "MixDrop 2.0") {
        OneMORE(content,false);
        if(isset(Servers,"_Mixdrop")){
            std::cout << Servers["_Mixdrop"] << endl;
            //https://jkanime.net/c3.php?u=7bc30453dVj8i&s=voe
            string Videored = string("https://c4.jkdesu.com/e/") + Servers["_Mixdrop"].get<string>();
            videourl = Net::REDIRECT(Videored,"");
            if(videourl.length()) {
                tempcon=MixDrop_Link(videourl);
                if(tempcon.length()) {videourl=tempcon;}

                videourl = "https://stardustcfw.github.io/player.html#" + string_to_hex(videourl);
            }
            //replace(videourl, "mdfx9dc8n.net", "mixdrop.ag");
            white=true;
        }
	}
	if (server == "MixDrop") {
		videourl = scrapElement(content,"jkvmixdrop.php?u=","\"");
		if(videourl.length()) {
			tempcon=MixDrop_Link(JkURL + videourl);
			if(tempcon.length()) {videourl=tempcon;}
		}
	}
	if (server == "Okru") {
		videourl = scrapElement(content,"jkokru.php?","\"");
        if (videourl.length() != 0) {
            replace(videourl, "jkokru.php?u=", "https://ok.ru/videoembed/");
            //videourl = JkURL + videourl;
        }
	}
	if (server == "Desu") {
		videourl = scrapElement(content,"um.php?","\"");
        if (videourl.length() != 0) {videourl = JkURL + videourl;}
	}
	if (server == "Fembed") {
		videourl = scrapElement(content,"jkfembed.php?u=","\"");
        if (videourl.length() != 0) {videourl = JkURL + videourl;}
	}
	if (server == "Xtreme S") {
		videourl = scrapElement(content,"jk.php?","\"");
        if (videourl.length() != 0) {videourl = JkURL + videourl;}
	}
	if (server == "Mega") {
		videourl = scrapElement(content,"https://mega.nz/embed/","\"");
        if (videourl.length() != 0) {videourl = JkURL + videourl;}
	}

	cout << "Aqui llega: "<< videourl << endl;
	if (videourl.length() != 0)
	{
		WebBrowserCall(videourl,false,white);
		videourl = "";
        white=false;
		return true;
	} else {
		cout << "Server no encontrado..." << onlineenlace << endl;
	}
	return false;
}
bool linktodownoadjkanime(string urltodownload,string directorydownload) {
	if (!Net::HasConnection()) {return false;}
	string videourl = "";
	string content = "";
	content = Net::GET(urltodownload);

	videourl = scrapElement(content, "um2.php?","\"");
	if(videourl.length())
	{
		videourl = Nozomi_Link(JkURL + videourl);
		if(videourl.length())
		{
			cout << videourl << endl;
			serverenlace = videourl;
			if(Net::DOWNLOAD(videourl, directorydownload)) return true;
			if(cancelcurl == 1) return false;
		}
	}

	videourl = scrapElement(content, "jkfembed.php?u=","\"");
	if (videourl.length())
	{
		videourl = Fembed_Link(JkURL + videourl);
		if (videourl.length())
		{
			cout << videourl << endl;
			serverenlace = videourl;
			if (Net::DOWNLOAD(videourl, directorydownload)) return true;
			if(cancelcurl == 1) return false;
		}
	}

	videourl = scrapElement(content, "https://www.mediafire.com/file/","\"");
	if(videourl.length())
	{
		cout << videourl << endl;
		string tempmedia = Net::GET(videourl);
		videourl = scrapElement(tempmedia, "https://download","\"");
		if(videourl.length())
		{
			replace(videourl, "\\", "");
			cout << videourl << endl;
			serverenlace = videourl;
			if(Net::DOWNLOAD(videourl, directorydownload)) return true;
			if(cancelcurl == 1) return false;
		}
	}
	{
        OneMORE(content,false);
        if(isset(Servers,"_Mixdrop")){
            string tempcon = "";
            std::cout << Servers["_Mixdrop"] << endl;
            //https://jkanime.net/c3.php?u=7bc30453dVj8i&s=voe
            string Videored = string("https://c4.jkdesu.com/e/") + Servers["_Mixdrop"].get<string>();
            videourl = Net::REDIRECT(Videored,"");
            if(videourl.length()) {
                tempcon=MixDrop_Link(videourl);
                if(tempcon.length()) {
                    videourl=tempcon;
                    serverenlace = videourl;
                    if(Net::DOWNLOAD(videourl, directorydownload)) return true;
                    if(cancelcurl == 1) return false;
                }
            }
            //replace(videourl, "mdfx9dc8n.net", "mixdrop.ag");
            //white=true;
        }
	}

	videourl = scrapElement(content,"jkvmixdrop.php?u=","\"");
	if(videourl.length())
	{
		cout << videourl << endl;
		videourl=MixDrop_Link(JkURL + videourl);
		if (videourl.length())
		{
			cout << videourl << endl;
			serverenlace = videourl;
			if(Net::DOWNLOAD(videourl, directorydownload)) return true;
			if(cancelcurl == 1) return false;
		}
	}

	videourl = scrapElement(content, "https://www24.zippyshare.com","\"");
	if(videourl.length())
	{
		cout << videourl << endl;
		string tempmedia = Net::GET(videourl);
		videourl = scrapElement(tempmedia, "https://www24.zippyshare.com/d","\"");
		if(videourl.length())
		{
			replace(videourl, "\\", "");
			cout << videourl << endl;
			serverenlace = videourl;
			if(Net::DOWNLOAD(videourl, directorydownload)) return true;
			if(cancelcurl == 1) return false;
		}
	}

	videourl = scrapElement(content, "jk.php?","\"");
	if(videourl.length())
	{
        videourl = JkURL + videourl;
		replace(videourl, "\\", "");
		replace(videourl, "https://jkanime.net/jk.php?u=", "https://jkanime.net/");
		cout << videourl << endl;
		serverenlace = videourl;
		if(Net::DOWNLOAD(videourl, directorydownload)) return true;
		if(cancelcurl == 1) return false;
	}

	return false;
}
