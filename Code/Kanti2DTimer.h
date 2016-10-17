#ifndef KANTI2D_TIME

struct State
{
	real32 Position; // Position
	real32 Velocity; // Velocity
};

struct Derivative
{
	float DerivativePosition;      // dx/dt = velocity
	float DerivativeVelocity;      // dv/dt = acceleration
};

struct FixedTimeStep
{
	real64 Time = 0.0;
	real64 DeltaTime = 0.01;

	real64 CurrentTime = 0.0;

	real64 Accumulator = 0.0;

	State Previous;
	State Current;

	State UpdateState;

	void Initialize()
	{
		CurrentTime = (real64)((real64)SDL_GetPerformanceCounter() / (real64)SDL_GetPerformanceFrequency());
	}

	real32 Acceleration(const State& CurrentState, real32 Time)
	{
		const real32 K = 10;
		const real32 B = 1;

		return -K * CurrentState.Position - B * CurrentState.Velocity;
	}

	Derivative Evaluate(const State& Initial, real32 Time, real32 DeltaTime, const Derivative& DerivativeValue)
	{
		State CurrentState;
		CurrentState.Position = Initial.Position + DerivativeValue.DerivativePosition * DeltaTime;
		CurrentState.Velocity = Initial.Velocity + DerivativeValue.DerivativeVelocity * DeltaTime;

		Derivative Result = {};
		Result.DerivativePosition = CurrentState.Velocity;
		Result.DerivativeVelocity = Acceleration(CurrentState, Time + DeltaTime);

		return (Result);
	}

	void Integrate(State& CurrentState, real32 Time, real32 DeltaTime)
	{
		Derivative A, B, C, D;

		A = Evaluate(CurrentState, Time, 0.0f, Derivative());
		B = Evaluate(CurrentState, Time, DeltaTime * 0.5f, A);
		C = Evaluate(CurrentState, Time, DeltaTime * 0.5f, B);
		D = Evaluate(CurrentState, Time, DeltaTime, C);

		real32 DerPosDeltaTime = 1.0f / 6.0f * (A.DerivativePosition + 2.0f * (B.DerivativePosition + C.DerivativePosition) + D.DerivativePosition);

		real32 DerVelDeltaTime = 1.0f / 6.0f * (A.DerivativeVelocity + 2.0f * (B.DerivativeVelocity + C.DerivativeVelocity) + D.DerivativeVelocity);

		CurrentState.Position = CurrentState.Position + DerPosDeltaTime * DeltaTime;
		CurrentState.Velocity = CurrentState.Velocity + DerVelDeltaTime * DeltaTime;
	}

	void Update()
	{
		real64 NewTime = (real64)( (real64)SDL_GetPerformanceCounter() / (real64)SDL_GetPerformanceFrequency());

		real64 FrameTime = NewTime - CurrentTime;

		if (FrameTime > 0.25)
		{
			FrameTime = 0.25;
		}

		CurrentTime = NewTime;

		Accumulator += FrameTime;

		while (Accumulator >= DeltaTime)
		{
			Previous = Current;

			Integrate(Current, (real32)Time, (real32)DeltaTime);
			Time += DeltaTime;
			Accumulator -= DeltaTime;
		}

		const real64 Alpha = Accumulator / DeltaTime;

		State Result = {};

		Result.Position = Lerp(Current.Position, (real32)Alpha, Previous.Position);

		Result.Velocity = Lerp(Current.Velocity, (real32)Alpha, Previous.Velocity);

		UpdateState = Result;
	}
};

class Time
{
public:

	k_internal uint64 TimeSinceStartup;

	k_internal int64 PerfomanceCountFrequency;
	k_internal int64 LastPerformanceCount;
	k_internal int64 EndPerformanceCount;

	k_internal int64 CounterElapsed;
	k_internal real64 DeltaTime;
	k_internal real64 CurrentFPS;

	k_internal FixedTimeStep TimeStep;

	k_internal const uint32 FPS = 50;
	k_internal const uint32 MAX_FRAME_TIME = 1000 / FPS;
	k_internal uint32 UpdateTime;
	k_internal uint32 LastUpdateTime;
	k_internal uint32 CurrentTimeMS;
	k_internal uint32 ElapsedTimeMS;

	k_internal void Initialize()
	{
		LastUpdateTime = SDL_GetTicks();

		//

		PerfomanceCountFrequency = SDL_GetPerformanceFrequency();

		LastPerformanceCount = SDL_GetPerformanceCounter();

		TimeStep.Initialize();
	}

	k_internal void Update()
	{
		CurrentTimeMS = SDL_GetTicks();
		ElapsedTimeMS = CurrentTimeMS - LastUpdateTime;
		UpdateTime = Min(ElapsedTimeMS, MAX_FRAME_TIME);
		LastUpdateTime = CurrentTimeMS;
		
		TimeSinceStartup++;

		EndPerformanceCount = SDL_GetPerformanceCounter();

		CounterElapsed = EndPerformanceCount - LastPerformanceCount;

		DeltaTime = (1000.0 * CounterElapsed / PerfomanceCountFrequency);
		CurrentFPS = (real64)(PerfomanceCountFrequency / CounterElapsed);

		LastPerformanceCount = EndPerformanceCount;

		TimeStep.DeltaTime = DeltaTime;
		TimeStep.Update();
	}
};

uint32 Time::UpdateTime = 0;
uint32 Time::LastUpdateTime = 0;
uint32 Time::CurrentTimeMS = 0;
uint32 Time::ElapsedTimeMS = 0;

real64 Time::DeltaTime = 0.0;

uint64 Time::TimeSinceStartup = 0;

int64 Time::PerfomanceCountFrequency = 0;
int64 Time::LastPerformanceCount = 0;
int64 Time::EndPerformanceCount = 0;

int64 Time::CounterElapsed = 0;
real64 Time::CurrentFPS = 0.0;

FixedTimeStep Time::TimeStep = {};

#define KANTI2D_TIME

#endif

