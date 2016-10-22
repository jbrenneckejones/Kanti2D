#ifndef KANTI2D_GAMEEVENTSYSTEM

class Point
{
public:

	// Variables

	int32 X;
	int32 Y;

public:

	// Functions

	inline Point()
	{

	}

	inline Point(int32 ValueX, int32 ValueY)
	{
		X = ValueX;
		Y = ValueY;
	}

	inline bool operator == (const Point& Comparer)
	{
		return X == Comparer.X && Y == Comparer.Y;
	}

	inline bool operator > (const Point& Comparer)
	{
		return X > Comparer.X && Y > Comparer.Y;
	}

	inline bool operator < (const Point& Comparer)
	{
		return X < Comparer.X && Y < Comparer.Y;
	}

	inline bool operator < (const Point& Comparer) const
	{
		return X < Comparer.X && Y < Comparer.Y;
	}

	inline string ToString()
	{
		string Result = "Point: " + std::to_string(X) + " , " + std::to_string(Y);
		
		return (Result);
	}
};

k_internal const Point CellSize = { 500, 500 };

struct CollisionPair
{
	class Collider* Caller;
	struct Collision* InfoCaller;

	class Collider* Collider;
	struct Collision* InfoCollider;

	CollisionPair(class Collider& A, class Collider& B);

	CollisionPair(class Collider* A, class Collider* B);

	CollisionPair(struct Collision& A, struct Collision& B);

	CollisionPair(struct Collision* A, struct Collision* B);

	void CallCollisionStart();

	void CallCollisionStay();

	void CallCollisionEnd();

	bool operator == (const CollisionPair& Other)
	{
		return Caller == Other.Caller &&
			Collider == Other.Collider;
	}
};

class CollisionCell
{
public:
	
	string ID;
	KRectangle Bounds;
	std::thread* UpdateThread = nullptr;
	std::vector<class Collider*> Colliders;
	std::vector<CollisionPair> CollisionPairs;

	inline CollisionCell()
	{

	}

	inline CollisionCell(string CellID, KRectangle CellBounds)
	{
		ID = CellID;
		Bounds = CellBounds;
	}

	inline bool operator ==(const CollisionCell& Comparer)
	{
		return ID == Comparer.ID &&
			Bounds == Comparer.Bounds &&
			UpdateThread == Comparer.UpdateThread &&
			Colliders == Comparer.Colliders; // && CollisionPairs == Comparer.CollisionPairs;
	}

	void Update();
};

volatile class GameEventSystem
{
public:

	// Variables

	k_internal GameEventSystem* EventSystem;

	std::map<string, class GameEntity*> GameEntities;

	std::vector<class Behaviour*> UpdateCalls;
	std::vector<class Behaviour*> FixedUpdateCalls;
	std::vector<class Renderer*> DrawCalls;

	// TODO(Julian): THink about combining these
	std::vector<class PhysicsComponent*> PhysicsCalls;
	std::vector<class Collider*> CollisionCalls;

	std::map<string, class CollisionCell> CollisionCells;

	
	// k_internal std::vector<std::function<void(Collider2D*)>> CollisionCalls;


public:

	// Functions

	void AddUpdateHandler(class Behaviour* Callback)
	{
		if (!Callback)
		{
			return;
		}

		bool32 Found = false;
		if (UpdateCalls.size() > 0)
		{
			Found = std::find(UpdateCalls.cbegin(), UpdateCalls.cend(), Callback) != UpdateCalls.end();
		}
		
		if (!Found)
		{
			UpdateCalls.push_back(Callback);
		}
	}

	void AddFixedUpdateHandler(class Behaviour* Callback)
	{
		if (!Callback)
		{
			return;
		}

		bool32 Found = false;
		if (FixedUpdateCalls.size() > 0)
		{
			Found = std::find(FixedUpdateCalls.cbegin(), FixedUpdateCalls.cend(), Callback) != FixedUpdateCalls.end();
		}

		if (!Found)
		{
			FixedUpdateCalls.push_back(Callback);
		}
	}

	void AddDrawHandler(class Renderer* Callback)
	{		
		if (!Callback)
		{
			return;
		}

		bool32 Found = false;
		if (DrawCalls.size() > 0)
		{
			Found = std::find(DrawCalls.cbegin(), DrawCalls.cend(), Callback) != DrawCalls.end();
		}

		if (!Found)
		{
			DrawCalls.push_back(Callback);
		}
	}

	void AddPhysicsHandler(class PhysicsComponent* Callback)
	{
		if (!Callback)
		{
			return;
		}

		bool32 Found = false;
		if (PhysicsCalls.size() > 0)
		{
			Found = std::find(PhysicsCalls.cbegin(), PhysicsCalls.cend(), Callback) != PhysicsCalls.end();
		}

		if (!Found)
		{
			PhysicsCalls.push_back(Callback);
		}
	}

	void AddCollisionHandler(class Collider* Callback)
	{
		if (!Callback)
		{
			return;
		}

		bool32 Found = false;
		if (CollisionCalls.size() > 0)
		{
			Found = std::find(CollisionCalls.cbegin(), CollisionCalls.cend(), Callback) != CollisionCalls.end();
		}

		if (!Found)
		{
			CollisionCalls.push_back(Callback);
		}
	}

	void CreateCollisionCells(KRectangle WindowSize)
	{
		uint32 CellCount = 0;
		for (int32 IndexWidth = 0; IndexWidth < WindowSize.Width; IndexWidth += CellSize.X)
		{
			for (int32 IndexHeight = 0; IndexHeight < WindowSize.Height; IndexHeight += CellSize.Y)
			{
				Point CellPoint = { IndexWidth, IndexHeight };

				KRectangle Bounds = KRectangle(IndexWidth, IndexHeight, CellSize.X, CellSize.Y);
				CollisionCells[CellPoint.ToString()] = CollisionCell(CellPoint.ToString(), Bounds);
				//std::pair<Point, CollisionCell*> Pair = { CellPoint, new CollisionCell(Cell) };
				//CollisionCells.insert(Pair);
			}
		}
	}
};

GameEventSystem* GameEventSystem::EventSystem = new GameEventSystem();

#define KANTI2D_GAMEEVENTSYSTEM
#endif
