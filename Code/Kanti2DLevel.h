#ifndef KANTI2D_LEVEL

struct Tileset
{
	SDL_Texture* Texture;
	int32 FirstGID;

	Tileset()
	{
		FirstGID = -1;
	}

	Tileset(SDL_Texture* TileSetTexture, int32 TilesetFirstGID)
	{
		Texture = TileSetTexture;
		FirstGID = TilesetFirstGID;
	}
};

class Level
{
public:

	inline Level()
	{

	}

	inline Level(string LevelMapName, Vector2 LevelSpawnPoint, Graphics& RenderGraphics)
	{
		MapName = LevelMapName;
		SpawnPoint = LevelSpawnPoint;
		LevelSize = Vector2(0, 0);

		LoadMap(MapName, RenderGraphics);
	}

	inline Level(string FileName, Graphics& RenderGraphics)
	{
		LoadMapFile(FileName, RenderGraphics);
	}

	inline ~Level()
	{

	}

	inline void LoadMapFile(string LevelMapName, Graphics& RenderGraphics)
	{
		SDL_RWops* File = SDL_RWFromFile(LevelMapName.c_str(), "rb");

		const int64 FileSize = (int64)File->size;
		char* Buffer = (char *)malloc(FileSize * sizeof(char));
		File->read(File, Buffer, FileSize * sizeof(char), 1);

		string FileContents = Buffer;

		uint32 Width = 0, Height = 0, TileWidth = 0, TileHeight = 0;

		nlohmann::json JsonFile = nlohmann::json::parse(FileContents.c_str());

		MapName = JsonFile["tilesets"][0]["name"].get<string>();

		Width = JsonFile["width"].get<uint32>();
		Height = JsonFile["height"].get<uint32>();

		TileWidth = JsonFile["tilewidth"].get<uint32>();
		TileHeight = JsonFile["tileheight"].get<uint32>();

		LevelSize = Vector2(Width, Height);

		string ImageUsed = JsonFile["tilesets"][0]["image"].get<string>();

		uint32 FirstGID = JsonFile["tilesets"][0]["firstgid"].get<uint32>();

		ImageUsed = ImageUsed.substr(ImageUsed.find_first_of('/'));
		ImageUsed = "../Data/" + ImageUsed;

		SDL_Texture* Texture = SDL_CreateTextureFromSurface(RenderGraphics.Renderer, RenderGraphics.LoadImage(ImageUsed));
		TileSets.push_back(Tileset(Texture, FirstGID));

		nlohmann::json LayerList = nlohmann::json(JsonFile["layers"]);

		for (uint32 LayerIndex = 0; LayerIndex < LayerList.size(); ++LayerIndex)
		{
			if (LayerList[LayerIndex]["name"].get<string>() == "Collisions")
			{
				for (uint32 ObjectIndex = 0; ObjectIndex < LayerList[LayerIndex]["objects"].size(); ++ObjectIndex)
				{
					real32 X = LayerList[LayerIndex]["objects"][ObjectIndex]["x"].get<real32>();
					real32 Y = LayerList[LayerIndex]["objects"][ObjectIndex]["y"].get<real32>();
					real32 Width = LayerList[LayerIndex]["objects"][ObjectIndex]["width"].get<real32>();
					real32 Height = LayerList[LayerIndex]["objects"][ObjectIndex]["height"].get<real32>();

					CollisionRectangles.push_back(Rectangle(X, Y, Width, Height));
				}
			}
			else
			{
				std::vector<uint32> TileData = LayerList[LayerIndex]["data"].get<std::vector<uint32>>();

				for (uint32 TileIndex = 0; TileIndex < TileData.size(); ++TileIndex)
				{
					uint32 GID = TileData[TileIndex];

					if (GID == 0)
					{
						continue;
					}

					Tileset Set;

					for (uint32 TileSetIndex = 0; TileSetIndex < TileSets.size(); ++TileSetIndex)
					{
						if (TileSets[TileSetIndex].FirstGID <= (int32)GID)
						{
							Set = TileSets.at(TileSetIndex);
							break;
						}
					}

					uint32 XX = 0;
					uint32 YY = 0;

					XX = TileIndex % Width;
					XX *= TileWidth;
					YY += TileHeight * (TileIndex / Width);

					Vector2 FinalTilePosition = Vector2(XX, YY);

					uint32 TileSetWidth, TileSetHeight;
					SDL_QueryTexture(Set.Texture, NULL, NULL, (int *)&TileSetWidth, (int *)&TileSetHeight);
					uint32 TSXX = GID % (TileSetWidth / TileWidth) - 1;
					TSXX *= TileWidth;

					uint32 TSYY = 0;
					uint32 Amount = (GID / (TileSetWidth / TileWidth));

					TSYY = TileHeight * Amount;

					Vector2 FinalTileSetPosition = Vector2(TSXX, TSYY);

					Tile BuildTile(Set.Texture, Vector2(TileWidth, TileHeight), FinalTileSetPosition, FinalTilePosition);
					TileList.push_back(BuildTile);
				}
			}
		}
	}

	inline void LoadMap(string LevelMapName, Graphics& RenderGraphics)
	{
		BackgroundTexture = SDL_CreateTextureFromSurface(RenderGraphics.Renderer, RenderGraphics.LoadImage("../Data/Images/bkBlue.bmp"));
		LevelSize = Vector2(1280, 960);
	}

	inline void Update(uint32 ElapsedTime)
	{

	}

	inline void Draw(Graphics& RenderGraphics)
	{
		/*
		SDL_Rect SourceRect = { 0, 0, 128, 128 };
		SDL_Rect DestinationRect;

		for (uint32 WindowX = 0; WindowX < LevelSize.X / 128; ++WindowX)
		{
			for (uint32 WindowY = 0; WindowY < LevelSize.Y / 128; ++WindowY)
			{
				DestinationRect.x = WindowX * 128;
				DestinationRect.y = WindowY * 128;
				DestinationRect.w = 128;
				DestinationRect.h = 128;

				RenderGraphics.BlitSurface(BackgroundTexture, &SourceRect, &DestinationRect);
			}
		}
		*/

		for (uint32 Index = 0; Index < TileList.size(); ++Index)
		{
			TileList[Index].Draw(RenderGraphics);
		}
	}

	inline std::vector<Rectangle> CheckTileCollisions(Rectangle CollisionToCheck)
	{
		std::vector<Rectangle> Collisions;

		for (uint32 Index = 0; Index < CollisionRectangles.size(); ++Index)
		{
			if (CollisionRectangles[Index].CollisionWith(CollisionToCheck))
			{
				Collisions.push_back(CollisionRectangles[Index]);
			}
		}

		return Collisions;
	}

	string MapName;
	Vector2 SpawnPoint;

	Vector2 LevelSize;
	Vector2 TileSize;

	SDL_Texture* BackgroundTexture;

	std::vector<Tile> TileList;
	std::vector<Tileset> TileSets;

	std::vector<Rectangle> CollisionRectangles;
};

#define KANTI2D_LEVEL
#endif

