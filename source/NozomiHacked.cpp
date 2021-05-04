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

static size_t SecondCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}
std::string getFirstKey(std::string enlace)
{

	CURL *curl;
	CURLcode res = CURLE_OK;
	std::string Buffer;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, enlace.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36");
		curl_easy_setopt(curl, CURLOPT_REFERER, "https://jkanime.net/");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, SecondCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Buffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		if (res != CURLE_OK){printf("\n%s\n",curl_easy_strerror(res));}
	}

     std::string codetemp = Buffer;
	   int temp1, temp2;
	   temp1 = codetemp.find("name=\"data\" value=\"");
	   temp1 = temp1 +19;
	   temp2 = codetemp.find("\"",temp1);
	  codetemp = codetemp.substr(temp1,temp2 - temp1);
	   std::cout << "FirstKey: "<< codetemp << std::endl;
       Buffer = codetemp;
	return Buffer;
}
std::string getSecondKey(std::string data)
{
	data = "data=" + data;
   std::string redirected = "";
	CURL *curl;
    CURLcode curl_res;

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();

    if (curl)
    {
		
        curl_easy_setopt(curl, CURLOPT_URL, "https://jkanime.net/gsplay/redirect_post.php");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		
        	curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36");
		 curl_easy_setopt(curl, CURLOPT_REFERER, "https://jkanime.net/");

        /* Perform the request, curl_res will get the return code */ 
        curl_res = curl_easy_perform(curl);

        /* Check for errors */ 
        if(curl_res != CURLE_OK)
          fprintf(stderr, "curl_easy_perform() failed: %s\n",
                  curl_easy_strerror(curl_res));

        if(CURLE_OK == curl_res) 
        {
             char *url;
             curl_res = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);

             if((CURLE_OK == curl_res) && url)
                { printf("CURLINFO_EFFECTIVE_URL: %s\n", url);
				redirected = url;}
        }

        /* always cleanup */ 
        curl_easy_cleanup(curl);

        /* we're done with libcurl, so clean it up */ 
        curl_global_cleanup();

    }
    else
    {
        printf("cURL error.\n");
    }
     std::cout << "Secondkey: "<< redirected << std::endl;
return redirected;

}

std::string getThirdKey(std::string secondKey)
{

	CURL *curl;
	CURLcode res = CURLE_OK;
	std::string Buffer;
    std::string second = "v=" + secondKey;
    replace(second,"https://jkanime.net/gsplay/player.html#","");
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://jkanime.net/gsplay/api.php");
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36");
		curl_easy_setopt(curl, CURLOPT_REFERER, "https://jkanime.net/");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, second.c_str());
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, SecondCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Buffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		if (res != CURLE_OK){printf("\n%s\n",curl_easy_strerror(res));}
	}

     std::string codetemp = Buffer;
	   int temp1, temp2;
	   temp1 = codetemp.find("https:");
	   temp2 = codetemp.find("\"",temp1);
	  codetemp = codetemp.substr(temp1,temp2 - temp1);
      replace(codetemp,"\\","");
	   std::cout << "ThirdKey: "<< codetemp << std::endl;
       Buffer = codetemp;
	return Buffer;
}