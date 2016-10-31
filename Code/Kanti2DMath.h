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

global_variable real32 const DEGREES_CONSTANT = Pi32 / 180.0f;

inline real32
ToRadians(real32 Degrees)
{
	real32 Result = Degrees * DEGREES_CONSTANT;

	return (Result);
}

global_variable real32 const RADIANS_CONSTANT = 180.0f / Pi32;

inline real32
ToDegrees(real32 Radians)
{
	real32 Result = Radians * RADIANS_CONSTANT;

	return (Result);
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

k_internal bool32
IsFloatEqual(real32 A, real32 B, real32 Threshold = 1.0f / 8192.0f)
{	
	return fabsf(A - B) < Threshold; 
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
	const k_internal Vector2 One;
	const k_internal Vector2 Up;
	const k_internal Vector2 Down;
	const k_internal Vector2 Left;
	const k_internal Vector2 Right;


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

	inline Vector2 operator*(const Vector2& A) const
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

	inline const Vector2& operator-() const
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

	inline bool32 operator==(const Vector2& A) const
	{
		bool32 Result = X == A.X && Y == A.Y;

		return (Result);
	}

	inline bool32 operator!=(Vector2 A)
	{
		bool32 Result = !(*this == A);

		return (Result);
	}

	inline bool32 operator!=(const Vector2& A) const
	{
		bool32 Result = !(*this == A);

		return (Result);
	}

	inline bool32 operator <(Vector2 A)
	{
		bool32 Result = this->X < A.X || (this->X == A.X && this->Y < A.Y);

		return (Result);
	}

	inline bool32 operator >(Vector2 A)
	{
		bool32 Result = this->X > A.X || (this->X == A.X && this->Y > A.Y);
		
		return (Result);
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
		// Vector2 Result = A / Length(A);
		Vector2 Result = A;

		real32 Length = Result.X * Result.X + Result.Y * Result.Y;
		if (Length > 0.0f)
		{
			Length = SquareRoot(Length);
			real32 InverseLength = 1.0 / Length;
			Result.X *= InverseLength;
			Result.Y *= InverseLength;
		}

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

	k_internal inline real32 Distance(Vector2 A, Vector2 B)
	{
		real32 Result = SquareRoot(Square(A.X - B.X) + Square(A.Y - B.Y));

		return (Result);
	}

	inline real32 Distance(Vector2 A)
	{
		real32 Result = Distance(*this, A);

		return (Result);
	}

	inline real32 Distance(const Vector2& A) const
	{
		real32 Result = Distance(*this, A);

		return (Result);
	}

	inline k_internal Vector2 Min(Vector2 A, Vector2 B)
	{
		return A.Length() < B.Length() ? A : B;
	}

	inline k_internal Vector2 Max(Vector2 A, Vector2 B)
	{
		return A.Length() > B.Length() ? A : B;
	}

	inline k_internal real32 DotProduct(Vector2 A, Vector2 B)
	{
		real32 Result = A.X * B.X + A.Y * B.Y;

		return (Result);
	}

	inline k_internal Vector2 RotateVector(Vector2 A, real32 Degrees)
	{
		real32 Radians = ToRadians(Degrees);
		real32 Sine = Sin(Radians);
		real32 Cosine = Cos(Radians);

		Vector2 Result;
		Result.X = A.X * Cosine - A.Y * Sine;
		Result.Y = A.X * Sine + A.Y * Cosine;

		return (Result);
	}

	inline k_internal Vector2 Rotate90Degrees(Vector2 A)
	{
		Vector2 Result;

		Result.X = -A.Y;
		Result.Y = A.X;

		return (Result);
	}

	inline k_internal Vector2 UnitVector(Vector2 A) 
	{ 
		real32 Length = A.Length();
		Vector2 Result = A;

		if (0.0f < Length)
		{
			return Result / Length;
		}

		return (Result);
	}

	inline k_internal bool32 IsVectorParallel(Vector2 A, Vector2 B)
	{
		Vector2 na = Rotate90Degrees(A);

		return IsFloatEqual(0.0f, DotProduct(na, B));
	}

	inline k_internal real32 Angle(Vector2 A, Vector2 B)
	{
		real32 DeltaY = B.Y - A.Y;
		real32	DeltaX = B.X - A.X;

		real32 AngleInDegrees = ATan2(DeltaY, DeltaX) * RADIANS_CONSTANT;

		return AngleInDegrees;

		return ATan2(B.Y, B.X) - ATan2(A.Y, A.X);

		real32 Dot = DotProduct(A, B);						// # dot product
		real32 Determinant = A.X * B.Y - A.Y * B.X;		   // # determinant
		real32 Result = ATan2(Determinant, Dot);		   // # atan2(y, x) or atan2(sin, cos)

		return (Result);
	}

	inline k_internal real32 EnclosedAngle(Vector2 A, Vector2 B)
	{
		 Vector2 UnitA = Vector2::UnitVector(A);
		 Vector2 UnitB = Vector2::UnitVector(B);

		 real32 DotProduct = Vector2::DotProduct(UnitA, UnitB);

		 real32 Result = ToDegrees(ACos(DotProduct));

		 return (Result);
	}
};

const Vector2 Vector2::Zero = Vector2(0.0f, 0.0f);
const Vector2 Vector2::One = Vector2(1.0f, 1.0f);
const Vector2 Vector2::Up = Vector2(0.0f, -1.0f);
const Vector2 Vector2::Down = Vector2(0.0f, 1.0f);
const Vector2 Vector2::Left = Vector2(-1.0f, 0.0f);
const Vector2 Vector2::Right = Vector2(1.0f, 0.0f);

// Unique ID



struct UniqueID
{
	uint64  Data1;
	uint32	Data2;
	uint32	Data3;
	uint8	Data4[8];

	inline bool32 operator ==(const UniqueID& Comparer)
	{
		return  Data1 == Comparer.Data1 &&
			Data2 == Comparer.Data2 &&
			Data3 == Comparer.Data3 &&
			Data4 == Comparer.Data4;
	}

	inline bool32 operator ==(const UniqueID& Comparer) const
	{
		return  Data1 == Comparer.Data1 &&
			Data2 == Comparer.Data2 &&
			Data3 == Comparer.Data3 &&
			Data4 == Comparer.Data4;
	}

	inline bool32 operator ==(UniqueID& Comparer)
	{
		return  Data1 == Comparer.Data1 &&
			Data2 == Comparer.Data2 &&
			Data3 == Comparer.Data3 &&
			Data4 == Comparer.Data4;
	}

	inline bool operator <(const UniqueID& Comparer) const
	{
		
		bool Result = memcmp(this, &Comparer, sizeof(Comparer)) < 0;

		return (Result);
	}

	/*
	inline bool32 operator <(const UniqueID& Comparer) const
	{
		return  Data1 < Comparer.Data1 &&
			Data2 < Comparer.Data2 &&
			Data3 < Comparer.Data3 &&
			Data4 < Comparer.Data4;
	}
	*/


	inline bool32 operator <(UniqueID& Comparer)
	{
		bool Result = memcmp(this, &Comparer, sizeof(Comparer)) < 0;

		return (Result);
	}

	inline string ToString()
	{
		return std::to_string(Data1) + "-" + std::to_string(Data2) + "-" + std::to_string(Data3);
	}
};

#define PLATFORM_GET_UUID(name) UniqueID name()
typedef PLATFORM_GET_UUID(platform_get_uuid);

global_variable platform_get_uuid* GetUUID;

#define KANTI2D_MATH

#endif

