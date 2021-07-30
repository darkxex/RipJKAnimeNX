//////////////////////////////////aquí empieza el pc.
//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
//Rendered texture
LTexture gTextTexture;
LTexture Farest;
LTexture Heart;
LTexture TChapters;
LTexture TPreview;
LTexture TSearchPreview;///////
LTexture TFavorite;
//Render Buttons
LTexture B_A;
LTexture B_B;
LTexture B_Y;
LTexture B_X;
LTexture B_L;
LTexture B_R;
LTexture B_ZR;
LTexture B_M;
LTexture B_P;
LTexture B_RIGHT;
LTexture B_LEFT;
LTexture B_UP;
LTexture B_DOWN;
//Render extra
LTexture FAV;
LTexture NOP;
LTexture CLEAR;
//Text and BOXES
LTexture VOX;
LTexture T_T;
LTexture T_D;
LTexture T_R;

//main SLD funct (Grafics On Display = GOD)
SDLB GOD;
	
//Gui Vars
enum states { programationstate, downloadstate, chapterstate, searchstate, favoritesstate };
enum statesreturn { toprogramation, tosearch, tofavorite };
int statenow = programationstate;
int returnnow = toprogramation;
//net
std::string urltodownload = "";
int porcendown = 0;
int sizeestimated = 0;
std::string temporallink = "";
int cancelcurl = 0;

//img
bool reloading = false;
bool preview = false;
int selectchapter = 0;
int porcentajereload = 0;
int imgNumbuffer = 0;
int porcentajebuffer = 0;
int porcentajebufferA = 0;
bool activatefirstimage = true;
std::string serverenlace = "...";
std::string DownTitle="...";

int maxcapit = 1;
int mincapit = 0;
int capmore = 1;

//search
int searchchapter = 0;
bool reloadingsearch = false;
bool activatefirstsearchimage = true;
std::string searchtext = "";
std::string tempimage = "";
//downloads
bool isDownloading=false;
std::string speedD="";
//favs
int favchapter = 0;
bool gFAV = false;
//server
int selectserver = 0;
bool serverpront = false;

#ifdef __SWITCH__
//std::string favoritosdirectory = "sdmc:/switch/RipJKAnime_NX/favoritos.txt";
#else
rootdirectory = "C:/respaldo2017/C++/test/Debug/";
#endif // SWITCH

//my vars
bool AppletMode=false;
bool isSXOS=false;
bool hasStealth=false;
AccountUid uid;
SDL_Thread* capithread = NULL;

#ifdef __SWITCH__
HidsysNotificationLedPattern blinkLedPattern(u8 times);
void blinkLed(u8 times);
#endif // ___SWITCH___

std::vector<std::string> arrayservers= {
"Fembed 2.0","Nozomi","MixDrop","Desu","Xtreme S","Okru"
};

std::vector<std::string> arrayserversbak= {
"Fembed 2.0","Nozomi","MixDrop","Desu","Xtreme S","Okru"
};
/*
std::vector<std::string> arrayserversbak= {
"Nozomi","Desu", "Xtreme S", "MixDrop", "Okru", "Fembed", "Local"
};
*/
json BigData;

bool quit=false;

int MKcapitBuffer();
int MKfavimgfix();

std::vector<std::string> downqueue;
std::vector<std::string> logqueue;
int downloadjkanimevideo(void* data)
{
	for (u64 x=0; x< downqueue.size();x++){
		DownTitle="................";
		serverenlace = "................";
		isDownloading=true;
		porcendown=0;
		if(cancelcurl){serverenlace = "Error de descarga"; break;}
		std::string urldown=downqueue[x];
		std::string namedownload = urldown;
		replace(namedownload, "https://jkanime.net/", "");
		replace(namedownload, "-", " ");
		replace(namedownload, "/", " ");
		namedownload = namedownload.substr(0, namedownload.length() - 1);
		mayus(namedownload);
		DownTitle=namedownload;
		logqueue[x] = ">>>> "+namedownload;
#ifdef __SWITCH__
		std::string directorydownload = "sdmc:/" +namedownload + ".mp4";
#else
	std::string directorydownload = "C:\\respaldo2017\\C++\\test\\Debug\\" + namedownload + ".mp4";
#endif // SWITCH

		if (!linktodownoadjkanime(urldown,directorydownload)){
			serverenlace = "Error de descarga";
			logqueue[x] = "Error: "+namedownload;
		} else {
			logqueue[x] = "100% : "+namedownload;
		}
	}
	isDownloading=false;
	statenow = downloadstate;
	return 0;
}

std::vector<std::string> arrayimages;
std::vector<std::string> arraychapter;
std::vector<std::string> arraysearch;
std::vector<std::string> arrayfavorites;
std::vector<std::string> arraysearchimages;

int sizeportraity = 210;
int sizeportraitx = 297;
int xdistance = 1010;
int ydistance = 340;
void callimage(int cain)
{
#ifdef __SWITCH__
	std::string temp = arrayimages[cain];
	replace(temp,"https://cdn.jkanime.net/assets/images/animes/image/","");

	std::string directorydownloadimage = rootdirectory+"DATA/";
	directorydownloadimage.append(temp);
#else
	std::string directorydownloadimage = "C:/respaldo2017/C++/test/Debug/RipJKAnime/";
	directorydownloadimage.append(temp);
#endif // SWITCH

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
	
#ifdef __SWITCH__
		std::string temp = arraysearchimages[cain];
		replace(temp,"https://cdn.jkanime.net/assets/images/animes/image/","");
		std::string directorydownloadimage = rootdirectory+"DATA/";
		directorydownloadimage.append(temp);
#else
		std::string directorydownloadimage = "C:/respaldo2017/C++/test/Debug/RipJKAnime/";
		directorydownloadimage.append(temp);
#endif // SWITCH

		TSearchPreview.loadFromFileCustom(directorydownloadimage.c_str(), sizeportraitx, sizeportraity);
		tempimage = directorydownloadimage;
}

void callimagefavorites(int cain)
{
		std::string temp = arrayfavorites[cain];
		replace(temp,"https://jkanime.net/","");
		replace(temp,"/","");
		std::string directorydownloadimage = rootdirectory+"DATA/";
		directorydownloadimage.append(temp+".jpg");
		
//		printf("# %d callimage imagen: %s \n",cain,directorydownloadimage.c_str());
		TFavorite.loadFromFileCustom(directorydownloadimage.c_str(), sizeportraitx, sizeportraity);
		tempimage = directorydownloadimage;
}
//
//BEGUING THREAD CHAIN
std::vector<std::string> con_full;
SDL_Thread* first = NULL;
int GETCONT(void* d){
con_full.clear();
for (int x = 0; x < (int)arraychapter.size()&& !quit; x++)
	{
		while (!HasConnection()){SDL_Delay(5000);if(quit) return 0;}
		porcentajebufferA = x+1;
		std::string link = arraychapter[x];
		int trace = link.find("/", 20);
		link = link.substr(0, trace + 1);
		con_full.push_back(gethtml(link));
		//printf("con_full %d -> %s\n",x,link.c_str());
	}
return 0;
}
int refrescarpro(void* data){
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
		arraychapter.push_back(gdrive);
		val3 = content.find("<img src=", val2) + 10;
		val4 = content.find('"', val3);
		std::string gpreview = content.substr(val3, val4 - val3);
		arrayimages.push_back(gpreview);
		
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
		if (BigData["latestchapter"] == arraychapter[0]) {haschange = false;}
	}
	if (haschange || BigData["TimeStamp"].empty()){
		//update TimeStamp
		std::time_t t = std::time(0);
		BigData["TimeStamp"] = std::to_string(t);
		std::cout << "New TimeStamp: " << BigData["TimeStamp"] << std::endl;
		first = SDL_CreateThread(GETCONT,"ContentThread",(void*)NULL);printf("firstCreated...\n");;
	}

	for (int x = 0; x < (int)arrayimages.size(); x++)
	{
		imgNumbuffer = x+1;
		std::string tempima = arrayimages[x];
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
		downloadfile(arrayimages[x],directorydownloadimage,false);
		activatefirstimage=true;
	} else printf("-");
	preview = true;

//	porcentajereload = ((x+1) * 100) / arrayimages.size();
	}
	printf("#\nEnd Image Download\n");
	imgNumbuffer=0;
	activatefirstimage=true;
	//return 0;

	MKfavimgfix();
	//exit after load the images cache
	if (AppletMode) quit=true;
	if (haschange) {
		MKcapitBuffer();
		BigData["latestchapter"] = arraychapter[0];
	}
	return 0;
}
int MKfavimgfix(){
	std::ifstream file(rootdirectory+"favoritos.txt");
	std::string str;
	std::string machu ="";
	while (std::getline(file, str)) {
		//std::cout << str << "\n";
		if (str.find("jkanime"))
		{
			machu=str;
			replace(machu, "https://jkanime.net/", "");
			replace(machu, "/", "");
			CheckImgNet(rootdirectory+"DATA/"+machu+".jpg");
		}
	}
	file.close();
	return 0;
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
	
	if ((int)a.find("<li><span>Tipo:</span> Pelicula</li>") != -1)
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
	std::string generosTMP="";
	while (indx1 != -1) {
		//if(indx4 < indx1) break;
		indx1 = a.find("https://jkanime.net/genero", indx1);
		if (indx1 == -1) { break; }
		indx2 = a.find(">",indx1);
		indx3 = a.find("</a>", indx1);
		generosTMP += a.substr(indx2+1, indx3 - indx2-1)+" ";
//		std::cout << generosTMP << std::endl;
		indx1 = indx3;
	}
	replace(generosTMP, "á","a");replace(generosTMP, "é","e");replace(generosTMP, "í","i");replace(generosTMP, "ó","o");replace(generosTMP, "ú","u");
	replace(generosTMP, "à","a");replace(generosTMP, "è","e");replace(generosTMP, "ì","i");replace(generosTMP, "ò","o");replace(generosTMP, "ù","u");
	BigData["DataBase"][name]["generos"] = ":"+generosTMP;

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
	}
	else
	{
		BigData["DataBase"][name]["maxcapit"] = 1;
	}
	BigData["DataBase"][name]["capmore"] = BigData["DataBase"][name]["maxcapit"];

	//empieza por 0?
	int zero1, zero2;
	std::string zerocontainer = "";
	std::string zerocontainer2 = "";
	zero1 = a.rfind("ajax/pagination_episodes/");
	zero2 = a.find("'", zero1);
	zerocontainer = "https://www.jkanime.net/" + a.substr(zero1, zero2 - zero1) + "/1/";
	std::cout << zerocontainer << std::endl;
	zerocontainer2 = gethtml(zerocontainer);

	int tempzero = zerocontainer2.find("\"0\"");
	if (tempzero != -1) {
		std::cout << "Si contiene" << std::endl;
		BigData["DataBase"][name]["maxcapit"] = BigData["DataBase"][name]["maxcapit"].get<int>() - 1;
		BigData["DataBase"][name]["mincapit"] = 0;
		BigData["DataBase"][name]["capmore"] = BigData["DataBase"][name]["maxcapit"];
	}
	else {
		std::cout << "no contiene" << std::endl;
		BigData["DataBase"][name]["mincapit"] = 1;
		BigData["DataBase"][name]["capmore"] = BigData["DataBase"][name]["maxcapit"];
	}
	BigData["DataBase"][name]["TimeStamp"] = BigData["TimeStamp"];
	std::cout << "Bufered: " << name << std::endl;
}

int MKcapitBuffer() {
	std::string a = "";
	for (int x = 0; x < (int)arraychapter.size()&& !quit; x++)
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
		std::string name = arraychapter[x];
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
		

			arraysearch.push_back(gdrive);
			val3 = content.find("data-setbg=", val2) + 12;
			val4 = content.find('"', val3);
			std::string gsearchpreview = content.substr(val3, val4 - val3);
			arraysearchimages.push_back(gsearchpreview);
			std::cout << gsearchpreview << std::endl;
			
			val1++;
		}
		
		for (int x = 0; x < (int)arraysearchimages.size(); x++) {
				std::string tempima = arraysearchimages[x];
				replace(tempima,"https://cdn.jkanime.net/assets/images/animes/image/","");
#ifdef __SWITCH__
				std::string directorydownloadimage = rootdirectory+"DATA/";
				directorydownloadimage.append(tempima);
#else
				std::string directorydownloadimage = "C:/respaldo2017/C++/test/Debug/RipJKAnime/";
				directorydownloadimage.append(tempima);
#endif // SWITCH
			if(!isFileExist(directorydownloadimage))
			downloadfile(arraysearchimages[x],directorydownloadimage,false);

			porcentajereload = ((x + 1) * 100) / arraysearchimages.size();
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
bool tienezero = false;
std::string rese = "";
bool enemision = false;
std::string nextdate = "";
std::string generos = "";

//get cap thread
int capit(void* data) {
	std::string a = "";
	a = gethtml(temporallink);
	std::string name = temporallink;
	replace(name, "https://jkanime.net/", "");
	replace(name, "/", "");
	if (statenow != chapterstate) return 0;
	PushDirBuffer(a,name);
	try{
		//std::cout << BigData << std::endl;
		rese = BigData["DataBase"][name]["sinopsis"];
		nextdate = BigData["DataBase"][name]["nextdate"];//"......";
		maxcapit = BigData["DataBase"][name]["maxcapit"];//-1;
		enemision = BigData["DataBase"][name]["enemision"] == "true" ? true : false;
		mincapit = BigData["DataBase"][name]["mincapit"];//1;
		capmore = BigData["DataBase"][name]["capmore"];//1;
		generos = BigData["DataBase"][name]["generos"];//"......";
	}catch(...){
		printf("Error \n");
	}
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
		rese = "......";
		nextdate = "......";
		maxcapit = -1;
		mincapit = 1;
		capmore = 1;
		generos = "......";
		capithread = SDL_CreateThread(capit, "capithread", (void*)NULL);
	} else {
		try{
			rese = BigData["DataBase"][name]["sinopsis"];
			nextdate = BigData["DataBase"][name]["nextdate"];//"......";
			maxcapit = BigData["DataBase"][name]["maxcapit"];//-1;
			enemision = BigData["DataBase"][name]["enemision"] == "true" ? true : false;
			mincapit = BigData["DataBase"][name]["mincapit"];//1;
			capmore = BigData["DataBase"][name]["capmore"];//1;
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

void delFavorite(int inst = -1){
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