#include <map>
#include <vector>
#include "nlohmann/json.hpp"
#include <iomanip>
using namespace std;
using json = nlohmann::json;

//Texture wrapper class
class LTexture
{
public:
//Initializes variables
LTexture();

//Deallocates memory
~LTexture();
//peer key funct
bool reverse=false;
unsigned long long time2=0;
void TickerColor(int& color,int min,int max,unsigned long long sec = 0);
void TickerRotate(int& angle,int min,int max, int addangle=5,bool clock=true);
void TickerBomb(int sizescale=0);
void TickerScale();

//Loads image at specified path
bool loadFromFile(std::string path);
bool loadFromFileCustom(std::string path, int h, int w);
//Creates image from font string
bool loadFromRenderedText(TTF_Font *fuente, std::string textureText, SDL_Color textColor);
//Creates image from font string
bool loadFromRenderedTextWrap(TTF_Font *fuente, std::string textureText, SDL_Color textColor, Uint32 size);
//Deallocates texture
void free();

//Set color modulation
void setColor(Uint8 red, Uint8 green, Uint8 blue);

//Set blending
void setBlendMode(SDL_BlendMode blending);

//Set alpha modulation
void setAlpha(Uint8 alpha);

//Renders texture at given point
void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
void render_T(int x, int y, std::string text="", bool presed=false);
bool render_AH(int x, int y, int w, int h, bool type);
void render_VOX(SDL_Rect Form,int R, int G, int B, int A);
//get touch
bool SP();
bool SPr();
//Gets image dimensions
int getWidth();
int getHeight();
int getX();
int getY();
bool mark;

//texture Scale
int offtik=0;

//texture boom
int offboom=0;
//default boom size
int offboom_min=0, offboom_size=11;
//W H overwrite
int offW=0, offH=0;

private:
//The actual hardware texture
SDL_Texture* mTexture;


//Image dimensions
int mWidth, mHeight, mX, mY, SelIns;
};


class SDLB {
public:
void intA();
std::map<std::string,LTexture> MapT {};
SDL_Renderer* gRenderer = NULL;
SDL_Window* gWindow = NULL;
//Globally used font
TTF_Font *B_O_F = NULL;
TTF_Font *Arista = NULL;
TTF_Font *gFont = NULL;
TTF_Font *gFont2 = NULL;
TTF_Font *gFont3 = NULL;
TTF_Font *gFont4 = NULL;
TTF_Font *gFont5 = NULL;
TTF_Font *gFont6 = NULL;
TTF_Font* digifont = NULL;
TTF_Font* digifont2 = NULL;
TTF_Font* digifontC = NULL;
TTF_Font *gFontcapit = NULL;
Mix_Music* gMusic = NULL;

enum SDL_Keys {
	BT_A, BT_B, BT_X, BT_Y,
	BT_L3, BT_R3,
	BT_L, BT_R, BT_ZL, BT_ZR,
	BT_P, BT_M,
	BT_LEFT, BT_UP, BT_RIGHT, BT_DOWN,
	BT_LS_LEFT, BT_LS_UP, BT_LS_RIGHT, BT_LS_DOWN,
	BT_RS_LEFT, BT_RS_UP, BT_RS_RIGHT, BT_RS_DOWN,
	BT_1
};
//list vars
int outof=0;

//Touch cords
bool fingerdown = false;
bool fingermotion = false;
bool fingermotion_UP = false;
bool fingermotion_DOWN = false;
bool fingermotion_LEFT = false;
bool fingermotion_RIGHT = false;

int HR=200,HG=200,HB=200;

int TouchX=-1;
int TouchY=-1;
int GenState=-1;
int MasKey=-1;
std::string WorKey="00";

void Image(std::string path,int X, int Y,int W, int H,int key);
void Cover(std::string path,int X, int Y,std::string Text = "",int WS = 300,int key=-1,bool selected=false);
void PleaseWait(std::string text,bool render = true);
void ListCover(int& selectindex,json Jlinks, bool ongrid=false,int limit=0);
void ListClassic(int& selectindex,json Jlinks);
void Cover_idx(std::string path,int X, int Y,std::string Text,int WS,int index,int& select,bool render);
void HandleList(int& selectchapter, int allsize, int key,bool ongrid);
void ScrollBarDraw(int X, int Y,int H,int W, int Total, int Select,bool ongrid);
void deint();
};






