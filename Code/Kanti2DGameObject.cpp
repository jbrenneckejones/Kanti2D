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

	if (dynamic_cast<BoxCollider*>(Caller) && dynamic_cast<EdgeCollider*>(Collided))
	{
		BoundingBoxEdgeCollision(dynamic_cast<BoxCollider*>(Caller), dynamic_cast<EdgeCollider*>(Collided), Info);
	}
	else if (dynamic_cast<EdgeCollider*>(Caller) && dynamic_cast<BoxCollider*>(Collided))
	{
		BoundingBoxEdgeCollision(dynamic_cast<BoxCollider*>(Collided), dynamic_cast<EdgeCollider*>(Caller), Info);
	}
	else if (dynamic_cast<BoxCollider*>(Caller) && dynamic_cast<BoxCollider*>(Collided))
	{
		BoundingBoxCollision(dynamic_cast<BoxCollider*>(Caller), dynamic_cast<BoxCollider*>(Collided), Info);
	}
}

void Collision::BoundingBoxEdgeCollision(BoxCollider* Box, EdgeCollider* Edge, Collision& Info)
{
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
			Info.Normal += Contact.Normal;
			Info.Contacts.push_back(Contact);
		}

		if (Segment::GetIntersection(BoxTop, Line) != Vector2::Zero)
		{
			ContactPoint Contact;
			Contact.Point = Segment::GetIntersection(BoxTop, Line);
			Contact.Normal = -Vector2::Rotate90Degrees(Contact.Point).Normalize();
			Info.Normal += Contact.Normal;
			Info.Contacts.push_back(Contact);
		}

		if (Segment::GetIntersection(BoxRight, Line) != Vector2::Zero)
		{
			ContactPoint Contact;
			Contact.Point = Segment::GetIntersection(BoxRight, Line);
			Contact.Normal = -Vector2::Rotate90Degrees(Contact.Point).Normalize();
			Info.Normal += Contact.Normal;
			Info.Contacts.push_back(Contact);
		}

		if (Segment::GetIntersection(BoxBottom, Line) != Vector2::Zero)
		{
			ContactPoint Contact;
			Contact.Point = Segment::GetIntersection(BoxBottom, Line);
			Contact.Normal = -Vector2::Rotate90Degrees(Contact.Point).Normalize();
			Info.Normal += Contact.Normal;
			Info.Contacts.push_back(Contact);
		}
	}

	Info.Normal = Info.Normal / Info.Contacts.size();
	Info.Normal = -Info.Normal;
}

void Collision::BoundingBoxCollision(BoxCollider* Box, BoxCollider* CollidedBox, Collision& Info)
{
	BoundingBox OurBoxBounds = Box->GetBounds();
	BoundingBox CollidedBoxBounds = CollidedBox->GetBounds();

	real32 DeltaX = CollidedBoxBounds.Center.X - OurBoxBounds.Center.X;
	real32 PixelX = (CollidedBoxBounds.Extents.X + OurBoxBounds.Extents.X) - AbsoluteValue(DeltaX);

	real32 DeltaY = CollidedBoxBounds.Center.Y - OurBoxBounds.Center.Y;
	real32 PixelY = (CollidedBoxBounds.Extents.Y + OurBoxBounds.Extents.Y) - AbsoluteValue(DeltaY);

	// Vector2 ClosestPoint = Ours.ClosestPoint(Box.Center);

	ContactPoint InitialContact;
	if (PixelX < PixelY)
	{
		real32 SurfaceX = SignOf(DeltaX);
		Info.RelativeVelocity.X = PixelX * SurfaceX;
		Info.Normal.X = SurfaceX;

		InitialContact.Normal.X = SurfaceX;
		InitialContact.Point.X = OurBoxBounds.Center.X + (OurBoxBounds.Extents.X * SurfaceX);
		InitialContact.Point.Y = CollidedBoxBounds.Center.Y;
		Info.Contacts.push_back(InitialContact);
	}
	else
	{
		real32 SurfaceY = SignOf(DeltaY);
		Info.RelativeVelocity.Y = PixelY * SurfaceY;
		Info.Normal.Y = SurfaceY;

		InitialContact.Normal.Y = SurfaceY;
		InitialContact.Point.X = CollidedBoxBounds.Center.X;
		InitialContact.Point.Y = OurBoxBounds.Center.Y + (OurBoxBounds.Extents.Y * SurfaceY);
		Info.Contacts.push_back(InitialContact);
	}
}

inline Renderer::Renderer()
{
	GameEventSystem::EventSystem->AddDrawHandler(this);
}

inline void Sprite::SetupSprite(const string& FileName, KRectangle TextureRectangle)
{
	TextureRect = TextureRectangle;

	SDL_Surface* Surface = RenderManager::CurrentRenderer->LoadImageFile(FileName);

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
	SDL_RWops* File = SDL_RWFromFile(LevelMapName.c_str(), "rb");

	const int64 FileSize = (int64)File->size;
	char* Buffer = (char *)malloc(FileSize * sizeof(char));
	File->read(File, Buffer, FileSize * sizeof(char), 1);

	string FileContents = Buffer;

	nlohmann::json JsonFile = nlohmann::json::parse(FileContents.c_str());

	MapName = JsonFile["tilesets"][0]["name"].get<string>();

	LevelSize.X = JsonFile["width"].get<uint32>();
	LevelSize.Y = JsonFile["height"].get<uint32>();

	LevelTileSize.X = JsonFile["tilewidth"].get<uint32>();
	LevelTileSize.Y = JsonFile["tileheight"].get<uint32>();

	for (uint32 TileSetIndex = 0; TileSetIndex <  JsonFile["tilesets"].size(); ++TileSetIndex)
	{
		Tileset NewSet;

		string ImagePath = JsonFile["tilesets"][TileSetIndex]["image"].get<string>();
		string ImageName = JsonFile["tilesets"][TileSetIndex]["name"].get<string>();

		uint32 FirstGID = JsonFile["tilesets"][TileSetIndex]["firstgid"].get<uint32>();
		uint32 TileCount = JsonFile["tilesets"][TileSetIndex]["tilecount"].get<uint32>();

		uint32 TileWidth = JsonFile["tilesets"][TileSetIndex]["tilewidth"].get<uint32>();;
		uint32 TileHeight = JsonFile["tilesets"][TileSetIndex]["tileheight"].get<uint32>();;

		uint32 TileColumns = JsonFile["tilesets"][TileSetIndex]["columns"].get<uint32>();;

		uint32 TextureWidth = JsonFile["tilesets"][TileSetIndex]["imagewidth"].get<uint32>();;
		uint32 TextureHeight = JsonFile["tilesets"][TileSetIndex]["imageheight"].get<uint32>();;

		string DataPath = "../Data/";

		ImagePath = ImagePath.substr(ImagePath.find_first_of('/'));
		ImagePath = DataPath + ImagePath;

		RenderManager::CurrentRenderer->SaveImage(ImageName, ImagePath);

		SDL_Texture* Texture = SDL_CreateTextureFromSurface(RenderManager::CurrentRenderer->SDLRenderer,
			RenderManager::CurrentRenderer->LoadImageFile(ImagePath));

		NewSet.SetTileSet(Texture, FirstGID, TileCount, TileWidth, TileHeight, TileColumns, TextureWidth, TextureHeight);

		// Animation data
		for (uint32 AnimationIndex = 0; AnimationIndex < JsonFile["tilesets"][TileSetIndex]["tiles"].size(); ++AnimationIndex)
		{
			// JsonFile["tilesets"][TileSetIndex]["tileproperties"][AnimationIndex]["AnimationName"].get<string>();

			for (auto Animation : JsonFile["tilesets"][TileSetIndex]["tiles"])
			{
				uint32 FirstID = Animation["animation"][0]["tileid"].get<uint32>();

				for (auto AnimDataIterator : Animation["animation"])
				{
					uint32 Duration = AnimDataIterator["duration"].get<uint32>();
					uint32 TileID = AnimDataIterator["tileid"].get<uint32>();

					NewSet.AddAnimation(FirstID, Duration, TileID);
				}

				// AnimData.AnimationDurations[AnimationName] = ;
			}


			//  ["18"]["animation"].get<string>();

		}		

		TileSets.push_back(NewSet);
	}

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
					if ((int32)GID  >= TileSets[TileSetIndex].FirstGID)
					{
						if ((int32)GID < TileSets[TileSetIndex].TileCount)
						{
							Set = TileSets.at(TileSetIndex);
							break;
						}
					}
				}

				uint32 XX = 0;
				uint32 YY = 0;

				XX = TileIndex % (int32)LevelSize.X;
				XX *= Set.TileWidth;
				YY += Set.TileHeight * (TileIndex / (int32)LevelSize.X);

				Vector2 FinalTilePosition = Vector2(XX, YY);

				uint32 TSXX = GID % (Set.TextureWidth / (int32)Set.TileWidth) - 1;
				TSXX *= Set.TileWidth;

				uint32 TSYY = 0;
				uint32 Amount = ((GID - Set.FirstGID) / (Set.TextureWidth / Set.TileWidth));

				TSYY = Set.TileHeight * Amount;

				Vector2 FinalTileSetPosition = Vector2(TSXX, TSYY);

				Vector2 TileSetSize = Vector2(Set.TileWidth, Set.TileHeight);

				Tile* BuildTile = nullptr;
				for (auto AnimGID_ID : Set.Animations)
				{
					if (AnimGID_ID.first == GID - Set.FirstGID)
					{
						BuildTile = new AnimatedTile(Set.Texture, FinalTilePosition, FinalTileSetPosition, TileSetSize, Set.Animations[GID - Set.FirstGID]);
					}
				}
				
				
				if (!BuildTile)
				{
					BuildTile = new Tile(Set.Texture, FinalTilePosition, FinalTileSetPosition, TileSetSize);
				}

				

				if (LayerList[LayerIndex]["name"].get<string>() == "Background")
				{
					BuildTile->Layer = 0;
				}
				else
				{
					BuildTile->Layer = 1;
				}

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