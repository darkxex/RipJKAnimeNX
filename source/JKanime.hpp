int MKcapitBuffer();
int MKfavimgfix();
	
std::string linktodownoadjkanime()
{
	std::string videourl = "";


	std::string content = "";
	std::string enlacejk = urltodownload;


	content = gethtml(enlacejk.c_str());
	int val1 = 0, val2 = 0;

	val1 = content.find("https://www.mediafire.com/file/");
	if (val1 != -1)
	{
		val2 = content.find('"', val1);
		
		videourl = content.substr(val1, val2 - val1);
		replace(videourl, "\\", "");
		std::string tempmedia = gethtml(videourl);
		val1 = tempmedia.find("https://download");
		if (val1 != -1)
		{
			val2 = tempmedia.find('"', val1);

			videourl = tempmedia.substr(val1, val2 - val1);
			replace(videourl, "\\", "");
			std::cout << videourl << std::endl;
			serverenlace = videourl;
			return videourl;//downloadfile(videourl, directorydownload);

		}
		else
		{
			val1 = content.find("https://jkanime.net/jk.php?");
			if (val1 != -1)
			{
				val2 = content.find('"', val1);

				videourl = content.substr(val1, val2 - val1);
				replace(videourl, "\\", "");
				replace(videourl, "https://jkanime.net/jk.php?u=", "https://jkanime.net/");

				std::cout << videourl << std::endl;
				serverenlace = videourl;
				return videourl;//downloadfile(videourl, directorydownload);
			}
		}
		
	
	}
	else

	{
		val1 = content.find("https://jkanime.net/jk.php?");
	if (val1 != -1)
	{
		val2 = content.find('"', val1);

		videourl = content.substr(val1, val2 - val1);
		replace(videourl, "\\", "");
		replace(videourl, "https://jkanime.net/jk.php?u=", "https://jkanime.net/");

		std::cout << videourl << std::endl;
		serverenlace = videourl;
		return videourl;//downloadfile(videourl, directorydownload);
	}
	}
	return "";
}

int downloadjkanimevideo(void* data)
{
	std::string videourl = linktodownoadjkanime();
	std::string namedownload = urltodownload;
	replace(namedownload, "https://jkanime.net/", "");
	replace(namedownload, "-", " ");
	replace(namedownload, "/", " ");
	namedownload = namedownload.substr(0, namedownload.length() - 1);
	mayus(namedownload);
	namedownload = namedownload + ".mp4";
#ifdef __SWITCH__
	std::string directorydownload = "sdmc:/" + namedownload;
#else
	std::string directorydownload = "C:\\respaldo2017\\C++\\test\\Debug\\" + namedownload;
#endif // SWITCH
	std::cout << namedownload << std::endl;
	std::cout << videourl << std::endl;

	downloadfile(videourl, directorydownload);
	return 0;
}



std::vector<std::string> arrayimages;
std::vector<std::string> arraychapter;
std::vector<std::string> arraysearch;
std::vector<std::string> arrayfavorites;
std::vector<std::string> arraysearchimages;

int sizeportraity = 180;
int sizeportraitx = 225;
int xdistance = 1000;
int ydistance = 448;
void callimage(int cain)
{
#ifdef __SWITCH__
	std::string temp = arrayimages[cain];
	replace(temp,"https://cdn.jkanime.net/assets/images/animes/image/","");

	std::string directorydownloadimage = rootdirectory;
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
		std::string directorydownloadimage = rootdirectory;
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
		std::string directorydownloadimage = rootdirectory;
		directorydownloadimage.append(temp+".jpg");
		
		printf("# %d callimage imagen: %s \n",cain,directorydownloadimage.c_str());
		TFavorite.loadFromFileCustom(directorydownloadimage.c_str(), sizeportraitx, sizeportraity);
		tempimage = directorydownloadimage;
}
//
//BEGUING THREAD CHAIN
std::vector<std::string> con_full;
SDL_Thread* first = NULL;

int GETCONT(void* d){
con_full.clear();
for (int x = 0; x < (int)arraychapter.size()&& quit == 0; x++)
	{
		porcentajebufferA = x+1;
		std::string link = arraychapter[x];
		int trace = link.find("/", 20);
		link = link.substr(0, trace + 1);
		con_full.push_back(gethtml(link));
		printf("con_full %d -> %s\n",x,link.c_str());
	}
return 0;
}


int refrescarpro(void* data){
	activatefirstimage = true;
	reloading = true;
	porcentajereload = 0;
	int  val0 = 0, val1 = 1, val2, val3, val4;
	std::string temporal = "";
	std::string content = gethtml("https://jkanime.net");
	while (val0 != -1) {
		val0 = content.find("play-button", val1);
		if (val0 == -1) { break; }

		val1 = 19 + content.find("play-button", val1);
		val2 = (content.find('"', val1));
		std::string gdrive = content.substr(val1, val2 - val1);

		arraychapter.push_back(gdrive);
		val3 = content.rfind("<img src=", val2) + 10;
		val4 = content.find('"', val3);
		std::string gpreview = content.substr(val3, val4 - val3);
		arrayimages.push_back(gpreview);
		
		//std::cout << arraycount << ". " << gdrive << std::endl;
		temporal = temporal + gdrive + "\n";
		temporal = temporal + gpreview + "\n";
		porcentajereload = val1;
		val1++;
	}
	
	printf(temporal.c_str());
	reloading = false;
	
	first = SDL_CreateThread(GETCONT,"ContentThread",(void*)NULL);printf("firstCreated...\n");;

	for (int x = 0; x < (int)arrayimages.size(); x++)
	{	
		imgNumbuffer = x+1;
		std::string tempima = arrayimages[x];
		replace(tempima,"https://cdn.jkanime.net/assets/images/animes/image/","");
#ifdef __SWITCH__
		std::string directorydownloadimage = rootdirectory;
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
//	threadID = SDL_CreateThread(MKcapitBuffer, "MainBuffer", (void*)NULL);
	activatefirstimage=true;
	MKfavimgfix();
	MKcapitBuffer();
	return 0;
}

int MKfavimgfix(){
	std::ifstream file(favoritosdirectory);
	std::string str;
	std::string machu ="";
		while (std::getline(file, str)) {
		std::cout << str << "\n";
		if (str.find("jkanime"))
		{
			machu=str;
			replace(machu, "https://jkanime.net/", "");
			replace(machu, "/", "");
			CheckImgNet(rootdirectory+machu+".jpg");
		}
	}
	file.close();
	return 0;
}

std::vector<std::string> con_rese;
std::vector<std::string> con_nextdate;
std::vector<bool> con_enemision;
std::vector<bool> con_tienezero;
std::vector<int> con_maxcapit;


int MKcapitBuffer() {
	con_rese.clear();
	con_nextdate.clear();
	con_enemision.clear();
	con_tienezero.clear();
	con_maxcapit.clear();
	std::string a = "";
	for (int x = 0; x < (int)arraychapter.size()&& quit == 0; x++)
	{
		porcentajebuffer = x+1;
		if (x > (int)con_full.size()-1) printf("Wait for vector...\n");
		while (x > (int)con_full.size()-1){printf("-");SDL_Delay(500);}
		printf("Get from vector...\n");
		a = con_full[x];
		
		//find sinopsis
		int re1, re2;
		re1 = a.find("Sinopsis: </strong>") + 19;
		re2 = a.find("</p>", re1);

		std::string terese = a.substr(re1, re2 - re1);
		replace(terese, "<br/>", "");
		con_rese.push_back(terese);

		//find next date
		re1 = a.find("<b>Próximo episodio</b>") + 25;
		re2 = a.find("<i class", re1);

		terese = "";
		terese = a.substr(re1, re2 - re1);
		replace(terese, "á","a");
		replace(terese, "ó","o");
		con_nextdate.push_back(terese);
	//	std::cout << rese << std::endl;
		if ((int)a.find("<b>En emision</b>") != -1)
		{
			con_enemision.push_back(true);
		}
		else
		{
			con_enemision.push_back(false);
		}

		int val0, val1, val2, val3;

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

			//std::cout << "Si contiene" << std::endl;
			con_tienezero.push_back(true);
		}
		else {

			//std::cout << "no contiene" << std::endl;
			con_tienezero.push_back(false);
		}

		val0 = a.rfind("href=\"#pag");

		if (val0 != -1) {

			val1 = a.find(">", val0);
			val1 = val1 + 1;
			val2 = a.find("<", val1);

			std::string urlx;


			urlx = (a.substr(val1, val2 - val1));
			val3 = urlx.find(" - ") + 3;
			urlx = urlx.substr(val3);
			con_maxcapit.push_back(atoi(urlx.c_str()));
		}
		else
		{
			con_maxcapit.push_back(1);//return "1";
		}
	}
	
	if (NULL == first) {
		printf("SDL_CreateThread NULL %s\n", SDL_GetError());
	}
	else {
		printf("firstwait END...\n");
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
//	int val1 = 1;
//	int val2;
//	int val0 = 0;
	int val3, val4;
//	int arrayselect = 0;

	
#ifndef __SWITCH__
	searchtext = "h2o";
#endif 

	replace(searchtext, " ", "_");
	replace(searchtext, "!", "");
	replace(searchtext, ";", "");
	if (searchtext.length() >= 2) {
		std::string content = gethtml("https://jkanime.net/buscar/" + searchtext + "/1/");
		content = content + gethtml("https://jkanime.net/buscar/" + searchtext + "/2/");
		int val1 = 1;
		int val2;
		int val0 = 0;

		while (val0 != -1) {
			val0 = content.find("portada-title", val1);
			if (val0 == -1) { break; }

			val1 = 6 + content.find("href=", val0);
			val2 = (content.find('"', val1));
			std::string gdrive = content.substr(val1, val2 - val1);
		

			arraysearch.push_back(gdrive);
			val3 = content.find("<img src=", val2) + 10;
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
				std::string directorydownloadimage = rootdirectory;
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
std::string capit(std::string b) {
	tienezero = false;
	std::string a = "";
	a = gethtml(b);
	//
	int re1, re2;
	re1 = a.find("Sinopsis: </strong>") + 19;
	re2 = a.find("</p>", re1);

	std::string terese = a.substr(re1, re2 - re1);
	replace(terese, "<br/>", "");
	rese = terese;

	//find next date
	re1 = a.find("<b>Próximo episodio</b>") + 25;
	re2 = a.find("<i class", re1);

	//utf-8
	nextdate = "";
	nextdate = a.substr(re1, re2 - re1);
	replace(nextdate, "á","a");
	replace(nextdate, "ó","o");

//	std::cout << rese << std::endl;
	if ((int)a.find("<b>En emision</b>") != -1)
	{
		enemision = true;
	}
	else
	{
		enemision = false;
	}

	int val0, val1, val2, val3;

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
		tienezero = true;
	}
	else {

		std::cout << "no contiene" << std::endl;
		tienezero = false;
	}




	val0 = a.rfind("href=\"#pag");

	if (val0 != -1) {

		val1 = a.find(">", val0);
		val1 = val1 + 1;
		val2 = a.find("<", val1);

		std::string urlx;


		urlx = (a.substr(val1, val2 - val1));
		val3 = urlx.find(" - ") + 3;
		urlx = urlx.substr(val3);

		return (urlx);
	}

	else

	{
		return "1";
	}


}




bool isFavorite(std::string fav){
	/*
		static std::string limit = "";
		if (limit == fav){
			return gFAV;
		} else limit = fav;
	*/
	std::ifstream file(favoritosdirectory);
	std::string str;
	while (std::getline(file, str)) {
	//	std::cout << str << "\n";
		if (str.find("jkanime"))
		{
			printf("---\n");
			printf("%s---\n",fav.c_str());
			printf("%s\n",str.c_str());
			if (fav == str){file.close(); return true;}
		}
	}
file.close();
return false;
}


void delFavorite(int inst = -1){
	std::string tmp="";
	if (inst >= 0){//delete all if is -1  delFavorite();
		std::ifstream file(favoritosdirectory);
		std::string str = "";

		int index = 0;
		while (std::getline(file, str)) {
			if (inst != index){tmp += str+"\n";}
			index++;
		}
		file.close();	
	} else gFAV=false;
	std::ofstream outfile(favoritosdirectory);
	outfile << tmp;
	outfile.close();
}