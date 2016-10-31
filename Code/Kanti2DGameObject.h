#ifndef KANTI2D_GAMEOBJECT

struct ContactPoint
{
	// Normal of the contact point.
	Vector2 Normal;

	// The point of contact.
	Vector2 Point;

	// The first collider in contact at the point.
	class Collider* ThisCollider;

	// The other collider in contact at the point.
	class Collider* OtherCollider;
};

struct Collision
{
	// The collider attached to the object receiving the collision message.
	class Collider* Caller;

	// Surface normal at the contact point.
	Vector2 Normal;

	// The incoming collider involved in the collision at this contact point.
	class Collider* Collider;

	// The specific points of contact with the incoming Collider2D.
	std::vector<ContactPoint> Contacts;

	// The relative linear velocity of the two colliding objects.
	Vector2 RelativeVelocity;

	inline Collision() {};

	Collision(class Collider* CalledOnCollider, class Collider* IncomingCollider);

	// Assumes that they're already confirmed colliding
	k_internal void GetCollisionInfo(class Collider* Caller, class Collider* Collided, struct Collision& Info);

	k_internal void BoundingBoxEdgeCollision(class BoxCollider* Box, class EdgeCollider* Edge, struct Collision& Info);

	k_internal void BoundingBoxCollision(BoxCollider* Box, BoxCollider* Edge, Collision& Info);
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
		Scale = Vector2::One;
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
};

class GameBehaviour : public Behaviour
{
public:

	// Variables

public:

	// Functions

	inline GameBehaviour()
	{
		AddHandlers();
	}

	inline void AddHandlers();

	virtual ~GameBehaviour() {};

	virtual void OnCollisionStart(const Collision& CollideInfo) {};
	virtual void OnCollisionStay(const Collision& CollideInfo) {};
	virtual void OnCollisionEnd(const Collision& CollideInfo) {};

	virtual void OnTriggerEnter(const Collider& Collided) {};
	virtual void OnTriggerStay(const Collider& Collided) {};
	virtual void OnTriggerExit(const Collider& Collided) {};

	virtual void Awake() {};
	virtual void Start() {};
	virtual void FixedUpdate() {};
	virtual void Update() {};
};

class Renderer : public GameComponent
{
public:

	// Variables

public:

	// Functions

	inline Renderer();

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

	KRectangle TextureRect;
	SDL_Texture* SpriteSheet;

	BoundingBox Bounds;

public:

	// Functions

	inline Sprite()
	{
		TextureRect = KRectangle(Vector2(0, 0), Vector2(32, 32));
		Bounds = BoundingBox(Vector2(16, 16), Vector2(32, 32));
	}

	inline Sprite(const string &FileName,
		struct KRectangle TextureRectangle)
	{
		SetupSprite(FileName, TextureRectangle);
	}

	inline void SetupSprite(const string &FileName, struct KRectangle TextureRectangle);
};

class Collider : public GameComponent
{
public:

	// Variables

	UniqueID* CellIndex;

	// The density of the collider used to calculate its mass (when auto mass is enabled).
	real32 Density;

	// The number of separate shaped regions in the collider.
	int32 ShapeCount;

	// Enabled Colliders will collide with other colliders, disabled Colliders won't.
	bool32 Enabled;

	// 	The local offset of the collider geometry.
	Vector2 Offset;

	// Is this collider configured as a trigger ?
	bool32 IsTrigger;

	// 	The world space bounding area of the collider.
	BoundingBox Bounds;

public:

	// Functions

	inline Collider();

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

	// Sent when another object enters a trigger collider attached to this object.
	inline virtual void OnTriggerEnter(const Collider& Collided)
	{
	}

	// Sent each frame where another object is within a trigger collider attached to this object.
	inline virtual void OnTriggerStay(const Collider& Collided)
	{
	}

	// Sent when another object leaves a trigger collider attached to this object.
	inline virtual void OnTriggerExit(const Collider& Collided)
	{
	}

	// Whether the collider is touching this collider or not.
	inline virtual bool32 IsTouching(Collider* Collided)
	{
		return GetBounds().Intersects(Collided->GetBounds());
	}

	// Does point overlap the collider?
	inline virtual bool32 OverlapPoint(Vector2 Point)
	{
		return GetBounds().Contains(Point);
	}

	// Whether this collider is touching any collider on the specified layerMask or not.
	inline virtual bool32 IsTouchingLayers(int32 LayerMask = 0) // All layers
	{
		return TRUE;
	}

	inline virtual BoundingBox GetBounds();

	bool operator == (const Collider& Other)
	{
		return InstanceID == Other.InstanceID;
	}

	k_internal bool32 line_oriented_rectangle_collide(Ray Line, BoundingBox r)
	{
		KRectangle lr;
		lr.Position.X = 0;
		lr.Position.Y = 0;
		lr.Size = r.Extents * 2;

		Ray ll;
		ll.Origin = Line.Origin - r.Center;
		ll.Origin = Vector2::RotateVector(ll.Origin, -r.Rotation); // -r.rotation);
		ll.Origin = ll.Origin + r.Extents;
		ll.Direction = Vector2::RotateVector(Line.Direction, -r.Rotation); // -r.rotation);
		return line_rectangle_collide(ll, lr);
	}

	k_internal bool32 line_rectangle_collide(Ray Line, KRectangle Rect)
	{
		Vector2 n = Vector2::Rotate90Degrees(Line.Direction);
		real32 dp1, dp2, dp3, dp4;
		Vector2 c1 = Rect.Position;
		Vector2 c2 = c1 + Rect.Size;
		Vector2 c3 = { c2.X, c1.Y };
		Vector2 c4 = { c1.X, c2.Y };
		c1 = c1 - Line.Origin;
		c2 = c2 - Line.Origin;
		c3 = c3 - Line.Origin;
		c4 = c4 - Line.Origin;
		dp1 = Vector2::DotProduct(n, c1);
		dp2 = Vector2::DotProduct(n, c2);
		dp3 = Vector2::DotProduct(n, c3);
		dp4 = Vector2::DotProduct(n, c4);
		return (dp1 * dp2 <= 0.0f) || (dp2 * dp3 <= 0.0f) || (dp3 * dp4 <= 0.0f);
	}

	k_internal bool32 rectangle_segment_collide(KRectangle r, Segment s)
	{
		Ray sLine;
		sLine.Origin = s.PointA;
		sLine.Direction = s.PointB - s.PointA;

		if (!line_rectangle_collide(sLine, r))
			return FALSE;

		Range rRange, sRange;

		rRange.Minimum = r.Position.X;
		rRange.Maximum = r.Position.X + r.Size.X;
		sRange.Minimum = s.PointA.X;
		sRange.Maximum = s.PointB.X;
		sRange = Range::Sort(sRange);

		if (!Range::IsOverlapping(rRange, sRange))
			return FALSE;

		rRange.Minimum = r.Position.Y;
		rRange.Maximum = r.Position.Y + r.Size.Y;
		sRange.Minimum = s.PointA.Y;
		sRange.Maximum = s.PointB.Y;
		sRange = Range::Sort(sRange);

		return Range::IsOverlapping(rRange, sRange);
	}

	k_internal bool32 oriented_rectangle_segment_collide(BoundingBox r, Segment s)
	{
		KRectangle lr;
		lr.Position.X = 0;
		lr.Position.Y = 0;
		lr.Size = r.Extents * 2;

		Segment ls;
		ls.PointA = s.PointA - r.Center;
		ls.PointA = Vector2::RotateVector(ls.PointA, -r.Rotation); // -r.rotation);
		ls.PointA = ls.PointA + r.Extents;
		ls.PointB = s.PointB - r.Center;
		ls.PointB = Vector2::RotateVector(ls.PointB, -r.Rotation);// -r.rotation);
		ls.PointB = ls.PointB + r.Extents;

		return rectangle_segment_collide(lr, ls);
	}

	k_internal bool32 overlapping(real32 minA, real32 maxA, real32 minB, real32 maxB)
	{ 
		return minB <= maxA && minA <= maxB; 
	}

	k_internal bool32 rectangles_collide(KRectangle a, KRectangle b)
	{
		real32 aLeft = a.Position.X;
		real32 aRight = aLeft + a.Size.X;
		real32 bLeft = b.Position.X;
		real32 bRight = bLeft + b.Size.X;
		real32 aBottom = a.Position.Y;
		real32 aTop = aBottom + a.Size.Y;

		float bBottom = b.Position.Y;
		float bTop = bBottom + b.Size.Y;

		return overlapping(aLeft, aRight, bLeft, bRight) && overlapping(aBottom, aTop, bBottom, bTop);
	}

	k_internal bool32 points_collide(Vector2 a, Vector2 b) 
	{ 
		return a.X == b.X && a.Y == b.Y; 
	}


};

class BoxCollider : public Collider
{
public:

	// Variables

	Vector2 Size;

public:

	// Functions

	inline BoxCollider()
	{
	}

	inline virtual bool32 IsTouching(Collider* Collided) override;
};

class EdgeCollider : public Collider
{
public:

	// Variables

	// Gets the number of edges.
	int32 GetEdgeCount()
	{
		return GetPointCount() - 1;
	}

	// Gets the number of points.
	int32 GetPointCount()
	{
		return Points.size();
	}

	// Get or set the points defining multiple continuous edges.
	std::vector<Vector2> Points;

public:

	// Functions

	inline EdgeCollider()
	{
	}

	void Reset()
	{
		Vector2 Min = GetMinPoint();
		Vector2 Max = GetMaxPoint();

		Points.clear();

		Points.push_back(Min);
		Points.push_back(Max);
	}

	inline Vector2 GetMinPoint()
	{
		Vector2 Low = Points[0];
		int32 LowIndex = 0;
		for (int32 Index = 0; Index < Points.size(); ++Index)
		{
			if (Points[Index] < Low)
			{
				Low = Points[Index];
				LowIndex = Index;
			}
		}

		return GetPoint(LowIndex);
	}

	inline Vector2 GetMaxPoint()
	{
		Vector2 High = Points[0];
		int32 HighIndex = 0;
		for (int32 Index = 0; Index < Points.size(); ++Index)
		{
			if (Points[Index] > High)
			{
				High = Points[Index];
				HighIndex = Index;
			}
		}

		return GetPoint(HighIndex);
	}

	inline Vector2 GetPoint(int32 Index)
	{
		Vector2 Result = Points[Index];

		Result += EntityAttachedTo->EntTransform.Position + Offset;

		return (Result);
	}

	inline Segment GetSegment(int32 Index)
	{
		Segment Result;
		Result.PointA = GetPoint(Index);
		Result.PointB = GetPoint(Index + 1);

		return (Result);
	}

	inline std::vector<Segment> GetSegments()
	{
		std::vector<Segment> Segments;
		for (uint32 Index = 0; Index < Points.size() - 1; ++Index)
		{
			Segments.push_back(GetSegment(Index));
		}

		return Segments;
	}

	// Whether the collider is touching this collider or not.
	inline virtual bool32 IsTouching(Collider* Collided)
	{
		if (GetBounds().Intersects(Collided->GetBounds()))
		{
			for (auto Seg : GetSegments())
			{
				if (oriented_rectangle_segment_collide(Collided->GetBounds(), Seg))
				{
					if (Collided->EntityAttachedTo->Name == "Player")
					{
						printf("Colliding with player");
					}

					return TRUE;
				}
			}

			return FALSE;

			Vector2 Start = Points[0], DeltaEnd = Points[Points.size() - 1];
			real32 PaddingX = 0;
			real32 PaddingY = 0;

			real32 ScaleX = 1.0 / DeltaEnd.X;
			real32 ScaleY = 1.0 / DeltaEnd.Y;
			int32 SignX = SignOf(ScaleX);
			int32 SignY = SignOf(ScaleY);

			real32 NearTimeX = (GetBounds().Center.X - SignX * (GetBounds().Extents.X + PaddingX) - Start.X) * ScaleX;
			real32 NearTimeY = (GetBounds().Center.Y - SignY * (GetBounds().Extents.Y + PaddingY) - Start.Y) * ScaleY;
			real32 FarTimeX = (GetBounds().Center.X + SignX * (GetBounds().Extents.X + PaddingX) - Start.X) * ScaleX;
			real32 FarTimeY = (GetBounds().Center.Y + SignY * (GetBounds().Extents.Y + PaddingY) - Start.Y) * ScaleY;

			if (NearTimeX > FarTimeY || NearTimeY > FarTimeX)
			{
				return FALSE;
			}

			real32 NearTime;
			if (NearTimeX > NearTimeY)
			{
				NearTime = NearTimeX;
			}
			else
			{
				NearTime = NearTimeY;
			}

			real32 FarTime;
			if (FarTimeX < FarTimeY)
			{
				FarTime = FarTimeX;
			}
			else
			{
				FarTime = FarTimeY;
			}

			if (NearTime >= 1 || FarTime <= 0)
			{
				return FALSE;
			}
		}

		return FALSE;
	}

	inline bool32 IsTouching(Vector2 Start, Vector2 DeltaEnd, Collision& HitInfo, int32 PaddingX = 0, int32 PaddingY = 0)
	{
		real32 ScaleX = 1.0 / DeltaEnd.X;
		real32 ScaleY = 1.0 / DeltaEnd.Y;
		int32 SignX = SignOf(ScaleX);
		int32 SignY = SignOf(ScaleY);

		real32 NearTimeX = (GetBounds().Center.X - SignX * (GetBounds().Extents.X + PaddingX) - Start.X) * ScaleX;
		real32 NearTimeY = (GetBounds().Center.Y - SignY * (GetBounds().Extents.Y + PaddingY) - Start.Y) * ScaleY;
		real32 FarTimeX = (GetBounds().Center.X + SignX * (GetBounds().Extents.X + PaddingX) - Start.X) * ScaleX;
		real32 FarTimeY = (GetBounds().Center.Y + SignY * (GetBounds().Extents.Y + PaddingY) - Start.Y) * ScaleY;

		if (NearTimeX > FarTimeY || NearTimeY > FarTimeX)
		{
			return FALSE;
		}

		real32 NearTime;
		if (NearTimeX > NearTimeY)
		{
			NearTime = NearTimeX;
		}
		else
		{
			NearTime = NearTimeY;
		}

		real32 FarTime;
		if (FarTimeX < FarTimeY)
		{
			FarTime = FarTimeX;
		}
		else
		{
			FarTime = FarTimeY;
		}

		if (NearTime >= 1 || FarTime <= 0)
		{
			return FALSE;
		}

		// Collision Hit;
		int32 DidHit = Clamp01(NearTime);
		if (NearTimeX > NearTimeY)
		{
			// Hit.Normal.X = -SignX;
			// Hit.Normal.Y = 0;
		}
		else
		{
			// Hit.Normal.X = 0;
			// Hit.Normal.Y = -SignY;
			// Hit.RelativeVelocity.X = DidHit * DeltaEnd.X;
			// Hit.RelativeVelocity.Y = DidHit * DeltaEnd.Y;

			ContactPoint Contact;
			// Contact.Point.X = GetBounds().Center.X + Hit.RelativeVelocity.X;
			// Contact.Point.Y = GetBounds().Center.Y + Hit.RelativeVelocity.Y;
			// Hit.Contacts.push_back(Contact);
		}

		// return Hit;
		return TRUE;
	}

	inline virtual BoundingBox GetBounds() override
	{
		Vector2 PointMin = GetMinPoint();
		Vector2 PointMax = GetMaxPoint();
		Bounds.SetMinMax(PointMin, PointMax);

		if (Bounds.Extents.X < 0.0f || Bounds.Extents.Y < 0.0f)
		{
			real32 TempY = PointMin.Y;
			PointMin.Y = PointMax.Y;
			PointMax.Y = TempY;
			Bounds.SetMinMax(PointMin, PointMax);
		}

		return Bounds;
	}
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
	}

	inline virtual void Draw() override
	{
		KRectangle DestinationRectangle = KRectangle(EntityAttachedTo->EntTransform.Position,
			SpriteToRender.Bounds.GetSize() * EntityAttachedTo->EntTransform.Scale);
		// RenderGraphics.BlitSurface(SpriteSheet, &SourceRect, &DestinationRectangle);

		BlitSprite(SpriteToRender.TextureRect.ToSDLRect(), DestinationRectangle.ToSDLRect());
	}

	inline void BlitSprite(SDL_Rect Source, SDL_Rect Destination);
};

struct AnimationData
{
	string AnimationName;

	std::vector<SDL_Rect> AnimationsRects;
	Vector2 Offsets;
	std::vector<uint32> AnimationDurations;
};

// TODO(Julian): Maybe do a override call instead of inheritence
class SpriteAnimator : public SpriteRenderer
{
public:

	// Variables

	// SpriteRenderer* Renderer;

	std::map<string, std::vector<SDL_Rect> > Animations;
	std::map<string, Vector2> Offsets;
	std::map<string, std::vector<uint32>> AnimationDurations;

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

		// GameEventSystem::EventSystem->AddDrawHandler(this);
	}

	inline void PlayAnimation(string Animation, bool32 PlayOnce = 0)
	{
		CurrentAnimPlayOnce = PlayOnce;

		if (CurrentAnim != Animation)
		{
			CurrentAnim = Animation;
			TimeToUpdate = AnimationDurations[Animation][FrameIndex];
			FrameIndex = 0;
		}
	}

	inline virtual void Draw() override
	{
		if (IsVisible == 1)
		{
			// TODO(Julian): Fix this
			KRectangle DestinationRectangle =
			{
				EntityAttachedTo->EntTransform.Position + Offsets[CurrentAnim],
				SpriteToRender.TextureRect.GetSize() * EntityAttachedTo->EntTransform.Scale
			};

			KRectangle SourceRectangle = { Animations[CurrentAnim][FrameIndex] };

			SpriteToRender.TextureRect = SourceRectangle;

			BlitSprite(SourceRectangle.ToSDLRect(), DestinationRectangle.ToSDLRect());
		}
	}

	inline void AddAnimation(struct AnimationData Animation)
	{
		Animations.insert(std::pair<string, std::vector<SDL_Rect> >(Animation.AnimationName, Animation.AnimationsRects));
		Offsets.insert(std::pair<string, Vector2>(Animation.AnimationName, Animation.Offsets));
		AnimationDurations.insert(std::pair<string, std::vector<uint32>>(Animation.AnimationName, Animation.AnimationDurations));
	}

	inline void AddAnimation(uint32 Frames, uint32 X, uint32 Y, string AnimationName, uint32 Width, uint32 Height, Vector2 Offset, std::vector<uint32> Durations)
	{
		std::vector<SDL_Rect> Rectangles;

		for (uint32 Index = 0; Index < Frames; ++Index)
		{
			SDL_Rect Rect = { (int32)(Index + X) * (int32)Width, (int32)Y, (int32)Width, (int32)Height };
			Rectangles.push_back(Rect);
		}

		Animations.insert(std::pair<string, std::vector<SDL_Rect> >(AnimationName, Rectangles));
		Offsets.insert(std::pair<string, Vector2>(AnimationName, Offset));
		AnimationDurations.insert(std::pair<string, std::vector<uint32>>(AnimationName, Durations));
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

	inline PhysicsComponent();
};

class RigidBody2D : public PhysicsComponent
{
public:

	// Variables

	real32 GravityRate = 70.0f;
	real32 GravityMax = 200.0f;
	real32 Friction = 0.5f;
	real32 Mass = 1.0f;

	Vector2 Velocity = Vector2(0, 0);
	Vector2 MaxVelocity = Vector2(10.0f, 100.0f);
	Vector2 Acceleration = Vector2(0, 0);

	Collider* BodyCollider;

	Collider* Ground;
	bool32 IsGrounded = FALSE;
	bool32 IsLeftWall = FALSE;
	bool32 IsRightWall = FALSE;

public:

	// Functions

	inline RigidBody2D()
	{
	}

	inline bool32 SlopeTest(const Collision& CollideInfo, Vector2& Average)
	{
		if (!IsGrounded)
		{
			return FALSE;
		}

		Transform* EntTransform = &EntityAttachedTo->EntTransform;

		BoundingBox Bounds = BodyCollider->GetBounds();

		if (CollideInfo.Contacts.size() > 1)
		{
			Average = Vector2::Zero;

			for (int32 Index = 0; Index < CollideInfo.Contacts.size(); ++Index)
			{
				Average += CollideInfo.Contacts[Index].Point;
			}
		}
		else
		{
			Average = CollideInfo.Contacts[0].Point;
		}

		Average = Average / CollideInfo.Contacts.size();

		real32 Angle = Vector2::Angle(Vector2::Up, -CollideInfo.Normal);
		if (AbsoluteValue(Angle) < 45.0f)
		{
			return TRUE;
		}

		return FALSE;
	}

	inline virtual void MovePosition(Vector2 Position)
	{
		MovePosition(Position.X, Position.Y);
	}

	inline virtual void MovePosition(real32 X, real32 Y)
	{
		Transform* EntTransform = &EntityAttachedTo->EntTransform;

		EntTransform->Position.X = Lerp(EntTransform->Position.X, Time::FixedUpdate, X);
		EntTransform->Position.Y = Lerp(EntTransform->Position.Y, Time::FixedUpdate, Y - BodyCollider->GetBounds().GetSize().Y);
	}

	inline virtual void MovementDetection(const Collision& CollideInfo)
	{
		Transform* EntTransform = &EntityAttachedTo->EntTransform;

		if (CollideInfo.Normal.Y > 0.0f)
		{
			IsGrounded = TRUE;
			Ground = CollideInfo.Collider;
			Velocity.Y = 0.0f;
		}
		else if (CollideInfo.Normal.Y < 0.0f)
		{
			Velocity.Y = 0.0f;
			Acceleration.Y = 0.0f;
			EntTransform->Position.Y -= SignOf(CollideInfo.Normal.Y);
		}

		if (AbsoluteValue(CollideInfo.Normal.X) > 0.0f)
		{
			Vector2 Average;
			if (SlopeTest(CollideInfo, Average))
			{
				// Velocity.Y = 0.0f;
				MovePosition(EntTransform->Position.X, Average.Y);
				// EntTransform->Position.Y -= BodyCollider->GetBounds().GetSize().Y;
			}
			else
			{
				if (AbsoluteValue(Velocity.X) > 0.0f)
				{
					Velocity.X = 0.0f;
					// Velocity.Y = 0.0f;
					EntTransform->Position.X -= SignOf(CollideInfo.Normal.X);
				}
			}
		}
	}

	inline virtual void OnCollisionStart(const Collision& CollideInfo) override
	{
		MovementDetection(CollideInfo);
	}

	inline virtual void OnCollisionStay(const Collision& CollideInfo) override
	{
		MovementDetection(CollideInfo);
	}

	inline virtual void OnCollisionEnd(const Collision& CollideInfo) override
	{
		if (CollideInfo.Collider == Ground)
		{
			Ground = nullptr;
			IsGrounded = FALSE;
		}

		if (CollideInfo.Normal.Y < 0.0f)
		{
			Velocity.Y = 0.0f;
			Acceleration.Y = 0.0f;
		}
	}

	inline void AddFriction()
	{
		if (!IsGrounded)
		{
			if (Acceleration.Y > GravityMax)
			{
				Acceleration.Y = GravityMax;
			}
			else
			{
				Acceleration.Y += GravityRate;
			}
		}
		else
		{
			if (Acceleration.Y > 0.0f)
			{
				Acceleration.Y = 0.0f;
			}
		}

		if (AbsoluteValue(Velocity.X) < 0.1f)
		{
			Velocity.X = 0.0f;
		}
		else
		{
			Velocity -= -Velocity * Friction;
		}

		Velocity.X = SignOf(Velocity.X) * Clamp(0.0f, AbsoluteValue(Velocity.X), MaxVelocity.X);
		Velocity.Y = SignOf(Velocity.Y) * Clamp(0.0f, AbsoluteValue(Velocity.Y), MaxVelocity.Y);
	}

	inline void Update() override
	{
	}

	inline void VerletVelocityMethod()
	{
		// Acceleration = force(Time::TimeSinceStartup, EntityAttachedTo->EntTransform.Position) / Mass;
		// EntityAttachedTo->EntTransform.Position += Time::FixedUpdate * (Velocity + Time::FixedUpdate * Acceleration / 2);
		// Vector2 NewAcceleration = force(Time::TimeSinceStartup, EntityAttachedTo->EntTransform.Position) / Mass;
		// Velocity += Time::FixedUpdate * (Acceleration + NewAcceleration) / 2;
	}

	inline void FixedUpdate() override
	{
		// AddFriction();
		if (!IsGrounded)
		{
			if (Acceleration.Y > GravityMax)
			{
				Acceleration.Y = GravityMax;
			}
			else
			{
				Acceleration.Y += GravityRate * Time::FixedUpdate;
			}
		}

		EntityAttachedTo->EntTransform.Position += Velocity * Time::FixedUpdate;
		Velocity += (Acceleration - (Velocity * Friction)) * Time::FixedUpdate;

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
	Collider* PlayerCollider;

	DIRECTION CurrentDirection;

	real32 Speed = 5.0f;
	real32 JumpAmount = 500.0f;

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
		Animator->SpriteToRender.SetupSprite("../Data/Images/MyChar.bmp",
			KRectangle(0, 0, 32, 32));

		// "../Data/Images/MyChar.bmp", SDL_Rect{ 0, 0, 32, 32 }, ObjectPosition, 300
		// GameObject->Components.push_back(Animator);

		Animator->AddAnimation(1, 0, 0, "IdleLeft", 32, 32, Vector2(0, 0), { 100, 100 });
		Animator->AddAnimation(1, 0, 32, "IdleRight", 32, 32, Vector2(0, 0), { 100, 100 });

		Animator->AddAnimation(3, 0, 0, "RunLeft", 32, 32, Vector2(0, 0), { 100, 100 });
		Animator->AddAnimation(3, 0, 32, "RunRight", 32, 32, Vector2(0, 0), { 100, 100 });

		Animator->PlayAnimation("RunRight");

		Name = "Player";

		EntityAttachedTo->Name = Name;
		EntityAttachedTo->Layer = 1;
		Animator->Layer = 1;

		RigidBody = EntityAttachedTo->AddComponent<RigidBody2D>();
		PlayerCollider = EntityAttachedTo->AddComponent<BoxCollider>();
		RigidBody->BodyCollider = PlayerCollider;
		PlayerCollider->Bounds = BoundingBox(Vector2(16, 16), Vector2(24, 32));
	}

	inline void Move(DIRECTION FacingDirection)
	{
		CurrentDirection = FacingDirection;

		switch (FacingDirection)
		{
		case DIRECTION::LEFT:
			// GameObject.EntTransform.Position.X = -Speed;
			RigidBody->Acceleration.X = -Speed;
			Animator->PlayAnimation("RunLeft");
			break;
		case DIRECTION::RIGHT:
			// Velocity.X = Speed;
			RigidBody->Acceleration.X = Speed;
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
		RigidBody->Acceleration.X = 0.0f;

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

	inline void Jump()
	{
		RigidBody->Acceleration.Y = -JumpAmount;
		RigidBody->IsGrounded = FALSE;
	}

	inline void EndJump()
	{
		RigidBody->Acceleration.Y = 0.0f;
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

	inline Tile()
	{
	}

	inline Tile(SDL_Texture* TileSetTexture, Vector2 TilePosition, Vector2 TileSetPosition, Vector2 TileSetSize)
	{
		Name = "Tile";
		TileSet = TileSetTexture;

		EntityAttachedTo = new GameEntity("Tile");
		EntityAttachedTo->EntTransform.Position = TilePosition;
		EntityAttachedTo->EntTransform.Scale = Vector2(1.0f, 1.0f);

		EntRenderer = EntityAttachedTo->AddComponent<SpriteRenderer>();
		EntRenderer->SpriteToRender.SpriteSheet = TileSetTexture;
		EntRenderer->SpriteToRender.TextureRect = KRectangle(TileSetPosition, TileSetSize);
	}

	inline void Update() override
	{
	}
};

class AnimatedTile : public Tile
{
public:

public:


	inline AnimatedTile(SDL_Texture* TileSetTexture, Vector2 TilePosition, Vector2 TileSetPosition, Vector2 TileSetSize, struct AnimationData Animation)
	{
		Name = "Tile";
		TileSet = TileSetTexture;

		EntityAttachedTo = new GameEntity("AnimatedTile");
		EntityAttachedTo->EntTransform.Position = TilePosition;
		EntityAttachedTo->EntTransform.Scale = Vector2(1.0f, 1.0f);

		EntRenderer = EntityAttachedTo->AddComponent<SpriteAnimator>();
		SpriteAnimator* Animator = (SpriteAnimator *)EntRenderer;

		Animator->AddAnimation(Animation);
		Animator->PlayAnimation(Animation.AnimationName);
		EntRenderer->SpriteToRender.SpriteSheet = TileSetTexture;
		EntRenderer->SpriteToRender.TextureRect = KRectangle(TileSetPosition, TileSetSize);
	}
};

class PhysicsTile : public Tile
{
public:

	// Variables

	BoxCollider* Collider;

public:

	inline PhysicsTile()
	{
	}

	inline PhysicsTile(SDL_Texture* TileSetTexture, BoundingBox PhysicsBox, Vector2 TileSetPosition, Vector2 TileSetTextureMin, Vector2 TileSetTextureMax) :
		Tile(TileSetTexture, TileSetPosition, TileSetTextureMin, TileSetTextureMax)
	{
		Name = "PhysicalTile";
		EntityAttachedTo->Name = "PhysicsTile";
		Collider = EntityAttachedTo->AddComponent<BoxCollider>();
		//Collider->Bounds.X = (uint32)TilePosition.X;
		//Collider->Bounds.Y = (uint32)TilePosition.Y;
		Collider->Bounds = PhysicsBox;
		EntRenderer->SpriteToRender.Bounds = PhysicsBox;
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
	int32 TileCount;

	uint32 TileWidth;
	uint32 TileHeight;

	uint32 TileColumns;

	uint32 TextureWidth;
	uint32 TextureHeight;

	std::map<uint32, AnimationData> Animations;

	Tileset()
	{
		FirstGID = -1;
	}

	Tileset(SDL_Texture* TileSetTexture, int32 TilesetFirstGID, int32 MaxTileCount,
		int32 TileSetWidth, int32 TileSetHeight, int32 TileSetColumns, int32 TileSetTextureWidth, int32 TileSetTextureHeight)
	{
		SetTileSet(TileSetTexture, TilesetFirstGID, MaxTileCount, TileSetWidth,
			TileSetHeight, TileSetColumns, TileSetTextureWidth, TileSetTextureHeight);
	}

	inline void SetTileSet(SDL_Texture* TileSetTexture, int32 TilesetFirstGID, int32 MaxTileCount,
		int32 TileSetWidth, int32 TileSetHeight, int32 TileSetColumns, int32 TileSetTextureWidth, int32 TileSetTextureHeight)
	{
		Texture = TileSetTexture;
		FirstGID = TilesetFirstGID;
		TileCount = MaxTileCount;
		TileWidth = TileSetWidth;
		TileHeight = TileSetHeight;
		TileColumns = TileSetColumns;
		TextureWidth = TileSetTextureWidth;
		TextureHeight = TileSetTextureHeight;
	}

	inline void AddAnimation(uint32 GID_ID, uint32 Duration, uint32 TileID)
	{
		Animations[GID_ID].AnimationName = std::to_string(GID_ID);
		Animations[GID_ID].AnimationDurations.push_back(Duration);

		// uint32 TSXX = (TileID + FirstGID) % (TextureWidth / (int32)TileWidth) - 1;
		uint32 TSXX = TileID % TileColumns;
		TSXX *= TileWidth;

		uint32 TSYY = 0;
		// uint32 Amount = (((TileID + FirstGID) - FirstGID) / (TextureWidth / TileWidth));
		uint32 Amount = TileID / TileColumns;

		TSYY = TileHeight * Amount;

		Vector2 FinalTileSetPosition = Vector2(TSXX, TSYY);

		Vector2 TileSetSize = Vector2(TileWidth, TileHeight);

		KRectangle TextureRect = { FinalTileSetPosition, TileSetSize };

		Animations[GID_ID].AnimationsRects.push_back(TextureRect.ToSDLRect());
		// Animation.Offsets[AnimationName] = Offset;
	}
};

class Level : public GameBehaviour
{
public:

	// Variables

	string MapName;
	Vector2 SpawnPoint;

	Vector2 LevelSize;
	Vector2 LevelTileSize;

	SDL_Texture* BackgroundTexture;

	std::vector<GameEntity*> SlopeList;
	std::vector<Tile*> TileList;
	std::vector<Tileset> TileSets;

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

	inline void LoadMapFile(string LevelMapName);

	inline void LoadMap(string LevelMapName);

	inline virtual void Update() override
	{
	}

	inline void Draw()
	{
	}
};

#define KANTI2D_GAMEOBJECT

#endif
