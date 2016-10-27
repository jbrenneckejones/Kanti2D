#include "Kanti2DGameObject.h"

template<class GameComponent>
inline GameComponent* GameEntity::AddComponent()
{
	GameComponent* Component = new GameComponent();

	if (Component)
	{
		Component->EntityAttachedTo = this;
		Components.push_back(Component);
	}

	return Component;
}

template<class GameComponent>
inline GameComponent* GameEntity::AddComponent(GameComponent* ComponentPrefab)
{
	if (ComponentPrefab)
	{
		ComponentPrefab->EntityAttachedTo = this;
		Components.push_back(ComponentPrefab);
	}

	return ComponentPrefab;
}

template<class GameComponent>
GameComponent* GameEntity::GetComponent()
{
	for (const auto& Iterator : Components)
	{
		GameComponent* Component = dynamic_cast<GameComponent*>(Iterator);

		if (Component)
		{
			return Component;
		}
	}

	return nullptr;
}

Collision::Collision(class Collider* CalledOnCollider, class Collider* IncomingCollider) :
	Caller(CalledOnCollider), Collider(IncomingCollider)
{
	// Vector2 Position = CalledOnCollider.EntityAttachedTo->EntTransform.Position;
	// RelativeVelocity = Body == nullptr ? Vector2::Zero : (Position + Body->Velocity) - Position;

	GetCollisionInfo(CalledOnCollider, IncomingCollider, *this);

	// Normal = GetNormal();
}

void Collision::GetCollisionInfo(class Collider* Caller, class Collider* Collided, Collision& Info)
{
	Info.Caller = Caller;
	Info.Collider = Collided;

	// First we need to get the initial bounds collision
	// as in top left right bottom
	// then we can get a more precise collision

	BoundingBox Ours = Caller->GetBounds();
	BoundingBox Box = Collided->GetBounds();

	real32 DeltaX = Box.Center.X - Ours.Center.X;
	real32 PixelX = (Box.Extents.X + Ours.Extents.X) - AbsoluteValue(DeltaX);

	real32 DeltaY = Box.Center.Y - Ours.Center.Y;
	real32 PixelY = (Box.Extents.Y + Ours.Extents.Y) - AbsoluteValue(DeltaY);

	// Vector2 ClosestPoint = Ours.ClosestPoint(Box.Center);

	ContactPoint InitialContact;
	if (PixelX < PixelY)
	{
		real32 SurfaceX = SignOf(DeltaX);
		Info.RelativeVelocity.X = PixelX * SurfaceX;
		Info.Normal.X = SurfaceX;

		InitialContact.Normal.X = SurfaceX;
		InitialContact.Point.X = Ours.Center.X + (Ours.Extents.X * SurfaceX);
		InitialContact.Point.Y = Box.Center.Y;
		Info.Contacts.push_back(InitialContact);
	}
	else
	{
		real32 SurfaceY = SignOf(DeltaY);
		Info.RelativeVelocity.Y = PixelY * SurfaceY;
		Info.Normal.Y = SurfaceY;

		InitialContact.Normal.Y = SurfaceY;
		InitialContact.Point.X = Box.Center.X;
		InitialContact.Point.Y = Ours.Center.Y + (Ours.Extents.Y * SurfaceY);
		Info.Contacts.push_back(InitialContact);
	}

	if (dynamic_cast<BoxCollider*>(Caller) && dynamic_cast<EdgeCollider*>(Collided))
	{
		BoxCollider* Box = dynamic_cast<BoxCollider*>(Caller);

		EdgeCollider* Edge = dynamic_cast<EdgeCollider*>(Collided);

		BoundingBox BoxBounds = Box->GetBounds();

		Segment BoxLeft;
		BoxLeft.PointA = BoxBounds.GetMin();
		BoxLeft.PointB = { BoxLeft.PointA.X, BoxBounds.GetMax().Y };

		Segment BoxBottom;
		BoxBottom.PointA = BoxLeft.PointB;
		BoxBottom.PointB = BoxBounds.GetMax();

		Segment BoxRight;
		BoxRight.PointA = BoxBottom.PointB;
		BoxRight.PointB = { BoxBounds.GetMax().X, BoxBounds.GetMin().Y };

		Segment BoxTop;
		BoxTop.PointA = BoxLeft.PointA;
		BoxTop.PointB = BoxRight.PointB;

		for (auto Line : Edge->GetSegments())
		{
			if (Segment::GetIntersection(BoxLeft, Line) != Vector2::Zero)
			{
				ContactPoint Contact;
				Contact.Point = Segment::GetIntersection(BoxLeft, Line);
				Contact.Normal = -Vector2::Rotate90Degrees(Contact.Point).Normalize();
				// Info.Normal.X = Contact.Normal.X;
				Info.Contacts.push_back(Contact);
			}

			if (Segment::GetIntersection(BoxTop, Line) != Vector2::Zero)
			{
				ContactPoint Contact;
				Contact.Point = Segment::GetIntersection(BoxTop, Line);
				Contact.Normal = -Vector2::Rotate90Degrees(Contact.Point).Normalize();
				// Info.Normal.Y = Contact.Normal.Y;
				Info.Contacts.push_back(Contact);
			}

			if (Segment::GetIntersection(BoxRight, Line) != Vector2::Zero)
			{
				ContactPoint Contact;
				Contact.Point = Segment::GetIntersection(BoxRight, Line);
				Contact.Normal = -Vector2::Rotate90Degrees(Contact.Point).Normalize();
				// Info.Normal.X = Contact.Normal.X;
				Info.Contacts.push_back(Contact);
			}

			if (Segment::GetIntersection(BoxBottom, Line) != Vector2::Zero)
			{
				ContactPoint Contact;
				Contact.Point = Segment::GetIntersection(BoxBottom, Line);
				Contact.Normal = -Vector2::Rotate90Degrees(Contact.Point).Normalize();
				// Info.Normal.Y = Contact.Normal.Y;
				Info.Contacts.push_back(Contact);
			}
		}
	}
	else if (dynamic_cast<EdgeCollider*>(Caller) && dynamic_cast<BoxCollider*>(Collided))
	{

		BoxCollider* Box = dynamic_cast<BoxCollider*>(Collided);

		if (Box->EntityAttachedTo->Name == "Player")
		{
			// __debugbreak();
		}

		EdgeCollider* Edge = dynamic_cast<EdgeCollider*>(Caller);

		BoundingBox BoxBounds = Box->GetBounds();

		Segment BoxLeft;
		BoxLeft.PointA = BoxBounds.GetMin();
		BoxLeft.PointB = { BoxLeft.PointA.X, BoxBounds.GetMax().Y };

		Segment BoxBottom;
		BoxBottom.PointA = BoxLeft.PointB;
		BoxBottom.PointB = BoxBounds.GetMax();

		Segment BoxRight;
		BoxRight.PointA = BoxBottom.PointB;
		BoxRight.PointB = { BoxBounds.GetMax().X, BoxBounds.GetMin().Y };

		Segment BoxTop;
		BoxTop.PointA = BoxLeft.PointA;
		BoxTop.PointB = BoxRight.PointB;

		for (auto Line : Edge->GetSegments())
		{
			// Vector2 Location;
			// if (Segment::lines_intersect(BoxLeft, Line, Location) == DO_INTERSECT)


			if (Segment::GetIntersection(BoxLeft, Line) != Vector2::Zero)
			{
				ContactPoint Contact;
				Contact.Point = Segment::GetIntersection(BoxLeft, Line);
				Contact.Normal = -Vector2::Rotate90Degrees(Contact.Point).Normalize();
				// Info.Normal.X = Contact.Normal.X;
				Info.Contacts.push_back(Contact);
			}

			if (Segment::GetIntersection(BoxTop, Line) != Vector2::Zero)
			{
				ContactPoint Contact;
				Contact.Point = Segment::GetIntersection(BoxTop, Line);
				Contact.Normal = -Vector2::Rotate90Degrees(Contact.Point).Normalize();
				// Info.Normal.Y = Contact.Normal.Y;
				Info.Contacts.push_back(Contact);
			}

			if (Segment::GetIntersection(BoxRight, Line) != Vector2::Zero)
			{
				ContactPoint Contact;
				Contact.Point = Segment::GetIntersection(BoxRight, Line);
				Contact.Normal = -Vector2::Rotate90Degrees(Contact.Point).Normalize();
				// Info.Normal.X = Contact.Normal.X;
				Info.Contacts.push_back(Contact);
			}

			if (Segment::GetIntersection(BoxBottom, Line) != Vector2::Zero)
			{
				ContactPoint Contact;
				Contact.Point = Segment::GetIntersection(BoxBottom, Line);
				Contact.Normal = -Vector2::Rotate90Degrees(Contact.Point).Normalize();
				// Info.Normal.Y = Contact.Normal.Y;
				Info.Contacts.push_back(Contact);
			}
		}
	}
	else if (dynamic_cast<BoxCollider*>(Caller) && dynamic_cast<BoxCollider*>(Collided))
	{
		// do stuff here
	}
}

inline Renderer::Renderer()
{
	GameEventSystem::EventSystem->AddDrawHandler(this);
}

inline void Sprite::SetupSprite(const string & FileName, KRectangle TextureRectangle )
{
	TextureRect = TextureRectangle;

	SDL_Surface* Surface = RenderManager::CurrentRenderer->LoadImage(FileName);

	SDL_SetColorKey(Surface, 1, SDL_MapRGB(Surface->format, 0, 0, 0));

	SpriteSheet = SDL_CreateTextureFromSurface(RenderManager::CurrentRenderer->SDLRenderer, Surface);

	if (!SpriteSheet)
	{
		printf("\nError: Unable to load image\n");
	}

	Bounds = BoundingBox::GetPositionSize(TextureRectangle.Position, TextureRectangle.Size);
}

inline Collider::Collider() : Bounds({ 0, 0 }, { 1, 1 })
{
	GameEventSystem::EventSystem->AddCollisionHandler(this);
}

inline BoundingBox Collider::GetBounds()
{
	Vector2 Center = Bounds.Center + EntityAttachedTo->EntTransform.Position + Offset;
	Vector2 Size = Bounds.GetSize();

	RigidBody2D* Body = EntityAttachedTo->GetComponent<RigidBody2D>();

	Body != nullptr ? Center += (Body->Velocity * Time::FixedUpdate), Size += Size * (Body->Velocity.Normalize() * Time::FixedUpdate) : Center += Vector2::Zero;

	BoundingBox Result = BoundingBox(Center, Size);

	return Result;
}

inline void SpriteRenderer::BlitSprite(SDL_Rect Source, SDL_Rect Destination)
{
	RenderManager::CurrentRenderer->BlitSurface(SpriteToRender.SpriteSheet, &Source, &Destination);
}

inline PhysicsComponent::PhysicsComponent()
{
	GameEventSystem::EventSystem->AddPhysicsHandler(this);
}

inline void Level::LoadMapFile(string LevelMapName)
{
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
				SDL_Texture* DebugTexture = SDL_CreateTextureFromSurface(RenderManager::CurrentRenderer->SDLRenderer,
					RenderManager::CurrentRenderer->LoadImage("../Data/Images/Debug/Collider.png"));

				for (uint32 ObjectIndex = 0; ObjectIndex < LayerList[LayerIndex]["objects"].size(); ++ObjectIndex)
				{
					real32 NewTileX = LayerList[LayerIndex]["objects"][ObjectIndex]["x"].get<real32>();
					real32 NewTileY = LayerList[LayerIndex]["objects"][ObjectIndex]["y"].get<real32>();
					real32 NewTileWidth = LayerList[LayerIndex]["objects"][ObjectIndex]["width"].get<real32>();
					real32 NewTileHeight = LayerList[LayerIndex]["objects"][ObjectIndex]["height"].get<real32>();

					Vector2 Pos = { NewTileX , NewTileY };
					Vector2 Size = { NewTileWidth, NewTileHeight };

					BoundingBox Rect = BoundingBox::GetPositionSize(Vector2::Zero, Size);
					Rect.Rotation = LayerList[LayerIndex]["objects"][ObjectIndex]["rotation"].get<real32>();
					// TODO(Julian): Find a way to get rid of the texture object
					PhysicsTile* PhysicalTile = new PhysicsTile(DebugTexture, Rect, Pos, Vector2::Zero, Vector2(32, 32));

					TileList.push_back(PhysicalTile);
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

					Vector2 TileSetSize = Vector2(TileWidth, TileHeight);

					Tile* BuildTile = new Tile(Set.Texture, FinalTilePosition, FinalTileSetPosition, TileSetSize);
					TileList.push_back(BuildTile);
				}
			}
			else if (LayerList[LayerIndex]["name"].get<string>() == "Slopes")
			{
				for (auto& Item : LayerList[LayerIndex]["objects"])
				{
					EdgeCollider* Edge = new EdgeCollider();
					GameEntity* Slope = new GameEntity("Slope");
					

					for (auto& Polyline : Item["polyline"])
					{
						Vector2 PolyPoint = { Polyline["x"].get<int32>(), Polyline["y"].get<int32>() };
						Edge->Points.push_back(PolyPoint);
					}

					GameEventSystem::EventSystem->AddCollisionHandler(Edge);
					GameEventSystem::EventSystem->GameEntities[Slope->InstanceID.ToString()] = Slope;
					Slope->AddComponent<EdgeCollider>(Edge);
					Edge->EntityAttachedTo->EntTransform.Position = Vector2(Item["x"].get<int32>(), Item["y"].get<int32>());

					SlopeList.push_back(Slope);
				}
			}
		}
	}
}

inline void Level::LoadMap(string LevelMapName)
{
	BackgroundTexture = SDL_CreateTextureFromSurface(RenderManager::CurrentRenderer->SDLRenderer,
		RenderManager::CurrentRenderer->LoadImage("../Data/Images/bkBlue.bmp"));
	LevelSize = Vector2(1280, 960);
}

inline void GameBehaviour::AddHandlers()
{
	// void* Pointer = (void *)(this->OnCollisionEnd);
	if (true)//(GameBehavour::*)Pointer == &GameBehaviour::OnCollisionStart)
		std::cout << "not overridden" << std::endl;
	else
		std::cout << "overridden" << std::endl;

	GameEventSystem::EventSystem->AddUpdateHandler(this);
	GameEventSystem::EventSystem->AddFixedUpdateHandler(this);
}

inline bool32 BoxCollider::IsTouching(Collider* Collided)
{
	// return Collider::IsTouching(Collided);
	BoxCollider* Box = dynamic_cast<BoxCollider*>(Collided);

	if (Box)
	{
		Collider::IsTouching(Box);
	}
	else if ((EdgeCollider *)Collided != nullptr)
	{

		EdgeCollider* Edge = (EdgeCollider *)Collided;

		Segment Seg = Edge->GetSegment(0);

		bool32 Result = oriented_rectangle_segment_collide(GetBounds(), Seg);

		if (Result)
		{
			if (this->EntityAttachedTo->Name == "Player")
			{
				 // __debugbreak();

				 bool32 Result = oriented_rectangle_segment_collide(GetBounds(), Seg);
			}
		}

		return Result;
	}
}
