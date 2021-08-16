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

extern int porcendown;
extern int sizeestimated;
extern int cancelcurl;
extern std::string speedD;

//Write file in mem to increase download speed on 3 or 5 times
struct MemoryStruct
{
  char *memory;
  size_t size;
  int mode;
};

static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userdata)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userdata;

  char *ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);

  if (ptr == NULL)
  {
      printf("Failed to realloc mem");
      return 0;
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

int progress_func(void* ptr, double TotalToDownload, double NowDownloaded,
	double TotalToUpload, double NowUploaded)
{


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
		printf ("Error extracting time stuff\n");
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
	if (cancelcurl == 1)
	{
		return 1;
	}

	return 0;

}

std::string gethtml(std::string enlace,std::string POSTFIEL,bool redirect)
{

	CURL *curl;
	CURLcode res = CURLE_OK;
	std::string Buffer;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, enlace.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36");
		curl_easy_setopt(curl, CURLOPT_REFERER, enlace.c_str());
		if (POSTFIEL.length()){
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, POSTFIEL.c_str());
		}
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		if(redirect)
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		else
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Buffer);
		res = curl_easy_perform(curl);
		if(redirect&&CURLE_OK == res)
        {
			CURLcode curl_res;
			char *url = "";
			curl_res = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
			if((CURLE_OK == curl_res) && url)
             {
				printf("CURLINFO_EFFECTIVE_URL: %s\n", url);
				Buffer = std::string(url);
			}
        }
		curl_easy_cleanup(curl);
		if (res != CURLE_OK){printf("\n%s\n",curl_easy_strerror(res));}
	}
	//std::cout << "Buffer:  " << Buffer << std::endl;
	return Buffer;
}

bool downloadfile(std::string enlace, std::string directorydown,bool progress)
{
	if(!HasConnection()){return false;}
	CURL *curl;
	CURLcode res = CURLE_OK;
	curl = curl_easy_init();
	bool allok=false;
	if (curl) {
	struct MemoryStruct chunk;
	FILE *fp = fopen(directorydown.c_str(), "wb");;
		if(fp){
            chunk.memory = (char*)malloc(1);
            chunk.size = 0;
			curl_easy_setopt(curl, CURLOPT_URL, enlace.c_str());
			curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36");
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
				// Install the callback function
			if (progress){
				curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
				curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func);
			} else {
				curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
			}
			
			res = curl_easy_perform(curl);
			if ((res == CURLE_OK)){
				int thism = directorydown.find(".mp4");
				printf("#size:%ld found:%ld %d in:%s\n",chunk.size,directorydown.find(".mp4"),thism,directorydown.c_str());
				if (chunk.size < 1000000  && thism != -1){
					printf("####size:%ld found:%ld in:%s\n",chunk.size,directorydown.find(".mp4"),directorydown.c_str());
					allok=false;//
				} else {
					fwrite(chunk.memory, 1, chunk.size, fp);// write from mem to file
					allok=true;
				}
			}else{
				allok=false;
				printf("\n%s\n",curl_easy_strerror(res));
			}
			/* always cleanup */
			curl_easy_cleanup(curl);
			free(chunk.memory);
		}
	fclose(fp);
	if (!allok){remove(directorydown.c_str());}
	}
return allok;
}

bool HasConnection()
{
    u32 strg = 0;
	nifmInitialize(NifmServiceType_User);
    nifmGetInternetConnectionStatus(NULL, &strg, NULL);
	return (strg > 0);
}

bool CheckImgNet(std::string image){
	if (!isFileExist(image.c_str())) {
		std::string tmp = "https://cdn.jkanime.net/assets/images/animes/image/"+image.substr(image.find_last_of("/\\") + 1);
		printf("# Missing %s Downloading\n",image.c_str());
		return downloadfile(tmp,image,false);
	}
return true;
}
