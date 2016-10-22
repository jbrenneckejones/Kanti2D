#ifndef KANTI2D_GAMEOBJECT

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
};

#define PLATFORM_GET_UUID(name) UniqueID name()
typedef PLATFORM_GET_UUID(platform_get_uuid);

global_variable platform_get_uuid* GetUUID;

struct Collision
{
	class Collider* Hit;
	real32 OverlapAmount;
	SIDE SideOverlapped;

	Collision()
	{

	}

	Collision(class Collider* Caller, class Collider* HitCollider);
};

class Entity
{
public:

	// Variables

	string Name;
	UniqueID InstanceID;

	uint32 Layer;

public:

	// Functions

	Entity()
	{
		Name = "Object";
		InstanceID = GetUUID();
		Layer = 0;
	}

	Entity(string EntityName)
	{
		InstanceID = GetUUID();
		Name = EntityName;
		Layer = 0;
	}

	virtual ~Entity() {};

	bool32 operator ==(const Entity& Comparer) 
	{
		return InstanceID == Comparer.InstanceID;
	}

	bool32 operator ==(Entity& Comparer)
	{
		return InstanceID == Comparer.InstanceID;
	}

	bool32 operator ==(Entity Comparer)
	{
		return InstanceID == Comparer.InstanceID;
	}

	bool32 operator ==(Entity* Comparer)
	{
		if (!Comparer)
		{
			return false;
		}

		return InstanceID == Comparer->InstanceID;
	}
};

class GameComponent : public Entity
{
public:

	// Variables

	class GameEntity* EntityAttachedTo;

public:

	// Functions

	inline GameComponent()
	{

	};

	virtual ~GameComponent() {};

	inline GameComponent(string ComponentName, GameEntity* AttachedTo)
	{
		Name = ComponentName;
		EntityAttachedTo = AttachedTo;
	}
};

class Transform : public GameComponent
{
public:

	// Variables

	Vector2 Position;
	Vector2 Scale;
	Vector2 Rotation;

	GameEntity* Parent;

public:

	// Functions

	inline Transform()
	{
		Position = Vector2::Zero;
		Scale = Vector2::Zero;
		Rotation = Vector2::Zero;
	}
};

class GameEntity : public Entity
{
public:

	// Variables

	bool32 IsActive;

	std::vector<class GameComponent*> Components;

	Transform EntTransform;

	const GameEntity* GameObject = this;

public:

	// Functions

	inline GameEntity()
	{

	}

	inline GameEntity(string ObjectName) : Entity(ObjectName)
	{
		Name = ObjectName;
	}

	virtual ~GameEntity() {};

	template<class GameComponent>
	GameComponent* AddComponent();

	template<class GameComponent>
	GameComponent* AddComponent(GameComponent* ComponentPrefab);

	template<class GameComponent>
	GameComponent* GetComponent();
};

class Behaviour : public GameComponent
{
public:

	// Variables

	bool32 IsActive;

public:

	// Functions

	inline Behaviour()
	{
		IsActive = true;
	}

	virtual ~Behaviour() {};

	virtual void Awake() = 0;
	virtual void Start() = 0;
	virtual void FixedUpdate() = 0;
	virtual void Update() = 0;
};

class GameBehaviour : public Behaviour
{
public:

	// Variables

public:

	// Functions

	inline GameBehaviour()
	{
		GameEventSystem::EventSystem->AddUpdateHandler(this);
		GameEventSystem::EventSystem->AddFixedUpdateHandler(this);
	}

	virtual ~GameBehaviour() {};

	inline virtual void OnCollisionStart(const Collision& CollideInfo)
	{
	}

	inline virtual void OnCollisionStay(const Collision& CollideInfo)
	{
	}

	inline virtual void OnCollisionEnd(const Collision& CollideInfo)
	{
	}

	// Inherited via Behaviour
	inline virtual void Awake() override
	{
	}

	inline virtual void Start() override
	{
	}

	inline virtual void FixedUpdate() override
	{
	}

	inline virtual void Update() override
	{
	}
};

class Renderer : public GameComponent
{
public:

	// Variables

public:

	// Functions

	inline Renderer()
	{
		GameEventSystem::EventSystem->AddDrawHandler(this);
	}

	virtual void Update() = 0;

	virtual void Draw() = 0;

	virtual uint32 GetLayer()
	{
		return EntityAttachedTo->Layer;
	}
};

class Sprite : public Entity
{
public:

	// Variables

	KRectangle SourceRect;
	SDL_Texture* SpriteSheet;

	KRectangle BoundingBox;

public:

	// Functions

	inline Sprite()
	{
	}

	inline Sprite(const string &FileName,
		KRectangle RenderRectangle)
	{
		SetupSprite(FileName, RenderRectangle);
	}

	inline void SetupSprite(const string &FileName,
		KRectangle RenderRectangle)
	{
		SourceRect = RenderRectangle;

		SDL_Surface* Surface = RenderManager::CurrentRenderer->LoadImage(FileName);

		SDL_SetColorKey(Surface, 1, SDL_MapRGB(Surface->format, 0, 0, 0));

		SpriteSheet = SDL_CreateTextureFromSurface(RenderManager::CurrentRenderer->SDLRenderer, Surface);

		if (!SpriteSheet)
		{
			printf("\nError: Unable to load image\n");
		}

		BoundingBox = SourceRect;
	}
};

class Collider : public GameComponent
{
public:

	// Variables

	string CellIndex = "NULL";
	Shape* Bounds;
	Vector2 Offset;

public:

	// Functions

	inline Collider()
	{
		GameEventSystem::EventSystem->AddCollisionHandler(this);
	}

	inline virtual bool32 IsColliding(Collider* Collide)
	{
		return FALSE;
	}

	inline virtual void OnCollisionStart(const Collision& CollideInfo)
	{
		for (const auto& Iterator : EntityAttachedTo->Components)
		{
			GameBehaviour* Collision = dynamic_cast<GameBehaviour*>(Iterator);

			if (Collision)
			{
				Collision->OnCollisionStart(CollideInfo);
			}
		}
	}

	inline virtual void OnCollisionStay(const Collision& CollideInfo)
	{
		for (const auto& Iterator : EntityAttachedTo->Components)
		{
			GameBehaviour* Collision = dynamic_cast<GameBehaviour*>(Iterator);

			if (Collision)
			{
				Collision->OnCollisionStay(CollideInfo);
			}
		}
	}

	inline virtual void OnCollisionEnd(const Collision& CollideInfo)
	{
		for (const auto& Iterator : EntityAttachedTo->Components)
		{
			GameBehaviour* Collision = dynamic_cast<GameBehaviour*>(Iterator);

			if (Collision)
			{
				Collision->OnCollisionEnd(CollideInfo);
			}
		}
	}

	inline virtual Collision GetCollisionInfo(Collider* Collide)
	{
		return Collision();
	}

	inline virtual Shape* GetBoundsCombined()
	{
		Shape* Result = new Shape();

		Result->Offset = Bounds->Offset + EntityAttachedTo->EntTransform.Position;

		return (Result);
	}
};

class ColliderRect : public Collider
{
public:

	// Variables

public:

	// Functions

	inline ColliderRect()
	{
		
	}

	inline KRectangle* GetRect()
	{
		return (KRectangle *)Bounds;
	}

	inline Shape* GetBoundsCombined()
	{
		KRectangle* Result = new KRectangle();

		Result->Position = GetRect()->GetCombinedOffset();
		Result->Size = GetRect()->Size;

		return (Result);
	}

	inline Collision GetCollisionInfo(Collider* Collide)
	{
		Collision Result = {};

		if (!Collide)
		{
			return Result;
		}

		ColliderRect* Rect = (ColliderRect *)Collide;

		if (!Rect)
		{
			return (Result);
		}

		real32 AmountTop, AmountBottom, AmountRight, AmountLeft;

		KRectangle* CominedRect = (KRectangle *)GetBoundsCombined();
		KRectangle* CombinedRectCollide = (KRectangle *)Rect->GetBoundsCombined();

		AmountTop = CombinedRectCollide->GetBottom() - CominedRect->GetTop();
		AmountBottom = CominedRect->GetBottom() - CombinedRectCollide->GetTop();
		AmountLeft = CombinedRectCollide->GetRight() - CominedRect->GetLeft();
		AmountRight = CominedRect->GetRight() - CombinedRectCollide->GetLeft();

		real32 Values[4] = { AbsoluteValue(AmountTop), AbsoluteValue(AmountBottom),
			AbsoluteValue(AmountLeft) , AbsoluteValue(AmountRight) };

		real32 Lowest = Values[0];
		uint32 LowIndex = 0;
		for (uint32 Index = 0; Index < 4; ++Index)
		{
			if (Values[Index] < Lowest)
			{
				Lowest = Values[Index];
				LowIndex = Index;
				Result.OverlapAmount = Values[Index];
			}
		}

		

		Result.Hit = Collide;
		Result.OverlapAmount = Lowest;
		Result.SideOverlapped = (SIDE)LowIndex;

		return (Result);
	}

	inline virtual bool32 IsColliding(Collider* Collide) override
	{
		if (!Collide)
		{
			return false;
		}

		return GetBoundsCombined()->CollidesWith(Collide->Bounds);
	}
};

class ColliderSlope : public Collider
{

};

class SpriteRenderer : public Renderer
{
public:

	// Variables

	Sprite SpriteToRender;

public:

	// Functions

	inline SpriteRenderer()
	{
	}

	inline virtual void Update() override
	{
		// BoundingBox = Rectangle(GameObjectPosition.X, GameObjectPosition.Y, (real32)SourceRect.w, (real32)SourceRect.h);
	}

	inline virtual void Draw() override
	{
		KRectangle DestinationRectangle = { EntityAttachedTo->EntTransform.Position, SpriteToRender.SourceRect.GetSize() };
		// RenderGraphics.BlitSurface(SpriteSheet, &SourceRect, &DestinationRectangle);

		BlitSprite(SpriteToRender.SourceRect.ToSDLRect(), DestinationRectangle.ToSDLRect());
	}

	inline void BlitSprite(SDL_Rect Source, SDL_Rect Destination)
	{
		RenderManager::CurrentRenderer->BlitSurface(SpriteToRender.SpriteSheet, &Source, &Destination);
	}
};

// TODO(Julian): Maybe do a override call instead of inheritence
class SpriteAnimator : public SpriteRenderer
{
public:

	// Variables

	// SpriteRenderer* Renderer;

	std::map<string, std::vector<SDL_Rect> > Animations;
	std::map<string, Vector2> Offsets;

	uint32 FrameIndex;
	real64 TimeElapsed;
	bool32 IsVisible;

	real64 TimeToUpdate;
	bool32 CurrentAnimPlayOnce;
	string CurrentAnim;

public:

	// Functions

	inline SpriteAnimator()
	{
		TimeToUpdate = 100.0;
		FrameIndex = 0;
		TimeElapsed = 0.0;
		IsVisible = 1;
		CurrentAnimPlayOnce = 0;
		CurrentAnim = "";

		// Renderer = new SpriteRenderer();

		// GameEventSystem::EventSystem->AddUpdateHandler(this);
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

	inline virtual void Draw() override
	{
		if (IsVisible == 1)
		{
			KRectangle DestinationRectangle;
			DestinationRectangle.Position = EntityAttachedTo->EntTransform.Position + Offsets[CurrentAnim];
			DestinationRectangle.Size = SpriteToRender.SourceRect.Size;

			KRectangle SourceRectangle(Animations[CurrentAnim][FrameIndex]);

			SpriteToRender.SourceRect = SourceRectangle;

			BlitSprite(SourceRectangle.ToSDLRect(), DestinationRectangle.ToSDLRect());
		}
	}

	inline void AddAnimation(uint32 Frames, uint32 X, uint32 Y, string AnimationName, uint32 Width, uint32 Height, Vector2 Offset)
	{
		std::vector<SDL_Rect> Rectangles;

		for (uint32 Index = 0; Index < Frames; ++Index)
		{
			SDL_Rect Rect = { (int32)(Index + X) * (int32)Width, (int32)Y, (int32)Width, (int32)Height };
			Rectangles.push_back(Rect);
		}

		Animations.insert(std::pair<string, std::vector<SDL_Rect> >(AnimationName, Rectangles));
		Offsets.insert(std::pair<string, Vector2>(AnimationName, Offset));
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

	inline virtual void Update() override
	{
		TimeElapsed += Time::DeltaTime;

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
};

class PhysicsComponent : public GameBehaviour
{
public:

	// Variables

	// Collider2D* Collider;

public:

	// Functions

	inline PhysicsComponent()
	{
		GameEventSystem::EventSystem->AddPhysicsHandler(this);
	}
};

class RigidBody2D : public PhysicsComponent
{
public:

	// Variables

	real32 GravityRate = 0.002f;
	real32 GravityMax = 0.8f;
	real32 Friction = 0.1f;
	real32 Mass = 1.0f;

	Vector2 Velocity = Vector2(0, 0);
	Vector2 MaxVelocity = Vector2(10.0f, GravityMax);
	Vector2 Acceleration = Vector2(0, 0);

	Collider* Collider;


	bool32 IsGrounded = FALSE;

public:

	// Functions

	inline RigidBody2D()
	{
	}

	inline virtual void OnCollisionStart(const Collision& CollideInfo) override
	{
		KRectangle* CurrentCollision = (KRectangle *)CollideInfo.Hit->Bounds;

		SIDE CollisionSide = CollideInfo.SideOverlapped;

		Transform* EntTransform = &EntityAttachedTo->EntTransform;

		if (CollisionSide != SIDE::NONE)
		{
			switch (CollisionSide)
			{
			case SIDE::TOP:
				// EntTransform->Position.Y = (real32)CurrentCollision.GetBottom() + 1.0f;
				EntTransform->Position.Y -= CollideInfo.OverlapAmount + 1.0f;
				Velocity.Y = 0.0f;
				break;

			case SIDE::BOTTOM:
				// EntTransform->Position.Y = (real32)CurrentCollision.GetTop() - (real32)CurrentCollision.Height - 1.0f;
				EntTransform->Position.Y += CollideInfo.OverlapAmount - 1.0f;
				Velocity.Y = 0.0f;
				IsGrounded = TRUE;
				break;

			case SIDE::LEFT:
				// EntTransform->Position.X = (real32)CurrentCollision.GetRight() + 1.0f;
				EntTransform->Position.X += CollideInfo.OverlapAmount + 1.0f;
				Velocity.X = 0.0f;
				break;

			case SIDE::RIGHT:
				// EntTransform->Position.X = (real32)CurrentCollision.GetLeft() - (real32)CurrentCollision.Width - 1.0f;
				EntTransform->Position.X -= CollideInfo.OverlapAmount - 1.0f;
				Velocity.X = 0.0f;
				break;

			default:
				break;
			}
		}
	}

	inline virtual void OnCollisionEnd(const Collision& CollideInfo) override
	{
		KRectangle* CurrentCollision = (KRectangle *)CollideInfo.Hit->Bounds;

		SIDE CollisionSide = CollideInfo.SideOverlapped;

		Transform* EntTransform = &EntityAttachedTo->EntTransform;

		if (CollisionSide != SIDE::NONE)
		{
			switch (CollisionSide)
			{
			case SIDE::TOP:

				break;

			case SIDE::BOTTOM:
				IsGrounded = FALSE;
				break;

			case SIDE::LEFT:
				break;

			case SIDE::RIGHT:
				break;

			default:
				break;
			}
		}
	}

	inline void AddFriction()
	{
		if (!IsGrounded)
		{
			Velocity.Y += GravityRate;
		}

		if (AbsoluteValue(Velocity.X) < 0.1f)
		{
			Velocity.X = 0.0f;
		}
		else
		{
			if (Velocity.X > 0.0f)
			{
				Velocity.X -= Friction;
			}
			else if (Velocity.X < -0.0f)
			{
				Velocity.X += Friction;
			}
		}
		
		Velocity.X = SignOf(Velocity.X) * Clamp(0.0f, AbsoluteValue(Velocity.X), MaxVelocity.X);
		Velocity.Y = SignOf(Velocity.Y) * Clamp(0.0f, AbsoluteValue(Velocity.Y), MaxVelocity.Y);
	}

	inline void Update() override
	{
		AddFriction();

		EntityAttachedTo->EntTransform.Position += Velocity * (real32)Time::FixedUpdate;

		// Position.X += Velocity.X * (real32)Time::DeltaTime;

		// Position.Y += Velocity.Y * (real32)Time::DeltaTime;

		/*

		if (Velocity.X != 0.0f && Velocity.Y != 0.0f)
		{
		AddFriction();
		}

		Velocity += Acceleration * (real32)Time::DeltaTime;

		int32 Factorial = Velocity.X > 0 ? 1 : -1;
		Velocity.X = Min(AbsoluteValue(Velocity.X), MaxVelocity.X) * Factorial;

		Position += Velocity;
		// Velocity.X = 0;

		// Position += (real32)Time::DeltaTime * (Velocity * Acceleration * 0.5f);
		// Velocity += Acceleration - (Friction * Velocity);

		*/

		// ObjectSprite->Update(Position, ElapsedTime);
	}
};

enum class DIRECTION
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

class Player : public GameBehaviour
{
public:

	// Variables

	SpriteAnimator* Animator;
	RigidBody2D* RigidBody;
	Collider* Collider;

	DIRECTION CurrentDirection;

	real32 Speed = 5.0f;

public:

	// Functions

public:

	inline Player()
	{
	}

	inline Player(GameEntity* GameObject, Vector2 ObjectPosition)
	{
		// GameObject->Location->Position = ObjectPosition;
		CurrentDirection = DIRECTION::RIGHT;

		// GameEntity* PlayerObject = new GameEntity("Player");
		EntityAttachedTo = GameObject;

		// GameObject = new GameEntity();
		// GameObject.EntTransform = GameObject->AddComponent<Transform>();
		// GameObject->EntTransform = new Transform();
		EntityAttachedTo->EntTransform.Position = ObjectPosition;
		Animator = EntityAttachedTo->AddComponent<SpriteAnimator>();
		// Animator->GameEnt = GameObject;
		// Animator->EntityAttachedTo = &GameObject;
		Animator->SpriteToRender.SetupSprite("../Data/Images/MyChar.bmp", SDL_Rect{ 0, 0, 32, 32 });

		// "../Data/Images/MyChar.bmp", SDL_Rect{ 0, 0, 32, 32 }, ObjectPosition, 300
		// GameObject->Components.push_back(Animator);

		Animator->AddAnimation(1, 0, 0, "IdleLeft", 32, 32, Vector2(0, 0));
		Animator->AddAnimation(1, 0, 32, "IdleRight", 32, 32, Vector2(0, 0));

		Animator->AddAnimation(3, 0, 0, "RunLeft", 32, 32, Vector2(0, 0));
		Animator->AddAnimation(3, 0, 32, "RunRight", 32, 32, Vector2(0, 0));

		Animator->PlayAnimation("RunRight");

		Name = "Player";

		EntityAttachedTo->Name = Name;
		EntityAttachedTo->Layer = 1;
		Animator->Layer = 1;

		RigidBody = EntityAttachedTo->AddComponent<RigidBody2D>();
		Collider = EntityAttachedTo->AddComponent<ColliderRect>();
		RigidBody->Collider = Collider;
		Collider->Bounds = &Animator->SpriteToRender.SourceRect;
	}

	inline void Move(DIRECTION FacingDirection)
	{
		CurrentDirection = FacingDirection;

		switch (FacingDirection)
		{
		case DIRECTION::LEFT:
			// GameObject.EntTransform.Position.X = -Speed;
			RigidBody->Velocity.X = -Speed;
			Animator->PlayAnimation("RunLeft");
			break;
		case DIRECTION::RIGHT:
			// Velocity.X = Speed;
			RigidBody->Velocity.X = Speed;
			Animator->PlayAnimation("RunRight");
			break;

		default:
			break;
		}
	}

	inline void StopMove()
	{
		// AnimatedSprite* AnimSprite = (AnimatedSprite *)ObjectSprite;
		// RigidBody->Velocity.X = 0.0f;
		switch (CurrentDirection)
		{
		case DIRECTION::LEFT:
			Animator->PlayAnimation("IdleLeft");
			break;
		case DIRECTION::RIGHT:
			Animator->PlayAnimation("IdleRight");
			break;

		default:
			break;
		}
	}

	virtual void OnCollisionStart(const Collision& Collide) override
	{

	}

	inline virtual void Awake() override {};
	inline virtual void Start() override {};
	inline virtual void FixedUpdate() override {};
	inline virtual void Update() override {};
};

class Tile : public GameBehaviour
{
public:

	SpriteRenderer* EntRenderer;

	SDL_Texture* TileSet;
	Vector2 TileSetPosition;

	Vector2 Size;
	Vector2 Position;

	inline Tile()
	{

	}

	inline Tile(SDL_Texture* TileSetTexture, Vector2 TileSize, Vector2 TileSetTexturePosition, Vector2 TilePosition)
	{
		Name = "Tile";
		TileSet = TileSetTexture;
		Size = TileSize;
		TileSetPosition = TileSetTexturePosition;
		Position = TilePosition;

		EntityAttachedTo = new GameEntity("Tile");
		EntityAttachedTo->EntTransform.Position = TilePosition;
		
		EntRenderer = EntityAttachedTo->AddComponent<SpriteRenderer>();
		EntRenderer->SpriteToRender.SpriteSheet = TileSetTexture;
		EntRenderer->SpriteToRender.SourceRect = { TileSetPosition, Size };
	}

	inline void Update() override
	{

	}

	inline void Draw()
	{
		SDL_Rect DestinationRect = { (int32)Position.X, (int32)Position.Y, (int32)Size.X, (int32)Size.Y };
		SDL_Rect SourceRect = { (int32)TileSetPosition.X, (int32)TileSetPosition.Y, (int32)Size.X, (int32)Size.Y };

		RenderManager::CurrentRenderer->BlitSurface(TileSet, &SourceRect, &DestinationRect);
	}
};

class PhysicsTile : public Tile
{
public:

	// Variables

	ColliderRect* Collider;

public:

	inline PhysicsTile()
	{
		
	}

	inline PhysicsTile(SDL_Texture* TileSetTexture, Vector2 TileSize, Vector2 TileSetTexturePosition, Vector2 TilePosition) : 
		Tile(TileSetTexture, TileSize, TileSetTexturePosition, TilePosition)
	{
		Collider = EntityAttachedTo->AddComponent<ColliderRect>();
		//Collider->Bounds.X = (uint32)TilePosition.X;
		//Collider->Bounds.Y = (uint32)TilePosition.Y;
		Collider->Bounds = &EntRenderer->SpriteToRender.SourceRect;
		// Collider->Bounds.X = 0;
		// Collider->Bounds.Y = 0;
		// Collider->Bounds.Width = (uint32)TileSize.X;
		// Collider->Bounds.Height = (uint32)TileSize.Y;
		// Collider->Offset = { 0, 0 };
	}
};

struct Tileset
{
	SDL_Texture* Texture;
	int32 FirstGID;

	Tileset()
	{
		FirstGID = -1;
	}

	Tileset(SDL_Texture* TileSetTexture, int32 TilesetFirstGID)
	{
		Texture = TileSetTexture;
		FirstGID = TilesetFirstGID;
	}
};

class Level : public GameBehaviour
{
public:

	// Variables

	string MapName;
	Vector2 SpawnPoint;

	Vector2 LevelSize;
	Vector2 TileSize;

	SDL_Texture* BackgroundTexture;

	std::vector<Tile*> TileList;
	std::vector<Tileset> TileSets;

	std::vector<KRectangle> CollisionRectangles;

public:

	// Functions

	inline Level()
	{

	}

	inline Level(string LevelMapName, Vector2 LevelSpawnPoint)
	{
		MapName = LevelMapName;
		SpawnPoint = LevelSpawnPoint;
		LevelSize = Vector2(0, 0);

		LoadMap(MapName);
	}

	inline Level(GameEntity* GameObject, string FileName)
	{
		Name = "Level";
		EntityAttachedTo = GameObject;

		LoadMapFile(FileName);
	}

	inline void LoadMapFile(string LevelMapName)
	{
		SDL_RWops* File = SDL_RWFromFile(LevelMapName.c_str(), "rb");

		const int64 FileSize = (int64)File->size;
		char* Buffer = (char *)malloc(FileSize * sizeof(char));
		File->read(File, Buffer, FileSize * sizeof(char), 1);

		string FileContents = Buffer;

		uint32 Width = 0, Height = 0, TileWidth = 0, TileHeight = 0;

		nlohmann::json JsonFile = nlohmann::json::parse(FileContents.c_str());

		MapName = JsonFile["tilesets"][0]["name"].get<string>();

		Width = JsonFile["width"].get<uint32>();
		Height = JsonFile["height"].get<uint32>();

		TileWidth = JsonFile["tilewidth"].get<uint32>();
		TileHeight = JsonFile["tileheight"].get<uint32>();

		LevelSize = Vector2(Width, Height);

		string ImageUsed = JsonFile["tilesets"][0]["image"].get<string>();

		uint32 FirstGID = JsonFile["tilesets"][0]["firstgid"].get<uint32>();

		ImageUsed = ImageUsed.substr(ImageUsed.find_first_of('/'));
		ImageUsed = "../Data/" + ImageUsed;

		SDL_Texture* Texture = SDL_CreateTextureFromSurface(RenderManager::CurrentRenderer->SDLRenderer,
			RenderManager::CurrentRenderer->LoadImage(ImageUsed));

		TileSets.push_back(Tileset(Texture, FirstGID));

		nlohmann::json LayerList = nlohmann::json(JsonFile["layers"]);

		for (uint32 LayerIndex = 0; LayerIndex < LayerList.size(); ++LayerIndex)
		{
			if (LayerList[LayerIndex]["name"].get<string>() == "Collisions")
			{
				for (uint32 ObjectIndex = 0; ObjectIndex < LayerList[LayerIndex]["objects"].size(); ++ObjectIndex)
				{
					real32 NewTileX = LayerList[LayerIndex]["objects"][ObjectIndex]["x"].get<real32>();
					real32 NewTileY = LayerList[LayerIndex]["objects"][ObjectIndex]["y"].get<real32>();
					real32 NewTileWidth = LayerList[LayerIndex]["objects"][ObjectIndex]["width"].get<real32>();
					real32 NewTileHeight = LayerList[LayerIndex]["objects"][ObjectIndex]["height"].get<real32>();

					CollisionRectangles.push_back(KRectangle(NewTileX, NewTileY, NewTileWidth, NewTileHeight));
				}
			}
			else if (LayerList[LayerIndex]["name"].get<string>() == "Background" || 
				LayerList[LayerIndex]["name"].get<string>() == "Foreground")
			{
				

				std::vector<uint32> TileData = LayerList[LayerIndex]["data"].get<std::vector<uint32>>();

				for (uint32 TileIndex = 0; TileIndex < TileData.size(); ++TileIndex)
				{
					uint32 GID = TileData[TileIndex];

					if (GID == 0)
					{
						continue;
					}

					Tileset Set;

					for (uint32 TileSetIndex = 0; TileSetIndex < TileSets.size(); ++TileSetIndex)
					{
						if (TileSets[TileSetIndex].FirstGID <= (int32)GID)
						{
							Set = TileSets.at(TileSetIndex);
							break;
						}
					}

					uint32 XX = 0;
					uint32 YY = 0;

					XX = TileIndex % Width;
					XX *= TileWidth;
					YY += TileHeight * (TileIndex / Width);

					Vector2 FinalTilePosition = Vector2(XX, YY);

					uint32 TileSetWidth, TileSetHeight;
					SDL_QueryTexture(Set.Texture, NULL, NULL, (int *)&TileSetWidth, (int *)&TileSetHeight);
					uint32 TSXX = GID % (TileSetWidth / TileWidth) - 1;
					TSXX *= TileWidth;

					uint32 TSYY = 0;
					uint32 Amount = (GID / (TileSetWidth / TileWidth));

					TSYY = TileHeight * Amount;

					Vector2 FinalTileSetPosition = Vector2(TSXX, TSYY);

					Tile* BuildTile = new Tile(Set.Texture, Vector2(TileWidth, TileHeight), FinalTileSetPosition, FinalTilePosition);
					TileList.push_back(BuildTile);
				}
			}
			else if (LayerList[LayerIndex]["name"].get<string>() == "Slopes")
			{
				// std::vector<> Slopes;

				for (auto& Item : LayerList[LayerIndex]["objects"])
				{
					// Item.get<string>();
				}
			}
		}

		SDL_Texture* DebugTexture = SDL_CreateTextureFromSurface(RenderManager::CurrentRenderer->SDLRenderer,
			RenderManager::CurrentRenderer->LoadImage("../Data/Images/Debug/Collider.png"));

		for (uint32 CollisionIndex = 0; CollisionIndex < CollisionRectangles.size(); ++CollisionIndex)
		{
			KRectangle Rect = CollisionRectangles[CollisionIndex];
			// TODO(Julian): Find a way to get rid of the texture object
			PhysicsTile* PhysicalTile = new PhysicsTile(DebugTexture, Rect.GetSize(), Vector2::Zero, Rect.GetPosition());

			TileList.push_back(PhysicalTile);
		}
	}

	inline void LoadMap(string LevelMapName)
	{
		BackgroundTexture = SDL_CreateTextureFromSurface(RenderManager::CurrentRenderer->SDLRenderer,
			RenderManager::CurrentRenderer->LoadImage("../Data/Images/bkBlue.bmp"));
		LevelSize = Vector2(1280, 960);
	}

	inline virtual void Update() override
	{

	}

	inline void Draw()
	{
	}


};

#define KANTI2D_GAMEOBJECT

#endif
