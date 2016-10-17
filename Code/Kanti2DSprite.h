#ifndef KANTI2D_SPRITE

class Sprite
{
public:

	inline Sprite()
	{

	}

	inline Sprite(Graphics &RenderGraphics, const string &FileName,
		SDL_Rect RenderRectangle, Vector2 StartPosition)
	{
		SourceRect = RenderRectangle;

		SDL_Surface* Surface = RenderGraphics.LoadImage(FileName);

		SDL_SetColorKey(Surface, 1, SDL_MapRGB(Surface->format, 0, 0, 0));

		SpriteSheet = SDL_CreateTextureFromSurface(RenderGraphics.Renderer, Surface);

		if (!SpriteSheet)
		{
			printf("\nError: Unable to load image\n");
		}

		BoundingBox = Rectangle((uint32)RenderRectangle.x, (uint32)RenderRectangle.y,
			(uint32)RenderRectangle.w, (uint32)RenderRectangle.h);
	}

	inline virtual ~Sprite()
	{

	}

	inline virtual void Update(Vector2 GameObjectPosition, uint32 ElapsedTime)
	{
		BoundingBox = Rectangle(GameObjectPosition.X, GameObjectPosition.Y, (real32)SourceRect.w, (real32)SourceRect.h);
	}

	inline virtual void Draw(Graphics &RenderGraphics, uint32 X, uint32 Y)
	{
		SDL_Rect DestinationRectangle = { (int32)X, (int32)Y, SourceRect.w, SourceRect.h };
		RenderGraphics.BlitSurface(SpriteSheet, &SourceRect, &DestinationRectangle);
	}

	inline SIDE GetCollisionSide(Rectangle Collider)
	{
		uint32 AmountTop, AmountBottom, AmountRight, AmountLeft;

		AmountTop = Collider.GetBottom() - BoundingBox.GetTop();
		AmountBottom = BoundingBox.GetBottom() - Collider.GetTop();
		AmountLeft = Collider.GetRight() - BoundingBox.GetLeft();
		AmountRight = BoundingBox.GetRight() - Collider.GetLeft();


		uint32 Values[4] = { (uint32)AbsoluteValue((real32)AmountTop), (uint32)AbsoluteValue((real32)AmountBottom),
			(uint32)AbsoluteValue((real32)AmountLeft) , (uint32)AbsoluteValue((real32)AmountRight) };

		uint32 Lowest = Values[0];
		uint32 LowIndex = 0;
		for (uint32 Index = 0; Index < 4; ++Index)
		{
			if (Values[Index] < Lowest)
			{
				Lowest = Values[Index];
				LowIndex = Index;
			}
		}

		SIDE Result = {};

		Result = (SIDE)LowIndex;

		return (Result);
	}

	SDL_Rect SourceRect;
	SDL_Texture* SpriteSheet;

	Rectangle BoundingBox;
};

class AnimatedSprite : public Sprite
{
public:

	inline AnimatedSprite()
	{

	}

	inline AnimatedSprite(Graphics& RenderGraphics, string FileName, SDL_Rect Rectangle, Vector2 StartPosition, real32 UpdateTime) :
		Sprite(RenderGraphics, FileName, Rectangle, StartPosition)
	{
		TimeToUpdate = UpdateTime;
		FrameIndex = 0;
		TimeElapsed = 0.0;
		IsVisible = 1;
		CurrentAnimPlayOnce = 0;
		CurrentAnim = "";
	}

	inline void PlayAnimation(string Animation, bool32 PlayOnce = 0)
	{
		CurrentAnimPlayOnce = PlayOnce;

		if (CurrentAnim != Animation)
		{
			CurrentAnim = Animation;
			FrameIndex = 0;
		}
	}

	inline virtual void Draw(Graphics& RenderGraphics, uint32 X, uint32 Y) override
	{
		if (IsVisible == 1)
		{
			SDL_Rect DestinationRectangle;
			DestinationRectangle.x = X + (uint32)Offsets[CurrentAnim].X;
			DestinationRectangle.y = Y + (uint32)Offsets[CurrentAnim].Y;
			DestinationRectangle.w = SourceRect.w;
			DestinationRectangle.h = SourceRect.h;

			SDL_Rect SourceRectangle = Animations[CurrentAnim][FrameIndex];

			RenderGraphics.BlitSurface(SpriteSheet, &SourceRectangle, &DestinationRectangle);
		}
	}

	real64 TimeToUpdate;
	bool32 CurrentAnimPlayOnce;
	string CurrentAnim;

	inline void AddAnimation(uint32 Frames, uint32 X, uint32 Y, string Name, uint32 Width, uint32 Height, Vector2 Offset)
	{
		std::vector<SDL_Rect> Rectangles;

		for (uint32 Index = 0; Index < Frames; ++Index)
		{
			SDL_Rect Rect = { (int32)(Index + X) * (int32)Width, (int32)Y, (int32)Width, (int32)Height };
			Rectangles.push_back(Rect);
		}

		Animations.insert(std::pair<string, std::vector<SDL_Rect> >(Name, Rectangles));
		Offsets.insert(std::pair<string, Vector2>(Name, Offset));
	}

	inline virtual void AnimationDone(string CurrentAnimation)
	{

	}

	inline void ResetAnimations()
	{
		Animations.clear();
		Offsets.clear();
	}

	inline void StopAnimation()
	{
		FrameIndex = 0;
		AnimationDone(CurrentAnim);
	}

	inline void SetVisible(bool32 Visible)
	{
		IsVisible = Visible;
	}

	inline virtual void Update(Vector2 GameObjectPosition, uint32 ElapsedTime) override
	{
		Sprite::Update(GameObjectPosition, ElapsedTime);

		TimeElapsed += ElapsedTime;

		if (TimeElapsed > TimeToUpdate)
		{
			TimeElapsed -= TimeToUpdate;

			if (FrameIndex < Animations[CurrentAnim].size() - 1)
			{
				FrameIndex++;
			}
			else
			{
				if (CurrentAnimPlayOnce == 1)
				{
					SetVisible(0);
				}
				
				StopAnimation();
			}
		}
	}

	std::map<string, std::vector<SDL_Rect> > Animations;
	std::map<string, Vector2> Offsets;

	uint32 FrameIndex;
	real64 TimeElapsed;
	bool32 IsVisible;

};

class Tile
{
public:

	SDL_Texture* TileSet;
	Vector2 TileSetPosition;

	Vector2 Size;
	Vector2 Position;

	inline Tile()
	{

	}

	inline Tile(SDL_Texture* TileSetTexture, Vector2 TileSize, Vector2 TileSetTexturePosition, Vector2 TilePosition)
	{
		TileSet = TileSetTexture;
		Size = TileSize;
		TileSetPosition = TileSetTexturePosition;
		Position = TilePosition;
	}

	inline void Update(uint32 ElapsedTime)
	{

	}

	inline void Draw(Graphics& RenderGraphics)
	{
		SDL_Rect DestinationRect = { (int32)Position.X, (int32)Position.Y, (int32)Size.X, (int32)Size.Y };
		SDL_Rect SourceRect = { (int32)TileSetPosition.X, (int32)TileSetPosition.Y, (int32)Size.X, (int32)Size.Y };

		RenderGraphics.BlitSurface(TileSet, &SourceRect, &DestinationRect);
	}
};

#define KANTI2D_SPRITE
#endif
