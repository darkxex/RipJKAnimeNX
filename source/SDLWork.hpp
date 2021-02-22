
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

	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};

void DrawImageCover(SDL_Renderer* gRenderer,std::string path,int X, int Y,std::string Text = "",int scale = 100);
void DrawImageFile(SDL_Renderer* gRenderer,std::string path,int X, int Y,std::string Text = "");





