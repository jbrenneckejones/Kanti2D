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

k_internal bool32 IsOverlappingMinMax(real32 MinA, real32 MaxA, real32 MinB, real32 MaxB)
{
	return MinB <= MaxA && MinA <= MaxB;
}

typedef int32 OutCode;
const OutCode NONE = -1;
const OutCode INSIDE = 0; // 0000
const OutCode LEFT = 1;   // 0001
const OutCode RIGHT = 2;  // 0010
const OutCode BOTTOM = 4; // 0100
const OutCode TOP = 8;    // 1000

struct BoxIntersects
{
	OutCode A = NONE;
	OutCode B = NONE;
};



						// Compute the bit code for a point (x, y) using the clip rectangle
						// bounded diagonally by (xmin, ymin), and (xmax, ymax)

						// ASSUME THAT xmax, xmin, ymax and ymin are global constants.

k_internal OutCode ComputeOutCode(real32 X, real32 Y, struct BoundingBox Box);

// Cohen�Sutherland clipping algorithm clips a line from
// P0 = (x0, y0) to P1 = (x1, y1) against a rectangle with 
// diagonal from (xmin, ymin) to (xmax, ymax).
k_internal BoxIntersects CohenSutherlandLineClipAndDraw(real32 x0, real32 y0, real32 x1, real32 y1, struct BoundingBox Box);

struct KRectangle
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

public:

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
};

struct Segment
{
	Vector2 PointA;
	Vector2 PointB;

	k_internal bool32 IsColliding(Segment A, Segment B);

	k_internal Vector2 GetIntersection(Segment A, Segment B)
	{
		Vector2 Result = Vector2::Zero;

		Vector2 b = A.PointB - A.PointA;
		Vector2 d = B.PointB - B.PointA;
		real32 bDotDPerp = b.X * d.Y - b.Y * d.X;

		Vector2 c = B.PointA - A.PointA;
		real32 t = (c.X * d.Y - c.Y * d.X) / bDotDPerp;
		if (t < 0 || t > 1)
			return Result;

		float u = (c.X * b.Y - c.Y * b.X) / bDotDPerp;
		if (u < 0 || u > 1)
			return Result;

		Result = A.PointA + t * b;

		return (Result);
	}

#define	DONT_INTERSECT    0
#define	DO_INTERSECT      1
#define COLLINEAR         2

	/**************************************************************
	*                                                            *
	*    NOTE:  The following macro to determine if two numbers  *
	*    have the same sign, is for 2's complement number        *
	*    representation.  It will need to be modified for other  *
	*    number systems.                                         *
	*                                                            *
	**************************************************************/

#define SAME_SIGNS( a, b )	\
		(((long) ((unsigned long) a ^ (unsigned long) b)) >= 0 )

	k_internal int32 lines_intersect(real32 x1, real32 y1,   /* First line segment */
		real32 x2, real32 y2,

		real32 x3, real32 y3,   /* Second line segment */
		real32 x4, real32 y4,

		real32* x,
		real32* y         /* Output value:
				  * point of intersection */
	)
	{
		long a1, a2, b1, b2, c1, c2; /* Coefficients of line eqns. */
		long r1, r2, r3, r4;         /* 'Sign' values */
		long denom, offset, num;     /* Intermediate values */

									 /* Compute a1, b1, c1, where line joining points 1 and 2
									 * is "a1 x  +  b1 y  +  c1  =  0".
									 */

		a1 = y2 - y1;
		b1 = x1 - x2;
		c1 = x2 * y1 - x1 * y2;

		/* Compute r3 and r4.
		*/


		r3 = a1 * x3 + b1 * y3 + c1;
		r4 = a1 * x4 + b1 * y4 + c1;

		/* Check signs of r3 and r4.  If both point 3 and point 4 lie on
		* same side of line 1, the line segments do not intersect.
		*/

		if (r3 != 0 &&
			r4 != 0 &&
			SAME_SIGNS(r3, r4))
			return (DONT_INTERSECT);

		/* Compute a2, b2, c2 */

		a2 = y4 - y3;
		b2 = x3 - x4;
		c2 = x4 * y3 - x3 * y4;

		/* Compute r1 and r2 */

		r1 = a2 * x1 + b2 * y1 + c2;
		r2 = a2 * x2 + b2 * y2 + c2;

		/* Check signs of r1 and r2.  If both point 1 and point 2 lie
		* on same side of second line segment, the line segments do
		* not intersect.
		*/

		if (r1 != 0 &&
			r2 != 0 &&
			SAME_SIGNS(r1, r2))
			return (DONT_INTERSECT);

		/* Line segments intersect: compute intersection point.
		*/

		denom = a1 * b2 - a2 * b1;
		if (denom == 0)
			return (COLLINEAR);
		offset = denom < 0 ? -denom / 2 : denom / 2;

		/* The denom/2 is to get rounding instead of truncating.  It
		* is added or subtracted to the numerator, depending upon the
		* sign of the numerator.
		*/

		num = b1 * c2 - b2 * c1;
		*x = (num < 0 ? num - offset : num + offset) / denom;

		num = a2 * c1 - a1 * c2;
		*y = (num < 0 ? num - offset : num + offset) / denom;

		return (DO_INTERSECT);
	} /* lines_intersect */

	k_internal int32 lines_intersect(Segment A, Segment B, Vector2& Location)
	{
		return lines_intersect(A.PointA.X, A.PointA.Y, A.PointB.X, A.PointB.Y,
			B.PointA.X, B.PointA.Y, B.PointB.X, B.PointB.Y, &Location.X, &Location.Y);
	}
};

struct Range
{
	real32 Minimum;
	real32 Maximum;

	k_internal Range Sort(Range A)
	{
		Range Result = A;
		if (Result.Minimum > Result.Maximum)
		{
			Result.Minimum = A.Maximum;
			Result.Maximum = A.Minimum;
		}

		return (Result);
	}

	k_internal Range ProjectSegment(Segment s, Vector2 onto)
	{
		Vector2 ontoUnit = Vector2::UnitVector(onto);
		Range r;
		r.Minimum = Vector2::DotProduct(ontoUnit, s.PointA);
		r.Maximum = Vector2::DotProduct(ontoUnit, s.PointB);
		r = Sort(r);

		return r;
	}

	k_internal bool32 IsOverlapping(Range A, Range B)
	{
		return IsOverlappingMinMax(A.Minimum, A.Maximum, B.Minimum, B.Maximum);
	}
};

struct Ray
{
	// The starting point of the ray in world space.
	Vector2 Origin;

	// The direction of the ray in world space.
	Vector2 Direction;

	Vector2 GetPoint(real32 Distance)
	{
		return Origin + Direction * Distance;
	}

	k_internal bool32 IsOnOneSide(Ray axis, Segment s) 
	{ 
		Vector2 d1 = s.PointA - axis.Origin;
		Vector2 d2 = s.PointB - axis.Origin;

		Vector2 n = Vector2::Rotate90Degrees(axis.Direction);

		real32 Product = Vector2::DotProduct(n, d1) * Vector2::DotProduct(n, d2);

		return Product > 0.0f;
	}

	k_internal bool32 RayEquivalent(Ray A, Ray B)
	{ 
		if (!Vector2::IsVectorParallel(A.Direction, B.Direction))
		{
			return FALSE;
		}

		Vector2 Difference = A.Origin - B.Origin;

		return Vector2::IsVectorParallel(Difference, A.Direction); 
	}

	k_internal bool32 RayCollides(Ray A, Ray B) 
	{ 
		if (Vector2::IsVectorParallel(A.Direction, B.Direction))
		{
			return RayEquivalent(A, B);
		}

		return TRUE; 
	}

};

struct BoundingBox
{
	// Variables

	// The center of the bounding box.
	Vector2 Center;

	// The extents of the box.This is always half of the size.
	// Note: If Bounds.extents has a negative value for any axis, Bounds.Contains always returns False.
	Vector2 Extents;

	real32 Rotation = 0.0f;

	// The total size of the box.This is always twice as large as the extents.
	// size.x is the width, size.y is the height and size.z is the depth of the box.
	Vector2 GetSize()
	{
		return Extents * 2.0f;
	}

	// Left hand top corner position
	// The minimal point of the box.
	// This is always equal to center - extents.
	Vector2 GetMin()
	{
		return Center - Extents;
	}

	// The maximal point of the box.
	// This is always equal to center + extents.
	Vector2 GetMax()
	{
		return Center + Extents;
	}

	// Functions

	inline BoundingBox()
	{
		Center = Vector2(16, 16);

		BoundingBox(Center, Vector2(32, 32));
	}

	/*
		Creates a new Bounds.

		Create a new Bounds with the given left hand position and total size.
		Bound extents will be half the given size.
	*/
	inline BoundingBox(Vector2 BoxCenter, Vector2 BoxSize)
	{
		Center = BoxCenter;
		Extents = BoxSize * 0.5f;
	}

	inline BoundingBox(Vector2 BoxCenter, Vector2 BoxSize, real32 BoxRotation)
	{
		Center = BoxCenter;
		Extents = BoxSize * 0.5f;

		Rotation = BoxRotation;
	}

	// Converts SDL Rect to our rect
	inline BoundingBox(SDL_Rect SDLRectangle)
	{
		BoundingBox({ SDLRectangle.x, SDLRectangle.y }, { SDLRectangle.w, SDLRectangle.h });
	}

	/*
		Returns : Vector2 The point on the bounding box or inside the bounding box.

		Description : The closest point on the bounding box.
		If the point is inside the bounding box, unmodified point position will be returned.
	*/
	inline Vector2 ClosestPoint(Vector2 Point)
	{
		if (Contains(Point))
		{
			return Point;
		}
		else
		{
			real32 boxWidth = GetSize().X;
			real32 boxHeight = GetSize().Y;

			Vector2 pointProjectedOnBox = Point - GetMin();
			real32 u = pointProjectedOnBox.X / boxWidth;
			real32 v = pointProjectedOnBox.Y / boxHeight;

			u = Clamp(u, 0, 1);
			v = Clamp(v, 0, 1);

			real32 uTemp = (u < 0.5f) ? 0 : 1;
			real32 vTemp = (v < 0.5f) ? 0 : 1;

			Vector2 p1 = GetMin() + Vector2(uTemp * boxWidth, v * boxHeight);
			Vector2 p2 = GetMin() + Vector2(u * boxWidth, vTemp * boxHeight);

			real32 d1 = Vector2::Length(Point - p1);
			real32 d2 = Vector2::Length(Point - p2);

			if (d1 < d2)
				Point = p1;
			else
				Point = p2;

			return Point;

			// Old

			real32 DeltaX = Point.X - Center.X;
			real32 PixelX = Extents.X - AbsoluteValue(DeltaX);
			if (PixelX <= 0)
			{
				return Vector2::Zero;
			}

			real32 DeltaY = Point.Y - Center.Y;
			real32 PixelY = Extents.Y - AbsoluteValue(DeltaY);
			if (PixelY <= 0)
			{
				return Vector2::Zero;
			}

			if (PixelX < PixelY)
			{
				real32 SurfaceX = SignOf(DeltaX);
				// hit.delta.x = px * sx
				// hit.normal.x = sx
				Point.X = Center.X + (Extents.X * SurfaceX);
				// Point.Y = Point.Y;
			}
			else
			{
				real32 SurfaceY = SignOf(DeltaY);
				// hit.delta.y = py * sy
				// hit.normal.y = sy
				// Point.X = Point.X;
				Point.Y = Point.Y + (Extents.Y * SurfaceY);
			}

			return Point;
		}
	}

	/*
		Description : Is point contained in the bounding box?
		If the point passed into Contains is inside the bounding box a value of True is returned.

		Note: If Bounds.extents contains a negative value in any coordinate then Bounds.Contains will always return False.
	*/
	inline bool32 Contains(Vector2 Point)
	{
		if (
			Point.X > GetMin().X && Point.X < GetMax().X &&
			Point.Y > GetMin().Y && Point.Y < GetMax().Y
			)
		{
			return TRUE;
		}

		return FALSE;
	}

	/*
		Description : Grows the Bounds to include the point.
	*/
	inline void Encapsulate(Vector2 Point)
	{
		SetMinMax(Vector2::Min(GetMin(), Point), Vector2::Max(GetMax(), Point));
	}

	/*
	Description : Grows the Bounds to include the point.
	*/
	inline void Encapsulate(BoundingBox Box)
	{
		Encapsulate(Box.Center - Box.Extents);
		Encapsulate(Box.Center + Box.Extents);
	}

	/*
		Description : Expand the bounds by increasing its size by amount along each side.
	*/
	inline void Expand(real32 ExpansionAmount)
	{
		ExpansionAmount *= 0.5f;
		Extents += Vector2(ExpansionAmount, ExpansionAmount);
	}

	inline void Expand(Vector2 ExpansionAmount)
	{
		Extents += ExpansionAmount * 0.5f;
	}

	/*
		Description : Does ray intersect this bounding box?
	*/
	inline bool32 IntersectsRay(Ray RayLine)
	{
	}

	/*
		Description : Does another bounding box intersect with this bounding box?
	*/
	inline bool32 Intersects(BoundingBox Box)
	{
		Vector2 Delta1, Delta2;
		Delta1 = Box.GetMin() - GetMax();
		Delta2 = GetMin() - Box.GetMax();

		if (Delta1.X > 0.0f || Delta1.Y > 0.0f)
			return false;

		if (Delta2.X > 0.0f || Delta2.Y > 0.0f)
			return false;

		return true;
	}

	/*
		Description : Sets the bounds to the min and max value of the box.
		Using this function is faster than assigning min and max separately.
	*/
	inline void SetMinMax(Vector2 BoxMin, Vector2 BoxMax)
	{
		Extents = (BoxMax - BoxMin) * 0.5f;
		Center = BoxMin + Extents;
	}

	inline void SetPositionSize(Vector2 BoxPosition, Vector2 BoxSize)
	{
		Center = BoxPosition + (BoxSize * 0.5f);
		Extents = BoxSize * 0.5f;
	}

	inline k_internal BoundingBox GetPositionSize(Vector2 BoxPosition, Vector2 BoxSize)
	{
		BoundingBox Result;
		Result.SetPositionSize(BoxPosition, BoxSize);

		return (Result);
	}

	/*
		Description : The smallest squared distance between the point and this bounding box.
	*/
	inline real32 SquareDistance(Vector2 Point)
	{
		real32 MinDistance = GetMin().Distance(Point);
		real32 MaxDistance = GetMax().Distance(Point);

		real32 Result = MinDistance < MaxDistance ? MinDistance : MaxDistance;

		return (Result);
	}
};

#define KANTI2D_RECTANGLE
#endif
