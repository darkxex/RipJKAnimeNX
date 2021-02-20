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
void downloadfile(std::string enlace, std::string directorydown)
{

	CURL *curl;
	FILE *fp;
	CURLcode res = CURLE_OK;

	curl = curl_easy_init();
	if (curl) {

		fp = fopen(directorydown.c_str(), "wb");
		curl_easy_setopt(curl, CURLOPT_URL, enlace.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		// Install the callback function
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func);
		res = curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
		fclose(fp);
	if (res != CURLE_OK){printf("\n%s\n",curl_easy_strerror(res));}
	}
}

int downloadjkanimevideo(void* data)
{
	std::string videourl = "";


	std::string content = "";
	std::string enlacejk = urltodownload;
	std::string namedownload = enlacejk;
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
			downloadfile(videourl, directorydownload);

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
				downloadfile(videourl, directorydownload);
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
		downloadfile(videourl, directorydownload);
	}
	}
	
	return 0;
}
void onlinejkanimevideo(std::string onlineenlace)
{
#ifdef __SWITCH__
	Result rc = 0;
#endif //  SWITCH


	std::string videourl = "";
	std::string content = "";


	std::string enlacejk = onlineenlace;

	content = gethtml(enlacejk.c_str());
	int val1 = 0, val2 = 0;

	val1 = content.find("https://jkanime.net/um.php?");
	if (val1 != -1)
	{
		val2 = content.find('"', val1);

		videourl = content.substr(val1, val2 - val1);
		replace(videourl, "\\", "");


		std::cout << videourl << std::endl;



	}
	else
	{
		val1 = content.find("https://jkanime.net/jk.php?");
		if (val1 != -1)
		{
			val2 = content.find('"', val1);

			videourl = content.substr(val1, val2 - val1);
			replace(videourl, "\\", "");


			std::cout << videourl << std::endl;

		}
	}
#ifdef __SWITCH__
	WebCommonConfig config;
	WebCommonReply reply;
	WebExitReason exitReason = (WebExitReason)0;

	// Create the config. There's a number of web*Create() funcs, see libnx web.h.
	// webPageCreate/webNewsCreate requires running under a host title which has HtmlDocument content, when the title is an Application. When the title is an Application when using webPageCreate/webNewsCreate, and webConfigSetWhitelist is not used, the whitelist will be loaded from the content. Atmosphère hbl_html can be used to handle this.
	rc = webPageCreate(&config, videourl.c_str());
	printf("webPageCreate(): 0x%x\n", rc);

	if (R_SUCCEEDED(rc)) {
		// At this point you can use any webConfigSet* funcs you want.

		rc = webConfigSetWhitelist(&config, "^http*"); // Set the whitelist, adjust as needed. If you're only using a single domain, you could remove this and use webNewsCreate for the above (see web.h for webNewsCreate).
		printf("webConfigSetWhitelist(): 0x%x\n", rc);

		if (R_SUCCEEDED(rc)) { // Launch the applet and wait for it to exit.
			printf("Running webConfigShow...\n");
			rc = webConfigShow(&config, &reply); // If you don't use reply you can pass NULL for it.
			printf("webConfigShow(): 0x%x\n", rc);
		}

		if (R_SUCCEEDED(rc)) { // Normally you can ignore exitReason.
			rc = webReplyGetExitReason(&reply, &exitReason);
			printf("webReplyGetExitReason(): 0x%x", rc);
			if (R_SUCCEEDED(rc)) printf(", 0x%x", exitReason);
			printf("\n");
		}
	}
#endif //  SWITCH

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
void callimage()
{
#ifdef __SWITCH__
	std::string directorydownloadimage = "sdmc:/RipJKAnime/";
	directorydownloadimage.append(std::to_string(selectchapter));
	directorydownloadimage.append(".jpg");
#else
	std::string directorydownloadimage = "C:/respaldo2017/C++/test/Debug/RipJKAnime/";
	directorydownloadimage.append(std::to_string(selectchapter));
	directorydownloadimage.append(".jpg");
#endif // SWITCH
	TPreview.loadFromFileCustom(directorydownloadimage.c_str(), sizeportraitx, sizeportraity);
	tempimage = directorydownloadimage;
}
void callimagesearch()
{
	
#ifdef __SWITCH__
		std::string directorydownloadimage = "sdmc:/RipJKAnime/s";
		directorydownloadimage.append(std::to_string(searchchapter));
		directorydownloadimage.append(".jpg");
#else
		std::string directorydownloadimage = "C:/respaldo2017/C++/test/Debug/RipJKAnime/s";
		directorydownloadimage.append(std::to_string(searchchapter));
		directorydownloadimage.append(".jpg");
#endif // SWITCH

		
		TSearchPreview.loadFromFileCustom(directorydownloadimage.c_str(), sizeportraitx, sizeportraity);
		tempimage = directorydownloadimage;
	
}
int refrescarpro(void* data)
{
	activatefirstimage = true;
	reloading = true;
	porcentajereload = 0;
	int val1 = 1;
	int val2;
	int val3, val4;
	int val0 = 0;
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
		
		val1++;

	}
	printf(temporal.c_str());

	CURL *curl;
	FILE *fp;
	CURLcode res = CURLE_OK;

	for (int x = 0; x < (int)arrayimages.size(); x++)
	{

	curl = curl_easy_init();
	if (curl) {
		std::string tempima = arrayimages[x];
		std::cout << tempima << selectchapter << std::endl;
#ifdef __SWITCH__
		std::string directorydownloadimage = "sdmc:/RipJKAnime/";
		directorydownloadimage.append(std::to_string(x));
		directorydownloadimage.append(".jpg");
#else
		std::string directorydownloadimage = "C:/respaldo2017/C++/test/Debug/RipJKAnime/" ;
		directorydownloadimage.append(std::to_string(x));
		directorydownloadimage.append(".jpg");
#endif // SWITCH

		fp = fopen(directorydownloadimage.c_str(), "wb");
		curl_easy_setopt(curl, CURLOPT_URL, tempima.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
		fclose(fp);
		if (res != CURLE_OK){printf("\n%s\n",curl_easy_strerror(res));}
		
	}
	porcentajereload = ((x+1) * 100) / arrayimages.size();
	}
	
	reloading = false;
	preview = true;

	return 0;
}


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
	if (searchtext.length() >= 3) {
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
			CURL *curl;
			FILE *fp;
			CURLcode res = CURLE_OK;


			curl = curl_easy_init();
			if (curl) {
				std::string tempima = arraysearchimages[x];
				std::cout << tempima << selectchapter << std::endl;
#ifdef __SWITCH__
				std::string directorydownloadimage = "sdmc:/RipJKAnime/s";
				directorydownloadimage.append(std::to_string(x));
				directorydownloadimage.append(".jpg");
#else
				std::string directorydownloadimage = "C:/respaldo2017/C++/test/Debug/RipJKAnime/s";
				directorydownloadimage.append(std::to_string(x));
				directorydownloadimage.append(".jpg");
#endif // SWITCH

				fp = fopen(directorydownloadimage.c_str(), "wb");
				curl_easy_setopt(curl, CURLOPT_URL, tempima.c_str());
				curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36");
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
				curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
				res = curl_easy_perform(curl);
				/* always cleanup */
				curl_easy_cleanup(curl);
				fclose(fp);
				if (res != CURLE_OK){printf("\n%s\n",curl_easy_strerror(res));}
				
			}

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
	std::cout << rese << std::endl;
	if (a.find("<b>En emision</b>"))
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


};