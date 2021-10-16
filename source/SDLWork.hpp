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
u64 time2=0;
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
void ScaleA(int H, int W);//Absolute
void ScaleR(int H, int W);//relative
int render_T(int x, int y, std::string text="", bool presed=false);
void render_VOX(SDL_Rect Form,int R, int G, int B, int A);
//render_AH
bool anend = false;
int delayp = 0;
bool render_AH(int x, int y, int w, int h, bool type);

//render_AW
bool anendW = false;
int delaypW = 0;
bool render_AW(int x, int y, int w, int h, bool type);


//get touch
bool SP();
bool SPr();
//Gets image dimensions
int getWidth();
int getHeight();
int getWidthR();
int getHeightR();
int getX();
int getY();
bool mark;
bool isZero();

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

bool isRe = false;

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
TTF_Font *AF_19 = NULL;
TTF_Font* AF_35 = NULL;
TTF_Font *Comic_16 = NULL;
TTF_Font *Arista_20 = NULL;
TTF_Font *Arista_27 = NULL;
TTF_Font *Arista_30 = NULL;
TTF_Font *Arista_40 = NULL;
TTF_Font *Arista_50 = NULL;
TTF_Font *Arista_100 = NULL;
TTF_Font *Arista_150 = NULL;
TTF_Font* digi_9 = NULL;
TTF_Font* digi_11 = NULL;
TTF_Font* digi_16 = NULL;
Mix_Music* gMusic = NULL;

//list vars
int outof=0;
int HR=200,HG=200,HB=200;
//Music
void SwapMusic(bool swap=true);
//Touch cords
bool fingerdown = false;
bool fingermotion = false;
bool fingermotion_UP = false;
bool fingermotion_DOWN = false;
bool fingermotion_LEFT = false;
bool fingermotion_RIGHT = false;
int TouchX=-1;
int TouchY=-1;
int GenState=-1;
int MasKey=-1;
std::string WorKey="00";
//Skin manager
void loadSkin(string img="");
void setSkin(string path="");
void selectskin(string val="");
//Main 
bool JKMainLoop();
bool Confirm(std::string text,std::string image,bool okonly = true);
void PleaseWait(std::string text,bool render = true);
void Image(std::string path,int X, int Y,int W, int H,int key);
void Cover(std::string path,int X, int Y,std::string Text = "",int WS = 300,int key=-1,int selected=0);
void ListCover(int& selectindex,json Jlinks, bool ongrid=false,int limit=0);
void ListClassic(int& selectindex,json Jlinks);
void Cover_idx(std::string path,int X, int Y,std::string Text,int WS,int index,int& select,bool render);
void HandleList(int& selectchapter, int allsize, int key,bool ongrid);
void ScrollBarDraw(int X, int Y,int H,int W, int Total, int Select,bool ongrid);
void deint();
};

extern SDLB GOD;
extern LTexture Farest;
extern LTexture VOX;
extern LTexture T_T;
extern LTexture Heart;
extern LTexture B_A, B_B, B_Y, B_X, B_L, B_R, B_ZR, B_ZL, B_M, B_P, B_RIGHT, B_LEFT, B_UP, B_DOWN;




