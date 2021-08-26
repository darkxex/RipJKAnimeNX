#include <cstring>
#include <switch.h>
#include <unistd.h>
#include <iostream>
#include <ostream>
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "applet.hpp"
#include "SDLWork.hpp"
#include "utils.hpp"

extern AccountUid uid;
extern std::string AccountID;
extern u32 __nx_applet_exit_mode;
extern std::string urlc;
extern bool quit;
extern int cancelcurl;
extern std::string rootsave;

bool LoadNRO(std::string path){
if(isFileExist(path)){
	//__nx_applet_exit_mode = 0;//LoadNRO("sdmc:/switch/pplay/pplay.nro");
	envSetNextLoad(path.c_str(), path.c_str());
	cancelcurl = 1;
	quit = true;
	std::cout << "Saliendo a:" << path << std::endl;
	return true;
}
std::cout << "No Existe:" << path << std::endl;
return false;
}

   
std::string FormatHex128(AccountUid Number)
{
    auto ptr = reinterpret_cast<u8*>(Number.uid);
    std::stringstream strm;
    strm << std::hex << std::uppercase;
    for(u32 i = 0; i < 16; i++) strm << (u32)ptr[i];
    return strm.str();
}

bool SelectUser(){
    AccountUid user = LaunchPlayerSelect();
    if(accountUidIsValid(&user))
    {
        uid=user;
        return true;
    }
    return false;
}
bool GetUserID(){
	Result rc = 0;
	rc =  accountInitialize(AccountServiceType_Application);
	if (R_SUCCEEDED(rc)) {
		accountGetPreselectedUser(&uid);
		AccountID=FormatHex128(uid);
		printf("Goted user uid: %s\n",AccountID.c_str());
		return true;
	} else {
		printf("failed to get user\n");
		return false;
	}
}
bool MountUserSave(FsFileSystem& acc){
	fsdevCommitDevice("save");
	fsdevUnmountDevice("save");
	fsFsClose(&acc);
	
	if(R_SUCCEEDED(fsOpen_SaveData (&acc,0x05B9DB505ABBE000,uid))) {
		fsdevMountDevice("save", acc);
		GetUserImage();
		return true;
	}
printf("failed to Mount User Save\n");
return false;
}
bool GetUserImage(){
	AccountProfile prof;
	auto res = accountGetProfile(&prof, uid);
	if(res == 0)
	{
	    u32 iconsize = 0;
	    accountProfileGetImageSize(&prof, &iconsize);
	    if(iconsize > 0)
	    {
			u8 *icon = new u8[iconsize]();
			u32 tmpsz;
			res = accountProfileLoadImage(&prof, icon, iconsize, &tmpsz);
			if(res == 0)
			{
	#ifdef USENAND
			    FILE *f = fopen((rootsave+"User.jpg").c_str(), "wb");
	#else
			    FILE *f = fopen("sdmc:/switch/RipJKAnime_NX/User.jpg", "wb");
	#endif
			    if(f)
			    {
					fwrite(icon, 1, iconsize, f);
					printf("saved user image\n");
					fclose(f);
			    } else {
					printf("failed to open output file image\n");
				}
			}
			delete[] icon;
	    } else
			printf("failed user image size\n");
		
	    accountProfileClose(&prof);
		return true;
	}
printf("failed user image\n");
return false;
}
bool GetAppletMode(){
	appletSetFocusHandlingMode(AppletFocusHandlingMode_NoSuspend);
	AppletType at = appletGetAppletType();
	if (at != AppletType_Application && at != AppletType_SystemApplication)
	{
		return true;
	}
	__nx_applet_exit_mode = 1;
	GetUserID();
	return false;
}
AccountUid LaunchPlayerSelect() {
	AccountUid uids;
	s32 max_uids=10,actual_total;
	accountListAllUsers(&uids,max_uids,&actual_total);
	//std::cout << "# accountListAllUsers: " << actual_total << std::endl;
	AccountUid out_id = {};
	if(actual_total > 1){
		LibAppletArgs args;
		libappletArgsCreate(&args, 0x10000);
		u8 st_in[0xA0] = {0};
		u8 st_out[0x18] = {0};
		size_t repsz;
		auto res = libappletLaunch(AppletId_LibraryAppletPlayerSelect, &args, st_in, 0xA0, st_out, 0x18, &repsz);
		if(R_SUCCEEDED(res))
		{
			u64 lres = *(u64*)st_out;
			AccountUid *uid_ptr = (AccountUid*)&st_out[8];
			if(lres == 0) memcpy(&out_id, uid_ptr, sizeof(out_id));
		}
	}
	return out_id;
}

SwkbdTextCheckResult Keyboard_ValidateText(char *string, size_t size) {
	if (strcmp(string, "") == 0) {
		strncpy(string, "No has escrito nada!.", size);
		return SwkbdTextCheckResult_Bad;
	}

	return SwkbdTextCheckResult_OK;
}

std::string KeyboardCall (std::string hint, std::string text){
	Result ret = 0;
	SwkbdConfig swkbd;
	static char input_string[256];

	if (R_FAILED(ret = swkbdCreate(&swkbd, 0))) {
		swkbdClose(&swkbd);
		return "";
	}

	swkbdConfigMakePresetDefault(&swkbd);
	swkbdConfigSetInitialCursorPos (&swkbd, 0);
	if (strlen(hint.c_str()) != 0)
		swkbdConfigSetGuideText(&swkbd, hint.c_str());

	if (strlen(text.c_str()) != 0)
		swkbdConfigSetInitialText(&swkbd, text.c_str());

	swkbdConfigSetTextCheckCallback(&swkbd, Keyboard_ValidateText);

	if (R_FAILED(ret = swkbdShow(&swkbd, input_string, sizeof(input_string)))) {
		swkbdClose(&swkbd);
		return "";
	}

	swkbdClose(&swkbd);

	char *buf = (char*)malloc(256);
	strcpy(buf, input_string);
	return std::string(buf);
}


Result WebBrowserCall(std::string url,bool nag){
	Result rc = 0;	
	if (nag){
		url = KeyboardCall ("Escribir URL http://", url);
		urlc = url;
		if (url.length() <= 0) return 0;
	}
	
	WebCommonConfig config;
	WebCommonReply reply;
	WebExitReason exitReason = (WebExitReason)0;
	// Create the config. There's a number of web*Create() funcs, see libnx web.h.
	// webPageCreate/webNewsCreate requires running under a host title which has HtmlDocument content, when the title is an Application. When the title is an Application when using webPageCreate/webNewsCreate, and webConfigSetWhitelist is not used, the whitelist will be loaded from the content. Atmosphère hbl_html can be used to handle this.
	rc = webPageCreate(&config, url.c_str());

	printf("webPageCreate(): 0x%x\n", rc);
	if (R_SUCCEEDED(rc)) {
 		if (nag){
			printf("Try to save logins\n");//
		   //mount user data to save logins and that stuff
			webConfigSetUid(&config,uid);
		}
		printf("SetMainConfigs\n");
		webConfigSetScreenShot (&config, true);
		webConfigSetBootDisplayKind (&config, WebBootDisplayKind_Black);
		webConfigSetPageCache (&config, true);
		webConfigSetBootLoadingIcon (&config, true);
		webConfigSetPageScrollIndicator (&config, true);
		webConfigSetMediaPlayerSpeedControl (&config, true);	
		webConfigSetMediaAutoPlay (&config, true);
		if (!nag){
			printf("SetCapConfigs\n");
			webConfigSetDisplayUrlKind (&config, false);
			webConfigSetMediaPlayerAutoClose (&config, true);
			//play direct links
			//if(url.substr(9,9) == "-delivery" || url.find("apidata.googleusercontent")) 
			webConfigSetBootAsMediaPlayer(&config, true);
			//webConfigSetFooter(&config, false);
			

			//block redirection
			if(url.substr(0,20) == "https://jkanime.net/") 
			{
				webConfigSetWhitelist(&config, "^https://jkanime\\.net($|/)");
			} else webConfigSetWhitelist(&config, "^http*");
		}
		

		// Launch the applet and wait for it to exit.
		printf("Running webConfigShow...\n");
		rc = webConfigShow(&config, &reply); // If you don't use reply you can pass NULL for it.
		printf("webConfigShow(): 0x%x\n", rc);

		if (R_SUCCEEDED(rc)) { // Normally you can ignore exitReason.
			rc = webReplyGetExitReason(&reply, &exitReason);
			printf("webReplyGetExitReason(): 0x%x", rc);
			if (R_SUCCEEDED(rc)) printf(", 0x%x", exitReason);
			printf("\n");
		}
	}
	return rc;
}

//KeyboardCall ("Buscar Anime (3 letras minimo.)", "");
 
