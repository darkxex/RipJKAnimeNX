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
int baseToInt(const std::string base36,int base) {
    //std::cout << ">>>:" << base36 << "---" << base << std::endl;
    return std::stoi(base36, nullptr, base);
}
/*
// Función para desofuscar el código usando el mapeo dado
std::string deobfuscate(const std::string& encoded, const std::vector<std::string>& map,int base) {
    std::string result = encoded;
    int i = 0;
    // Encontrar todas las coincidencias de \b\w+\b (palabras) y reemplazarlas
    std::regex wordRegex("\\b\\w+\\b");
    std::smatch match;
    while (std::regex_search(result, match, wordRegex)) {
        i++;
        if (i > 100) break;
        std::string word = match.str();
        std::cout << ">>" << word << "<<" << std::endl;
        
        int index = baseToInt(word,base);

        if (index < map.size()) {
            result.replace(match.position(), word.length(), map[index]);
            //std::cout << "<<<<<: " << result << std::endl;
        }
        
    }

    return result;
}
*/
std::string deobfuscate(const std::string& encoded, const std::vector<std::string>& map, int base) {
    std::string result;
    
    for (size_t i = 0; i < encoded.length(); ++i) {
        std::string character = encoded.substr(i, 1);  // Tomar un solo carácter como string
        
        if (std::isalnum(character[0])) {  // Verificar si el carácter es alfanumérico

            int index = baseToInt(character, base);
            //std::cout << ">>" << character << "<<" << index << std::endl;

            if (index < (int)map.size()) {
                if (map[index] == "ZZZ"){
                    //revisar si se debe saltear
                    result += character;
                } else {
                     // Agregar el carácter mapeado al resultado
                    result += map[index];
                    //std::cout << ">>" << character << ">> " << index << " >>" << map[index] << std::endl;
                }
            } else {
                result += character;  // Agregar el carácter original si no hay mapeo
            }
        } else {
            result += character;  // Agregar el carácter no alfanumérico al resultado
        }
    }
    
    vector<string> list = split (result, ";");
    json data;
	for (u64 i=0; i < list.size(); i++) {
        vector<string> list2 = split (list[i], "=\"");
        string a,b;
        a = list2[0];
        if (list2.size() > 1){
            b = list2[1];
        }
        replace(a, "MDCore.", "");
        replace(b, "\"", "");

        //std::cout << a << "<--->" << b << std::endl;
        data[a] = b;
    }
    cout << std::setw(4) << data << std::endl;

    return "https:"+data["wurl"].get<string>();
}

string defuscate_mixdrop(std::string Link) {
    string decode="";
	replace(Link, "https://jkanime.net/jkvmixdrop.php?u=", "https://mixdrop.co/e/");

    string ofucode="";
    string mapcode="";
	//tags
    string tag1 = "MDCore.ref", tag2 = "</script>", tag3 = "return p}('", tag4 = ",'";
    decode = Link;
    cout << decode << endl;
    string rrr = Net::GET(decode);
    string yyy = "";
    yyy = scrapElement(rrr, tag1,"</script>");
    yyy = scrapElement(yyy, "return p}('",".split('|')");
    
    cout << yyy << endl;
    cout << "--------------------------------" << endl;
    
    ofucode = scrapElement(yyy, tag3,"'");
    mapcode = scrapElement(yyy, tag4,"'");
    replace(ofucode, tag3, "");
    replace(mapcode, tag4, "");
    replace(mapcode, "||", "|ZZZ|");

    if(mapcode.substr(0, 1) == "|"){
        mapcode = "ZZZ"+mapcode;
    }
    cout << ofucode << endl;
    cout << mapcode << endl;
    
    // Mapeo del código JavaScript ofuscado
    std::vector<std::string> map = split (mapcode, "|");

    // Desofuscar y mostrar el código
    std::string deobfuscatedCode = deobfuscate(ofucode, map, 36);
    std::cout << "Codigo desofuscado:\n" << deobfuscatedCode << std::endl;

    return deobfuscatedCode;
}

//Private Link decoders
string MixDrop_Link(string Link){
    try{
        return defuscate_mixdrop(Link);
    } catch(...) {
        std::cout << "Error defuscate... " << std::endl;
    }
    string decode="";
    replace(Link, "https://jkanime.net/jkvmixdrop.php?u=", "https://mixdrop.co/e/");
    // Encuentra la posición del último '/'
    size_t lastSlashPos = Link.find_last_of('/');
    
    // Obtiene la subcadena desde esa posición + 1 hasta el final
    std::string vidname = Link.substr(lastSlashPos + 1);

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
			
			string playkey = "";
			string numkey = "";
			string numkey2 = "";
            //int vidnamelen = 32;
            string vertype = "v";
            if (decode.find("|v2|")) 
            {
                vertype="v2";
                //vidnamelen = 15;
                //nl80orpgtlwoq3k
            }

            
			for (u64 i=0; i < list.size(); i++) {
				//if ( (list[i]).length() == vidnamelen) vidname = list[i];
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
