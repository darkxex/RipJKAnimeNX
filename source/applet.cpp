#include <cstring>
#include <switch.h>
#ifdef __SWITCH__
#include <unistd.h>
#include "applet.hpp"

extern AccountUid uid;
extern u32 __nx_applet_exit_mode;

bool GetAppletMode()
{
	AppletType at = appletGetAppletType();
	if (at != AppletType_Application && at != AppletType_SystemApplication)
	{
		return true;
	}
	__nx_applet_exit_mode = 1;
	return false;
}

SwkbdTextCheckResult Keyboard_ValidateText(char *string, size_t size) {
	if (strcmp(string, "") == 0) {
		strncpy(string, "The name cannot be empty.", size);
		return SwkbdTextCheckResult_Bad;
	}

	return SwkbdTextCheckResult_OK;
}

const char *Keyboard_GetText(const char *guide_text, const char *initial_text) {
	Result ret = 0;
	SwkbdConfig swkbd;
	static char input_string[256];

	if (R_FAILED(ret = swkbdCreate(&swkbd, 0))) {
		swkbdClose(&swkbd);
		return "";
	}

	swkbdConfigMakePresetDefault(&swkbd);

	if (strlen(guide_text) != 0)
		swkbdConfigSetGuideText(&swkbd, guide_text);

	if (strlen(initial_text) != 0)
		swkbdConfigSetInitialText(&swkbd, initial_text);

	swkbdConfigSetTextCheckCallback(&swkbd, Keyboard_ValidateText);

	if (R_FAILED(ret = swkbdShow(&swkbd, input_string, sizeof(input_string)))) {
		swkbdClose(&swkbd);
		return "";
	}

	swkbdClose(&swkbd);
	return input_string;
}

std::string KeyboardCall (std::string hint, std::string text){
char *buf = (char*)malloc(256);
strcpy(buf, Keyboard_GetText(hint.c_str(), text.c_str()));
return std::string(buf);
}


Result WebBrowserCall(std::string url,bool nag){
	Result rc = 0;	
	if (nag){
		url = KeyboardCall ("Escribir URL http://", url);
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
//		webConfigSetPlayReport (&config, false);
		webConfigSetMediaAutoPlay (&config, true);

		if (!nag){
			printf("SetCapConfigs\n");
			webConfigSetDisplayUrlKind (&config, false);
			webConfigSetMediaPlayerAutoClose (&config, true);
			webConfigSetFooter(&config, false);
		}

		webConfigSetWhitelist(&config, "^http*");// ^http*.jkstremum.* \r\n^http*.mega.*

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
#endif 
