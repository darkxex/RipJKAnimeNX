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

extern int porcendown;
extern int sizeestimated;
extern int cancelcurl;

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

	// create the "meter"
	int ii = 0;
	porcendown = fractiondownloaded * 100;
	sizeestimated = TotalToDownload;
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

std::string gethtml(std::string enlace)
{

	CURL *curl;
	CURLcode res = CURLE_OK;
	std::string Buffer;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, enlace.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Buffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		if (res != CURLE_OK){printf("\n%s\n",curl_easy_strerror(res));}
	}
	return Buffer;
}
bool downloadfile(std::string enlace, std::string directorydown,bool progress)
{
	CURL *curl;
	CURLcode res = CURLE_OK;
	curl = curl_easy_init();
	if (curl) {
	FILE *fp = fopen(directorydown.c_str(), "wb");;
		if(fp){
			struct MemoryStruct chunk;
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
            if (res == CURLE_OK) fwrite(chunk.memory, 1, chunk.size, fp);// write from mem to file
			/* always cleanup */
			curl_easy_cleanup(curl);
			free(chunk.memory);
		}
	fclose(fp);
	if (res == CURLE_OK){return true;}else{printf("\n%s\n",curl_easy_strerror(res));}
	}
return false;
}


bool CheckImgNet(std::string image){
	if (!isFileExist(image.c_str())) {
		std::string tmp = "https://cdn.jkanime.net/assets/images/animes/image/"+image.substr(image.find_last_of("/\\") + 1);
		printf("# Missing %s Downloading\n",image.c_str());
		return downloadfile(tmp,image,false);
	}
return true;
}
