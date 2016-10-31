#ifndef KANTI2D_RENDERMANAGER

class RenderManager
{
public:

	// Variables

	SDL_Window* SDLWindow;
	SDL_Renderer* SDLRenderer;

	std::map<string, SDL_Surface*> SpriteSheets;

	k_internal RenderManager* CurrentRenderer;

public:

	// Functions

	inline RenderManager()
	{
		CurrentRenderer = this;
	}

	inline ~RenderManager()
	{
		SDL_DestroyWindow(SDLWindow);
		SDL_DestroyRenderer(SDLRenderer);
	}

	inline void CreateWindow(Vector2 WindowSize, string WindowName, uint32 Flags = 0)
	{
		SDL_CreateWindowAndRenderer((uint32)WindowSize.X, (uint32)WindowSize.Y, Flags, &SDLWindow, &SDLRenderer);
		SDL_SetWindowTitle(SDLWindow, WindowName.c_str());
	}

	inline void CreateWindow(Vector2 WindowSize, string WindowName, SDL_Window* Window, SDL_Renderer* Renderer, uint32 Flags = 0)
	{
		SDL_CreateWindowAndRenderer((uint32)WindowSize.X, (uint32)WindowSize.Y, Flags, &Window, &Renderer);
		SDL_SetWindowTitle(Window, WindowName.c_str());
	}

	inline void SaveImage(const string& Name, const string& FileName)
	{
		if (SpriteSheets.count(Name) == 0)
		{
			SpriteSheets[Name] = IMG_Load(FileName.c_str());
		}
	}

	inline SDL_Surface* LoadImageFile(const string& FileName)
	{
		string Name;
		uint32 SlashIndex = FileName.find_last_of('/');
		if (SlashIndex != FileName.size())
		{
			Name = FileName.substr(SlashIndex + 1);
		}

		uint32 DotIndex = Name.find_last_of('.');
		if (DotIndex != Name.size())
		{
			Name = Name.substr(0, DotIndex);
		}

		if (SpriteSheets.count(Name) == 0)
		{
			SpriteSheets[Name] = IMG_Load(FileName.c_str());
		}

		return SpriteSheets[Name];
	}

	inline SDL_Surface* LoadImage(const string& Name)
	{
		return SpriteSheets[Name];
	}

	inline void BlitSurface(SDL_Texture* Texture, SDL_Rect* SourceRect, SDL_Rect* DestinationRect)
	{
		SDL_RenderCopy(SDLRenderer, Texture, SourceRect, DestinationRect);
	}

	inline void Draw()
	{
		SDL_RenderPresent(SDLRenderer);
	}

	inline void Clear()
	{
		SDL_RenderClear(SDLRenderer);
	}
};

RenderManager* RenderManager::CurrentRenderer = new RenderManager();

#define KANTI2D_RENDERMANAGER
#endif

