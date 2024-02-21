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
#include "Networking.hpp"
#include <nspmini.hpp>
#include <sys/stat.h>
#include <fstream>
namespace fs = std::filesystem;


bool LoadNRO(std::string path){
	if(isFileExist(path)) {
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
bool InstallNSP(std::string nsp){
	std::cout << "Install: " << nsp << std::endl;
	return mini::InstallSD(nsp);
}
bool IsRunning(string servname) {//check if service is running
	auto srv_name = smEncodeName(servname.c_str());
	Handle tmph = 0;
	auto rc = smRegisterService(&tmph, srv_name, false, 1);
	if(R_FAILED(rc)) {
		return true;
	}
	smUnregisterService(srv_name);
	return false;
}
bool ReloadDNS(){
	if (IsRunning("sfdnsres")) {
		static Service sfdnsresSrv;
		// Call sfdnsres service to reload hosts file
		smGetService(&sfdnsresSrv, "sfdnsres");
		serviceDispatch(&sfdnsresSrv, 65000);
		serviceClose(&sfdnsresSrv);
		return true;
	} else {
		cout <<"AMS DNS not running" <<std::endl;
		return false;
	}
}

bool mount_theme(string in,bool mount){
	string file = rootdirectory+"themes00.romfs";
	erase(rootdirectory+"theme.romfs");
	erase(rootdirectory+"themes.romfs");
	if (mount) {
		if(isFileExist(in+":/")) {
			//cout << in+":/  is mounted" << endl;
			return true;
		}
		if(isFileExist(file)) {
			if (R_FAILED(romfsMountFromFsdev(file.c_str(), 0, "themes"))) {
				cout << "unable to mount  "+ in << endl;
				return false;
			}else{
                string ExDR = read_FL("romfs:/ED");
                string ExDT = read_FL(in+":/ED");
                if (ExDR != ExDT){
                    cout << "MainV" << ExDR << " ThemeV" << ExDT <<" Mistmatch"<< endl;
                    ThemeNeedUpdate = true;
                    return false;
                }
				return true;
			}
		}
	} else {
		romfsUnmount (in.c_str());
		cout << in+":/  now Closed" << endl;
		return true;
	}
	cout << "unable to mount  "+ file << endl;
	return false;
}

//Tx Part
Result txStealthMode(uint64_t enable) {
	Result rc=0x0;
	if (IsRunning("tx")) {
		static Service g_txSrv;
		rc = smGetService(&g_txSrv, "tx");
		rc = serviceDispatchIn(&g_txSrv, 134, enable);
		serviceClose(&g_txSrv);
	}
	return rc;
}


std::string FormatHex128(AccountUid Number){
	auto ptr = reinterpret_cast<u8*>(Number.uid);
	std::stringstream strm;
	strm << std::hex << std::uppercase;
	for(u32 i = 0; i < 16; i++) strm << (u32)ptr[i];
	return strm.str();
}
std::string u64toHex(u64 Number){
	char TID[0x9F];
	sprintf(TID, "%016lX",Number);
	return std::string(TID);
}

//Get ncap Title Info
NacpStruct TitleIDinfo(u64 tid){
	nsInitialize();
	NacpStruct nacp;
	uint64_t outSize = 0;
	NsApplicationControlData *ctrlData = new NsApplicationControlData;
	NacpLanguageEntry *ent;
	Result ctrlRes = nsGetApplicationControlData(NsApplicationControlSource_Storage, tid, ctrlData, sizeof(NsApplicationControlData), &outSize);
	Result nacpRes = nacpGetLanguageEntry(&ctrlData->nacp, &ent);
	size_t iconSize = outSize - sizeof(ctrlData->nacp);

	if(R_SUCCEEDED(ctrlRes) && !(outSize < sizeof(ctrlData->nacp)) && R_SUCCEEDED(nacpRes) && iconSize > 0)
	{
		//Copy nacp
		memcpy(&nacp, &ctrlData->nacp, sizeof(NacpStruct));
		cout << "Gotted ncap of title: " << u64toHex(tid) << endl;
	}
	else
	{
		memset(&nacp, 0, sizeof(NacpStruct));
		cout << "Fail to get ncap of title: " << u64toHex(tid) << endl;
	}
	delete ctrlData;
	return nacp;
}
/*
0x010000000000100A
0x010000000000100B
0x0100000000001010

010000000000100a-8000000000001071-0000000000000039
010000000000100b-8000000000001061
0100000000001010-8000000000001091-000000000000001d
        testsave(0x8000000000001071);
        testsave(0x8000000000001061);
        testsave(0x8000000000001091);


void testsave(u64 sid){
    FsFileSystem dataW;
    std::stringstream ss;
    ss << std::hex << sid;
    string e = ss.str();
    mkdir("sdmc:/titles",0777);
    mkdir(("sdmc:/titles/"+e).c_str(),0777);
    string out = "sdmc:/titles/"+e+"/";

    if(R_SUCCEEDED(fsOpen_SystemSaveData (&dataW,FsSaveDataSpaceId_System,sid,uid))) {
        cout <<"Open save bro OK " << e <<std::endl;
        fsdevMountDevice("bro", dataW);
        std::string path = "bro:/";
        for (const auto & entry : fs::directory_iterator(path)){
            string h = entry.path();
            replace(h,"bro:/","");
            std::cout << entry.path() << std::endl;      
            copy_me(entry.path(), out+h);
            //remove((entry.path()).c_str());
        }
        //copy_me("bro:/", out+"");
        fsdevCommitDevice("bro");
        fsdevUnmountDevice("bro");
    }else{
        cout <<"Open save bro not OK " << e <<std::endl;
    }

	fsFsClose(&dataW);
}
*/

namespace user {
AccountUid uid;
string AccountID;
FsFileSystem savedata;

AccountUid g_uid(){
	return uid;
}
string g_ID(){
	return AccountID;
}
void recover(){
	if(isFileExist(rootdirectory+AccountID+"UserData.json")) {
		if(!isFileExist(rootsave+"UserData.json")) {
			if (copy_me(rootdirectory+AccountID+"UserData.json", rootsave+"UserData.json")) {
				fsdevCommitDevice("save");
				remove((rootdirectory+AccountID+"UserData.json").c_str());
				remove((rootdirectory+AccountID+"User.jpg").c_str());
			}
		}
	}
}
bool initUser(){
	Result rc = 0;
	hidInitialize();
	setsysInitialize();
	setInitialize();

	rc =  accountInitialize(AccountServiceType_Administrator);
	if (R_SUCCEEDED(rc)) {
		accountGetPreselectedUser(&uid);
		if(!accountUidIsValid(&uid)) {
			cout <<"U:2" <<std::endl;
			accountTrySelectUserWithoutInteraction (&uid, false);
		}

		if(!accountUidIsValid(&uid)) {
			cout <<"U:3" <<std::endl;
			accountGetLastOpenedUser (&uid);
		}

		if(!accountUidIsValid(&uid)) {
			cout <<"U:4" <<std::endl;
			accountGetLastOpenedUser (&uid);
		}
		/*
		   //may crash some times
		   if(!accountUidIsValid(&uid)) {
		        cout <<"U:5" <<std::endl;
		        s32 actual_total;
		        accountListAllUsers(&uid,1,&actual_total);
		   }

		 */
        
		cout <<"User Init OK" <<std::endl;
		return GetUserID();
	} else {
		cout << "Failied to init User" <<std::endl;
		return false;
	}
}
bool GetUserID(){
	if(accountUidIsValid(&uid))
	{
		AccountID=FormatHex128(uid);
		cout << "Gotted user uid:"<< AccountID.c_str() <<std::endl;
		return true;
	}
	cout << "Failied to get user ID" <<std::endl;
	return false;
}
bool SelectUser(){
	AccountUid s_uids;
	s32 max_uids=10,actual_total;
	accountListAllUsers(&s_uids,max_uids,&actual_total);
	//std::cout << "# accountListAllUsers: " << actual_total << std::endl;
	AccountUid user = {};
	if(actual_total > 1) {
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
			if(lres == 0) memcpy(&user, uid_ptr, sizeof(user));
		}
	}

	if(accountUidIsValid(&user))
	{
		if (FormatHex128(user) == FormatHex128(uid)) {
			return false;
		}

		uid=user;
		return GetUserID();
	}
	return false;
}
bool createSaveData(uint64_t _tid, AccountUid _userID) {
	//createSaveData For the actual user
	FsFileSystem abc;
	if(R_SUCCEEDED(fsOpen_SaveData (&abc,0x05B9DB505ABBE000,_userID))) {
		//cout << "Save Exist" << endl;
		fsFsClose(&abc);
		return true;
	}

	NacpStruct nacp;
	nacp = TitleIDinfo(_tid);

	FsSaveDataAttribute attr;
	memset(&attr, 0, sizeof(FsSaveDataAttribute));
	attr.application_id = _tid;
	attr.uid = _userID;
	attr.system_save_data_id = 0;
	attr.save_data_type = FsSaveDataType_Account;
	attr.save_data_rank = 0;
	attr.save_data_index = 0;

	FsSaveDataCreationInfo svCreate;
	memset(&svCreate, 0, sizeof(FsSaveDataCreationInfo));
	int64_t saveSize = 0, journalSize = 0;

	saveSize = nacp.user_account_save_data_size;
	journalSize = nacp.user_account_save_data_journal_size;

	svCreate.save_data_size = saveSize;
	svCreate.journal_size = journalSize;
	svCreate.available_size = 0x4000;
	svCreate.owner_id = nacp.save_data_owner_id;
	svCreate.flags = 0;
	svCreate.save_data_space_id = FsSaveDataSpaceId_User;

	FsSaveDataMetaInfo meta;
	memset(&meta, 0, sizeof(FsSaveDataMetaInfo));

	meta.size = 0x40060;
	meta.type = FsSaveDataMetaType_Thumbnail;

	Result res = 0;
	if(R_SUCCEEDED(res = fsCreateSaveDataFileSystem(&attr, &svCreate, &meta))) {
		cout << "Save Created" << endl;
		return true;
	} else {
		cout << "Save Fail" << endl;
	}
	return false;
}
bool MountUserSave(){
	if(accountUidIsValid(&uid))
	{
		deinitUser();
		createSaveData(0x05B9DB505ABBE000,uid);
		if(R_SUCCEEDED(fsOpen_SaveData (&savedata,0x05B9DB505ABBE000,uid))) {
			fsdevMountDevice("save", savedata);
			rootsave = "save:/";
			recover();
			GetUserImage();
			return true;
		} else {
			rootsave = rootdirectory+AccountID;
			GetUserImage();
			cout << "Failied to Mount User Save" <<std::endl;
			return false;
		}
	} else {
		cout << "Invalid User UID" <<std::endl;
	}
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
					//cout << "Saved user image: "+rootsave+"User.jpg" <<std::endl;
					fclose(f);
				} else {
					cout << "Failied to open output file image" <<std::endl;
				}
			}
			delete[] icon;
		} else
			cout << "Failied user image size" <<std::endl;

		accountProfileClose(&prof);
		return true;
	}
	cout << "Failied user image" <<std::endl;
	return false;
}
bool commit(){
	fsdevCommitDevice("save");
	return true;
}
bool deinitUser(){
	fsdevCommitDevice("save");
	fsdevUnmountDevice("save");
	fsFsClose(&savedata);
	return true;
}
}

namespace emmc {
FsFileSystem appdata;
bool isMounted=false;
//Mount user
bool init(){
    bool MountEMMC = true;
    MountEMMC = (DInfo()["config"]["MountSD"].get<int>() == 0);
    if (MountEMMC){
        if(R_SUCCEEDED(fsOpenBisFileSystem(&appdata, FsBisPartitionId_User, ""))) {
            fsdevMountDevice("emmc", appdata);
            isMounted=true;
            return isMounted;
        }
	}
    
	rootdirectory = oldroot;
	isMounted=false;
	fsFsClose(&appdata);
	return isMounted;
}
bool commit(){
	if (isMounted) {
		fsdevCommitDevice("emmc");
	}
	return isMounted;
}
bool Save(){
	// write prettified JSON
	write_DB(BD,rootdirectory+"DataBase.json");
	write_DB(AB,rootdirectory+"AnimeMeta.json");
	write_DB(UD,rootsave+"UserData.json");
	return true;
}
bool deinit(){
	if (isMounted) {
		fsdevCommitDevice("emmc");
		fsdevUnmountDevice("emmc");
		fsFsClose(&appdata);
        isMounted=false;
	}
	return isMounted;
}
}

bool GetAppletMode(){
	//nxlinkStdio();
	isSXOS=IsRunning("tx");
	apmInitialize();
	AppletType at = appletGetAppletType();
	if (at != AppletType_Application && at != AppletType_SystemApplication)
	{
		return true;
	}
    //if (DInfo()["TID"] == "05B9DB505ABBE000")
	{
		__nx_applet_exit_mode = 1;
	}
	return false;
}

json DInfo(string ver){
	static json info;
	if (info["App"].is_null()) {
		//INIT
		setInitialize();
		setsysInitialize();
		splInitialize();
		pminfoInitialize();
		pmdmntInitialize();

		Result ret = 0;

		//Get Config file , order sdmc -> romfs -> Nand -> Default
		json config;
		if (!read_DB(config,oldroot+"JK.config")) {
            if (!read_DB(config,"sdmc:/JK.config")) {
                std::cout << "# Using default Config" <<std::endl;
                if (!read_DB(config,"romfs:/JK.config")) {
                    std::cout << "# Error default Config" <<std::endl;
                }
			}
		}
		
		info["config"]=config;
        CDNURL = config["CDNURL"].get<string>();
        //std::cout << "# CDMURL " << CDNURL <<std::endl;
		info["UNIX"]=time(0);
		//DeviceID
		u64 id = 0;
		if (R_FAILED(ret = splGetConfig(SplConfigItem_DeviceId, &id))) {
			printf("splGetConfig(SplConfigItem_DeviceId,) Failied: 0x%x.\n\n", ret);
		} else {
			char DeviceID[0x9F];
			sprintf(DeviceID, "%lX",id);
			info["DeviceID"]=std::string(DeviceID);

		}

		//Running APP
		u64 m_pid = 0,m_tid = 0;
		pmdmntGetApplicationProcessId(&m_pid);
		pminfoGetProgramId(&m_tid, m_pid);
		char TID[0x9F];
		sprintf(TID, "%016lX",m_tid);
		info["TID"]=std::string(TID);
		info["PID"]=m_pid;

		//Get Device Firmware Vercion
		SetSysFirmwareVersion ver;
		if (R_FAILED(ret = setsysGetFirmwareVersion(&ver))) {
			printf("setsysGetFirmwareVersion() Failied: 0x%x.\n\n", ret);
		} else {
			info["Firmware"]=std::string(ver.display_version);
		}

		//Device NickName
		SetSysDeviceNickName nick;
		if (R_FAILED(ret = setGetDeviceNickname(&nick))) {
			printf("setGetDeviceNickname() Failied: 0x%x.\n\n", ret);
		} else {
			info["nickname"]=std::string(nick.nickname);
		}

		//Public Serial Number
		SetSysSerialNumber serialNX;
		if (R_FAILED(ret = setsysGetSerialNumber(&serialNX))) {
			printf("setsysGetSerialNumber() Failied: 0x%x.\n\n", ret);
		} else {
			if(strlen(serialNX.number) == 0) {
				//info["Incognito"]="true";
				sprintf(serialNX.number, "XAW00000000000");
			} else {
				if (string(serialNX.number) != "XAW00000000000" && string(serialNX.number) != "XAW00000000001") {
					//info["Blank_prod"]="true";
				}
				//info["Incognito"]="false";
			}
			info["Serial"]=std::string(serialNX.number);
		}

		//Model of switch board
		SetSysProductModel modelo;
		if (R_FAILED(ret = setsysGetProductModel(&modelo))) {
			printf("setsysGetProductModel() Failied: 0x%x.\n\n", ret);
		} else {
			string a="UM";//"Unknow Model";
			switch(modelo) {
			case SetSysProductModel_Invalid:
				a="IM";//"Invalid Model";
				break;
			case SetSysProductModel_Nx:
				a="ERM";//Erista Model
				break;
			case SetSysProductModel_Copper:
				a="ESM";//"Erista Simulation Model";
				break;
			case SetSysProductModel_Iowa:
				a="MKM";//"Mariko Model";
				break;
			case SetSysProductModel_Hoag:
				a="MLM";//"Mariko Lite Model";
				break;
			case SetSysProductModel_Calcio:
				a="MSM";//"Mariko Simulation Model";
				break;
			case SetSysProductModel_Aula:
				a="MOM";//"Mariko oled Model(?) ";
				break;
			}
			info["Model"]=a;
		}

		//Region of console
		SetRegion region;
		if (R_FAILED(ret = setGetRegionCode(&region))) {
			printf("setGetRegionCode() Failied: 0x%x.\n\n", ret);
		} else {
			string a="Wut?";
			switch(region) {
			case SetRegion_JPN:
				a="JPN";//Japan
				break;
			case SetRegion_USA:
				a="USA";//The Americas
				break;
			case SetRegion_EUR:
				a="EUR";//Europe
				break;
			case SetRegion_AUS:
				a="AUS";//Australia/New Zealand
				break;
			case SetRegion_HTK:
				a="HTK";//Hong Kong/Taiwan/Korea
				break;
			case SetRegion_CHN:
				a="CHN";//China
				break;
			}
			info["Region"]=a;
		}

		//App Ver
		info["App"]=read_FL("romfs:/V");;

		setsysExit();
		cout << std::setw(4) << info << std::endl;
	}
	if (ver.length() > 0) {
		info["App"]=ver;
	}
	return info;
}

bool ChainManager(bool Chain,bool AndChaing){
	if (Chain) {
		if (AndChaing) {
			appletSetCpuBoostMode(ApmCpuBoostMode_FastLoad);
			appletSetAutoSleepDisabled(true);
			appletSetAutoSleepTimeAndDimmingTimeEnabled(false);
			appletSetFocusHandlingMode(AppletFocusHandlingMode_NoSuspend);
			cout << "& Chain App >" <<std::endl;
			return true;
		} else return false;
	} else {
		if (AndChaing) {
			appletSetCpuBoostMode(ApmCpuBoostMode_Normal);
			appletCancelCpuBoostMode();
			appletSetAutoSleepDisabled(false);
			appletSetAutoSleepTimeAndDimmingTimeEnabled(true);
			appletSetFocusHandlingMode(AppletFocusHandlingMode_SuspendHomeSleep);
			cout << "& UnChain App >" <<std::endl;
			return true;
		} else return false;
	}
}

bool GetCookies(string &cookies){
    //Obtener y salvar las cookies del navegador
    //0x8000000000001071
    FsFileSystem dataW;
    bool stateS = true;
    if(R_SUCCEEDED(fsOpen_SystemSaveData (&dataW,FsSaveDataSpaceId_System,0x8000000000001071,user::uid))) {
        fsdevMountDevice("bro", dataW);
        std::string path = "bro:/Cookie.dat";

        std::ifstream inf(path);
        if(!inf.fail()) {
            std::string tempcookie="";
            for(int f = 0; !inf.eof(); f++)
            {
                string TempLine = "";
                getline(inf, TempLine);
                tempcookie += TempLine;
            }
            inf.close();

            string cookiestr="#Cookies Generated by Browser\n\n";
            int r = 8,t=0,s=tempcookie.size();
            while (t<3 && r < s)
            {
                string hexd = string_to_hex(tempcookie.substr(r,1));
                if (hexd != "00"){
                    cookiestr+=tempcookie.substr(r,1);
                    t=0;
                } else {
                    cookiestr+="\n";
                    t++;
                }
                r++;
            }
            cookies = cookiestr;
            std::ofstream otf(rootdirectory+"COOKIES.txt");
            otf << cookiestr;
            otf.close();
            std::cout << "Cookie Imported... size " << cookiestr.size() << std::endl;
        } else {
            stateS = false;
        }		
        fsdevUnmountDevice("bro");
    }else{
        cout << "Cookie not Readed... " <<std::endl;
        stateS = false;
    }
	fsFsClose(&dataW);
    return stateS;
}
bool ClearWebData(u64 uidsave){
    //Borrar datos del navegador
/*
    ClearWebData(0x8000000000001071);
    ClearWebData(0x8000000000001091);
    ClearWebData(0x8000000000001061);
*/
    FsFileSystem dataW;
    bool stateS = true;
    if(R_SUCCEEDED(fsOpen_SystemSaveData (&dataW,FsSaveDataSpaceId_System,uidsave,user::uid))) {
        fsdevMountDevice("bro", dataW);
        remove("bro:/Autofill.dat");
        remove("bro:/Cookie.dat");
        remove("bro:/VisitedLink.dat");
        remove("bro:/WebStorage.dat");
        fsdevCommitDevice("bro");
        fsdevUnmountDevice("bro");
        cout << "Data Cleared... " << uidsave <<std::endl;
    }else{
        cout << "Data not Cleared... " <<std::endl;
        stateS = false;
    }

	fsFsClose(&dataW);
    return stateS;
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

	if (R_FAILED(ret = swkbdCreate(&swkbd, 64))) {
		swkbdClose(&swkbd);
		return "";
	}

	swkbdConfigMakePresetDefault(&swkbd);
	swkbdConfigSetInitialCursorPos (&swkbd, 0);
	swkbdConfigSetOkButtonText(&swkbd,"Buscar");
	if (hint=="Buscar el Anime") {
		swkbdConfigSetHeaderText(&swkbd, "Buscador de Anime");
		swkbdConfigSetSubText(&swkbd, "Escribe el Nombre del Anime que Deseas Buscar");
		swkbdConfigSetStringLenMax(&swkbd, 32);
	} else if (hint == "Escribir URL http://") {
		swkbdConfigSetHeaderText(&swkbd, "Navegador Integrado");
		swkbdConfigSetSubText(&swkbd, "Escribe la URL que quieres visitar");
		swkbdConfigSetStringLenMax(&swkbd, 32);
	}

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

Result WebBrowserCloud(std::string url){
    string cburl = url+"404.shtml";
    url = url+to_string(Frames)+"M/";
    cout << "Cloud WEB :"+url <<std::endl;

	Result rc = 0;
	WebCommonConfig config;
	WebCommonReply reply;

	rc = webNewsCreate(&config, url.c_str());
	printf("webPageCreate(): 0x%x\n", rc);
	if (R_SUCCEEDED(rc)) {
		//mount user data to save logins and that stuff
		webConfigSetUid(&config,user::uid);

        //webConfigSetCallbackUrl (&config, (url+"?__cf_chl_tk=").c_str()); 	
        webConfigSetCallbackUrl (&config, (cburl).c_str());
        webConfigSetFooter(&config, false);
		webConfigSetJsExtension (&config, true);
		webConfigSetBootDisplayKind (&config, WebBootDisplayKind_Black);
		webConfigSetPageCache (&config, true);
		webConfigSetBootLoadingIcon (&config, true);
		webConfigSetPageScrollIndicator (&config, true);
        webConfigSetPointer(&config, false);
        webConfigSetTouchEnabledOnContents(&config, false);

		//block redirection
		if(url.substr(0,20) == "https://jkanime.net/") {
			webConfigSetWhitelist(&config, "^https://jkanime\\.net($|/)");
		} else {
			webConfigSetWhitelist(&config, "^http*");
		}

        WebExitReason exitReason = WebExitReason_UnknownE;
        //webConfigShow (WebCommonConfig *config, WebCommonReply *out)
        webConfigShow(&config, &reply);
        webConfigRequestExit (&config);
		rc = webReplyGetExitReason(&reply, &exitReason);
		printf("webReplyGetExitReason(): 0x%x", rc);
		if (R_SUCCEEDED(rc)) printf(", 0x%x", exitReason);
		printf("\n");
	}
	return rc;
}
Result WebBrowserCall(std::string url,bool nag,bool withe){//https://switchbrew.github.io/libnx/web_8h.html
	Result rc = 0;
	cout << "WEB :"+url <<std::endl;
	if (nag) {
		url = KeyboardCall ("Escribir URL http://", url);
		urlc = url;
		if (url.length() <= 0) return 0;
	}
	WebCommonConfig config;
	WebCommonReply reply;
	// Create the config. There's a number of web*Create() funcs, see libnx web.h.
	// webPageCreate/webNewsCreate requires running under a host title which has HtmlDocument content, when the title is an Application. When the title is an Application when using webPageCreate/webNewsCreate, and webConfigSetWhitelist is not used, the whitelist will be loaded from the content. Atmosphère hbl_html can be used to handle this.
	rc = webNewsCreate(&config, url.c_str());
	printf("webPageCreate(): 0x%x\n", rc);
	if (R_SUCCEEDED(rc)) {
		//mount user data to save logins and that stuff
		printf("Try to save logins\n");//
		webConfigSetUid(&config,user::uid);

		printf("SetMainConfigs\n");
        //webConfigSetUserAgentAdditionalString(&config, "(Windows NT 10.0; Win64; x64; rv:105.0) Gecko/20100101 Firefox/105.0");
        //webConfigSetPageCache (&config, true);
/*

        webConfigSetPlayReport(&config, false);
        webConfigSetEcClientCert(&config, false);
		webConfigSetScreenShot (&config, true);


*/
		webConfigSetJsExtension (&config, true);
		webConfigSetBootDisplayKind (&config, WebBootDisplayKind_Black);
		webConfigSetPageCache (&config, true);
		webConfigSetBootLoadingIcon (&config, true);
		webConfigSetPageScrollIndicator (&config, true);
		webConfigSetMediaPlayerSpeedControl (&config, true);
		webConfigSetMediaAutoPlay (&config, true);
		//webConfigSetTransferMemory (&config, true);

		//Player section
		if (!nag) {
			bool Direct=false;
			printf("SetCapConfigs\n");
			//webConfigSetDisplayUrlKind (&config, false);
			//webConfigSetPlayReport(&config, false);
			//webConfigSetFooter(&config, false);

			//Ignore for direct play
			//if(url.find("/gsplay/player.html") == string::npos){Direct=false;}

			//Use direct play on
			if(url.find("mxdcontent.net/") != string::npos) {Direct=true;}//Mixdrop
			if(url.find("fvs.io/redirector") != string::npos) {Direct=true;}//Fembed

			if (Direct) {
				webConfigSetMediaPlayerAutoClose (&config, true);
				webConfigSetBootAsMediaPlayer(&config, true);
			}

			//block redirection
            /*
			if(url.substr(0,20) == "https://jkanime.net/") {
				webConfigSetWhitelist(&config, "^https://jkanime\\.net($|/)");
            */
            if (withe){
                string tempcon = "";
                tempcon = scrapElement(url, "https://","/");
                replace(tempcon, ".", "\\\\.");
                tempcon = string("^") + tempcon + "($|/)";
                std::cout << "Withe list:" << tempcon << endl;;
                webConfigSetWhitelist(&config, tempcon.c_str());
			} else {
				webConfigSetWhitelist(&config, "^http*");
			}
		}
        WebExitReason exitReason = WebExitReason_UnknownE;
        //webConfigShow (WebCommonConfig *config, WebCommonReply *out)
        
        webConfigShow(&config, &reply);
        webConfigRequestExit (&config);
		rc = webReplyGetExitReason(&reply, &exitReason);
		printf("webReplyGetExitReason(): 0x%x", rc);
		if (R_SUCCEEDED(rc)) printf(", 0x%x", exitReason);
/*
        webConfigRequestExit (&config);
        
	//WebSession session;
        
		// Create Session
		webSessionCreate (&session, &config);

		//Star A web Session and NOT wait for exit if is player session
		Event *out_event;
		rc = webSessionStart (&session, &out_event);
		printf("Running session... 0x%x ", rc);
//		if (nag)
		{
			rc = webSessionWaitForExit (&session, &reply);
			if (R_SUCCEEDED(rc)) printf(", 0x%x", rc);

			if (R_SUCCEEDED(rc)) { // Normally you can ignore exitReason.
				rc = webReplyGetExitReason(&reply, &exitReason);
				printf("webReplyGetExitReason(): 0x%x", rc);
				if (R_SUCCEEDED(rc)) printf(", 0x%x", exitReason);
			}
		}
                else {
                        sleep(3);
                }
 */
		printf("\n");
	}
	return rc;
}

Result WebWiFiCall(std::string url){//https://switchbrew.github.io/libnx/web_8h.html
    nsvmInitialize();
	Result rc = 0;
	cout << "WiFiWEB :"+url <<std::endl;
    WebWifiConfig config;
    WebWifiReturnValue out;
    webWifiCreate(&config, NULL, url.c_str(), (Uuid) {0} , 0);
    rc = webWifiShow (&config, &out);

	printf("webPageCreate(): 0x%d\n", rc);
	if (R_SUCCEEDED(rc)) {
        printf(", 0x%d", rc);
    }
    nsvmExit();
	return rc;
}

//KeyboardCall ("Buscar Anime (3 letras minimo.)", "");

