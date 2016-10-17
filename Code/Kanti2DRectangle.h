#ifndef KANTI2D_RECTANGLE

enum class SIDE
{
	TOP,
	BOTTOM,
	LEFT,
	RIGHT,
	NONE
};

k_internal inline SIDE GetOppositeSide(SIDE CurrentSide)
{
	switch (CurrentSide)
	{
	case SIDE::TOP:
		return SIDE::BOTTOM;
	case SIDE::BOTTOM:
		return SIDE::TOP;
	case SIDE::LEFT:
		return SIDE::RIGHT;
	case SIDE::RIGHT:
		return SIDE::LEFT;
	case SIDE::NONE:
		return SIDE::NONE;
	default:
		return SIDE::NONE;
	}
}

class Rectangle
{
public:

	uint32 X, Y, Width, Height;

	inline Rectangle()
	{
	}

	inline Rectangle(real32 RectX, real32 RectY, real32 RectWidth, real32 RectHeight)
	{
		X = CeilReal32ToInt32(RectX);
		Y = CeilReal32ToInt32(RectY);
		Width = CeilReal32ToInt32(RectWidth);
		Height = CeilReal32ToInt32(RectHeight);
	}

	inline Rectangle(uint32 RectX, uint32 RectY, uint32 RectWidth, uint32 RectHeight)
	{
		X = RectX;
		Y = RectY;
		Width = RectWidth;
		Height = RectHeight;
	}

	inline Vector2 GetCenter()
	{
		Vector2 Result = { X + (Width / 2), Y + (Height / 2) };

		return (Result);
	}

	inline uint32 GetLeft()
	{
		return X;
	}

	inline uint32 GetRight()
	{
		return X + Width;
	}

	inline uint32 GetTop()
	{
		return Y;
	}

	inline uint32 GetBottom()
	{
		return Y + Height;
	}

	const uint32 GetSide(SIDE Side)
	{
		switch (Side)
		{
		case SIDE::LEFT:
			return GetLeft();
		case SIDE::RIGHT:
			return GetRight();
		case SIDE::BOTTOM:
			return GetBottom();
		case SIDE::TOP:
			return GetTop();
		}

		return 0;
	}

	bool32 CollisionWith(Rectangle Other)
	{
		bool32 Result = GetRight() >= Other.GetLeft() 
			&& GetLeft() <= Other.GetRight() 
			&& GetTop() <= Other.GetBottom() 
			&& GetBottom() >= Other.GetTop();

		return (Result);
	}

	bool32 IsValidRect()
	{
		bool32 Result = X >= 0 && Y >= 0 && Width >= 0 && Height >= 0;

		return (Result);
	}
};

#define KANTI2D_RECTANGLE
#endif


