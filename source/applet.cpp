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
#include "utils.hpp"
#include "applet.hpp"
#include "SDLWork.hpp"
 /// Console Product Models
 typedef enum {
     SetSysProductModel_Invalid = 0, ///< Invalid Model
     SetSysProductModel_Nx      = 1, ///< Erista Model
     SetSysProductModel_Copper  = 2, ///< Erista "Simulation" Model
     SetSysProductModel_Iowa    = 3, ///< Mariko Model
     SetSysProductModel_Hoag    = 4, ///< Mariko Lite Model
     SetSysProductModel_Calcio  = 5, ///< Mariko "Simulation" Model
     SetSysProductModel_Aula    = 6, ///< Mariko Pro Model(?)
 } SetSysProductModel;
 
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

std::string FormatHex128(AccountUid Number){
    auto ptr = reinterpret_cast<u8*>(Number.uid);
    std::stringstream strm;
    strm << std::hex << std::uppercase;
    for(u32 i = 0; i < 16; i++) strm << (u32)ptr[i];
    return strm.str();
}

bool initUser(){
	Result rc = 0;
	rc =  accountInitialize(AccountServiceType_Application);
	if (R_SUCCEEDED(rc)) {
		accountGetPreselectedUser(&uid);
		printf("User Init OK\n");
		return GetUserID();
	} else {
		printf("failed to init User\n");
		return false;
	}
}

bool SelectUser(){
    AccountUid user = LaunchPlayerSelect();
    if(accountUidIsValid(&user))
    {
		uid=user;
		return GetUserID();
    }
return false;
}
bool GetUserID(){
    if(accountUidIsValid(&uid))
    {
		AccountID=FormatHex128(uid);
 		printf("Gotted user uid: %s\n",AccountID.c_str());
       return true;
    }
printf("failed to get user ID\n");
return false;
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
			    FILE *f = fopen((rootsave+"User.jpg").c_str(), "wb");
			    if(f)
			    {
					fwrite(icon, 1, iconsize, f);
					printf("Saved user image: %s\n",(rootsave+"User.jpg").c_str());
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
	initUser();
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

json DInfo(){
	json info;
	setInitialize();
	setsysInitialize();
	splInitialize();
	Result ret = 0;
	
	//DeviceID
	u64 id = 0;
	splGetConfig(SplConfigItem_DeviceId, &id);
	char DeviceID[0x9F];
	sprintf(DeviceID, "%lX",id);
	info["DeviceID"]=std::string(DeviceID);

	//Get Device Firmware Vercion
	SetSysFirmwareVersion ver;
    if (R_FAILED(ret = setsysGetFirmwareVersion(&ver))) {
        printf("setsysGetFirmwareVersion() failed: 0x%x.\n\n", ret);
    } else {
		info["Firmware"]=std::string(ver.display_version);
	}
	
	//Device NickName
	SetSysDeviceNickName nick;
    if (R_FAILED(ret = setGetDeviceNickname(&nick))) {
        printf("setGetDeviceNickname() failed: 0x%x.\n\n", ret);
    } else {
		info["nickname"]=std::string(nick.nickname);
	}

	//Public Serial Number
	SetSysSerialNumber serialNX;
    if (R_FAILED(ret = setsysGetSerialNumber(&serialNX))) {
        printf("setsysGetSerialNumber() failed: 0x%x.\n\n", ret);
    } else {
		if(strlen(serialNX.number) == 0) {
			//info["Incognito"]="true";
			sprintf(serialNX.number, "XAW00000000000");
		} else {
			if (serialNX.number != "XAW00000000000" && serialNX.number != "XAW00000000001"){
				//info["Blank_prod"]="true";
			}
			//info["Incognito"]="false";
		}
		info["Serial"]=std::string(serialNX.number);
	}
	
	//Model of switch board
	s32 modelo;
    if (R_FAILED(ret = setsysGetProductModel(&modelo))) {
        printf("setsysGetProductModel() failed: 0x%x.\n\n", ret);
    } else {
		string a="Invalid Model";
		switch(modelo){
			case SetSysProductModel_Nx:
				a="Erista Model.";
				break;
			case SetSysProductModel_Copper:
				a="Erista Simulation Model.";
				break;
			case SetSysProductModel_Iowa:
				a="Mariko Model.";
				break;
			case SetSysProductModel_Hoag:
				a="Mariko Lite Model.";
				break;
			case SetSysProductModel_Calcio:
				a="Mariko Simulation Model.";
				break;
			case SetSysProductModel_Aula:
				a="Mariko Pro Model(?) ";
				break;
		}
		info["Model"]=a;
	}
	
	//Region of console
	SetRegion region;
    if (R_FAILED(ret = setGetRegionCode(&region))) {
        printf("setGetRegionCode() failed: 0x%x.\n\n", ret);
    } else {
		string a="Wut?";
		switch(region){
			case SetRegion_JPN:
				a="Japan.";
				break;
			case SetRegion_USA:
				a="The Americas.";
				break;
			case SetRegion_EUR:
				a="Europe.";
				break;
			case SetRegion_AUS:
				a="Australia/New Zealand.";
				break;
			case SetRegion_HTK :
				a="Hong Kong/Taiwan/Korea.";
				break;
			case SetRegion_CHN:
				a="China.";
				break;			
		}
		info["Region"]=a;
	}
	
	setsysExit();
	std::cout << std::setw(4) << info << std::endl;
    return info;
}

bool ChainManager(bool Chain,bool AndChaing){
	if (Chain){
		if (AndChaing){
			appletSetAutoSleepDisabled(true);
			appletSetAutoSleepTimeAndDimmingTimeEnabled(false);
			appletSetFocusHandlingMode(AppletFocusHandlingMode_NoSuspend);
			printf("Chain App...\n");
			return true;
		} else return false;
	} else {
		if (AndChaing){
			appletSetAutoSleepDisabled(false);
			appletSetAutoSleepTimeAndDimmingTimeEnabled(true);
			appletSetFocusHandlingMode(AppletFocusHandlingMode_SuspendHomeSleep);
			printf("UnChain App...\n");
			return true;
		} else return false;
	}
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
 
