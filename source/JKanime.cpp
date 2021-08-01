#include "Networking.hpp"
#include "applet.hpp"
#include "utils.hpp"
#include "JKanime.hpp"
#include "SDLWork.hpp"

extern json BigData;
extern std::string temporallink;
extern std::string sinopsis;
extern std::string nextdate;
extern int mincapit;
extern int maxcapit;
extern int capmore;
extern bool enemision;
extern std::string generos;
extern std::string rootdirectory;
extern SDL_Thread* capithread;
extern bool gFAV;
extern std::string tempimage;

extern LTexture gTextTexture;
extern LTexture Farest;
extern LTexture Heart;
extern LTexture TChapters;
extern LTexture TPreview;
extern LTexture TSearchPreview;///////
extern LTexture TFavorite;
extern int sizeportraity;
extern int sizeportraitx;

//img
extern bool reloading;
extern bool preview;
extern int selectchapter;
extern int porcentajereload;
extern int imgNumbuffer;
extern int porcentajebuffer;
extern int porcentajebufferA;
extern bool activatefirstimage;
extern std::string serverenlace;
extern std::string DownTitle;
extern bool quit;
extern bool isDownloading;
extern int porcendown;
extern int cancelcurl;
extern bool AppletMode;

enum states { programationstate, downloadstate, chapterstate, searchstate, favoritesstate };
enum statesreturn { toprogramation, tosearch, tofavorite };
extern int statenow;
extern int returnnow;

extern bool reloadingsearch;
extern bool activatefirstsearchimage;
extern std::string searchtext;


void callimage(int cain)
{
	std::string temp = BigData["arrays"]["chapter"]["images"][cain];
	replace(temp,"https://cdn.jkanime.net/assets/images/animes/image/","");

	std::string directorydownloadimage = rootdirectory+"DATA/";
	directorydownloadimage.append(temp);

	if(isFileExist(directorydownloadimage)){
		//printf("# %d imagen: %s \n",cain,directorydownloadimage.c_str());
		
	} else {
		directorydownloadimage="romfs:/nop.png";
		printf("# %d callimage imagen: %s \n",cain,directorydownloadimage.c_str());
	}
	
	TPreview.loadFromFileCustom(directorydownloadimage.c_str(), sizeportraitx, sizeportraity);
	tempimage = directorydownloadimage;
}

void callimagesearch(int cain)
{
	std::string temp = BigData["arrays"]["search"]["images"][cain];
	replace(temp,"https://cdn.jkanime.net/assets/images/animes/image/","");
	std::string directorydownloadimage = rootdirectory+"DATA/";
	directorydownloadimage.append(temp);

	TSearchPreview.loadFromFileCustom(directorydownloadimage.c_str(), sizeportraitx, sizeportraity);
	tempimage = directorydownloadimage;
}

void callimagefavorites(int cain)
{
	std::string temp = BigData["arrays"]["favorites"][cain];
	replace(temp,"https://jkanime.net/","");
	replace(temp,"/","");
	std::string directorydownloadimage = rootdirectory+"DATA/";
	directorydownloadimage.append(temp+".jpg");
		
//		printf("# %d callimage imagen: %s \n",cain,directorydownloadimage.c_str());
	TFavorite.loadFromFileCustom(directorydownloadimage.c_str(), sizeportraitx, sizeportraity);
	tempimage = directorydownloadimage;
}

void PushDirBuffer(std::string a,std::string name){
	//Sinopsis
	std::string terese = scrapElement(a, "<p rel=\"sinopsis\">","</p>");
	replace(terese, "<p rel=\"sinopsis\">", "");
	replace(terese, "<br/>", "");
	replace(terese, "&quot;", "");
	BigData["DataBase"][name]["sinopsis"] = terese;
	//std::cout << BigData["DataBase"][name]["sinopsis"] << std::endl;

	//es una peli ?
	terese = "...";
	
	if ((int)a.find("<span>Tipo:</span> Pelicula</li>") != -1)
	{
		terese="Pelicula";
	} else {
		//find next date
		int re1 =0, re2=0;
		re1 = a.find("<b>Próximo episodio</b>");
		if(re1 > 1){
			re1 += 25;
			re2 = a.find("<i class", re1);
			terese = "."+a.substr(re1, re2 - re1);
			replace(terese, "á","a");
			replace(terese, "ó","o");
		}
	}
	BigData["DataBase"][name]["nextdate"] = terese;
	
	//Generos 
	int indx1 = 1, indx2, indx3;
	indx1 = a.find("<span>Genero:</span>", indx1);
	terese="";
	while (indx1 != -1) {
		//if(indx4 < indx1) break;
		indx1 = a.find("https://jkanime.net/genero", indx1);
		if (indx1 == -1) { break; }
		indx2 = a.find(">",indx1);
		indx3 = a.find("</a>", indx1);
		terese += a.substr(indx2+1, indx3 - indx2-1)+",  ";
//		std::cout << terese << std::endl;
		indx1 = indx3;
	}
	replace(terese, "á","a");replace(terese, "é","e");replace(terese, "í","i");replace(terese, "ó","o");replace(terese, "ú","u");
	replace(terese, "à","a");replace(terese, "è","e");replace(terese, "ì","i");replace(terese, "ò","o");replace(terese, "ù","u");
	if (terese.length() == 0) terese +="-.-";
	BigData["DataBase"][name]["generos"] = terese;

	//Esta en emision?
	if ((int)a.find("En emision") != -1)
	{BigData["DataBase"][name]["enemision"] = "true";} else {BigData["DataBase"][name]["enemision"] = "false";}
	
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

		BigData["DataBase"][name]["maxcapit"] = atoi(urlx.c_str());
	} else {
		BigData["DataBase"][name]["maxcapit"] = 1;
	}

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
		BigData["DataBase"][name]["maxcapit"] = BigData["DataBase"][name]["maxcapit"].get<int>() - 1;
		BigData["DataBase"][name]["mincapit"] = 0;
	}
	else {
		BigData["DataBase"][name]["mincapit"] = 1;
	}
	BigData["DataBase"][name]["TimeStamp"] = BigData["TimeStamp"];
	std::cout << "Bufered: " << name << std::endl;
}

//BEGUING THREAD CHAIN
std::vector<std::string> con_full;
SDL_Thread* first = NULL;
int GETCONT(void* d){
con_full.clear();
for (int x = 0; x < (int)BigData["arrays"]["chapter"]["link"].size()&& !quit; x++)
	{
		while (!HasConnection()){SDL_Delay(5000);if(quit) return 0;}
		porcentajebufferA = x+1;
		std::string link = BigData["arrays"]["chapter"]["link"][x];
		int trace = link.find("/", 20);
		link = link.substr(0, trace + 1);
		con_full.push_back(gethtml(link));
		//printf("con_full %d -> %s\n",x,link.c_str());
	}
return 0;
}

int downloadjkanimevideo(void* data)
{
	for (u64 x=0; x< BigData["arrays"]["downloads"]["queue"].size();x++){
		DownTitle="................";
		serverenlace = "................";
		isDownloading=true;
		porcendown=0;
		if(cancelcurl){serverenlace = "Error de descarga"; break;}
		std::string urldown=BigData["arrays"]["downloads"]["queue"][x];
		std::string namedownload = urldown;
		replace(namedownload, "https://jkanime.net/", "");
		replace(namedownload, "-", " ");
		replace(namedownload, "/", " ");
		namedownload = namedownload.substr(0, namedownload.length() - 1);
		mayus(namedownload);
		DownTitle=namedownload;
		BigData["arrays"]["downloads"]["log"][x] = ">>>> "+namedownload;
		mkdir("sdmc:/Videos",0777);
		std::string directorydownload = "sdmc:/Videos/" +namedownload + ".mp4";

		if (!linktodownoadjkanime(urldown,directorydownload)){
			serverenlace = "Error de descarga";
			BigData["arrays"]["downloads"]["log"][x] = "Error: "+namedownload;
		} else {
			BigData["arrays"]["downloads"]["log"][x] = "100% : "+namedownload;
		}
	}
	isDownloading=false;
	statenow = downloadstate;
	return 0;
}

int refrescarpro(void* data){
	//clear allocate
	BigData["arrays"] = "{}"_json;;

	while (!HasConnection()){SDL_Delay(5000);if(quit) return 0;}
	activatefirstimage = true;
	reloading = true;
	porcentajereload = 0;
	int  val0 = 0, val1 = 1, val2, val3, val4;
	std::string temporal = "";
	std::string content = gethtml("https://jkanime.net");
	
	int temp0=0,temp1=0;
//	std::cout << "---" << val0 << std::endl;
	temp0=content.find("Programación");
	temp1=content.find("TOP ANIMES",temp0);
	content = content.substr(temp0,temp1-temp0);
	while (val0 != -1 && !quit) {
		val0 = content.find("<a href=", val1);
		if (val0 == -1) { break; }

		val1 = 9 + content.find("<a href=", val1);
		val2 = (content.find('"', val1));
		std::string gdrive = content.substr(val1, val2 - val1);
		//std::cout << gdrive << std::endl;
		BigData["arrays"]["chapter"]["link"].push_back(gdrive);
		val3 = content.find("<img src=", val2) + 10;
		val4 = content.find('"', val3);
		std::string gpreview = content.substr(val3, val4 - val3);
		BigData["arrays"]["chapter"]["images"].push_back(gpreview);
		
		//std::cout << gdrive << "  .  " << gpreview << std::endl;
		temporal = temporal + gdrive + "\n";
		temporal = temporal + gpreview + "\n";
		porcentajereload = val1;
		val1++;
	}
	//printf(temporal.c_str());
	reloading = false;
	//
	bool haschange = true;
	if (!BigData["latestchapter"].empty()){
		if (BigData["latestchapter"] == BigData["arrays"]["chapter"]["link"][0]) {haschange = false;}
	}
	if (haschange || BigData["TimeStamp"].empty()){
		//update TimeStamp
		std::time_t t = std::time(0);
		BigData["TimeStamp"] = std::to_string(t);
		std::cout << "New TimeStamp: " << BigData["TimeStamp"] << std::endl;
		first = SDL_CreateThread(GETCONT,"ContentThread",(void*)NULL);printf("firstCreated...\n");;
	}

	for (int x = 0; x < (int)BigData["arrays"]["chapter"]["images"].size(); x++)
	{
		imgNumbuffer = x+1;
		std::string tempima = BigData["arrays"]["chapter"]["images"][x];
		replace(tempima,"https://cdn.jkanime.net/assets/images/animes/image/","");
#ifdef __SWITCH__
		std::string directorydownloadimage = rootdirectory+"DATA/";
		directorydownloadimage.append(tempima);
#else
		std::string directorydownloadimage = "C:/respaldo2017/C++/test/Debug/RipJKAnime/" ;
		directorydownloadimage.append(tempima);
#endif // SWITCH
	if(!isFileExist(directorydownloadimage)){
		printf("\n# %d imagen: %s \n",x,tempima.c_str());
		downloadfile(BigData["arrays"]["chapter"]["images"][x],directorydownloadimage,false);
		activatefirstimage=true;
	} else printf("-");
	preview = true;

//	porcentajereload = ((x+1) * 100) / BigData["arrays"]["chapter"]["images"].size();
	}
	printf("#\nEnd Image Download\n");
	imgNumbuffer=0;
	activatefirstimage=true;
	//return 0;

	MKfavimgfix(true);
	//exit after load the images cache
	if (AppletMode) quit=true;
	if (haschange) {
		MKcapitBuffer();
		BigData["latestchapter"] = BigData["arrays"]["chapter"]["link"][0];
	}
	MKfavimgfix(false);
	return 0;
}

int MKcapitBuffer() {
	std::string a = "";
	for (int x = 0; x < (int)BigData["arrays"]["chapter"]["link"].size()&& !quit; x++)
	{
		porcentajebuffer = x+1;
		//if (x > (int)con_full.size()-1) printf("Wait for vector...\n");
		while (x > (int)con_full.size()-1){
			//printf("-");
			if(quit) return 0;
			SDL_Delay(500);
		}
		//printf("Get from vector...\n");
		a = con_full[x];
		std::string name = BigData["arrays"]["chapter"]["link"][x];
		int v5 = name.find("/", 20);
		name = name.substr(0, v5 + 1);
		replace(name, "https://jkanime.net/", "");
		replace(name, "/", "");
		PushDirBuffer(a,name);
	}
	
	if (NULL == first) {
		printf("SDL_CreateThread NULL %s\n", SDL_GetError());
	}
	else {
		//printf("firstwait END...\n");
		int returnval;
		SDL_WaitThread(first, &returnval);
		printf("first END ... %d\n",returnval);
	}
	porcentajebuffer = 0;
	porcentajebufferA = 0;
	con_full.clear();
	return true;
}
//END THREAD CHAIN

int searchjk(void* data)
{
	porcentajereload = 0;
	activatefirstsearchimage = true;
	reloadingsearch = true;	 

	replace(searchtext, " ", "_");
	replace(searchtext, "!", "");
	replace(searchtext, ";", "");
	if (searchtext.length() >= 2) {
		std::cout << searchtext << std::endl;
		std::string content = "";
		
		int page = 1;
		while (true){
			std::string tempCont=gethtml("https://jkanime.net/buscar/" + searchtext + "/"+std::to_string(page)+"/");
			content += tempCont;
			
			std::string scrap = scrapElement(tempCont, "Resultados Siguientes");
			std::cout << scrap << "  # " << std::to_string(page) << std::endl;
			if (scrap.length() > 0){
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
		

			BigData["arrays"]["search"]["link"].push_back(gdrive);
			val3 = content.find("data-setbg=", val2) + 12;
			val4 = content.find('"', val3);
			std::string gsearchpreview = content.substr(val3, val4 - val3);
			BigData["arrays"]["search"]["images"].push_back(gsearchpreview);
			std::cout << gsearchpreview << std::endl;
			
			val1++;
		}
		
		for (int x = 0; x < (int)BigData["arrays"]["search"]["images"].size(); x++) {
				std::string tempima = BigData["arrays"]["search"]["images"][x];
				replace(tempima,"https://cdn.jkanime.net/assets/images/animes/image/","");
#ifdef __SWITCH__
				std::string directorydownloadimage = rootdirectory+"DATA/";
				directorydownloadimage.append(tempima);
#else
				std::string directorydownloadimage = "C:/respaldo2017/C++/test/Debug/RipJKAnime/";
				directorydownloadimage.append(tempima);
#endif // SWITCH
			if(!isFileExist(directorydownloadimage))
			downloadfile(BigData["arrays"]["search"]["images"][x],directorydownloadimage,false);

			porcentajereload = ((x + 1) * 100) / BigData["arrays"]["search"]["images"].size();
		}
	}
	else
	{
		statenow = programationstate;
		returnnow = toprogramation;
	}
	reloadingsearch = false;
	return 0;
}

//get cap thread
int capit(void* data) {
	std::string a = "";
	a = gethtml(temporallink);
	std::string name = temporallink;
	replace(name, "https://jkanime.net/", ""); replace(name, "/", "");
	
	PushDirBuffer(a,name);
	try{
		//std::cout << BigData << std::endl;
		sinopsis = BigData["DataBase"][name]["sinopsis"];
		nextdate = BigData["DataBase"][name]["nextdate"];//"......";
		mincapit = BigData["DataBase"][name]["mincapit"];//1;
		maxcapit = BigData["DataBase"][name]["maxcapit"];//-1;
		generos = BigData["DataBase"][name]["generos"];//"......";
		enemision = BigData["DataBase"][name]["enemision"] == "true";
	}catch(...){
		printf("Error \n");
	}
return 0;
}


int MKfavimgfix(bool images){
	std::ifstream file(rootdirectory+"favoritos.txt");
	std::string str;
	std::string name ="";
	while (std::getline(file, str)) {
		//std::cout << str << "\n";
		if (str.find("jkanime"))
		{
				name=str;
				replace(name, "https://jkanime.net/", "");
				replace(name, "/", "");
				
			if (images) {
				CheckImgNet(rootdirectory+"DATA/"+name+".jpg");
			} else {
				if (BigData["DataBase"][name]["TimeStamp"].empty()){
					std::string a = gethtml(str);
					PushDirBuffer(a,name);
				}
			}
		}
	}
	file.close();
	return 0;
}

//anime manager
int capBuffer () {
	//std::cout << "Prety json" << std::endl;
	//std::cout << BigData << std::endl;
	std::string name = temporallink;
	replace(name, "https://jkanime.net/", "");
	replace(name, "/", "");
	std::cout << "KeyName: " << name << std::endl;

	if (BigData["DataBase"][name]["TimeStamp"].empty())
	{
		sinopsis = "......";
		nextdate = "......";
		maxcapit = -1;
		mincapit = 1;
		capmore = 1;
		generos = "......";
		capithread = SDL_CreateThread(capit, "capithread", (void*)NULL);
	} else {
		try{
			sinopsis = BigData["DataBase"][name]["sinopsis"];
			nextdate = BigData["DataBase"][name]["nextdate"];//"......";
			maxcapit = BigData["DataBase"][name]["maxcapit"];//-1;
			enemision = BigData["DataBase"][name]["enemision"] == "true" ? true : false;
			mincapit = BigData["DataBase"][name]["mincapit"];//1;
			capmore = BigData["DataBase"][name]["maxcapit"];//1;
			generos = BigData["DataBase"][name]["generos"];//"......";
			if (BigData["DataBase"][name]["TimeStamp"] != BigData["TimeStamp"]){
				nextdate = "Loading...";
				capithread = SDL_CreateThread(capit, "capithread", (void*)NULL);
			}
		}catch(...){
			printf("Error \n");
		}
	}
return 0;
}

bool isFavorite(std::string fav){
	/*
		static std::string limit = "";
		if (limit == fav){
			return gFAV;
		} else limit = fav;
	*/
	std::ifstream file(rootdirectory+"favoritos.txt");
	std::string str;
	while (std::getline(file, str)) {
	//	std::cout << str << "\n";
		if (str.find("jkanime"))
		{
//			printf("---\n");
//			printf("%s---\n",fav.c_str());
//			printf("%s\n",str.c_str());
			if (fav == str){file.close(); return true;}
		}
	}
file.close();
return false;
}

void delFavorite(int inst){
	std::string tmp="";
	if (inst >= 0){//delete all if is -1  delFavorite();
		std::ifstream file(rootdirectory+"favoritos.txt");
		std::string str = "";

		int index = 0;
		while (std::getline(file, str)) {
			if (inst != index){tmp += str+"\n";}
			index++;
		}
		file.close();	
	} else gFAV=false;
	std::ofstream outfile(rootdirectory+"favoritos.txt");
	outfile << tmp;
	outfile.close();
}