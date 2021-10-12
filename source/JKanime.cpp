#include "Networking.hpp"
#include "applet.hpp"
#include "utils.hpp"
#include "JKanime.hpp"
#include "SDLWork.hpp"
#include "Link.hpp"

extern SDL_Thread* capithread;
enum UnixT {U_day=86400, U_week=604800};
u32 voidd =0;

//Private Functions
bool DataMaker(json LinkList,int& part, int& ofall);
int MkTOP();
int MkHOR();
int MkAGR(string content);
int MkBNR(string content);
int MkDIR();
void DataUpdate(string Link);
int capit(void* data);
int TimeNow(){
	time_t t = time(0);
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
		while (!Net::HasConnection()) {
			if (AppletMode) preview = false;
			if (AppletMode) InstallNSP("romfs:/05B9DB505ABBE000.nsp");
			SDL_Delay(3000);
			if (AppletMode) quit=true;
			if(quit) return 0;
		}
		
		//execute this once, or if Mgate is true
		static bool Mgate=true;
		if (Mgate){
			#ifdef ISDEBUG
				#include "Debug.h"
			#endif
			//Check if dns are correct
			int Req = Net::HEAD("https://bvc-hac-lp1.cdn.nintendo.net/13-0-0")["CODE"];
			 switch(Req){
				case 0:
				case 503:
					std::cout << "# Place DNS" << std::endl;
					copy_me("romfs:/default.txt","sdmc:/atmosphere/hosts/default.txt");
					break;
			}
			Mgate=false;
		}
		//Check for app Updates
		CheckUpdates(AppletMode);

		steep++;
		if(!reloading) {
			//Download All not existing images
			CheckImgVector(BD["arrays"]["chapter"]["images"],imgNumbuffer);
		}

		steep++;//Get main page
		json MainPage=Net::REQUEST("https://jkanime.net/");
		//Check headers ToDo
		string content = MainPage["BODY"];
		if (content.length() < 2){
			cout << "- Error can't connect with Web" << endl;
			throw "Error Connect";
		}

		steep++;//Get Programation list, Links and Images
		int temp0=0,temp1=0;
		temp0=content.find("Programación");
		temp1=content.find("TOP ANIMES",temp0);
		string temcont = content.substr(temp0,temp1-temp0);
		steep++;//rebuild list
		vector<string> ChapLink=scrapElementAll(temcont, "https://jkanime.net/");
		vector<string> ChapImag=scrapElementAll(temcont, "https://cdn.jkanime.net/assets/images/");
		BD["arrays"]["chapter"]["date"]=scrapElementAll(temcont, "<span>","</span>");

		steep++;//Download All not existing images
		cout << "# IMG Recent " << std::endl;
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
		
		if (haschange || BD["TimeStamp"].empty()) {
			//update TimeStamp
			if (BD["arrays"]["chapter"]["link"].empty() || BD["arrays"]["chapter"]["link"][0] != ChapLink[0] || BD["TimeStamp"].empty()) {
				BD["TimeStamp"] = to_string(TimeNow());
				cout << "# New TimeStamp: " << BD["TimeStamp"] << endl;

				//merge vectors
				if (!BD["arrays"]["chapter"]["images"].empty() && !BD["arrays"]["chapter"]["link"].empty())
				{
					vector<string> OChapLink=BD["arrays"]["chapter"]["link"];
					vector<string> OChapImag=BD["arrays"]["chapter"]["images"];

					ChapLink.erase(find(ChapLink.begin(), ChapLink.end(), OChapLink[0]),ChapLink.end());
					ChapImag.erase(find(ChapImag.begin(), ChapImag.end(), OChapImag[0]),ChapImag.end());

					ChapLink.insert(ChapLink.end(), OChapLink.begin(), OChapLink.end());
					ChapImag.insert(ChapImag.end(), OChapImag.begin(), OChapImag.end());

					if (ChapLink.size() > 100) {ChapLink.erase(ChapLink.begin()+100,ChapLink.end());}
					if (ChapImag.size() > 100) {ChapImag.erase(ChapImag.begin()+100,ChapImag.end());}
				}
				Frames=1;
				BD["arrays"]["chapter"]["link"]=ChapLink;
				BD["arrays"]["chapter"]["images"]=ChapImag;
			} 
		} else {
			cout << "# TimeStamp: " << BD["TimeStamp"] << endl;
		}

 		steep++;//Get history
		cout << "# IMG History " << endl;
		CheckImgVector(UD["history"],imgNumbuffer);

		steep++;//Agregados to Database
		temp0=content.find("Listado de últimos agregados");
		temp1=content.find("</section>",temp0);
		temcont = content.substr(temp0,temp1-temp0);
		MkAGR(temcont);
		
		steep++;//banner to Database
		temp0=content.find("<section class=\"hero\">");
		temp1=content.find("<div class=\"solopc\">",temp0);
		temcont = content.substr(temp0,temp1-temp0);
		MkBNR(temcont);
		steep++;//Top, Hour to Database
		MkTOP();
		MkHOR();

		steep++;//Download All not existing images of Favorites
		if(UD["favoritos"].empty()) {
			cout << "# Get fav list " << endl;
			getFavorite();
			cout << "# Goted fav list " << endl;
		}
		cout << "# IMG favoritos " << endl;
		CheckImgVector(UD["favoritos"],porcentajebufferF);
		
		cout << "# End Image Download " << endl;


		steep++;//Load to cache all Programation Chaps
		cout << "# Cache Recent " << endl;
		if (haschange) {
			cout << "# Cache Recent, haschange " << endl;
			if(DataMaker(BD["arrays"]["chapter"]["link"], part, ofall)) {
				BD["latestchapter"] = BD["arrays"]["chapter"]["link"][0];
			}
		}

		steep++;//Load to cache all Favorites Chaps
		cout << "# Cache favoritos " << endl;
		DataMaker(UD["favoritos"], part, ofall);

		steep++;//Cache Top
		cout << "# Cache Top " << endl;
		DataMaker(BD["arrays"]["Top"]["link"], part, ofall);

		steep++;//Cache Horario
		cout << "# Cache HourGlass " << endl;
		DataMaker(BD["arrays"]["HourGlass"]["link"], part, ofall);

		steep++;//Load Directory
		MkDIR();
	} catch(...) {
		led_on(2);
		cout << "- Thread Chain Error Catched, Steep#" << steep <<std::endl;
		appletOverrideAutoSleepTimeAndDimmingTime(1800, 0, 500, 0);
		//cout << UD << endl;
	}
	if(quit) write_DB(AB,rootdirectory+"AnimeBase.json");
	cout << "# End Thread Chain" << endl;
	isChained=false;
	ChainManager(false,!isDownloading&&!isChained);
	//exit after load the cache if are in applet mode
	if (AppletMode) quit=true;
	return 0;

}
bool DataMaker(json LinkList,int& part, int& ofall) {
	bool hasmchap=false;
	string a = "";
	part=0;
	//int sep=0;
	ofall=LinkList.size();
	if (ofall <= 0){return false;}
	for (int x = 0; x < ofall&& !quit; x++)
	{
		if(hasmchap) part = x+1;
		if (!isConnected) while (!Net::HasConnection()) {SDL_Delay(2000); if(quit) return false; }
		string link = LinkList[x];
		string name = KeyOfLink(link);
		if (AB["AnimeBase"][name]["TimeStamp"] != BD["TimeStamp"] ) {
			if (AB["AnimeBase"][name]["TimeStamp"].empty() || AB["AnimeBase"][name]["enemision"]=="true") {
				part = x+1;
				DataUpdate(link);
				hasmchap=true;
				/*
				if (sep >= 101){
					//write_DB(BD,"sdmc:/DataBase.json");
					write_DB(AB,rootdirectory+"AnimeBase.json");
					sep=0;
				}
				
				sep++;
				*/
			} else {
				//AB["AnimeBase"][name]["TimeStamp"] = BD["TimeStamp"];
			}
		}
	}
	part=0;
	ofall=0;
	if(quit) return false;
	if(hasmchap) {
		write_DB(AB,rootdirectory+"AnimeBase.json");//write json
	}
	return true;
}
int MkTOP(){
	//load Top
	vector<string> TOPC={};
	cout << "# Get Top " << endl;
	string content=Net::GET("https://jkanime.net/top/");
	replace(content,"https://jkanime.net/top/","");
	replace(content,"https://jkanime.net///","");
	TOPC=scrapElementAll(content,"https://jkanime.net/");
	TOPC.erase(unique(TOPC.begin(),TOPC.end()),TOPC.end());
	BD["arrays"]["Top"]["link"]=TOPC;
	
	cout << "# IMG Top " << endl;
	CheckImgVector(TOPC,imgNumbuffer);
	return 0;
}

int MkBNR(string content){
	//Get Latest added animes
	cout << "# Get Banner " << endl;
	BD["arrays"]["Banner"]["link"]=scrapElementAll(content, "https://jkanime.net/");
	BD["arrays"]["Banner"]["img"]=scrapElementAll(content, "https://cdn.jkanime.net/assets/images/animes/video/image/");
	BD["arrays"]["Banner"]["name"]=scrapElementAll(content, "<h2>","</h2>");
	
	cout << "# IMG Banner " << endl;
	BD["arrays"]["Banner"]["files"].clear();
	int sizeI = BD["arrays"]["Banner"]["img"].size();
	for (int i=0; i < sizeI; i++){
		string url = BD["arrays"]["Banner"]["img"][i];
		string name = url;
		replace(name,"https://cdn.jkanime.net/assets/images/animes/video/image/","");
		name = rootdirectory+"DATA/"+name;
		BD["arrays"]["Banner"]["files"].push_back(name);

		CheckImgNet(name,url);
	}
	//cout << std::setw(4) << BD["arrays"]["Banner"] << std::endl;
	return 0;
}

int MkAGR(string content){
	//Get Latest added animes
	cout << "# Get Agregados " << endl;
	BD["arrays"]["Agregados"]["link"]=scrapElementAll(content, "https://jkanime.net/");

	cout << "# IMG Agregados " << endl;
	CheckImgVector(BD["arrays"]["Agregados"]["link"],part);
	return 0;
}
int MkHOR(){
	//load Horario
	cout << "# Get HourGlass " << endl;
	string content=Net::GET("https://jkanime.net/horario/");
	replace(content,"https://jkanime.net/horario/","");
	vector<string> STP={};

	STP=split(content,"<div class='box semana'>");
	BD["arrays"]["HourGlass"]["Lunes"]=scrapElementAll(STP[1],"https://jkanime.net/","\"","Lunes");
	BD["arrays"]["HourGlass"]["Martes"]=scrapElementAll(STP[2],"https://jkanime.net/","\"","Martes");
	BD["arrays"]["HourGlass"]["Miercoles"]=scrapElementAll(STP[3],"https://jkanime.net/","\"","Miercoles");
	BD["arrays"]["HourGlass"]["Jueves"]=scrapElementAll(STP[4],"https://jkanime.net/","\"","Jueves");
	BD["arrays"]["HourGlass"]["Viernes"]=scrapElementAll(STP[5],"https://jkanime.net/","\"","Viernes");
	BD["arrays"]["HourGlass"]["Sabado"]=scrapElementAll(STP[6],"https://jkanime.net/","\"","Sabado");
	BD["arrays"]["HourGlass"]["Domingo"]=scrapElementAll(STP[7],"https://jkanime.net/","\"","Domingo");

	vector<string> MPO={};
	vector<string> HORC={};

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
	cout << "# IMG HourGlass " << endl;
	CheckImgVector(HORC,imgNumbuffer);
	return 0;
}
int MkDIR(){
	//make directory
	if (BD["arrays"]["Directory"]["TimeStamp"].empty()){BD["arrays"]["Directory"]["TimeStamp"]=0;}
	if (BD["arrays"]["Directory"]["InTime"].empty()){BD["arrays"]["Directory"]["InTime"]=0;}
	if (BD["arrays"]["Directory"]["page"].empty()){BD["arrays"]["Directory"]["page"]=1;}
	try{
	
		if ((TimeNow()-BD["arrays"]["Directory"]["TimeStamp"].get<int>()) > U_week){
			
			//Flush Resents by week
			vector<string> ChapLink=BD["arrays"]["chapter"]["link"];
			if (ChapLink.size() > 30){
				ChapLink.erase(ChapLink.begin()+30,ChapLink.end());
				BD["arrays"]["chapter"]["link"]=ChapLink;
			}

			cout << "# Get Directory " << endl;
			vector<string> DIR={};
			vector<string> TDIR={};

			part=1;
			ofall=0;
			while (!quit) {
				string content=Net::GET("https://jkanime.net/directorio/"+to_string(BD["arrays"]["Directory"]["page"].get<int>())+"/");
				replace(content,"https://jkanime.net/directorio/","");
				replace(content,"https://jkanime.net///","");
				TDIR=scrapElementAll(content,"https://jkanime.net/");
				

				if (TDIR.size() > voidd){
					DIR.insert(DIR.end(), TDIR.begin(), TDIR.end());
				} else 
					break;//just in case
				
				string scrap = scrapElement(content, "Resultados Siguientes");
				//cout << scrap << "  # " << to_string(BD["arrays"]["Directory"]["page"]) << endl;
				if (scrap.length() > 0) {
					//some code here soon
				} else {
					break;
				}
				BD["arrays"]["Directory"]["page"]=BD["arrays"]["Directory"]["page"].get<int>()+1;
				ofall++;
			}
			ofall=0;
			if(quit) return false;
			BD["arrays"]["Directory"]["pageT"]=BD["arrays"]["Directory"]["page"];
			BD["arrays"]["Directory"]["page"]=1;
			DIR.erase(unique(DIR.begin(),DIR.end()),DIR.end());
			BD["arrays"]["Directory"]["TimeStamp"]=TimeNow();
			BD["arrays"]["Directory"]["link"]=DIR;
			//cout << BD["arrays"]["Directory"] << endl;
			BD["arrays"]["Directory"]["InTime"]=0;
			write_DB(BD,rootdirectory+"DataBase.json");
		}
	} catch(...) {
		led_on(2);
		cout << "- Thread Chain Error Catched, Get Dir Error" <<std::endl;
		return 0;
	}
	if(quit) return false;
	//Build All Data Base
	if (BD["arrays"]["Directory"]["InTime"].get<int>() == 0)
	{
		cout << "# Cache Directory " << endl;
		try{
			if (DataMaker(BD["arrays"]["Directory"]["link"], part, ofall)){
				BD["arrays"]["Directory"]["InTime"]=1;
				write_DB(AB,rootdirectory+"AnimeBase.json");
			}
		} catch(...) {
			led_on(2);
			cout << "- Thread Chain Error Catched,Dir Error" <<std::endl;
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
		string urldown=BD["arrays"]["downloads"]["queue"][x];
		string namedownload = urldown;
		NameOfLink(namedownload);
		mayus(namedownload);
		DownTitle=namedownload;
		//namedownload = namedownload.substr(0, namedownload.length() - 1);
		BD["arrays"]["downloads"]["log"][x] = ">>>>  "+namedownload;
		mkdir("sdmc:/Videos",0777);
		string directorydownload = "sdmc:/Videos/" +namedownload + ".mp4";

		if (!linktodownoadjkanime(urldown,directorydownload)) {
			serverenlace = "Error de descarga";
			BD["arrays"]["downloads"]["log"][x] = "Error: "+namedownload;
		} else {
			BD["arrays"]["downloads"]["log"][x] = "100% : "+namedownload;
		}
	}
	if(cancelcurl==0) led_on(3); else led_on(0);
	} catch(...) {
		led_on(2);
		cout << "- Thread Download Error Catched" <<std::endl;
		cout << BD["arrays"]["downloads"] << endl;
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

	string texts = BD["searchtext"];
	replace(texts, " ", "_");
	replace(texts, "!", "");
	replace(texts, ";", "");
	if (texts.length() > 0) {
		cout << "# Search: " << texts << endl;
		string content = "";
		int page = 1;
		while (!quit) {
			string tempCont=Net::GET("https://jkanime.net/buscar/" + texts + "/"+to_string(page)+"/");
			content += tempCont;
			string scrap = scrapElement(tempCont, "Resultados Siguientes");
			cout << scrap << "  # " << to_string(page) << endl;
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
			string gdrive = content.substr(val1, val2 - val1);
			BD["arrays"]["search"]["link"].push_back(gdrive);

			val3 = content.find("data-setbg=", val2) + 12;
			val4 = content.find('"', val3);
			string gsearchpreview = content.substr(val3, val4 - val3);
			BD["arrays"]["search"]["images"].push_back(gsearchpreview);
			//cout << gsearchpreview << endl;

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
		led_on(2);
		cout << "- Thread Search Error Catched" <<std::endl;
		cout << BD["arrays"]["search"] << endl;
	}
	reloadingsearch = false;
	DataMaker(BD["arrays"]["search"]["link"], porcentajebufferS, porcentajebufferAllS);
	return 0;
}
int capit(void* data) {//Get chap thread
	if (!Net::HasConnection()) return 0;
	string link = BD["com"]["ActualLink"];
	string name = KeyOfLink(link);
	DataUpdate(link);
	try{
		//cout << BD << endl;
		BD["com"]["sinopsis"] = AB["AnimeBase"][name]["sinopsis"];
		BD["com"]["nextdate"] = AB["AnimeBase"][name]["nextdate"];//"......";
		BD["com"]["generos"] = AB["AnimeBase"][name]["generos"];//"......";
		BD["com"]["Emitido"] = AB["AnimeBase"][name]["Emitido"];
		BD["com"]["enemision"] = AB["AnimeBase"][name]["enemision"];
		mincapit = AB["AnimeBase"][name]["mincapit"];//1;
		maxcapit = AB["AnimeBase"][name]["maxcapit"];//-1;
		//write json
		//write_DB(AB,rootdirectory+"AnimeBase.json");

		//Get Image
		string image = rootdirectory+"DATA/"+name+".jpg";
		if (!AB["AnimeBase"][name]["Image"].empty()) {
			CheckImgNet(image,AB["AnimeBase"][name]["Image"]);
		}
	}catch(...) {
		led_on(2);
		cout << "- Error "+name <<std::endl;
	}
	return 0;
}
int capBuffer (string Tlink) {//anime manager
	int v2 = Tlink.find("/", 20);
	Tlink = Tlink.substr(0, v2 + 1);
	BD["com"]["ActualLink"] = Tlink;

	string name = Tlink;
	replace(name, "https://jkanime.net/", "");
	replace(name, "/", "");
	KeyName = name;
//	cout << "KeyName: " << name << endl;

	cout << "Link: " << Tlink << endl;

	string image = rootdirectory+"DATA/"+name+".jpg";
	statenow = chapterstate;

	if (AB["AnimeBase"][name]["TimeStamp"].empty())
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
			BD["com"]["sinopsis"] = AB["AnimeBase"][name]["sinopsis"];
			BD["com"]["nextdate"] = AB["AnimeBase"][name]["nextdate"];//"......";
			BD["com"]["generos"] = AB["AnimeBase"][name]["generos"];//"......";
			BD["com"]["Emitido"] = AB["AnimeBase"][name]["Emitido"];
			BD["com"]["enemision"] = AB["AnimeBase"][name]["enemision"];
			maxcapit = AB["AnimeBase"][name]["maxcapit"];
			mincapit = AB["AnimeBase"][name]["mincapit"];
			//check For latest cap seend
			if (UD["chapter"][name].empty()||UD["chapter"][name]["latest"].empty()) {
				UD["chapter"].erase(name);
				//get position to the latest cap if in emision
				if (BD["com"]["enemision"] == "true") {
					latest = AB["AnimeBase"][name]["maxcapit"];//is in emision
				} else {
					latest = AB["AnimeBase"][name]["mincapit"];//is not in emision
				}
				latestcolor = -1;
			}
			else{
				latest = UD["chapter"][name]["latest"];
				latestcolor = UD["chapter"][name]["latest"];
			}

			//Get Image
			if (!AB["AnimeBase"][name]["Image"].empty()) {
				CheckImgNet(image,AB["AnimeBase"][name]["Image"]);
			}

			if (AB["AnimeBase"][name]["TimeStamp"] != BD["TimeStamp"]) {
				BD["com"]["nextdate"] = "Loading...";
				capithread = SDL_CreateThread(capit, "capithread", (void*)NULL);
			}
		}catch(...) {
			led_on(2);
			cout << "- Error buff"+name << endl;
		}
	}
	CheckImgNet(image);
	return 0;
}
void DataUpdate(string Link) {//Get info off chapter
	if(quit) return;
	string name = KeyOfLink(Link);
	string a = Net::GET("https://jkanime.net/"+name+"/");
	if(quit) return;
	json AnimeINF=AB["AnimeBase"][name];//
	string TMP="";
	if (AnimeINF["sinopsis"].empty()){
		//Sinopsis
		TMP = scrapElement(a, "<p rel=\"sinopsis\">","</p>");
		replace(TMP, "<p rel=\"sinopsis\">", ""); replace(TMP, "<br/>", ""); replace(TMP, "&quot;", "");
		//AnimeINF["sinopsis"] = TMP.substr(0,800);
		AnimeINF["sinopsis"] = TMP;
	}
	if (AnimeINF["Image"].empty()){
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

		string urlx;
		urlx = (a.substr(val1, val2 - val1));
		val3 = urlx.find(" - ") + 3;
		urlx = urlx.substr(val3);

		AnimeINF["maxcapit"] = atoi(urlx.c_str());
	} else {
		AnimeINF["maxcapit"] = 1;
	}

	if (AnimeINF["mincapit"].empty())
	{
		//empieza por 0?
		int zero1, zero2;
		string zerocontainer = "";
		string zerocontainer2 = "";
		zero1 = a.rfind("ajax/pagination_episodes/");
		zero2 = a.find("'", zero1);
		zerocontainer = "https://jkanime.net/" + a.substr(zero1, zero2 - zero1) + "/1/";
		replace(zerocontainer, "//a", "/a");
		replace(zerocontainer, "//1/", "/1/");
		//cout << zerocontainer << endl;
		zerocontainer2 = Net::GET(zerocontainer);

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
		stringstream strm;
	try{
//		strm << setw(4) << base;
		strm << AnimeINF;
		//write to DB
		AB["AnimeBase"][name]=AnimeINF;
		cout << "Saved: " << name << endl;
	} catch(...) {
		led_on(2);
		cout << "Anime Problemático: "<< name << endl;
		cout << strm.str() << endl;
	}
}

//Favorito
void addFavorite(string text) {
	if(!isFavorite(text)) {
		UD["favoritos"].push_back(text);
		write_DB(UD,rootsave+"UserData.json");
	}
}
void getFavorite() {
	if (UD["favoritos"].empty()) {
		if (isFileExist(rootdirectory+"favoritos.txt")) {
			string temp;
			ifstream file(rootdirectory+"favoritos.txt");
			string str;
			while (getline(file, str)) {
				string strtmp = str;
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
bool isFavorite(string fav){
	if (!UD["favoritos"].empty()) {
		if(find(UD["favoritos"].begin(), UD["favoritos"].end(), fav) != UD["favoritos"].end())
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
