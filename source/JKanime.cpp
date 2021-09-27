#include "Networking.hpp"
#include "applet.hpp"
#include "utils.hpp"
#include "JKanime.hpp"
#include "SDLWork.hpp"
#include "Link.hpp"
extern json BD;
extern json UD;
extern int mincapit;
extern int maxcapit;
extern int latest;
extern int latestcolor;
extern std::string rootdirectory;
extern std::string rootsave;
extern SDL_Thread* capithread;
extern bool gFAV;
extern int porcentajebufferS;
extern int porcentajebufferAllS;

extern LTexture gTextTexture;
extern LTexture Farest;
extern LTexture Heart;
extern int sizeportraity;
extern int sizeportraitx;
extern bool isConnected;
//img
extern bool reloading;
extern bool preview;
extern int selectchapter;
extern int imgNumbuffer;
extern int porcentajebuffer;
extern int porcentajebufferAll;
extern int porcentajebufferF;
extern int porcentajebufferFF;
extern bool quit;
extern bool isDownloading;
extern int porcendown;
extern int cancelcurl;
extern bool AppletMode;
extern bool isChained;
extern std::string serverenlace;
extern std::string DownTitle;
extern std::string KeyName;
extern std::string AccountID;

enum states { programationstate, downloadstate, chapterstate, searchstate, favoritesstate, historystate, hourglass, topstate, programationsliderstate};
extern int statenow;
extern int returnnow;

extern bool reloadingsearch;
extern int Frames;
enum UnixT {U_day=86400, U_week=604800};
u32 voidd =0;

//Private Functions
bool DataMaker(json LinkList,int& part, int& ofall);
int MkTOP();
int MkHOR();
int MkDIR();
void DataUpdate(std::string Link);
int capit(void* data);
int TimeNow(){
	std::time_t t = std::time(0);
	return t;
}

//BEGUING THREAD CHAIN
int AnimeLoader(void* data){
	int steep=0;
	try{
		isChained=true;
		ChainManager(true,true);
		steep++;//Wait for connection
		if (BD["arrays"]["chapter"]["link"].empty()) {
			//hide the list for rebuild
			reloading = true;
			preview = false;
		}
		steep++;
		while (!HasConnection()) {
			if (AppletMode) preview = false;
			SDL_Delay(3000);
			if (AppletMode) quit=true;
			if(quit) return 0;
		}
		
		
		CheckUpdates(AppletMode);
		steep++;
		if(!reloading) {
			//Download All not existing images
			CheckImgVector(BD["arrays"]["chapter"]["images"],imgNumbuffer);
		}

		steep++;//Get main page
		std::string content = gethtml("https://jkanime.net");

		steep++;//Get Programation list, Links and Images
		int temp0=0,temp1=0;
		temp0=content.find("Programación");
		temp1=content.find("TOP ANIMES",temp0);
		content = content.substr(temp0,temp1-temp0);

		steep++;//rebuild list
		std::vector<std::string> ChapLink=scrapElementAll(content, "https://jkanime.net/");
		std::vector<std::string> ChapImag=scrapElementAll(content, "https://cdn.jkanime.net/assets/images/");
		BD["arrays"]["chapter"]["date"]=scrapElementAll(content, "<span>","</span>");

		steep++;//Download All not existing images
		std::cout << "# IMG Recent" << std::endl;
		CheckImgVector(ChapImag,imgNumbuffer);

		steep++;//'haschange' See if there is any new chap
		bool haschange = true;
		if (!BD["latestchapter"].empty()) {
			if (BD["latestchapter"] == ChapLink[0] && !reloading)
			{
				haschange = false;
			}
		}
		steep++;//Display List
		if(reloading) {Frames=0; reloading = false;}
		
		steep++;//TimeStamp indicate if a chap sout be reloaded
		if (haschange || BD["TimeStamp"].empty()) {
			//update TimeStamp
			if (BD["arrays"]["chapter"]["link"].empty() || BD["arrays"]["chapter"]["link"][0] != ChapLink[0] || BD["TimeStamp"].empty()) {
				BD["TimeStamp"] = std::to_string(TimeNow());
				std::cout << "New TimeStamp: " << BD["TimeStamp"] << std::endl;

				//merge vectors
				if (!BD["arrays"]["chapter"]["images"].empty() && !BD["arrays"]["chapter"]["link"].empty())
				{
					std::vector<std::string> OChapLink=BD["arrays"]["chapter"]["link"];
					std::vector<std::string> OChapImag=BD["arrays"]["chapter"]["images"];

					ChapLink.erase(std::find(ChapLink.begin(), ChapLink.end(), OChapLink[0]),ChapLink.end());
					ChapImag.erase(std::find(ChapImag.begin(), ChapImag.end(), OChapImag[0]),ChapImag.end());

					ChapLink.insert(ChapLink.end(), OChapLink.begin(), OChapLink.end());
					ChapImag.insert(ChapImag.end(), OChapImag.begin(), OChapImag.end());

					if (ChapLink.size() > 61) {ChapLink.erase(ChapLink.begin()+60,ChapLink.end());}
					if (ChapImag.size() > 61) {ChapImag.erase(ChapImag.begin()+60,ChapImag.end());}

				}
				Frames=1;
				BD["arrays"]["chapter"]["link"]=ChapLink;
				BD["arrays"]["chapter"]["images"]=ChapImag;
			}
		}

 		steep++;//Get history
		std::cout << "# IMG history" << std::endl;
		CheckImgVector(UD["history"],imgNumbuffer);

		steep++;//Top, Hour to Database
		MkTOP();
		MkHOR();

		steep++;//Download All not existing images of Favorites
		if(UD["favoritos"].empty()) {
			std::cout << "# Get fav list" << std::endl;
			getFavorite();
			std::cout << "# Goted fav list" << std::endl;
		}
		std::cout << "# IMG fav" << std::endl;
		CheckImgVector(UD["favoritos"],porcentajebufferF);
		
		std::cout << "# End Image Download" << std::endl;


		steep++;//Load to cache all Programation Chaps
		std::cout << "# Cache Recent" << std::endl;
		if (haschange) {
			std::cout << "# Cache Recent, haschange" << std::endl;
			if(DataMaker(BD["arrays"]["chapter"]["link"], porcentajebuffer, porcentajebufferAll)) {
				BD["latestchapter"] = BD["arrays"]["chapter"]["link"][0];
			}
		}

		steep++;//Load to cache all Favorites Chaps
		std::cout << "# Cache favs" << std::endl;
		DataMaker(UD["favoritos"], porcentajebuffer, porcentajebufferAll);

		steep++;//Cache Top
		std::cout << "# Cache Top" << std::endl;
		DataMaker(BD["arrays"]["Top"]["link"], porcentajebuffer, porcentajebufferAll);

		steep++;//Cache Horario
		std::cout << "# Cache Horario" << std::endl;
		DataMaker(BD["arrays"]["HourGlass"]["link"], porcentajebuffer, porcentajebufferAll);

/*
		steep++;//extra vector
		std::vector<std::string> vec={};
		//load main
		std::cout << "# Cache Main" << std::endl;
		content = gethtml("https://jkanime.net");
		replace(content, "\"https://jkanime.net/\"", "");
		vec=scrapElementAll(content, "https://jkanime.net/");
		sort( vec.begin(),vec.end());
		vec.erase(unique(vec.begin(),vec.end()),vec.end());
		BD["arrays"]["Main"]["link"]=vec;
		DataMaker(vec, porcentajebuffer, porcentajebufferAll);
*/
		steep++;//Load Directory
		MkDIR();
	} catch(...) {
		printf("Thread Chain Error Catched, Steep#%d\n",steep);
		appletOverrideAutoSleepTimeAndDimmingTime(1800, 0, 500, 0);
		//std::cout << UD << std::endl;
	}
	if(quit) write_DB(BD,rootdirectory+"DataBase.json");
	std::cout << "# End Thread Chain" << std::endl;
	isChained=false;
	ChainManager(false,!isDownloading&&!isChained);
	//exit after load the cache if are in applet mode
	if (AppletMode) quit=true;
	return 0;

}
bool DataMaker(json LinkList,int& part, int& ofall) {
	bool hasmchap=false;
	std::string a = "";
	part=0;
	int sep=0;
	ofall=LinkList.size();
	if (ofall <= 0){return false;}
	for (int x = 0; x < ofall&& !quit; x++)
	{
		if(hasmchap) part = x+1;
		if (!isConnected) while (!HasConnection()) {SDL_Delay(2000); if(quit) return false; }
		std::string link = LinkList[x];
		std::string name = KeyOfLink(link);
		if (BD["DataBase"][name]["TimeStamp"] != BD["TimeStamp"] ) {
			if (BD["DataBase"][name]["TimeStamp"].empty() || BD["DataBase"][name]["enemision"]=="true") {
				part = x+1;
				DataUpdate(link);
				hasmchap=true;
				if (sep >= 101){
					//write_DB(BD,"sdmc:/DataBase.json");
					write_DB(BD,rootdirectory+"DataBase.json");
					sep=0;
				}
				sep++;
			} else {
				//BD["DataBase"][name]["TimeStamp"] = BD["TimeStamp"];
			}
		}
	}
	part=0;
	ofall=0;
	if(quit) return false;
	if(hasmchap) {
		write_DB(BD,rootdirectory+"DataBase.json");//write json
	}
	return true;
}
int MkTOP(){
	//load Top
	std::vector<std::string> TOPC={};
	std::cout << "# Get Top" << std::endl;
	std::string content=gethtml("https://jkanime.net/top/");
	replace(content,"https://jkanime.net/top/","");
	replace(content,"https://jkanime.net///","");
	TOPC=scrapElementAll(content,"https://jkanime.net/");
	TOPC.erase(unique(TOPC.begin(),TOPC.end()),TOPC.end());
	BD["arrays"]["Top"]["link"]=TOPC;
	
	std::cout << "# IMG Top" << std::endl;
	CheckImgVector(TOPC,imgNumbuffer);
	return 0;
}
int MkHOR(){
	//load Horario
	std::cout << "# Get HourGlass" << std::endl;
	std::string content=gethtml("https://jkanime.net/horario/");
	replace(content,"https://jkanime.net/horario/","");
	std::vector<std::string> STP={};

	STP=split(content,"<div class='box semana'>");
	BD["arrays"]["HourGlass"]["Lunes"]=scrapElementAll(STP[1],"https://jkanime.net/","\"","Lunes");
	BD["arrays"]["HourGlass"]["Martes"]=scrapElementAll(STP[2],"https://jkanime.net/","\"","Martes");
	BD["arrays"]["HourGlass"]["Miercoles"]=scrapElementAll(STP[3],"https://jkanime.net/","\"","Miercoles");
	BD["arrays"]["HourGlass"]["Jueves"]=scrapElementAll(STP[4],"https://jkanime.net/","\"","Jueves");
	BD["arrays"]["HourGlass"]["Viernes"]=scrapElementAll(STP[5],"https://jkanime.net/","\"","Viernes");
	BD["arrays"]["HourGlass"]["Sabado"]=scrapElementAll(STP[6],"https://jkanime.net/","\"","Sabado");
	BD["arrays"]["HourGlass"]["Domingo"]=scrapElementAll(STP[7],"https://jkanime.net/","\"","Domingo");

	std::vector<std::string> MPO={};
	std::vector<std::string> HORC={};

	MPO=scrapElementAll(STP[1],"https://jkanime.net/","\"","Lunes");
	HORC.insert(HORC.end(), MPO.begin(), MPO.end());
	MPO=scrapElementAll(STP[2],"https://jkanime.net/","\"","Martes");
	HORC.insert(HORC.end(), MPO.begin(), MPO.end());
	MPO=scrapElementAll(STP[3],"https://jkanime.net/","\"","Miercoles");
	HORC.insert(HORC.end(), MPO.begin(), MPO.end());
	MPO=scrapElementAll(STP[4],"https://jkanime.net/","\"","Jueves");
	HORC.insert(HORC.end(), MPO.begin(), MPO.end());
	MPO=scrapElementAll(STP[5],"https://jkanime.net/","\"","Viernes");
	HORC.insert(HORC.end(), MPO.begin(), MPO.end());
	MPO=scrapElementAll(STP[6],"https://jkanime.net/","\"","/Sabado");
	HORC.insert(HORC.end(), MPO.begin(), MPO.end());
	MPO=scrapElementAll(STP[7],"https://jkanime.net/","\"","Domingo");
	HORC.insert(HORC.end(), MPO.begin(), MPO.end());

	//HORC.erase(unique(HORC.begin(),HORC.end()),HORC.end());
	BD["arrays"]["HourGlass"]["link"]=HORC;
	BD["arrays"]["HourGlass"]["Todos"]=HORC;
	std::cout << "# IMG HourGlass" << std::endl;
	CheckImgVector(HORC,imgNumbuffer);
	return 0;
}
int MkDIR(){
	//make directory
	if (BD["arrays"]["Directory"]["TimeStamp"].empty()){BD["arrays"]["Directory"]["TimeStamp"]=0;}
	if (BD["arrays"]["Directory"]["InTime"].empty()){BD["arrays"]["Directory"]["InTime"]=0;}
	if (BD["arrays"]["Directory"]["page"].empty()){BD["arrays"]["Directory"]["page"]=1;}
	if ((TimeNow()-BD["arrays"]["Directory"]["TimeStamp"].get<int>()) > U_week){
		std::cout << "# Get Directory" << std::endl;
		std::vector<std::string> DIR={};
		std::vector<std::string> TDIR={};

		while (!quit) {
			std::string content=gethtml("https://jkanime.net/directorio/"+std::to_string(BD["arrays"]["Directory"]["page"].get<int>())+"/");
			replace(content,"https://jkanime.net/directorio/","");
			replace(content,"https://jkanime.net///","");
			TDIR=scrapElementAll(content,"https://jkanime.net/");
			

			if (TDIR.size() > voidd){
				DIR.insert(DIR.end(), TDIR.begin(), TDIR.end());
			} else 
				break;//just in case
			
			std::string scrap = scrapElement(content, "Resultados Siguientes");
			//std::cout << scrap << "  # " << std::to_string(BD["arrays"]["Directory"]["page"]) << std::endl;
			if (scrap.length() > 0) {
				//some code here soon
			} else {
				break;
			}
			BD["arrays"]["Directory"]["page"]=BD["arrays"]["Directory"]["page"].get<int>()+1;
		}
		if(quit) return false;
		BD["arrays"]["Directory"]["pageT"]=BD["arrays"]["Directory"]["page"];
		BD["arrays"]["Directory"]["page"]=1;
		DIR.erase(unique(DIR.begin(),DIR.end()),DIR.end());
		BD["arrays"]["Directory"]["TimeStamp"]=TimeNow();
		BD["arrays"]["Directory"]["link"]=DIR;
		//std::cout << BD["arrays"]["Directory"] << std::endl;
		BD["arrays"]["Directory"]["InTime"]=0;
		write_DB(BD,rootdirectory+"DataBase.json");
	}
	if(quit) return false;
	//Build All Data Base
	if (BD["arrays"]["Directory"]["InTime"].get<int>() == 0)
	{
		std::cout << "# Cache Directory" << std::endl;
		try{
			if (DataMaker(BD["arrays"]["Directory"]["link"], porcentajebuffer, porcentajebufferAll)){
				BD["arrays"]["Directory"]["InTime"]=1;
				write_DB(BD,rootdirectory+"DataBase.json");
			}
		} catch(...) {
			printf("Thread Chain Error Catched,Dir Error\n");
			//write_DB(BD,rootdirectory+"DataBase.json");
			return 0;
		}
	}
	return 0;
}
//END THREAD CHAIN

int downloadjkanimevideo(void* data) {//Download THREAD
	try{
	ChainManager(true,true);
	for (u64 x=0; x< BD["arrays"]["downloads"]["queue"].size(); x++) {
		if (x==0) {led_on(1);}
		DownTitle="................";
		serverenlace = "................";
		isDownloading=true;
		porcendown=0;
		if(cancelcurl) {serverenlace = "Error de descarga"; break;}
		std::string urldown=BD["arrays"]["downloads"]["queue"][x];
		std::string namedownload = urldown;
		NameOfLink(namedownload);
		mayus(namedownload);
		DownTitle=namedownload;
		//namedownload = namedownload.substr(0, namedownload.length() - 1);
		BD["arrays"]["downloads"]["log"][x] = ">>>>  "+namedownload;
		mkdir("sdmc:/Videos",0777);
		std::string directorydownload = "sdmc:/Videos/" +namedownload + ".mp4";

		if (!linktodownoadjkanime(urldown,directorydownload)) {
			serverenlace = "Error de descarga";
			BD["arrays"]["downloads"]["log"][x] = "Error: "+namedownload;
		} else {
			BD["arrays"]["downloads"]["log"][x] = "100% : "+namedownload;
		}
	}
	if(cancelcurl==0) led_on(3); else led_on(0);
	} catch(...) {
		printf("Thread Download Error Catched\n");
		std::cout << BD["arrays"]["downloads"] << std::endl;
	}
	cancelcurl = 0;
	isDownloading=false;
	statenow = downloadstate;
	ChainManager(false,!isDownloading&&!isChained);
	return 0;
}
int searchjk(void* data) {//Search Thread
	try{
	BD["com"]["porcentajereload"] = 0;
	BD["com"]["porcentajereloadAll"]=0;
	BD["arrays"]["search"]["link"].clear();
	BD["arrays"]["search"]["images"].clear();
	BD["arrays"]["search"]["date"].clear();
	reloadingsearch = true;

	std::string texts = BD["searchtext"];
	replace(texts, " ", "_");
	replace(texts, "!", "");
	replace(texts, ";", "");
	if (texts.length() >= 2) {
		std::cout << texts << std::endl;
		std::string content = "";
		int page = 1;
		while (!quit) {
			std::string tempCont=gethtml("https://jkanime.net/buscar/" + texts + "/"+std::to_string(page)+"/");
			content += tempCont;
			std::string scrap = scrapElement(tempCont, "Resultados Siguientes");
			std::cout << scrap << "  # " << std::to_string(page) << std::endl;
			if (scrap.length() > 0) {
				//some code here soon
			} else {
				break;
			}
			page++;
		}

		int val0 = 0,val1 = 1,val2,val3, val4;
		while (val0 != -1) {
			val0 = content.find("<div class=\"anime__item\">", val1);
			if (val0 == -1) { break; }

			val1 = 6 + content.find("href=", val0);
			val2 = (content.find('"', val1));
			std::string gdrive = content.substr(val1, val2 - val1);
			BD["arrays"]["search"]["link"].push_back(gdrive);

			val3 = content.find("data-setbg=", val2) + 12;
			val4 = content.find('"', val3);
			std::string gsearchpreview = content.substr(val3, val4 - val3);
			BD["arrays"]["search"]["images"].push_back(gsearchpreview);
			//std::cout << gsearchpreview << std::endl;

			val3 = content.find("<p>", val4) + 3;
			val4 = content.find("</p>", val3);
			gsearchpreview = content.substr(val3, val4 - val3);
			RemoveAccents(gsearchpreview);
			if (gsearchpreview.length() > 90) {
				gsearchpreview=gsearchpreview.substr(0,90)+"...";
			}
			replace(gsearchpreview, "&quot;", "");
			BD["arrays"]["search"]["date"].push_back(gsearchpreview);
			val1++;
		}

		porcentajebufferFF=BD["arrays"]["search"]["images"].size();
		CheckImgVector(BD["arrays"]["search"]["images"],porcentajebufferF);
		porcentajebufferFF=0;
	}
	else
	{
		statenow = programationstate;
		returnnow = programationstate;
	}
	} catch(...) {
		printf("Thread Search Error Catched\n");
		std::cout << BD["arrays"]["search"] << std::endl;
	}
	reloadingsearch = false;
	DataMaker(BD["arrays"]["search"]["link"], porcentajebufferS, porcentajebufferAllS);
	return 0;
}
int capit(void* data) {//Get chap thread
	if (!HasConnection()) return 0;
	std::string link = BD["com"]["ActualLink"];
	std::string name = KeyOfLink(link);
	DataUpdate(link);
	try{
		//std::cout << BD << std::endl;
		BD["com"]["sinopsis"] = BD["DataBase"][name]["sinopsis"];
		BD["com"]["nextdate"] = BD["DataBase"][name]["nextdate"];//"......";
		BD["com"]["generos"] = BD["DataBase"][name]["generos"];//"......";
		BD["com"]["Emitido"] = BD["DataBase"][name]["Emitido"];
		BD["com"]["enemision"] = BD["DataBase"][name]["enemision"];
		mincapit = BD["DataBase"][name]["mincapit"];//1;
		maxcapit = BD["DataBase"][name]["maxcapit"];//-1;
		//write json
		write_DB(BD,rootdirectory+"DataBase.json");

		//Get Image
		std::string image = rootdirectory+"DATA/"+name+".jpg";
		if (!BD["DataBase"][name]["Image"].empty()) {
			CheckImgNet(image,BD["DataBase"][name]["Image"]);
		}
	}catch(...) {
		printf("Error \n");
	}
	return 0;
}
int capBuffer (std::string Tlink) {//anime manager
	int v2 = Tlink.find("/", 20);
	Tlink = Tlink.substr(0, v2 + 1);
	BD["com"]["ActualLink"] = Tlink;

	std::string name = Tlink;
	replace(name, "https://jkanime.net/", "");
	replace(name, "/", "");
	KeyName = name;
//	std::cout << "KeyName: " << name << std::endl;

	std::cout << "Link: " << Tlink << std::endl;

	std::string image = rootdirectory+"DATA/"+name+".jpg";
	statenow = chapterstate;

	if (BD["DataBase"][name]["TimeStamp"].empty())
	{
		BD["com"]["sinopsis"] = "......";
		BD["com"]["nextdate"] = "......";
		BD["com"]["generos"] = "......";
		BD["com"]["Emitido"] = "......";
		maxcapit = -1;
		mincapit = 1;
		latest = 1;
		capithread = SDL_CreateThread(capit, "capithread", (void*)NULL);
	} else {
		try{
			BD["com"]["sinopsis"] = BD["DataBase"][name]["sinopsis"];
			BD["com"]["nextdate"] = BD["DataBase"][name]["nextdate"];//"......";
			BD["com"]["generos"] = BD["DataBase"][name]["generos"];//"......";
			BD["com"]["Emitido"] = BD["DataBase"][name]["Emitido"];
			BD["com"]["enemision"] = BD["DataBase"][name]["enemision"];
			maxcapit = BD["DataBase"][name]["maxcapit"];
			mincapit = BD["DataBase"][name]["mincapit"];
			//check For latest cap seend
			if (UD["chapter"][name].empty()||UD["chapter"][name]["latest"].empty()) {
				UD["chapter"].erase(name);
				//get position to the latest cap if in emision
				if (BD["com"]["enemision"] == "true") {
					latest = BD["DataBase"][name]["maxcapit"];//is in emision
				} else {
					latest = BD["DataBase"][name]["mincapit"];//is not in emision
				}
				latestcolor = -1;
			}
			else{
				latest = UD["chapter"][name]["latest"];
				latestcolor = UD["chapter"][name]["latest"];
			}

			//Get Image
			if (!BD["DataBase"][name]["Image"].empty()) {
				CheckImgNet(image,BD["DataBase"][name]["Image"]);
			}

			if (BD["DataBase"][name]["TimeStamp"] != BD["TimeStamp"]) {
				BD["com"]["nextdate"] = "Loading...";
				capithread = SDL_CreateThread(capit, "capithread", (void*)NULL);
			}
		}catch(...) {
			printf("Error \n");
		}
	}
	CheckImgNet(image);
	return 0;
}
void DataUpdate(std::string Link) {//Get info off chapter
	if(quit) return;
	std::string name = KeyOfLink(Link);
	std::string a = gethtml("https://jkanime.net/"+name+"/");
	if(quit) return;
	json AnimeINF=BD["DataBase"][name];//
	std::string TMP="";
	if (AnimeINF["sinopsis"].empty()){
		//Sinopsis
		TMP = scrapElement(a, "<p rel=\"sinopsis\">","</p>");
		replace(TMP, "<p rel=\"sinopsis\">", ""); replace(TMP, "<br/>", ""); replace(TMP, "&quot;", "");
		//AnimeINF["sinopsis"] = TMP.substr(0,800);
		AnimeINF["sinopsis"] = TMP;
	}
	if (AnimeINF[""].empty()){
		//get image
		TMP = scrapElement(a, "https://cdn.jkanime.net/assets/images/animes/image/");
		AnimeINF["Image"] = TMP;
	}

	if (AnimeINF["Tipo"].empty()){
		//Anime info
		TMP = scrapElement(a, "<span>Tipo:","</li");
		replace(TMP, "<span>Tipo:", ""); replace(TMP, "</span> ", ""); replace(TMP, "</span>", "");
		AnimeINF["Tipo"] = TMP;
	}

	if (AnimeINF["Idiomas"].empty()){
		TMP = scrapElement(a, "Idiomas:","</li");
		replace(TMP, "Idiomas:", ""); replace(TMP, "  ", " "); replace(TMP, "</span> ", ""); replace(TMP, "</span>", "");
		AnimeINF["Idiomas"] = TMP;
	}

	TMP = scrapElement(a, "Episodios:","</li");
	replace(TMP, "Episodios:", ""); replace(TMP, "</span> ", ""); replace(TMP, "</span>", "");
	AnimeINF["Episodios"] = TMP;

	TMP = scrapElement(a, "Duracion:","</li");
	replace(TMP, "Duracion:", ""); replace(TMP, "</span> ", ""); replace(TMP, "</span>", "");
	AnimeINF["Duracion"] = TMP;

	TMP = scrapElement(a, "Emitido:","</li");
	replace(TMP, "Emitido:", ""); replace(TMP, "</span> ", ""); replace(TMP, "</span>", "");
	AnimeINF["Emitido"] = TMP;

	int bal1=0;
	bal1=a.find("Secuela");
	if (bal1 > 1) {
		TMP = scrapElement(a.substr(bal1), "https://jkanime.net/","");
		AnimeINF["Secuela"] = TMP;
	}

	bal1=a.find("Precuela");
	if (bal1 > 1) {
		TMP = scrapElement(a.substr(bal1), "https://jkanime.net/","");
		AnimeINF["Precuela"] = TMP;
	}

	//find next date
	TMP = "...";
	if ((int)a.find("<span>Tipo:</span> Pelicula</li>") != -1)
	{
		//es una peli ?
		TMP="Pelicula";
	} else {
		//find next date
		int re1 =0, re2=0;
		re1 = a.find("<b>Próximo episodio</b>");
		if(re1 > 1) {
			re1 += 25;
			re2 = a.find("<i class", re1);
			TMP = "."+a.substr(re1, re2 - re1);
			RemoveAccents(TMP);
		}
	}
	AnimeINF["nextdate"] = TMP;

	if (AnimeINF["generos"].empty()){
		//Generos
		int indx1 = 1, indx2, indx3;
		indx1 = a.find("<span>Genero:</span>", indx1);
		TMP="";
		while (indx1 != -1) {
			indx1 = a.find("https://jkanime.net/genero", indx1);
			if (indx1 == -1) { break; }
			indx2 = a.find(">",indx1);
			indx3 = a.find("</a>", indx1);
			TMP += a.substr(indx2+1, indx3 - indx2-1)+", ";
			indx1 = indx3;
		}
		RemoveAccents(TMP);

		if (TMP.length() == 0) TMP +="-.-";
		AnimeINF["generos"] = TMP;
	}

	//Esta en emision?
	if ((int)a.find("En emision") != -1)
	{AnimeINF["enemision"] = "true";} else {AnimeINF["enemision"] = "false";}

	//Get Caps number
	int val0, val1, val2, val3;
	val0 = a.rfind("href=\"#pag");
	if (val0 != -1) {
		val1 = a.find(">", val0);
		val1 = val1 + 1;
		val2 = a.find("<", val1);

		std::string urlx;
		urlx = (a.substr(val1, val2 - val1));
		val3 = urlx.find(" - ") + 3;
		urlx = urlx.substr(val3);

		AnimeINF["maxcapit"] = atoi(urlx.c_str());
	} else {
		AnimeINF["maxcapit"] = 1;
	}

	if (AnimeINF["mincapit"].empty()){
		//empieza por 0?
		int zero1, zero2;
		std::string zerocontainer = "";
		std::string zerocontainer2 = "";
		zero1 = a.rfind("ajax/pagination_episodes/");
		zero2 = a.find("'", zero1);
		zerocontainer = "https://www.jkanime.net/" + a.substr(zero1, zero2 - zero1) + "/1/";
		//std::cout << zerocontainer << std::endl;
		zerocontainer2 = gethtml(zerocontainer);

		int tempzero = zerocontainer2.find("\"0\"");
		if (tempzero != -1) {
			AnimeINF["mincapit"] = 0;
		}
		else {
			AnimeINF["mincapit"] = 1;
		}
	}
	if(AnimeINF["mincapit"].get<int>()==0)
	AnimeINF["maxcapit"] = AnimeINF["maxcapit"].get<int>() - 1;
	
	
	AnimeINF["TimeStamp"] = BD["TimeStamp"];
		std::stringstream strm;
	try{
//		strm << std::setw(4) << base;
		strm << AnimeINF;
		//write to DB
		BD["DataBase"][name]=AnimeINF;
		std::cout << "Saved: " << name << std::endl;
	} catch(...) {
		std::cout << "Anime Problemático: "<< name << std::endl;
		std::cout << strm.str() << std::endl;
	}
}

//Favorito
void addFavorite(std::string text) {
	if(!isFavorite(text)) {
		UD["favoritos"].push_back(text);
		write_DB(UD,rootsave+"UserData.json");
	}
}
void getFavorite() {
	if (UD["favoritos"].empty()) {
		if (isFileExist(rootdirectory+"favoritos.txt")) {
			std::string temp;
			std::ifstream file(rootdirectory+"favoritos.txt");
			std::string str;
			while (std::getline(file, str)) {
				std::string strtmp = str;
				replace(str,"\"","");
				if (str.find("jkanime"))
				{
					UD["favoritos"].push_back(str);
				}
			}
			file.close();
			rename((rootdirectory+"favoritos.txt").c_str(), (rootdirectory+"favoritos.bak").c_str());
			remove((rootdirectory+"favoritos.txt").c_str());
			write_DB(UD,rootsave+"UserData.json");
		}
	}
}
bool isFavorite(std::string fav){
	if (!UD["favoritos"].empty()) {
		if(std::find(UD["favoritos"].begin(), UD["favoritos"].end(), fav) != UD["favoritos"].end())
		{
			return true;
		}

	}
	return false;
}
void delFavorite(int inst){
	if (!UD["favoritos"].empty()) {
		UD["favoritos"].erase(inst);
		write_DB(UD,rootsave+"UserData.json");
	}
}
