#ifndef KANTI2D_GRAPHICS

class Graphics 
{
public:

	inline Graphics()
	{
		SDL_CreateWindowAndRenderer(640, 480, 0, &Window, &Renderer);
		SDL_SetWindowTitle(Window, "Cavestory");
	}

	inline ~Graphics()
	{
		SDL_DestroyWindow(Window);
		SDL_DestroyRenderer(Renderer);
	}

	inline SDL_Surface* LoadImage(const string &FileName)
	{
		if (SpriteSheets.count(FileName) == 0)
		{
			SpriteSheets[FileName] = IMG_Load(FileName.c_str());
		}

		return SpriteSheets[FileName];
	}

	inline void BlitSurface(SDL_Texture* Texture, SDL_Rect* SourceRect, SDL_Rect* DestinationRect)
	{
		SDL_RenderCopy(Renderer, Texture, SourceRect, DestinationRect);
	}

	inline void Draw()
	{
		SDL_RenderPresent(Renderer);
	}

	inline void Clear()
	{
		SDL_RenderClear(Renderer);
	}

		SDL_Window* Window;
		SDL_Renderer* Renderer;

		std::map<string, SDL_Surface*> SpriteSheets;
};

#define KANTI2D_GRAPHICS

#endif
