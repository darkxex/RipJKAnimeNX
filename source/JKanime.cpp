#include "Networking.hpp"
#include "applet.hpp"
#include "utils.hpp"
#include "JKanime.hpp"

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