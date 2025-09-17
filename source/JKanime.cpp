#include "Networking.hpp"
#include "applet.hpp"
#include "utils.hpp"
#include "JKanime.hpp"
#include "SDLWork.hpp"
#include "Link.hpp"

extern SDL_Thread* capithread;
enum UnixT {U_day=86400, U_week=604800, U_Month=2419200};
u32 voidd =0;

//Private Functions
bool DataMaker(json LinkList, int& part, int& ofall);
bool DataGeter(json LinkList,int& part, int& ofall);
void DataMakerT(json LinkList);
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
bool ThemeISUpdating = false;
//BEGUING THREAD CHAIN
void ThemeDown(){
    ThemeISUpdating = true;
    if (Net::DOWNLOAD("https://github.com/darkxex/RipJKAnimeNX/raw/master/imgs/themes00.romfs",rootdirectory+"themes00.romfs")) {
        ThemeNeedUpdate = false;
        if (mount_theme("themes",true))
        {
            GOD.SkinInit(roottheme);//Esto añade a la lista de skins
            GOD.SkinInit(oldroot+"theme/",true);//Esto añade a la lista de skins
            GOD.ReloadSkin=true;
        } else
            Mromfs = true;
    }
    ThemeISUpdating = false;
}

int AnimeLoader(void* data){
	int steep=0;
    std::thread themeT;
	try{
		isChained=true;
		ChainManager(true,true);
		steep++;//Wait for connection
		if (BD["arrays"]["chapter"]["link"].is_null()) {
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
			if(quit) throw "Error is AppletMode";;
		}

		//execute this once, or if Mgate is true
		static bool Mgate=true;
		if (Mgate) {
			#if __has_include("Debug.h")
				#include "Debug.h"
			#endif
			int Req = Net::HEAD("https://bvc-hac-lp1.cdn.nintendo.net/18-0-0")["CODE"];
			switch(Req) {
			case 0:
			case 503:
				if (!isSXOS) {
					//Check if dns are correct
					cout << "# Place DNS" << endl;
					fsdevDeleteDirectoryRecursively("sdmc:/atmosphere/hosts");
					mkdir("sdmc:/atmosphere/hosts",0777);
					copy_me("romfs:/default.txt","sdmc:/atmosphere/hosts/default.txt");
					cout << "# Reload DNS" << endl;
					ReloadDNS();
				} else{
					if (AppletMode) InstallNSP("romfs:/05B9DB505ABBE000.nsp");
					//Disable Stealth mode
					//txStealthMode(0);
				}
				break;
			}
            Mgate=false;
		}
		//Check for app Updates
		CheckUpdates(AppletMode);
		steep++;
        //exit thread if are in applet mode
        if (AppletMode) {quit=true; throw "Error is AppletMode";}
                
		steep++;//Get main page
        //GetCookies();//Get Cookies
		json MainPage=Net::REQUEST("https://jkanime.net/");
		//std::cout << std::setw(4) << MainPage << std::endl;
		//Check headers ToDo
		string content = MainPage["BODY"];
        int numCode = MainPage["CODE"];
        switch (numCode) {
            case 0:
                cout << "- Error can't connect with Web #" << numCode << endl;
                throw "Error Connect";
                break;
            case 403:
            case 503:
                cout << "- Error cloudflare active in Web #" << numCode << endl;
                //Esto dará una visual al usuario que no se puede entrar por cloudflare
                ClFlock = true;
                //Si se me permite luego y si el navegador de la switch lo admite
                //quisiera extraer los cookies para poder usar la web icluso si cloudflare esta activo
                throw "Error cloudflare active";
                break;
			
			case 200:
				cout << "# Web State #" << numCode << endl;
				break;

            default:
                cout << "# Web State #" << numCode << endl;
				throw "Error unknow state";
                break;
        }

        //Aqui creamos otra ramificación para no detener el cargador de animes -.-
		if (Mromfs) {
			//Download themes
			if (!mount_theme("themes",true) || ThemeNeedUpdate)
			{
                if (!ThemeISUpdating){
                   mount_theme("themes",false);
                   themeT = std::thread(ThemeDown);
                }
			}
            Mromfs = false;
		}
		//Obtener UserAgent si se cambia de versión
		if (BD["Firm"].is_null()){
			UAG=true;
		} else {
			if (BD["Firm"].get<string>() != DInfo()["Firmware"].get<string>()){
				UAG=true;
			}
		}

		steep++;
		if(!reloading) {
			//Download All not existing images
			//CheckImgVector(BD["arrays"]["chapter"]["images"],imgNumbuffer);
		}

		steep++;//Get Programation list, Links and Images
		int temp0=0,temp1=0;
		temp0=content.find("Programación");
		temp1=content.find("Animes recientes",temp0);
		string temcont = content.substr(temp0,temp1-temp0);
		steep++;//rebuild list
		vector<string> ChapLink=scrapElementAll(temcont, "https://jkanime.net/");
		vector<string> ChapImag=scrapElementAll(temcont, "https://"+CDNURL+"/assets/images/");
		BD["arrays"]["chapter"]["date"]=scrapElementAll(temcont, "</i>","</span>");

		steep++;//Download All not existing images
		cout << "# Recent IMG " << endl;
		CheckImgVector(ChapLink,imgNumbuffer);


		steep++;//'haschange' See if there is any new chap
		bool haschange = true;
		if (!BD["latestchapter"].is_null()) {
			if (BD["latestchapter"] == ChapLink[0] && !reloading)
			{
				haschange = false;
			}
		}
		steep++;//Display List
		if(reloading) {Frames=0; reloading = false;}

		if (haschange || BD["TimeStamp"].is_null()) {
			//update TimeStamp
            bool areNEQ = true;
            if (BD["arrays"]["chapter"]["link"].size() > 0) areNEQ = BD["arrays"]["chapter"]["link"][0] != ChapLink[0];
                
			if (BD["arrays"]["chapter"]["link"].is_null() || areNEQ || BD["TimeStamp"].is_null()) {

				BD["TimeStamp"] = to_string(TimeNow());
				cout << "# New TimeStamp: " << BD["TimeStamp"] << endl;

				//merge vectors
				if (!BD["arrays"]["chapter"]["link"].is_null())
				{
					vector<string> OChapLink=BD["arrays"]["chapter"]["link"];
                    if (OChapLink.size() > 0) {
                        cout << "# Merge: chapter link <-- 30 List in web" << endl;
                        ChapLink.erase(find(ChapLink.begin(), ChapLink.end(), OChapLink[0]),ChapLink.end());
                        ChapLink.insert(ChapLink.end(), OChapLink.begin(), OChapLink.end());
                        //if (ChapLink.size() > 100) {ChapLink.erase(ChapLink.begin()+100,ChapLink.end());} // esto  es para controlar el tamaño máximo de la lista de animes
                    }
				}
				/*
				if (!BD["arrays"]["chapter"]["images"].is_null())
				{
					vector<string> OChapImag=BD["arrays"]["chapter"]["images"];
                    if (OChapImag.size() > 0) {
                        cout << "# Merge: chapter images <-- 30 List in web" << endl;
                        ChapImag.erase(find(ChapImag.begin(), ChapImag.end(), OChapImag[0]),ChapImag.end());
                        ChapImag.insert(ChapImag.end(), OChapImag.begin(), OChapImag.end());
                        //if (ChapImag.size() > 100) {ChapImag.erase(ChapImag.begin()+100,ChapImag.end());} // esto  es para controlar el tamaño máximo de la lista de animes
                    }
				}
				*/
				DoubleKill(ChapLink);
				//ChapImag.erase(unique(ChapImag.begin(),ChapImag.end()),ChapImag.end());

				GOD.PlayEffect(GOD.proc);
				Frames=1;
                if (ChapLink.size() > 0) BD["arrays"]["chapter"]["link"]=ChapLink;
                //if (ChapImag.size() > 0) BD["arrays"]["chapter"]["images"]=ChapImag;
				haschange = true;
			}

		} else {
			cout << "# TimeStamp: " << BD["TimeStamp"] << endl;
		}

		steep++;//Get history
		cout << "# History IMG " << endl;
		CheckImgVector(UD["history"],imgNumbuffer);
		steep++;//Agregados to Database
		temp0=content.find("Animes recientes");
		temp1=content.find("Top animes",temp0);
		temcont = content.substr(temp0,temp1-temp0);

		MkAGR(temcont);

		steep++;//banner to Database /* */

		if (haschange)
		{
			//Borrar Archivos temporales
            fsdevDeleteDirectoryRecursively((rootdirectory+"TEMP").c_str());
            mkdir((rootdirectory+"TEMP").c_str(), 0777);

		}

		//if (haschange)
        {
            temp0=content.find("<section class=\"hero\">");
            temp1=content.find("</section>",temp0);
            temcont = content.substr(temp0,temp1-temp0);
            MkBNR(temcont);
		}
		//return 0;
        
//cout << "# HERE 1 " << endl;
        //hourglas
		if (haschange) {
			MkHOR();
		}
//cout << "# HERE 2 " << endl;
		
		steep++;//Top, Hour to Database
		MkTOP();


		steep++;//Download All not existing images of Favorites
		if(UD["favoritos"].is_null()) {
			cout << "# Get fav list " << endl;
			getFavorite();
			cout << "# Goted fav list " << endl;
		}
		cout << "# favoritos IMG ";
		CheckImgVector(UD["favoritos"],porcentajebufferF);

		cout << "# End Image Download " << endl;


		steep++;//Load to cache all Programation Chaps
		cout << "# Cache Recent ";
		if (haschange) {
			cout << " , haschange ";
			if(DataMaker(BD["arrays"]["chapter"]["link"], part, ofall)) {
				BD["latestchapter"] = BD["arrays"]["chapter"]["link"][0];
			}
		}

		steep++;//Load to cache all Favorites Chaps
		cout << "| favoritos ";
		DataMaker(UD["favoritos"], part, ofall);

		steep++;//Cache Top
		cout << "| Top ";
		DataMaker(BD["arrays"]["Top"]["link"], part, ofall);

		steep++;//Cache Horario
		cout << "| HourGlass " <<endl;;
		DataMaker(BD["arrays"]["HourGlass"]["link"], part, ofall);

		steep++;//Load Directory
		//MkDIR();
    /*} catch(const char* errorMessage) {
        std::cout << "Error: " << errorMessage << std::endl;
    } catch(const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;*/
    } catch(...) {
		LOG::E(2);
		cout << "- Thread Chain Error Catched, Steep#" << steep <<endl;
		//appletOverrideAutoSleepTimeAndDimmingTime(1800, 0, 500, 0);
		//cout << UD << endl;
	}
	if(quit) write_DB(AB,rootdirectory+"AnimeMeta.json");
    if (themeT.joinable()) {themeT.join();}
	cout << "# End Thread Chain" << endl;
	ChainManager(false,!isDownloading&&!isChained);
	isChained=false;
	return 0;

}
/*
bool DataGeter(json LinkList,int& part, int& ofall){
	if(LinkList.is_null()) {
		return false;
	}
	std::thread chaps1;
	std::thread chaps2;

	vector<string> chain1 = LinkList;
	vector<string> chain2 = LinkList;

	std::reverse(chain1.begin(), chain1.end());

	std::default_random_engine e(time(0));
	std::shuffle(std::begin(chain1), std::end(chain1), e);
	std::shuffle(std::begin(chain2), std::end(chain2), e);

	chaps1 = std::thread (DataMakerT,chain1);
	chaps2 = std::thread (DataMakerT,chain2);

	bool mik = DataMaker(LinkList,part, ofall);

	std::cout << ">>> Whait for thread"  << std::endl;
	if (chaps1.joinable()) {chaps1.join();}
	if (chaps2.joinable()) {chaps2.join();}
	return mik;
}
void DataMakerT(json LinkList) {
	int mytotal;
	string a = "";
	//int sep=0;
	mytotal=LinkList.size();
	if (mytotal <= 0) {return;}
	try{

		for (int x = 0; x < mytotal&& !quit; x++)
		{
			if (!isConnected) while (!Net::HasConnection()) {SDL_Delay(2000); if(quit) return; }
			string link = LinkList[x];
			string name = KeyOfLink(link);
			if (AB[name]["TimeStamp"] != BD["TimeStamp"] ) {
				if (AB[name]["TimeStamp"].is_null() || AB[name]["enemision"]=="true") {
					DataUpdate(link);
				} else {

				}
			}
		}
    } catch(const char* errorMessage) {
        std::cout << ">Error: " << errorMessage << std::endl;
    } catch(const std::exception& e) {
        std::cout << ">>Error: " << e.what() << std::endl;
    } catch(...) {
		cout << "- Thread Search Error Catched" <<endl;
		cout << BD["arrays"]["search"] << endl;
	}
	return;
}
*/

bool DataMaker(json LinkList,int& part, int& ofall) {
	bool hasmchap=false;
	string a = "";
	part=0;
	//int sep=0;
	ofall=LinkList.size();
	if (ofall <= 0) {return false;}
	for (int x = 0; x < ofall&& !quit; x++)
	{
		if(hasmchap) part = x+1;
		if (!isConnected) while (!Net::HasConnection()) {SDL_Delay(2000); if(quit) return false; }
		string link = LinkList[x];
		string name = KeyOfLink(link);
		if (AB[name]["TimeStamp"] != BD["TimeStamp"] ) {
			if (AB[name]["TimeStamp"].is_null() || AB[name]["enemision"]=="true") {
				part = x+1;
				DataUpdate(link);
				hasmchap=true;
				/*
				   if (sep >= 101){
				        //write_DB(BD,"sdmc:/DataBase.json");
				        write_DB(AB,rootdirectory+"AnimeMeta.json");
				        sep=0;
				   }

				   sep++;
				 */
			} else {
				//AB[name]["TimeStamp"] = BD["TimeStamp"];
			}
		}
	}
	part=0;
	ofall=0;
	if(quit) return false;
	if(hasmchap) {
		write_DB(AB,rootdirectory+"AnimeMeta.json");//write json
	}
	return true;
}
int MkTOP(){
	//load Top
	vector<string> TOPC={};
	int temp0=0,temp1=0;
	cout << "# Top Get ";
	string content=Net::GET("https://jkanime.net/top/");
	temp0=content.find("<form id=\"niceform\"");
	temp1=content.find("<div class=\"searchbg\"",temp0);
	content = content.substr(temp0,temp1-temp0);

	
	replace(content,"https://jkanime.net/top/","");
	replace(content,"https://jkanime.net/top","");
	replace(content,"https://jkanime.net///","");
	TOPC=scrapElementAll(content,"https://jkanime.net/");
	TOPC.erase(unique(TOPC.begin(),TOPC.end()),TOPC.end());
	BD["arrays"]["Top"]["link"]=TOPC;

	cout << "IMG " << endl;
	CheckImgVector(TOPC,imgNumbuffer);
	return 0;
}
int MkBNR(string content){
	//Get Latest added animes
	cout << "# Banner Get ";
    string imgurl = "https://"+CDNURL+"";
	
	vector<string> BannerElem={};
	BannerElem=split(content,"<div class=\"hero__items set-bg\"");
    bool hasbaner = false;
	cout << "IMG " << endl;
	vector<string> BannerFile;
	int sizeI = BannerElem.size();
    json banerall = "[]"_json;
	for (int i=0; i < sizeI; i++) {
		string tCont = BannerElem[i];
//cout << tCont << endl;
        string name = scrapElement(tCont, "<h2>","</h2>");
        string link = scrapElement(tCont, "href=\"","\"");
        string img = scrapElement(tCont, ""+imgurl+"/assets/images/animes/video/image/","\"");
        //string img = scrapElement(tCont, "data-setbg=\"","\"");
        string file = rootdirectory+"TEMP/"+img.substr(img.rfind("/")+1);;
        
        replace(link,"href=\"","");
        replace(img,"data-setbg=\"","");
        replace(name,"<h2>","");
        
        if (img.length() > 0 &&link.length() > 0){
            json dataT;
            dataT["name"] = name;
            dataT["link"] = link;
            dataT["img"] = img;
            dataT["file"] = file;
            CheckImgNet(file,img);
            banerall.push_back(dataT);
            hasbaner = true;
        }
        /*
        		TMP = scrapElement(a, "<p rel=\"sinopsis\">","</p>");
    //cout << "Banner" << content << endl;
	//BD["arrays"]["Be nner"]["link"]=
	//BD["arrays"]["Be nner"]["img"]=scrapElementAll(content, imgurl+"/assets/images/animes/video/image/");//https://"+CDNURL+"/assets/images/animes/video/image/jkvideo_85d3aaa2683a8047cc191aa9a39d5d8d.jpg
	//BD["arrays"]["Be nner"]["name"]=scrapElementAll(content, "<h2>","</h2>");
scrapElementAll(content, "data-setbg=\""+imgurl+"/assets/images/animes/video/image/","\"");
            temp0=content.find("<section class=\"hero\">");
            temp1=content.find("<div class=\"solopc\">",temp0);
            temcont = content.substr(temp0,temp1-temp0);
            
		string name = url;
		replace(name,imgurl+"/assets/images/animes/video/image/","");
		name = rootdirectory+"TEMP/"+name;
		BannerFile.push_back(name);

		CheckImgNet(name,url);
        
        */
	}
    if (hasbaner){
        BD["arrays"]["Benner"] = banerall;
    }

	//cout << setw(4) << BD["arrays"]["Benner"] << endl;
	return 0;
}
int MkAGR(string content){
	//Get Latest added animes
	cout << "# Agregados Get ";
	BD["arrays"]["Agregados"]["link"]=scrapElementAll(content, "https://jkanime.net/");

	cout << "IMG " << endl;
	CheckImgVector(BD["arrays"]["Agregados"]["link"],part);
	return 0;
}
int MkHOR(){
	//load Horario
	cout << "# HourGlass Get ";
	string content=Net::GET("https://jkanime.net/horario/");
	replace(content,"https://jkanime.net/horario/","");
	/*
	replace(content,"https://"+CDNURL+"/assets/images/animes/image/","https://jkanime.net/");
	replace(content,".jpg","/");
	replace(content,".png","/");
	*/
	
	//poner limite a split esto hace q STP tenga 7 valores mas unos mas para el resto de la pagina  
	replace(content,"<div class='box semana filtro'>","<div class='box semana'>");
	replace(content,"Buscar anime","<div class='box semana'>");
	
	
	//<div class="box semana filtro"
	
	//dividimos en dias 7 para lunes a domingo
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

/*
	MPO=BD["arrays"]["HourGlass"]["Lunes"];
	HORC.insert(HORC.end(), MPO.begin(), MPO.end());
	MPO=BD["arrays"]["HourGlass"]["Martes"];
	HORC.insert(HORC.end(), MPO.begin(), MPO.end());
	MPO=BD["arrays"]["HourGlass"]["Miercoles"];
	HORC.insert(HORC.end(), MPO.begin(), MPO.end());
	MPO=BD["arrays"]["HourGlass"]["Jueves"];
	HORC.insert(HORC.end(), MPO.begin(), MPO.end());
	MPO=BD["arrays"]["HourGlass"]["Viernes"];
	HORC.insert(HORC.end(), MPO.begin(), MPO.end());
	MPO=BD["arrays"]["HourGlass"]["Sabado"];
	HORC.insert(HORC.end(), MPO.begin(), MPO.end());
	MPO=BD["arrays"]["HourGlass"]["Domingo"];
	HORC.insert(HORC.end(), MPO.begin(), MPO.end());
*/
	//unimos todos en un solo vector asi podemso ver todos a la vez
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
	MPO=scrapElementAll(STP[6],"https://jkanime.net/","\"","Sabado");
	HORC.insert(HORC.end(), MPO.begin(), MPO.end());
	MPO=scrapElementAll(STP[7],"https://jkanime.net/","\"","Domingo");
	HORC.insert(HORC.end(), MPO.begin(), MPO.end());

	//HORC.erase(unique(HORC.begin(),HORC.end()),HORC.end());
	BD["arrays"]["HourGlass"]["link"]=HORC;
	BD["arrays"]["HourGlass"]["Todos"]=HORC;
	cout << "IMG " << endl;
	CheckImgVector(HORC,imgNumbuffer);
	return 0;
}
int MkDIR(){
    //Esto genera y actualiza la base de datos de los animes
	if (BD["arrays"]["Directory"]["TimeStamp"].is_null()) {BD["arrays"]["Directory"]["TimeStamp"]=0;}
	if (BD["arrays"]["Directory"]["InTime"].is_null()) {BD["arrays"]["Directory"]["InTime"]=0;}
	if (BD["arrays"]["Directory"]["page"].is_null()) {BD["arrays"]["Directory"]["page"]=1;}
	if (BD["arrays"]["Directory"]["link"].is_null()) {BD["arrays"]["Directory"]["link"]={};}
	try{
        //La base de datos se re escanea cada mes pero también se va actualizando según salen animes
		if ((TimeNow()-BD["arrays"]["Directory"]["TimeStamp"].get<int>()) > U_Month || BD["arrays"]["Directory"]["link"].size() == 0) {

            //Aquí se obtiene una lista de todos los animes para luego procesar uno por uno
			cout << "# Directory Get ";
			vector<string> DIR={};
			vector<string> TDIR={};

			part=1;
			ofall=0;
			while (!quit) {
				string content=Net::GET("https://jkanime.net/directorio?p="+to_string(BD["arrays"]["Directory"]["page"].get<int>()));
				content = scrapElement(content, "var animes=","var mode");

				replace(content,";","");
				replace(content,"var animes=","");
				
				replace(content,"card-title-alt\"><a href=\"https://jkanime.net/","");
				replace(content,"card-title\"><a href=\"https://jkanime.net/","");
				replace(content,"https://jkanime.net///","");
				TDIR=scrapElementAll(content,"https://jkanime.net/");


				if (TDIR.size() > 0) {
					DIR.insert(DIR.end(), TDIR.begin(), TDIR.end());
				} else
					break; //Por si acaso

                //verificar si ya no hay mas animes que listar
				if (content.find("Resultados Siguientes &raquo;") != string::npos) {
					cout << "  #" << to_string(ofall);
					//some code here soon
				} else {
					cout << "end  #" << to_string(ofall) << endl;
					break;
				}
				BD["arrays"]["Directory"]["page"]=BD["arrays"]["Directory"]["page"].get<int>()+1;
				ofall++;
			}
			if(quit) { ofall=0; return false;}//si la app se cierra por el usuario dejar esto en espera para continuar luego
            //
			BD["arrays"]["Directory"]["pageT"]=BD["arrays"]["Directory"]["page"];
			BD["arrays"]["Directory"]["page"]=1;
			DIR.erase(unique(DIR.begin(),DIR.end()),DIR.end());//borrar cosas repetidas
            //cerrar escaneo de directorio por un mes
			if (ofall != 0 && DIR.size() != 0) {
				BD["arrays"]["Directory"]["TimeStamp"]=TimeNow();
			}
			BD["arrays"]["Directory"]["link"]=DIR;
			//cout << BD["arrays"]["Directory"] << endl;
			BD["arrays"]["Directory"]["InTime"]=0;
			ofall=0;
			write_DB(BD,rootdirectory+"DataBase.json");
		}
    } catch(const char* errorMessage) {
        std::cout << "Error: " << errorMessage << std::endl;
    } catch(const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    } catch(...) {
		LOG::E(3);
		cout << "- Thread Chain Error Catched, Get Dir Error" <<endl;
		return 0;
	}
	if(quit) return false;
	//Con la lista de arriba hecha pasamos a procesar todos los elementos de la lista
	if (BD["arrays"]["Directory"]["InTime"].get<int>() == 0)
	{
		cout << "# Cache Directory " << endl;
		try{
			if (DataMaker(BD["arrays"]["Directory"]["link"], part, ofall)) {
				BD["arrays"]["Directory"]["InTime"]=1;
				write_DB(AB,rootdirectory+"AnimeMeta.json");
			}
		} catch(...) {
			LOG::E(4);
			cout << "- Thread Chain Error Catched,Dir Error" <<endl;
			//write_DB(BD,rootdirectory+"DataBase.json");
			return 0;
		}
        
        //Esta Parte limpia a lista de animes recientes a 60 elementos (no queremos que esta se extienda hasta el infinito)
        vector<string> ChapLink=BD["arrays"]["chapter"]["link"];
        if (ChapLink.size() > 60) {
            ChapLink.erase(ChapLink.begin()+60,ChapLink.end());
            BD["arrays"]["chapter"]["link"]=ChapLink;
        }
		/*
        vector<string> ChapImg=BD["arrays"]["chapter"]["images"];
        if (ChapImg.size() > 60) {
        	ChapImg.erase(ChapImg.begin()+60,ChapImg.end());
        	BD["arrays"]["chapter"]["images"]=ChapImg;
        }
		*/
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
    } catch(const char* errorMessage) {
        std::cout << "Error: " << errorMessage << std::endl;
    } catch(const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    } catch(...) {
		LOG::E(5);
		cout << "- Thread Download Error Catched" <<endl;
		cout << BD["arrays"]["downloads"] << endl;
	}
	cancelcurl = 0;
	isDownloading=false;
	statenow = download_s;
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
			string content = Net::GET("https://jkanime.net/buscar/" + texts);
			
			/*
			for (int c=1; c < 10 && !quit; c++) {
				string tempCont=Net::GET("https://jkanime.net/buscar/" + texts + "/"+to_string(c)+"/");
				content += tempCont;

				if (tempCont.find("Resultados Siguientes &raquo;") != string::npos) {
					cout << "  #" << to_string(c);
					//some code here soon
				} else {
					cout << "  #" << to_string(c) << endl;
					break;
				}
			}
			*/

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
			statenow = programation_s;
			returnnow = programation_s;
		}
    } catch(const char* errorMessage) {
        std::cout << "Error: " << errorMessage << std::endl;
    } catch(const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    } catch(...) {
		LOG::E(6);
		cout << "- Thread Search Error Catched" <<endl;
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
		BD["com"]["sinopsis"] = AB[name]["sinopsis"];
		BD["com"]["nextdate"] = AB[name]["nextdate"];//"......";
		BD["com"]["generos"] = AB[name]["generos"];//"......";
		BD["com"]["Emitido"] = AB[name]["Emitido"];
		BD["com"]["enemision"] = AB[name]["enemision"];
		BD["com"]["Estado"] = AB[name]["Estado"];
		mincapit = AB[name]["mincapit"];//1;
		maxcapit = AB[name]["maxcapit"];//-1;
		//write json
		//write_DB(AB,rootdirectory+"AnimeMeta.json");

		//Get Image
		string image = rootdirectory+"DATA/"+name+".jpg";
		if (!AB[name]["Image"].is_null()) {
			CheckImgNet(image,AB[name]["Image"]);
		}
    } catch(const char* errorMessage) {
        std::cout << "Error: " << errorMessage << std::endl;
    } catch(const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    } catch(...) {
		LOG::E(7);
		cout << "- Error "+name <<endl;
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
	statenow = chapter_s;

	if (AB[name]["TimeStamp"].is_null())
	{
		BD["com"]["sinopsis"] = " ";
		BD["com"]["nextdate"] = " ";
		BD["com"]["generos"] = " ";
		BD["com"]["Emitido"] = " ";
		BD["com"]["Estado"] = " ";
		maxcapit = -1;
		mincapit = 1;
		latest = 1;
		capithread = SDL_CreateThread(capit, "capithread", (void*)NULL);
	} else {
		try{
            if (!AB[name]["sinopsis"].is_null()) {
                BD["com"]["sinopsis"] = AB[name]["sinopsis"];
            } else {
                BD["com"]["sinopsis"] = " ";
            }
			BD["com"]["nextdate"] = AB[name]["nextdate"];//"......";
			BD["com"]["generos"] = AB[name]["generos"];//"......";
			BD["com"]["Emitido"] = AB[name]["Emitido"];
			BD["com"]["enemision"] = AB[name]["enemision"];
			BD["com"]["Estado"] = AB[name]["Estado"];
			maxcapit = AB[name]["maxcapit"];
			mincapit = AB[name]["mincapit"];
			//check For latest cap seend
			if (!isset(UD["chapter"],name) || !isset(UD["chapter"][name],"latest")) {
				//UD["chapter"].erase(name);
				//get position to the latest cap if in emision
				if (BD["com"]["enemision"] == "true") {
					latest = AB[name]["maxcapit"];//is in emision
				} else {
					latest = AB[name]["mincapit"];//is not in emision
				}
				latestcolor = -1;
			}
			else{
				latest = UD["chapter"][name]["latest"];
				latestcolor = UD["chapter"][name]["latest"];
			}

			//Get Image
			if (!AB[name]["Image"].is_null()) {
				CheckImgNet(image,AB[name]["Image"]);
			}

			if (AB[name]["TimeStamp"] != BD["TimeStamp"]) {
				BD["com"]["nextdate"] = "Loading...";
				capithread = SDL_CreateThread(capit, "capithread", (void*)NULL);
			}
		}catch(...) {
			LOG::E(8);
			cout << "- Error buff"+name << endl;
		}
	}
	CheckImgNet(image);
	return 0;
}
void DataUpdate(string Link) {//Get info off chapter
	if(quit) return;
	string name = KeyOfLink(Link);
    Link = "https://jkanime.net/"+name+"/";
    if (Link == "https://jkanime.net/studio/") return;
    json DataPage=Net::REQUEST(Link);
	SDL_Delay(50);
	
	while(!quit){
		if (DataPage["CODE"] == 429){
			cout << "Firewall detected halt 60s " << Link << endl;
			cout << DataPage["BODY"] << endl;
			genTime(60);
			DataPage=Net::REQUEST(Link);
		}
		if (DataPage["CODE"] == 200){
			break;
		}
	}
	if(quit) return;
/* */ 
    cout << "-------------------- " << Link << endl;
    cout << "Link " << Link << endl;
    cout << "COUNT " << DataPage["COUNT"] << endl;
    cout << "RED " << DataPage["RED"] << endl;
    cout << "CODE " << DataPage["CODE"] << endl;
  
    if (DataPage["COUNT"] > 0 && DataPage["RED"] == "https://jkanime.net/404.shtml"){
        //puede darse el caso de que aparezca un link venenoso 
        //y los thread no logran manejarlo y peta todo
        cout << "Poison " << Link << endl;
        return;
    }

	string a = DataPage["BODY"];
	if(quit) return;
	
	json AnimeINF;//
	if(!AB[name].is_null()){
		AnimeINF=AB[name];
	}
	//
	string TMP="";
	if (AnimeINF["sinopsis"].is_null()) {
		//Sinopsis
		//TMP = scrapElement(a, "<p rel=\"sinopsis\">","</p>");
		TMP = scrapElement(a, "<p class=\"scroll\">","</p>");
		replace(TMP, "<p class=\"scroll\">", ""); replace(TMP, "\n", ""); replace(TMP, "<br/>", ""); replace(TMP, "&quot;", "'"); replace(TMP, "&#039;", "'");
		//AnimeINF["sinopsis"] = TMP.substr(0,800);
		AnimeINF["sinopsis"] = TMP;
	} else {
        //cout << "-- HERE1 " <<endl;
        string taser = AnimeINF["sinopsis"];
        //cout << "-- HERE2 " <<endl;
        if(taser.length() < 8){
            //Sinopsis
            //TMP = scrapElement(a, "<p rel=\"sinopsis\">","</p>");
            TMP = scrapElement(a, "<p class=\"scroll\">","</p>");
            replace(TMP, "<p class=\"scroll\">", ""); replace(TMP, "<br/>", ""); replace(TMP, "&quot;", "");
            //AnimeINF["sinopsis"] = TMP.substr(0,800);
            AnimeINF["sinopsis"] = TMP;
        }
    }
	if (AnimeINF["Image"].is_null()) {
		//get image
		TMP = scrapElement(a, "https://"+CDNURL+"/assets/images/animes/image/");
		AnimeINF["Image"] = TMP;
	}

	if (AnimeINF["Tipo"].is_null()) {
		//Anime info
		TMP = scrapElement(a, "<span>Tipo:","</li");
		replace(TMP, "<span>Tipo:", ""); replace(TMP, "</span> ", ""); replace(TMP, "</span>", "");
		AnimeINF["Tipo"] = TMP;
	}

	if (AnimeINF["Idiomas"].is_null()) {
		TMP = scrapElement(a, "Idiomas:","</li");
		replace(TMP, "Idiomas:", ""); replace(TMP, "  ", " "); replace(TMP, "</span> ", ""); replace(TMP, "</span>", "");
		AnimeINF["Idiomas"] = TMP;
	}

	TMP = scrapElement(a, "Episodios:","</li");
	replace(TMP, "Episodios:", ""); replace(TMP, "</span> ", ""); replace(TMP, "</span>", "");
	AnimeINF["Episodios"] = TMP;
	AnimeINF["maxcapit"] = atoi(TMP.c_str());

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
	TMP = " ";
	if ((int)a.find("<span>Tipo:</span> Pelicula</li>") != -1)
	{
		//es una peli ?
		TMP="Pelicula";
	} else {
		//find next date
		int re1 =0, re2=0;
		re1 = a.find("<b>Próximo episodio:</b>");
		if(re1 > 1) {
			re1 += 25;
			re2 = a.find("<i class", re1);
			TMP = " "+a.substr(re1, re2 - re1);
			RemoveAccents(TMP);
		}
	}
	AnimeINF["nextdate"] = TMP;

	if (AnimeINF["generos"].is_null()) {
		//Generos
		int indx1 = 1, indx2, indx3;
		indx1 = a.find("<span>Generos:</span>", indx1);
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

		if (TMP.length() == 0) TMP +=" ";
		AnimeINF["generos"] = TMP;
	}

	//Esta en emision?
	if (a.find(">Concluido<") != string::npos) {
		AnimeINF["enemision"] = "false";
		AnimeINF["Estado"] = "Concluido";
	} else {
		AnimeINF["enemision"] = "true";
		AnimeINF["Estado"] = "En Emisión";
		AnimeINF["maxcapit"] = 99;//hardcoded
		// si esta en emision tratamos de obtener el ultimo cap
		TMP = scrapElement(a, "<b>Último episodio</b>:","</a>");
		std::cout << "--:" << TMP << std::endl;

		if (TMP.length() > 0){
			TMP = scrapElement(TMP, "https://jkanime.net/","\"");
			std::cout << "--:" << TMP << std::endl;

			replace(TMP, Link, "");
			std::cout << "--:" << TMP << std::endl;
			replace(TMP, "/", "");
			std::cout << "--:" << TMP << std::endl;
			std::cout << "Capitulo Maximo:" << TMP << std::endl;
			AnimeINF["maxcapit"] =  atoi(TMP.c_str());
		}

		if (a.find(">Por estrenar<") != string::npos) {
			AnimeINF["Estado"] = "Por estrenar";
		}
		if (a.find(">En espera<") != string::npos) {
			AnimeINF["Estado"] = "En espera";
		}
	}
	/*


	 */
	//Get Caps number
	/*
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
	*/

	//cout << ">>>>>>>>>>> HERE! 1 --> " << Link << endl;

	if (AnimeINF["mincapit"].is_null())
	{
		//empieza por 0?
		//https://jkanime.net/img/404.png
		
		std::string haszero = Net::GET(Link+"0/");
		if (haszero.find("https://jkanime.net/img/404.png") != string::npos) {
			AnimeINF["mincapit"] = 1;
		} else {
			AnimeINF["mincapit"] = 0;
			AnimeINF["maxcapit"] = AnimeINF["maxcapit"].get<int>() - 1;
		}

		/*
		
		int zero9 = a.rfind("");

		
		int zero1, zero2;
		string zerocontainer = "";
		string zerocontainer2 = "";
		zero1 = a.rfind("/ajax/episodes/");
		zero2 = a.find("'", zero1);
		
		if (zero1 > 0 && false){
			zerocontainer = "https://jkanime.net" + a.substr(zero1, zero2 - zero1) + "/1";
			replace(zerocontainer, "//a", "/a");
			replace(zerocontainer, "//1", "/1");
			
			std::string tokenT = scrapElement(a, "<meta name=\"csrf-token\" content=\"","\"");
			replace(tokenT, "<meta name=\"csrf-token\" content=\"", "");
			std::string token = "_token="+tokenT;//"_token=biljhxPAv91RXjw2NjS3wTdbpu5B0mqdfjWAVjoy";
			//cout << zerocontainer << endl;
			zerocontainer2 = Net::POST(zerocontainer,token);

			cout << tokenT << " Token " << name << endl;
			cout << token << " Token " << name << endl;
			cout << zerocontainer << " " << name << endl;
			cout << zerocontainer2 << " " << name << endl;
			
		} else {
			//cout << ">>>>>>>>>>>  " << a << endl;

			cout << ">>>>>>>>>>> Error Catched --> " << zero1 << " --> "<< zero2 << endl;
		}

		if (zerocontainer2.find("\"number\":\"0\"") != string::npos) {
			AnimeINF["mincapit"] = 0;
			AnimeINF["maxcapit"] = AnimeINF["maxcapit"].get<int>() - 1;
		} else {
			AnimeINF["mincapit"] = 1;
		}
		*/
	}
	//cout << ">>>>>>>>>>> HERE! 2 --> " << Link << endl;
/*
	if(AnimeINF["mincapit"].get<int>()==0)
		AnimeINF["maxcapit"] = AnimeINF["maxcapit"].get<int>() - 1;
*/

	AnimeINF["TimeStamp"] = BD["TimeStamp"];
	stringstream strm;
	try{
//		strm << setw(4) << base;
		strm << AnimeINF;
		//write to DB
		AB[name]=AnimeINF;
		cout << "Saved: " << name << endl;
    } catch(const char* errorMessage) {
        std::cout << "Error: " << errorMessage << std::endl;
    } catch(const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    } catch(...) {
		LOG::E(9);
		cout << "Anime Problemático: "<< name << endl;
		cout << strm.str() << endl;
	}
}

//Favorito
void addFavorite(string text) {
	if(!isFavorite(text)) {
		UD["favoritos"].push_back(text);
	}
}
void getFavorite() {
	if (UD["favoritos"].is_null()) {
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
		}
	}
}
bool isFavorite(string fav){
	if (!UD["favoritos"].is_null()) {
		if(find(UD["favoritos"].begin(), UD["favoritos"].end(), fav) != UD["favoritos"].end())
		{
			return true;
		}
	}
	return false;
}
void delFavorite(int inst){
	if (!UD["favoritos"].is_null()) {
		UD["favoritos"].erase(inst);
	}
}
