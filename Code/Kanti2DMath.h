#ifndef KANTI2D_MATH

// Intrinsics

#include "math.h"

inline real64
Epsilon()
{
	local_persist real64 MachineEpsilon = 1;

	while (1.0 + (MachineEpsilon / 2) > 1.0)
	{
		MachineEpsilon = MachineEpsilon / 2;
	}

	return MachineEpsilon;
}

inline int32
SignOf(int32 Value)
{
	int32 Result = (Value >= 0) ? 1 : -1;
	return(Result);
}

inline real32
SignOf(real32 Value)
{
	real32 Result = (Value >= 0) ? 1.0f : -1.0f;
	return(Result);
}

inline real32
SquareRoot(real32 Real32)
{
	real32 Result = sqrtf(Real32);
	return(Result);
}

inline real32
AbsoluteValue(real32 Real32)
{
	real32 Result = (real32)fabs(Real32);
	return(Result);
}

inline uint32
RotateLeft(uint32 Value, int32 Amount)
{
#if COMPILER_MSVC
	uint32 Result = _rotl(Value, Amount);
#else
	// TODO(casey): Actually port this to other compiler platforms!
	Amount &= 31;
	uint32 Result = ((Value << Amount) | (Value >> (32 - Amount)));
#endif

	return(Result);
}

inline uint32
RotateRight(uint32 Value, int32 Amount)
{
#if COMPILER_MSVC
	uint32 Result = _rotr(Value, Amount);
#else
	// TODO(casey): Actually port this to other compiler platforms!
	Amount &= 31;
	uint32 Result = ((Value >> Amount) | (Value << (32 - Amount)));
#endif

	return(Result);
}

inline int32
RoundReal32ToInt32(real32 Real32)
{
	int32 Result = (int32)roundf(Real32);
	return(Result);
}

inline uint32
RoundReal32ToUInt32(real32 Real32)
{
	uint32 Result = (uint32)roundf(Real32);
	return(Result);
}

inline int32
FloorReal32ToInt32(real32 Real32)
{
	int32 Result = (int32)floorf(Real32);
	return(Result);
}

inline int32
CeilReal32ToInt32(real32 Real32)
{
	int32 Result = (int32)ceilf(Real32);
	return(Result);
}

inline int32
TruncateReal32ToInt32(real32 Real32)
{
	int32 Result = (int32)Real32;
	return(Result);
}

inline real32
Sin(real32 Angle)
{
	real32 Result = sinf(Angle);
	return (Result);
}

inline real32
ASin(real32 Angle)
{
	real32 Result = (real32)asin(Angle);
	return (Result);
}

inline real32
Cos(real32 Angle)
{
	real32 Result = cosf(Angle);
	return (Result);
}

inline real32
ACos(real32 Angle)
{
	real32 Result = (real32)acos(Angle);
	return (Result);
}

inline real32
Tan(real32 Angle)
{
	real32 Result = (real32)tan(Angle);
	return (Result);
}

inline real32
ATan2(real32 Y, real32 X)
{
	real32 Result = (real32)atan2f(Y, X);
	return (Result);
}

inline real32
Log(real32 Angle)
{
	real32 Result = (real32)log(Angle);
	return (Result);
}

inline real32
Exp(real32 Value)
{
	real32 Result = (real32)exp(Value);
	return (Result);
}

inline real32
Power(real32 X, real32 Y)
{
	real32 Result = (real32)pow(X, Y);
	return (Result);
}

inline int32
Power(int32 X, int32 Y)
{
	int32 Result = (int32)pow(X, Y);
	return (Result);
}

inline real32
Max(real32 X, real32 Y)
{
	real32 Result = (X > Y) ? X : Y;

	return (Result);
}

inline real32
ToRadians(real32 Degrees)
{
	return (Degrees * 0.01745329251994329576923690768489f);
}

// Math

inline real32
Square(real32 A)
{
	real32 Result = A * A;

	return(Result);
}

inline real32
Sin01(real32 Time)
{
	real32 Result = Sin(Pi32 * Time);

	return(Result);
}

inline real32
Triangle01(real32 Time)
{
	real32 Result = 2.0f * Time;
	if (Result > 1.0f)
	{
		Result = 2.0f - Result;
	}

	return(Result);
}

inline real32
Lerp(real32 A, real32 Time, real32 B)
{
	real32 Result = (1.0f - Time) * A + Time * B;

	return(Result);
}

inline real32
Mix(real32 A, real32 Time, real32 B)
{
	real32 Result = (A)+Time * (B - A);

	return(Result);
}

inline real32
Clamp(real32 Min, real32 Value, real32 Max)
{
	real32 Result = Value;

	if (Result < Min)
	{
		Result = Min;
	}
	else if (Result > Max)
	{
		Result = Max;
	}

	return(Result);
}

inline real32
Clamp01(real32 Value)
{
	real32 Result = Clamp(0.0f, Value, 1.0f);

	return(Result);
}

inline real32
Clamp01MapToRange(real32 Min, real32 Time, real32 Max)
{
	real32 Result = 0.0f;

	real32 Range = Max - Min;
	if (Range != 0.0f)
	{
		Result = Clamp01((Time - Min) / Range);
	}

	return(Result);
}

inline real32
ClampAboveZero(real32 Value)
{
	real32 Result = (Value < 0) ? 0.0f : Value;
	return(Result);
}

inline real32
ClampAnyAboveZero(real32 Value)
{
	real32 Sign = SignOf(Value);
	real32 Result = (AbsoluteValue(Value) < 0) ? 0.0f : AbsoluteValue(Value);
	Result = Result * Sign;

	return(Result);
}

inline real32
SafeRatioN(real32 Numerator, real32 Divisor, real32 N)
{
	real32 Result = N;

	if (Divisor != 0.0f)
	{
		Result = Numerator / Divisor;
	}

	return(Result);
}

inline real32
SafeRatio0(real32 Numerator, real32 Divisor)
{
	real32 Result = SafeRatioN(Numerator, Divisor, 0.0f);

	return(Result);
}

inline real32
SafeRatio1(real32 Numerator, real32 Divisor)
{
	real32 Result = SafeRatioN(Numerator, Divisor, 1.0f);

	return(Result);
}

k_internal uint32 
Min(uint32 X, uint32 Y)
{
	uint32 Result = (X < Y) ? X : Y;

	return (Result);
}

// Vector 2

class Vector2
{
public:

	union
	{
		struct
		{
			real32 X, Y;
		};

		struct
		{
			real32 U, V;
		};

		real32 E[2];
	};

	const k_internal Vector2 Zero;

	inline Vector2()
	{
		this->X = 0;
		this->Y = 0;
	};

	inline Vector2(real32 Value)
	{
		this->X = Value;
		this->Y = Value;
	};

	inline Vector2(int32 X, int32 Y)
	{
		this->X = (real32)X;
		this->Y = (real32)Y;
	}

	inline Vector2(uint32 X, uint32 Y)
	{
		this->X = (real32)X;
		this->Y = (real32)Y;
	}

	inline Vector2(real32 X, real32 Y)
	{
		this->X = X;
		this->Y = Y;
	}

	inline Vector2 operator*(real32 A)
	{
		Vector2 Result = { this->X * A, this->Y * A };

		return (Result);
	}

	inline Vector2 operator*(Vector2 A)
	{
		Vector2 Result = { this->X * A.X, this->Y * A.Y };

		return (Result);
	}

	inline friend Vector2 operator*(real32 A, Vector2 B)
	{
		Vector2 Result = { A * B.X, A * B.Y };

		return(Result);
	}

	inline Vector2 operator*=(real32 A)
	{
		this->X *= A;
		this->Y *= A;
	}

	inline Vector2 operator-()
	{
		Vector2 Result = { -this->X, -this->Y };

		return(Result);
	}

	inline Vector2 operator+(Vector2 A)
	{
		Vector2 Result = {};

		Result = { this->X + A.X, this->Y + A.Y };

		return(Result);
	}

	inline void operator+=(Vector2 A)
	{
		this->X += A.X;
		this->Y += A.Y;
	}

	inline Vector2 operator-(Vector2 A)
	{
		Vector2 Result = { this->X - A.X, this->Y - A.Y };

		return(Result);
	}

	inline void operator-=(Vector2 A)
	{
		this->X -= A.X;
		this->Y -= A.Y;
	}

	inline bool32 operator==(Vector2 A)
	{
		bool32 Result = X == A.X && Y == A.Y;

		return (Result);
	}

	inline bool32 operator <(Vector2 A)
	{
		return X < A.X && Y < A.Y;
	}

	inline Vector2 operator/(real32 A)
	{
		Vector2 Result = { this->X / A, this->Y / A };

		return (Result);
	}

	inline Vector2 Perp()
	{
		Vector2 Result = { -this->Y, this->X };

		return (Result);
	}

	inline Vector2 Hadamard(Vector2 A)
	{
		Vector2 Result = { this->X * A.X, this->Y * A.Y };

		return(Result);
	}

	inline k_internal real32 Inner(Vector2 A, Vector2 B)
	{
		real32 Result = A.X * B.X + A.Y * B.Y;

		return(Result);
	}

	inline real32 Inner(Vector2 A)
	{
		real32 Result = Inner(*this, A);

		return(Result);
	}

	inline k_internal real32 LengthSq(Vector2 A)
	{
		real32 Result = Inner(A, A);

		return(Result);
	}

	inline real32 LengthSq()
	{
		real32 Result = LengthSq(*this);

		return(Result);
	}

	inline k_internal real32 Length(Vector2 A)
	{

		real32 Result = SquareRoot(LengthSq(A));

		return(Result);

	}

	inline real32 Length()
	{
		real32 Result = Length(*this);

		return(Result);
	}

	inline Vector2 Normalize()
	{
		Vector2 Result = Normalize(*this);

		return(Result);
	}

	k_internal inline Vector2 Normalize(Vector2 A)
	{
		Vector2 Result = A / Length(A);

		return(Result);
	}

	inline Vector2 Clamped(real32 Min = 0.0f, real32 Max = 1.0f)
	{
		Vector2 Result = { Clamp(Min, this->X, Max), Clamp(Min, this->Y, Max) };

		return(Result);
	}

	k_internal inline Vector2 Arm2(real32 Angle)
	{
		Vector2 Result = { Cos(Angle), Sin(Angle) };

		return(Result);
	}
};

const Vector2 Vector2::Zero = Vector2(0.0f, 0.0f);

#define KANTI2D_MATH

#endif

