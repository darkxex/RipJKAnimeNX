#include "Networking.hpp"
#include "applet.hpp"
#include "utils.hpp"
#include "JKanime.hpp"
#include "SDLWork.hpp"

extern json BD;
extern int mincapit;
extern int maxcapit;
extern int capmore;
extern std::string rootdirectory;
extern SDL_Thread* capithread;
extern bool gFAV;

extern LTexture gTextTexture;
extern LTexture Farest;
extern LTexture Heart;
extern LTexture TChapters;
extern LTexture TPreview;
extern LTexture TPreviewb;
extern LTexture TPreviewa;
extern int sizeportraity;
extern int sizeportraitx;

//img
extern bool reloading;
extern bool preview;
extern int selectchapter;
extern int porcentajereload;
extern int imgNumbuffer;
extern int porcentajebuffer;
extern int porcentajebufferF;
extern int porcentajebufferFF;
extern bool activatefirstimage;
extern bool quit;
extern bool isDownloading;
extern int porcendown;
extern int cancelcurl;
extern bool AppletMode;

extern std::string serverenlace;
extern std::string DownTitle;
extern std::string KeyName;

enum states { programationstate, downloadstate, chapterstate, searchstate, favoritesstate };
enum statesreturn { toprogramation, tosearch, tofavorite };
extern int statenow;
extern int returnnow;

extern bool reloadingsearch;
extern bool activatefirstsearchimage;


void callimage(int pos,std::vector<std::string> imageV) {
	TPreview.free();
	TPreviewb.free();
	TPreviewa.free();
	
	std::string temp = imageV[pos];
	replace(temp,"https://cdn.jkanime.net/assets/images/animes/image/","");
	temp = rootdirectory+"DATA/"+temp;

	if(!isFileExist(temp)){
		temp="romfs:/nop.png";
		printf("# %d callimage imagen: %s \n",pos,temp.c_str());
	}
	
	TPreview.loadFromFileCustom(temp.c_str(), sizeportraitx, sizeportraity);
	
	//extra img 
	int bval,aval;
	int sisea = imageV.size();
	if (pos == 0){bval = sisea-1;} else {bval = pos-1;}
	if (pos+1 == sisea){aval = 0;} else {aval = pos+1;}
	
	temp = imageV[bval];
	replace(temp,"https://cdn.jkanime.net/assets/images/animes/image/","");
	temp = rootdirectory+"DATA/"+temp;
	if(!isFileExist(temp)){temp = "romfs:/nop.png";}
	TPreviewb.loadFromFileCustom(temp, 155, 110);
	
	temp = imageV[aval];
	replace(temp,"https://cdn.jkanime.net/assets/images/animes/image/","");
	temp = rootdirectory+"DATA/"+temp;
	if(!isFileExist(temp)){temp = "romfs:/nop.png";}
	TPreviewa.loadFromFileCustom(temp, 155, 110);
}
void PushDirBuffer(std::string a,std::string name) {
	if(quit) return;

	//Sinopsis
	std::string terese = scrapElement(a, "<p rel=\"sinopsis\">","</p>");
	replace(terese, "<p rel=\"sinopsis\">", ""); replace(terese, "<br/>", ""); replace(terese, "&quot;", "");
	BD["DataBase"][name]["sinopsis"] = terese.substr(0,810);
	//std::cout << BD["DataBase"][name]["sinopsis"] << std::endl;

	//new values
	//std::cout << "--------" << std::endl;

	terese = scrapElement(a, "<span>Tipo:","</li");
	replace(terese, "<span>Tipo:", ""); replace(terese, "</span> ", ""); replace(terese, "</span>", "");
	//std::cout << terese << std::endl;
	BD["DataBase"][name]["Tipo"] = terese;
	
	terese = scrapElement(a, "Idiomas:","</li");
	replace(terese, "Idiomas:", ""); replace(terese, "  ", " "); replace(terese, "</span> ", ""); replace(terese, "</span>", "");
	//std::cout << terese << std::endl;
	BD["DataBase"][name]["Idiomas"] = terese;
	
	terese = scrapElement(a, "Episodios:","</li");
	replace(terese, "Episodios:", ""); replace(terese, "</span> ", ""); replace(terese, "</span>", "");
	//std::cout << terese << std::endl;
	BD["DataBase"][name]["Episodios"] = terese;

	terese = scrapElement(a, "Duracion:","</li");
	replace(terese, "Duracion:", ""); replace(terese, "</span> ", ""); replace(terese, "</span>", "");
	//std::cout << terese << std::endl;
	BD["DataBase"][name]["Duracion"] = terese;

	terese = scrapElement(a, "Emitido:","</li");
	replace(terese, "Emitido:", ""); replace(terese, "</span> ", ""); replace(terese, "</span>", "");
	//std::cout << terese << std::endl;
	BD["DataBase"][name]["Emitido"] = terese;
	
	int bal1=0;
	bal1=a.find("Secuela");
	if (bal1 > 1){
		terese = scrapElement(a.substr(bal1), "https://jkanime.net/","");
		//std::cout << bal1 << " Secuela " <<  terese << std::endl;
		BD["DataBase"][name]["Secuela"] = terese;
	}
	
	bal1=a.find("Precuela");
	if (bal1 > 1){
		terese = scrapElement(a.substr(bal1), "https://jkanime.net/","");
		//std::cout << bal1 << " Precuela " <<  terese << std::endl;
		BD["DataBase"][name]["Precuela"] = terese;
	}

	//std::cout << "--------" << std::endl;
	
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
	BD["DataBase"][name]["nextdate"] = terese;
	
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
		terese += a.substr(indx2+1, indx3 - indx2-1)+", ";
//		std::cout << terese << std::endl;
		indx1 = indx3;
	}
	replace(terese, "á","a");replace(terese, "é","e");replace(terese, "í","i");replace(terese, "ó","o");replace(terese, "ú","u");
	replace(terese, "à","a");replace(terese, "è","e");replace(terese, "ì","i");replace(terese, "ò","o");replace(terese, "ù","u");
	if (terese.length() == 0) terese +="-.-";
	BD["DataBase"][name]["generos"] = terese;

	//Esta en emision?
	if ((int)a.find("En emision") != -1)
	{BD["DataBase"][name]["enemision"] = "true";} else {BD["DataBase"][name]["enemision"] = "false";}
	
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

		BD["DataBase"][name]["maxcapit"] = atoi(urlx.c_str());
	} else {
		BD["DataBase"][name]["maxcapit"] = 1;
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
		BD["DataBase"][name]["maxcapit"] = BD["DataBase"][name]["maxcapit"].get<int>() - 1;
		BD["DataBase"][name]["mincapit"] = 0;
	}
	else {
		BD["DataBase"][name]["mincapit"] = 1;
	}
	BD["DataBase"][name]["TimeStamp"] = BD["TimeStamp"];
	std::cout << "Bufered: " << name << std::endl;

}
int downloadjkanimevideo(void* data) {
	led_on(1);
	appletSetAutoSleepDisabled(true);
	for (u64 x=0; x< BD["arrays"]["downloads"]["queue"].size();x++){
		DownTitle="................";
		serverenlace = "................";
		isDownloading=true;
		porcendown=0;
		if(cancelcurl){serverenlace = "Error de descarga"; break;}
		std::string urldown=BD["arrays"]["downloads"]["queue"][x];
		std::string namedownload = urldown;
		replace(namedownload, "https://jkanime.net/", "");
		replace(namedownload, "-", " ");
		replace(namedownload, "/", " ");
		namedownload = namedownload.substr(0, namedownload.length() - 1);
		mayus(namedownload);
		DownTitle=namedownload;
		BD["arrays"]["downloads"]["log"][x] = ">>>> "+namedownload;
		mkdir("sdmc:/Videos",0777);
		std::string directorydownload = "sdmc:/Videos/" +namedownload + ".mp4";

		if (!linktodownoadjkanime(urldown,directorydownload)){
			serverenlace = "Error de descarga";
			BD["arrays"]["downloads"]["log"][x] = "Error: "+namedownload;
		} else {
			BD["arrays"]["downloads"]["log"][x] = "100% : "+namedownload;
		}
	}
	isDownloading=false;
	statenow = downloadstate;
	led_on(3);
	appletSetAutoSleepDisabled(false);
	return 0;
}

//BEGUING THREAD CHAIN
int refrescarpro(void* data){
	appletSetAutoSleepDisabled(true);
	//clear allocate
	BD["arrays"] = "{}"_json;;

	while (!HasConnection()){SDL_Delay(2000);if(quit) return 0;}
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
		BD["arrays"]["chapter"]["link"].push_back(gdrive);
		val3 = content.find("<img src=", val2) + 10;
		val4 = content.find('"', val3);
		std::string gpreview = content.substr(val3, val4 - val3);
		BD["arrays"]["chapter"]["images"].push_back(gpreview);
		
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
	if (!BD["latestchapter"].empty()){
		if (BD["latestchapter"] == BD["arrays"]["chapter"]["link"][0]) {haschange = false;}
	}
	
	if (haschange || BD["TimeStamp"].empty()){
		//update TimeStamp
		if (BD["latestchaptertemp"] != BD["arrays"]["chapter"]["link"][0] || BD["TimeStamp"].empty()){
			std::time_t t = std::time(0);
			BD["TimeStamp"] = std::to_string(t);
			std::cout << "New TimeStamp: " << BD["TimeStamp"] << std::endl;
			BD["latestchaptertemp"] = BD["arrays"]["chapter"]["link"][0];
		}
	}

	for (int x = 0; x < (int)BD["arrays"]["chapter"]["images"].size(); x++)
	{
		imgNumbuffer = x+1;
		std::string tempima = BD["arrays"]["chapter"]["images"][x];
		replace(tempima,"https://cdn.jkanime.net/assets/images/animes/image/","");
		std::string directorydownloadimage = rootdirectory+"DATA/";
		directorydownloadimage.append(tempima);
	if(!isFileExist(directorydownloadimage)){
		printf("\n# %d imagen: %s \n",x,tempima.c_str());
		downloadfile(BD["arrays"]["chapter"]["images"][x],directorydownloadimage,false);
		activatefirstimage=true;
	} else printf("-");
	preview = true;

//	porcentajereload = ((x+1) * 100) / BD["arrays"]["chapter"]["images"].size();
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
		BD["latestchapter"] = BD["arrays"]["chapter"]["link"][0];
	}
	MKfavimgfix(false);
	if (!isDownloading) appletSetAutoSleepDisabled(false);
	return 0;
}
int MKcapitBuffer() {
	std::string a = "";
	for (int x = 0; x < (int)BD["arrays"]["chapter"]["link"].size()&& !quit; x++)
	{
		porcentajebuffer = x+1;
		while (!HasConnection()){SDL_Delay(2000);if(quit) return 0;}
		std::string link = BD["arrays"]["chapter"]["link"][x];
		int trace = link.find("/", 20);
		link = link.substr(0, trace + 1);
		std::string name = link;
		replace(name, "https://jkanime.net/", "");
		replace(name, "/", "");
		if (BD["DataBase"][name]["TimeStamp"] != BD["TimeStamp"]){
			a = gethtml(link);
			PushDirBuffer(a,name);
		}
	}
	//write json
	write_DB(BD,rootdirectory+"DataBase.json");
	porcentajebuffer = 0;
	return true;
}
int MKfavimgfix(bool images){
	std::ifstream file(rootdirectory+"favoritos.txt");
	std::string str;
	std::string name ="";
	bool hasanychange=false;
	while (std::getline(file, str)) {
		//std::cout << str << "\n";
		if(quit) return 0;
		if (str.find("jkanime"))
		{
			name=str;
			replace(name, "https://jkanime.net/", "");
			replace(name, "/", "");
				
			if (images) {
				porcentajebufferFF++;
				CheckImgNet(rootdirectory+"DATA/"+name+".jpg");
			} else {
				porcentajebufferF++;
				if (BD["DataBase"][name]["TimeStamp"].empty() || BD["DataBase"][name]["TimeStamp"] != BD["TimeStamp"]){
					std::string a = gethtml(str);
					PushDirBuffer(a,name);
					hasanychange=true;
				}
			}
		}
	}
	file.close();
	porcentajebufferF=0;
	if (!images) {
		porcentajebufferFF=0;
		printf("# End fav Download\n");
		if (hasanychange){
			//write json
			write_DB(BD,rootdirectory+"DataBase.json");
		}
	}
	return 0;
}
//END THREAD CHAIN

int searchjk(void* data) {
	porcentajereload = 0;
	activatefirstsearchimage = true;
	reloadingsearch = true;	 
	
	std::string texts = BD["searchtext"];
	replace(texts, " ", "_");
	replace(texts, "!", "");
	replace(texts, ";", "");
	if (texts.length() >= 2) {
		std::cout << texts << std::endl;
		std::string content = "";
		int page = 1;
		while (true){
			std::string tempCont=gethtml("https://jkanime.net/buscar/" + texts + "/"+std::to_string(page)+"/");
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
		

			BD["arrays"]["search"]["link"].push_back(gdrive);
			val3 = content.find("data-setbg=", val2) + 12;
			val4 = content.find('"', val3);
			std::string gsearchpreview = content.substr(val3, val4 - val3);
			BD["arrays"]["search"]["images"].push_back(gsearchpreview);
			std::cout << gsearchpreview << std::endl;
			
			val1++;
		}
		
		for (int x = 0; x < (int)BD["arrays"]["search"]["images"].size(); x++) {
			std::string tempima = BD["arrays"]["search"]["images"][x];
			replace(tempima,"https://cdn.jkanime.net/assets/images/animes/image/","");

			std::string directorydownloadimage = rootdirectory+"DATA/";
			directorydownloadimage.append(tempima);

			if(!isFileExist(directorydownloadimage))
			downloadfile(BD["arrays"]["search"]["images"][x],directorydownloadimage,false);

			porcentajereload = ((x + 1) * 100) / BD["arrays"]["search"]["images"].size();
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
std::string linktmpc="";
int capit(void* data) {
	std::string bb = linktmpc;
	std::string a = "";
	a = gethtml(bb);
	std::string name = bb;
	replace(name, "https://jkanime.net/", ""); replace(name, "/", "");
	
	PushDirBuffer(a,name);
	try{
		//std::cout << BD << std::endl;
		BD["com"]["sinopsis"] = BD["DataBase"][name]["sinopsis"];
		BD["com"]["nextdate"] = BD["DataBase"][name]["nextdate"];//"......";
		BD["com"]["generos"] = BD["DataBase"][name]["generos"];//"......";
		BD["com"]["Emitido"] = BD["DataBase"][name]["Emitido"];
		BD["com"]["enemision"] = BD["DataBase"][name]["enemision"];
		mincapit = BD["DataBase"][name]["mincapit"];//1;
		maxcapit = BD["DataBase"][name]["maxcapit"];//-1;
	}catch(...){
		printf("Error \n");
	}
	//write json
	write_DB(BD,rootdirectory+"DataBase.json");
return 0;
}
//anime manager
int capBuffer (std::string Tlink) {
	std::string name = Tlink;
	replace(name, "https://jkanime.net/", "");
	replace(name, "/", "");
	KeyName = name;
	std::cout << "KeyName: " << name << std::endl;

	activatefirstimage=true;
	statenow = chapterstate;
	std::string temp = rootdirectory+"DATA/"+name+".jpg";
	CheckImgNet(temp);
	TChapters.free();
	TChapters.loadFromFileCustom(temp, 550, 400);

	if (BD["DataBase"][name]["TimeStamp"].empty())
	{
		BD["com"]["sinopsis"] = "......";
		BD["com"]["nextdate"] = "......";
		BD["com"]["generos"] = "......";
		BD["com"]["Emitido"] = "......";
		maxcapit = -1;
		mincapit = 1;
		capmore = 1;
		linktmpc=Tlink;
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
			if (BD["DataBase"][name]["capmore"].empty())
				capmore = BD["DataBase"][name]["maxcapit"];
			else
				capmore = BD["DataBase"][name]["capmore"];
			
			if (BD["DataBase"][name]["TimeStamp"] != BD["TimeStamp"]){
				BD["com"]["nextdate"] = "Loading...";
				linktmpc=Tlink;
				capithread = SDL_CreateThread(capit, "capithread", (void*)NULL);
			}
		}catch(...){
			printf("Error \n");
		}
	}
return 0;
}

void get_favorites() {
	BD["arrays"]["favorites"]["link"].clear();
	BD["arrays"]["favorites"]["images"].clear();
	std::string temp;
	std::ifstream infile;

	std::ifstream file(rootdirectory+"favoritos.txt");
	std::string str;
	while (std::getline(file, str)) {
		//std::cout << str << "\n";
		std::string strtmp = str;
		if (str.find("jkanime"))
		{
			BD["arrays"]["favorites"]["link"].push_back(str);
			replace(strtmp, "https://jkanime.net/", "");
			replace(strtmp, "/", ".jpg");
			strtmp = "https://cdn.jkanime.net/assets/images/animes/image/"+strtmp;
			BD["arrays"]["favorites"]["images"].push_back(strtmp);				
		}
	}
	file.close();
	//std::cout << "-----  " << BD << std::endl;
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