#include "Kanti2DRectangle.h"

bool32 Segment::IsColliding(Segment A, Segment B)
{
	Ray axisA, axisB;
	axisA.Origin = A.PointA;

	axisA.Direction = A.PointB - A.PointA;

	if (Ray::IsOnOneSide(axisA, B))
		return FALSE;

	axisB.Origin = B.PointA;
	axisB.Direction = B.PointB - B.PointA;

	if (Ray::IsOnOneSide(axisB, A))
		return FALSE;

	if (Vector2::IsVectorParallel(axisA.Direction, axisB.Direction))
	{
		Range rangeA = Range::ProjectSegment(A, axisA.Direction);
		Range rangeB = Range::ProjectSegment(B, axisA.Direction);

		return Range::IsOverlapping(rangeA, rangeB);
	}

	return TRUE;

}

k_internal OutCode ComputeOutCode(real32 X, real32 Y, BoundingBox Box)

{
	OutCode Code;

	Code = INSIDE;          // initialised as being inside of [[clip window]]

	if (X < Box.GetMin().X)           // to the left of clip window
		Code |= LEFT;
	else if (X > Box.GetMax().X)      // to the right of clip window
		Code |= RIGHT;
	if (Y < Box.GetMin().Y)           // below the clip window
		Code |= BOTTOM;
	else if (Y > Box.GetMax().Y)      // above the clip window
		Code |= TOP;

	return Code;
}

k_internal BoxIntersects CohenSutherlandLineClipAndDraw(real32 x0, real32 y0, real32 x1, real32 y1, BoundingBox Box)

{
	// compute outcodes for P0, P1, and whatever point lies outside the clip rectangle
	OutCode outcode0 = ComputeOutCode(x0, y0, Box);
	OutCode outcode1 = ComputeOutCode(x1, y1, Box);
	bool accept = false;

	BoxIntersects Result;

	while (true) {
		if (!(outcode0 | outcode1)) { // Bitwise OR is 0. Trivially accept and get out of loop
			accept = true;
			break;
		}
		else if (outcode0 & outcode1) { // Bitwise AND is not 0. Trivially reject and get out of loop
			break;
		}
		else {
			// failed both tests, so calculate the line segment to clip
			// from an outside point to an intersection with clip edge
			double x, y;

			// At least one endpoint is outside the clip rectangle; pick it.
			OutCode outcodeOut = outcode0 ? outcode0 : outcode1;

			// Now find the intersection point;
			// use formulas y = y0 + slope * (x - x0), x = x0 + (1 / slope) * (y - y0)
			if (outcodeOut & TOP) {           // point is above the clip rectangle
				x = x0 + (x1 - x0) * (Box.GetMax().Y - y0) / (y1 - y0);
				y = Box.GetMax().Y;
			}
			else if (outcodeOut & BOTTOM) { // point is below the clip rectangle
				x = x0 + (x1 - x0) * (Box.GetMin().Y - y0) / (y1 - y0);
				y = Box.GetMin().Y;
			}
			else if (outcodeOut & RIGHT) {  // point is to the right of clip rectangle
				y = y0 + (y1 - y0) * (Box.GetMax().X - x0) / (x1 - x0);
				x = Box.GetMax().X;
			}
			else if (outcodeOut & LEFT) {   // point is to the left of clip rectangle
				y = y0 + (y1 - y0) * (Box.GetMin().X - x0) / (x1 - x0);
				x = Box.GetMin().X;
			}

			// Now we move outside point to intersection point to clip
			// and get ready for next pass.
			if (outcodeOut == outcode0) {
				x0 = x;
				y0 = y;
				outcode0 = ComputeOutCode(x0, y0, Box);
			}
			else {
				x1 = x;
				y1 = y;
				outcode1 = ComputeOutCode(x1, y1, Box);
			}
		}
	}
	if (accept) {
		// Following functions are left for implementation by user based on
		// their platform (OpenGL/graphics.h etc.)
		// DrawRectangle(xmin, ymin, xmax, ymax);
		// LineSegment(x0, y0, x1, y1);
		Result.A = outcode0;
		Result.B = outcode1;
	}

	return (Result);
}
