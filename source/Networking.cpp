#include <switch.h>
#include <curl/curl.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <math.h>
#include <cstring>
#include <sys/stat.h>
#include "Networking.hpp"
#include "utils.hpp"
#include <ctime>
#include "applet.hpp"


struct MemoryStruct
{
	char *memory;
	size_t size;
	int mode;
	FILE *fp;
};

static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userdata) {
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userdata;

	char *ptr;

	ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);

	if (ptr == NULL)//handle memory overflow
	{
		printf("Failed to realloc mem\n");
		printf("Writing... %luMb To file\n",mem->size / 1000000 + 1);
		fwrite(mem->memory, 1, mem->size, mem->fp);
		free(mem->memory);
		mem->memory = (char*)malloc(1);
		mem->size = 0;
		ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
		if (ptr == NULL) return 0;
	}

	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

int progress_func(void* ptr, double TotalToDownload, double NowDownloaded,double TotalToUpload, double NowUploaded){

	// ensure that the file to be downloaded is not empty
	// because that would cause a division by zero error later on
	if (TotalToDownload <= 0.0) {
		return 0;
	}

	// how wide you want the progress meter to be
	int totaldotz = 20;
	double fractiondownloaded = NowDownloaded / TotalToDownload;
	// part of the progressmeter that's already "full"
	int dotz = round(fractiondownloaded * totaldotz);

	//get console time
	static int secr=0;
	static double temporal=0;
	static double herencia=0;
	time_t now = time(0);
	struct tm *tm;
	if ((tm = localtime (&now)) == NULL) {
		cout << "Error extracting time stuff" << endl;
	}
	//printf ("working %f\n",doneF);
	//calculate speed
	if(tm->tm_sec != secr)
	{
		herencia = NowDownloaded/1000000 - temporal;
		speedD=std::to_string(herencia);
		temporal = NowDownloaded/1000000;
		secr = tm->tm_sec;
	}

	// create the "meter"
	int ii = 0;
	porcendown = fractiondownloaded * 100;
	sizeestimated = TotalToDownload;
	int d=NowDownloaded,t=TotalToDownload;
	printf("%f m/s  %d / %d - ",herencia,d/1000000,t/1000000);
	printf("%3.0f%% [", fractiondownloaded * 100);
	// part  that's full already
	for (; ii < dotz; ii++) {
		printf("=");
	}
	// remaining part (spaces)
	for (; ii < 20; ii++) {
		printf(" ");
	}
	// and back to line begin - do not forget the fflush to avoid output buffering problems!
	printf("]\r");
	fflush(stdout);
	// if you don't return 0, the transfer will be aborted - see the documentation
	if(quit) return 1;
	if (cancelcurl == 1)
	{
		return 1;
	}

	return 0;

}

int progress_func_str(void* ptr, double TotalToDownload, double NowDownloaded,double TotalToUpload, double NowUploaded)
{
	if (cancelcurl == 1)
		return 1;
	return 0;
}


namespace Net {
string UserAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36";
int DebugNet = 1;        //0 no debug ,  1 some debug, 2 All debug

//Simplification
string REDIRECT(string url,string POSTFIEL){
	return REQUEST(url,POSTFIEL)["RED"];
}
string POST(string url,string POSTFIEL){
	if (POSTFIEL.length() == 0) {POSTFIEL="0";}
	return REQUEST(url,POSTFIEL)["BODY"];
}
string GET(string url){
	return REQUEST(url)["BODY"];
}
json HEAD(string url){
	json deb = REQUEST(url,"",true,true);
	if ( DebugNet > 0) {
		cout << "# Nintendo Web : " << deb["CODE"] << std::endl;
	}
	return deb;
}

//General Request
json REQUEST(string url,string POSTFIEL,bool HEADR,bool Verify){
	replace(url," ","%20");
	CURL *curl;
	CURLcode res = CURLE_OK;
	std::string Buffer;
	json data="{}"_json;
	data["URL"] = url;
	char *red = "";
	long http_code=0;
	long redirects=0;
	u64 sizeh=1;
	data["RED"] = "";

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, UserAgent.c_str());
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);

		if (HEADR) {
			curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		} else if (POSTFIEL.length()>0) {
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, POSTFIEL.c_str());
			data["POST"] = POSTFIEL;
		}
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, (rootdirectory+"COOKIES.txt").c_str());
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, (rootdirectory+"COOKIES.txt").c_str());
		curl_easy_setopt(curl, CURLOPT_REFERER, url.c_str());
		if (Verify) {
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1L);
		} else {
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		}
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Buffer);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			data["ERROR"] = curl_easy_strerror(res);
		} else {
			//Get Info
			curl_easy_getinfo(curl, CURLINFO_HEADER_SIZE, &sizeh);
			curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
			curl_easy_getinfo(curl, CURLINFO_REDIRECT_COUNT, &redirects);

			if (redirects > 0) {
				curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &red);
			}
			data["RED"] = red;
			if (DebugNet > 0 && redirects > 0) {
				std::cout << " EURL: " << red << std::endl;
			}
		}
		curl_easy_cleanup(curl);
	}
	//data["RAW"] = Buffer;
	data["CODE"] = http_code;
	data["COUNT"] = redirects;
	data["SIZE"] = sizeh;
	if (HEADR) {
		data["HEAD"] = split(Buffer.substr(0,sizeh), "\r\n");
	} else {
		data["HEAD"].push_back(Buffer.substr(0,sizeh));         //split(Buffer.substr(0,sizeh), "\r\n");
		data["BODY"] = Buffer.substr(sizeh);
	}

	if (DebugNet > 1) {
		if (url.find("https://jkanime.net/gsplay") != string::npos|| DebugNet > 2)
		{
			std::cout << " ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ " << std::endl;
			try {
				std::cout << std::setw(4) << data << std::endl;

			}catch(...) {
				LOG::E(10);
				std::cout << " CODE: " << http_code << std::endl;
				std::cout << " Redirects: " << redirects << std::endl;
				std::cout << " EURL: " << red << std::endl;
				std::cout << " error display info " << std::endl;
				data["RED"]="";
			}


		}
	}
	return data;
}

bool DOWNLOAD(string url,string path,bool progress){
	if(!Net::HasConnection()) {return false;}
	replace(url," ","%20");
	CURL *curl;
	CURLcode res = CURLE_OK;
	curl = curl_easy_init();
	bool allok=false;
	if (curl) {
		struct MemoryStruct chunk;
		remove(path.c_str());
		chunk.fp = fopen(path.c_str(), "a");
		if(chunk.fp) {
			chunk.memory = (char*)malloc(1);
			chunk.size = 0;
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_USERAGENT, UserAgent.c_str());
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
			// Install the callback function
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
			if (progress) {
				curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func);
			} else {
				curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func_str);
			}

			res = curl_easy_perform(curl);
			if ((res == CURLE_OK)) {
				cout << "#size:" << chunk.size << " found:" << path.find(".mp4") << " in:" << path.c_str() << endl;
				if (chunk.size < 1000000  && path.find(".mp4") != string::npos) {
					cout << "####size:" << chunk.size << " found:" << path.find(".mp4") << " in:" << path.c_str() << endl;
					allok=false;        //
				} else {
					fwrite(chunk.memory, 1, chunk.size, chunk.fp);        // write from mem to file
					allok=true;
				}
			}else{
				allok=false;
				cout << curl_easy_strerror(res) << endl;
			}
			/* always cleanup */
			curl_easy_cleanup(curl);
			free(chunk.memory);
		}
		fclose(chunk.fp);
		if (!allok) {remove(path.c_str());}
	}
	return allok;
}

bool HasConnection(){
	NifmInternetConnectionType connectionType;
	NifmInternetConnectionStatus connectionStatus;
	u32 strg = 0;
	nifmInitialize(NifmServiceType_System);
	nifmGetInternetConnectionStatus(&connectionType, &strg, &connectionStatus);
	if (connectionStatus == NifmInternetConnectionStatus_Connected) return true;
	return (strg > 0);
}
}

void CheckImgVector(json List,int& index){
	index=0;
	int listsize=List.size();
	if (listsize <= 0) {return;}
	for (int x = 0; x < listsize && !quit; x++)
	{
		index = x+1;
		std::string tempima = List[x];
		if (tempima.length() == 0) continue;
		replace(tempima,"\"","");
		replace(tempima,"https://cdn.jkanime.net/assets/images/animes/image/","");
		replace(tempima,"https://jkanime.net/", "");
		replace(tempima,".jpg", "/");
		int v2 = tempima.find("/");
		if (v2 > 0) {tempima = tempima.substr(0, v2);}
		//std::cout << "img:" << tempima << std::endl;
		CheckImgNet(rootdirectory+"DATA/"+tempima+".jpg");
	}
	index=0;
}
bool CheckImgNet(std::string image,std::string url){
	replace(image,"\"","");
	if (!isFileExist(image.c_str())) {
		std::string tmp = "";
		if(url.length() > 0) {
			tmp = url;
		} else {
			tmp = "https://cdn.jkanime.net/assets/images/animes/image/"+image.substr(image.find_last_of("/\\") + 1);
		}
		cout << "# Missing "+image+", Downloading..." << endl;
		return Net::DOWNLOAD(tmp,image,false);
	}
	return true;
}
bool CheckUpdates(bool force){
	try{
		string Ver = DInfo()["App"];
		//Get Config
		json config = DInfo()["config"];
		if (config["AutoUpdate"].is_null()) {
			std::cout << "- Config empty " <<std::endl;
			return false;
		}

		//Get AutoUpdate state use 0 to disable
		if (config["AutoUpdate"].get<int>() != 1) {
			std::cout << "- AutoUpdate Disabled" <<std::endl;
			return false;
		}
		string reurl="https://api.github.com/repos/"+config["author"].get<string>()+"/"+config["repo"].get<string>()+"/releases";

		std::string APIJ = Net::GET(reurl);
		if(json::accept(APIJ))
		{
			//Parse and use the JSON data
			json base = json::parse(APIJ);
			//main vars
			if (!base[0]["tag_name"].is_null()) {

				//New ver
				string New = base[0]["tag_name"];
				if (New.length() > 5) New = New.substr(0,5); //Tomar Major, Minor, y Micro solamente

				//Check for new updates
				if (Ver != New || force) {
					//get assets
					json asset = base[0]["assets"];
					if (asset.size() <= 0) {
						std::cout << "- NO assets " <<std::endl;
						return false;
					}

					string Nurl= asset[0]["browser_download_url"];
					if (config["Beta"] == 1) {
						Nurl= config["Beta_URL"].get<string>();
					}

					//check if URl is ok
					if (Nurl.find(".nsp") == string::npos) {
						std::cout << "- NO " << Nurl <<std::endl;
						for (u64 i=0; i<asset.size(); i++ ) {
							Nurl= asset[i]["browser_download_url"];
							if (Nurl.find(".nsp") != string::npos) {
								break;
							}
						}
						if (Nurl.find(".nsp") == string::npos) {
							std::cout << "- NO, bad " << Nurl <<std::endl;
							return false;

						}
					}
					std::cout << "- OK " << Nurl <<std::endl;



					string fileU=rootdirectory+"update.nsp";

					//Have the update?
					json UP;
					bool needDown = true;
					if (read_DB(UP,fileU+".json")) {
						if(UP["update"] == New) {
							needDown = false;
						}

					}
					if (!isFileExist(fileU)) {
						needDown = true;
					}
					std::cout << Ver << " -> " << New <<std::endl;

					if (needDown) {
						//Download New Update
						if (!Net::DOWNLOAD(Nurl, fileU,false))
						{
							std::cout << "Download error" << New <<std::endl;
							return false;
						} else {
							//Store update data
							UP["update"]=New;
							write_DB(UP,fileU+".json");
							std::cout << Ver << " --> " << New <<std::endl;
						}
					}

					//Install New Update nsp
					if (InstallNSP(fileU)) {
						DInfo(New);
						std::cout << Ver << " ---> " << New <<std::endl;
                        //Relaunch After update success
                        if (config["ReLaunch"].get<int>() == 1) {
                            if(!quit) AppletMode = true;
                        }
						return true;
					}
				}
			}
		}
	} catch(...) {
		LOG::E(11);
		std::cout << "# Update Error catch" << std::endl;
	}
	return false;
}

/*

   Spected:
   JK.config
        {
                "AutoUpdate": 1,
                "Beta": 1,
                "Beta_URL": "https://mysitio.blabla/beta.nsp",
                "author":"??"
                "repo": "??"
        }

   On sd card root on the root dir of the app
   sdmc:/JK.config
   User:/RipJKAnime/JK.config
 */
