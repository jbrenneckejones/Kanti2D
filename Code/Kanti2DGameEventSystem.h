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

k_internal const Point CellSize = { 640, 480 };

struct CollisionPair
{
	struct Collision InfoCaller;

	CollisionPair(class Collider* A, class Collider* B);

	void Update();

	void CallCollisionStart();

	void CallCollisionStay();

	void CallCollisionEnd();

	bool operator == (const CollisionPair& Other) const
	{
		return InfoCaller.Caller == Other.InfoCaller.Caller &&
			InfoCaller.Collider == Other.InfoCaller.Collider;
	}
};

volatile class CollisionCell
{
public:
	
	UniqueID ID;
	BoundingBox Bounds;
	std::vector<class Collider*> Colliders;
	std::vector<CollisionPair> CollisionPairs;
	std::vector<CollisionPair> RemovedPairs;

	inline CollisionCell()
	{
		ID = GetUUID();
	}

	inline CollisionCell(BoundingBox CellBounds)
	{
		ID = GetUUID();
		Bounds = CellBounds;
	}

	inline bool32 operator ==(const CollisionCell& Comparer)
	{
		return ID == Comparer.ID; /* &&
			Bounds == Comparer.Bounds &&
			UpdateThread == Comparer.UpdateThread &&
			Colliders == Comparer.Colliders; // && CollisionPairs == Comparer.CollisionPairs;
			*/
	}

	inline bool32 operator ==(const CollisionCell& Comparer) const
	{
		return ID == Comparer.ID &&
			// Bounds == Comparer.Bounds &&
			// UpdateThread == Comparer.UpdateThread &&
			Colliders == Comparer.Colliders; // && CollisionPairs == Comparer.CollisionPairs;
	}

	void Update();

	void UpdateRemovals()
	{
		if (RemovedPairs.size() == 0)
		{
			return;
		}

		for (auto Pair : RemovedPairs)
		{
			auto FoundPair = std::find(CollisionPairs.begin(), CollisionPairs.end(), Pair);

			if (FoundPair != CollisionPairs.end())
			{
				CollisionPairs.erase(FoundPair);
			}
		}
	}
};

volatile class GameEventSystem
{
public:

	// Variables

	k_internal GameEventSystem* EventSystem;

	std::map<string, class GameEntity*> GameEntities;

	std::vector<class GameBehaviour*> UpdateCalls;
	std::vector<class GameBehaviour*> FixedUpdateCalls;
	std::vector<class Renderer*> DrawCalls;

	// TODO(Julian): THink about combining these
	std::vector<class PhysicsComponent*> PhysicsCalls;
	std::vector<class Collider*> CollisionCalls;

	std::map<struct UniqueID, class CollisionCell> CollisionCells;

	
	// k_internal std::vector<std::function<void(Collider2D*)>> CollisionCalls;


public:

	// Functions

	void AddUpdateHandler(class GameBehaviour* Callback)
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

	void AddFixedUpdateHandler(class GameBehaviour* Callback)
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

	void CreateCollisionCells(Vector2 WindowSize)
	{
		for (int32 IndexWidth = 0; IndexWidth < WindowSize.X; IndexWidth += CellSize.X)
		{
			for (int32 IndexHeight = 0; IndexHeight < WindowSize.Y; IndexHeight += CellSize.Y)
			{
				Vector2 Position = { IndexWidth, IndexHeight };
				Vector2 Size = { CellSize.X, CellSize.Y };
				BoundingBox Bounds = BoundingBox::GetPositionSize(Position, Size);
				CollisionCell Cell(Bounds);
				UniqueID ID = GetUUID();
				CollisionCells[ID] = CollisionCell(Bounds);
				CollisionCells[ID].ID = ID;
				//std::pair<Point, CollisionCell*> Pair = { CellPoint, new CollisionCell(Cell) };
				//CollisionCells.insert(Pair);
			}
		}
	}
};

GameEventSystem* GameEventSystem::EventSystem = new GameEventSystem();

#define KANTI2D_GAMEEVENTSYSTEM
#endif
