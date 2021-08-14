

class SDLB {
public:
void intA();
SDL_Renderer* gRenderer = NULL;
SDL_Window* gWindow = NULL;
//Globally used font
TTF_Font *B_O_F = NULL;
TTF_Font *gFont = NULL;
TTF_Font *gFont2 = NULL;
TTF_Font *gFont3 = NULL;
TTF_Font *gFont4 = NULL;
TTF_Font *gFont5 = NULL;
TTF_Font* digifont = NULL;
TTF_Font* digifontC = NULL;
TTF_Font *gFontcapit = NULL;
Mix_Music* gMusic = NULL;
int TouchX=-1;
int TouchY=-1;
int GenState=-1;
void Image(std::string path,int X, int Y,std::string Text = "",bool off=false);
void Cover(std::string path,int X, int Y,std::string Text = "",int HS = 100);
void PleaseWait(std::string text,bool render = true);
void deint();
};

//Texture wrapper class
class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

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
	void render_VOX(SDL_Rect Form ,int R, int G, int B, int A);
	//get touch
	bool SP();
	//Gets image dimensions
	int getWidth();
	int getHeight();
	int getX();
	int getY();
	bool mark;
private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
	int mX;
	int mY;
	int SelIns;
	
};






