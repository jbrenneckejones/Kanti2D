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

class Shape
{
public:

	// Variables
	Vector2 Offset;

public:

	// Functions

	virtual bool32 CollidesWith(Shape* Comparer)
	{
		return FALSE;
	}

	virtual bool32 DoesOverlap(Shape* Comparer)
	{
		return FALSE;
	}

	virtual real32 GetSide(SIDE Side)
	{
		return 0;
	}

	virtual Vector2 GetCombinedOffset()
	{
		return Offset;
	}
};

class KRectangle : public Shape
{
public:

	union
	{
		struct
		{
			real32 X, Y, Width, Height;
		};

		struct
		{
			Vector2 Position;
			Vector2 Size;
		};
	};

	inline KRectangle()
	{
	}

	inline KRectangle(real32 RectX, real32 RectY, real32 RectWidth, real32 RectHeight)
	{
		X = (RectX);
		Y = (RectY);
		Width = (RectWidth);
		Height = (RectHeight);
	}

	inline KRectangle(uint32 RectX, uint32 RectY, uint32 RectWidth, uint32 RectHeight)
	{
		X = (real32)RectX;
		Y = (real32)RectY;
		Width = (real32)RectWidth;
		Height = (real32)RectHeight;
	}

	inline KRectangle(int32 RectX, int32 RectY, int32 RectWidth, int32 RectHeight)
	{
		X = (real32)RectX;
		Y = (real32)RectY;
		Width = (real32)RectWidth;
		Height = (real32)RectHeight;
	}

	inline KRectangle(Vector2 RectPosition, Vector2 RectSize)
	{
		Position = RectPosition;
		Size = RectSize;
	}

	inline KRectangle(SDL_Rect SDLRectangle)
	{
		SetRectangle(SDLRectangle);
	}

	inline bool32 operator == (const KRectangle& Comparer)
	{
		bool32 Result = Position == Comparer.Position && Size == Comparer.Size;

		return (Result);
	}

	bool32 operator <(const KRectangle& Comparer)
	{
		return Position < Comparer.Position && Position < Comparer.Size;
	}

	inline void SetRectangle(SDL_Rect Rectangle)
	{
		X = (real32)Rectangle.x;
		Y = (real32)Rectangle.y;
		Width = (real32)Rectangle.w;
		Height = (real32)Rectangle.h;
	}

	bool32 IsValidRect()
	{
		bool32 Result = X >= 0 && Y >= 0 && Width >= 0 && Height >= 0;

		return (Result);
	}

	SDL_Rect ToSDLRect()
	{
		SDL_Rect Result = { (int32)X, (int32)Y, (int32)Width, (int32)Height };

		return (Result);
	}

	inline Vector2 GetCenter()
	{
		Vector2 Result = { X + (Width / 2), Y + (Height / 2) };

		return (Result);
	}

	inline real32 GetLeft()
	{
		return X;
	}

	inline real32 GetRight()
	{
		return X + Width;
	}

	inline real32 GetTop()
	{
		return Y;
	}

	inline real32 GetBottom()
	{
		return Y + Height;
	}

	inline Vector2 GetSize()
	{
		return (Size);
	}

	inline Vector2 GetPosition()
	{
		return (Position);
	}

	virtual real32 GetSide(SIDE Side) override
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

	virtual bool32 CollidesWith(Shape* Comparer) override
	{
		KRectangle* Rect = (KRectangle *)Comparer;

		if (!Rect)
		{
			return FALSE;
		}

		bool32 Result =
			GetRight() >= Rect->GetLeft() &&
			GetLeft() <= Rect->GetRight() &&
			GetTop() <= Rect->GetBottom() &&
			GetBottom() >= Rect->GetTop();

		return (Result);
	}

	virtual bool32 DoesOverlap(Shape* Comparer) override
	{
		KRectangle* Rect = (KRectangle *)Comparer;

		if (!Rect)
		{
			return FALSE;
		}

		return GetLeft() < Rect->GetRight() &&
			GetRight() > Rect->GetLeft() &&
			GetTop() < Rect->GetBottom() &&
			GetBottom() > Rect->GetTop();
	}

	inline virtual Vector2 GetCombinedOffset() override
	{
		Vector2 Result = { Position + Offset };

		return (Result);
	}
};

class Slope : public Shape
{
public:

	// Variables

	Vector2 PointOne;
	Vector2 PointTwo;
	real32 SlopeAmount;

public:

	// Functions

	inline Slope()
	{
	}

	inline Slope(Vector2 SlopePointOne, Vector2 SlopePointTwo)
	{
		PointOne = SlopePointOne;
		PointTwo = SlopePointTwo;

		if (PointTwo.X - PointOne.X != 0)
		{
			SlopeAmount = CalculateSlope(PointOne, PointTwo);
		}
	}

	k_internal inline real32 CalculateSlope(Vector2 SlopePointOne, Vector2 SlopePointTwo)
	{
		real32 Result = (fabs(SlopePointTwo.Y) - fabs(SlopePointOne.Y))
			/ (fabs(SlopePointTwo.X) - fabs(SlopePointOne.X));

		return (Result);
	}

	virtual bool32 CollidesWith(Shape* Comparer) override
	{
		KRectangle* Rect = (KRectangle *)Comparer;

		if (!Rect)
		{
			return FALSE;
		}

		return (
			(
				Rect->GetRight() >= PointTwo.X &&
				Rect->GetLeft() <= PointOne.X &&
				Rect->GetTop() <= PointTwo.Y &&
				Rect->GetBottom() >= PointOne.Y
			)
			|| 
			(
				Rect->GetRight() >= PointOne.X &&
				Rect->GetLeft() <= PointTwo.X &&
				Rect->GetTop() <= PointOne.Y &&
				Rect->GetBottom() >= PointTwo.Y
			)
			||
			(
				Rect->GetLeft() <= PointOne.X &&
				Rect->GetRight() >= PointTwo.X &&
				Rect->GetTop() <= PointOne.Y &&
				Rect->GetBottom() >= PointTwo.Y
			)
			||
			(
				Rect->GetLeft() <= PointTwo.X &&
				Rect->GetRight() >= PointOne.X &&
				Rect->GetTop() <= PointTwo.Y &&
				Rect->GetBottom() >= PointOne.Y
			)
			);
	}
};

#define KANTI2D_RECTANGLE
#endif
