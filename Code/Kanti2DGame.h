#ifndef KANTI2D_GAME

class Game
{
public:

	inline Game()
	{
		SDL_Init(SDL_INIT_EVERYTHING);
		GameLoop();
	}

	inline ~Game()
	{
	}

	inline void TestCases()
	{

		{
			Segment s = { { 1, 8 },{ 7, 5 } };
			BoundingBox r = { { 5, 4 },{ 3, 2 }, 30 };

			bool32 Result = Collider::oriented_rectangle_segment_collide(r, s);

			Assert(Result == TRUE);
		}

		{
			Segment A = { { 157, 222 }, { 157 + 69, 222 + 35 } };
			BoundingBox B;

			B = BoundingBox::GetPositionSize({ 161.0f, 243.0f }, { 42.0f, 64.0f });
			B.Rotation = 0.0f;

			bool32 ResultA = Collider::oriented_rectangle_segment_collide(B, A);
			Assert(TRUE == ResultA);
		}

		{
			Vector2 a = { 8, 2 };
			Vector2 b = { -2, 8 };
			Vector2 c = { -5, 5 };
			Assert(0.0f == Vector2::DotProduct(a, b));
			Assert(0.0f > Vector2::DotProduct(a, c));
			Assert(0.0f < Vector2::DotProduct(b, c));
		}

		{
			Vector2 a = { 8, 2 };
			Vector2 b = { -2, 8 };
			Assert(90.0f == Vector2::EnclosedAngle(a, b));
			Assert(0.0f == Vector2::DotProduct(a, b));

		}

		{
			KRectangle a = { { 1, 1 },{ 4, 4 } }; 
			KRectangle b = { { 2, 2 },{ 5, 5 } };
			KRectangle c = { { 6, 4 },{ 4, 2 } };
			Assert(TRUE == Collider::rectangles_collide(a, b));
			Assert(TRUE == Collider::rectangles_collide(b, c));
			Assert(FALSE == Collider::rectangles_collide(a, c));
		}

		{
			Vector2 a = { 2, 3 };
			Vector2 b = { 2, 3 };
			Vector2 c = { 3, 4 };
			Assert(TRUE == Collider::points_collide(a, b));
			Assert(FALSE == Collider::points_collide(a, c));
			Assert(FALSE == Collider::points_collide(b, c));
		}

		{
			Vector2 a = { 3, 5 };
			Vector2 b = { 3, 2 };
			Vector2 c = { 8, 4 };
			Vector2 down = { 5, -1 };
			Vector2 up = { 5, 2 };

			Ray l1 = { a, down };

			Ray l2 = { a, up };
			Ray l3 = { b, up };
			Ray l4 = { c, down };

			Assert(TRUE == Ray::RayCollides(l1, l2));
			Assert(TRUE == Ray::RayCollides(l1, l3));
			Assert(FALSE == Ray::RayCollides(l2, l3));
			Assert(TRUE == Ray::RayCollides(l1, l4));

		}

		{
			Vector2 a = { 3, 4 };
			Vector2 b = { 11, 1 };
			Vector2 c = { 8, 4 };
			Vector2 d = { 11, 7 };
			Segment s1 = { a, b };

			Segment s2 = { c, d }; 
			Assert(FALSE == Segment::IsColliding(s1, s2));
		}

		{
			BoundingBox a = { { 3, 5 },{ 1, 3 }, 15 };
			BoundingBox b = { { 10, 5 },{ 2, 2 }, -15 };
			// Assert(FALSE == Collider::(a, b));

		}

		{

		}
	}

	inline void GameLoop()
	{
		TestCases();

		// GameEventSystem::EventSystem = new GameEventSystem();
		RenderManager::CurrentRenderer->CreateWindow(Vector2(640, 480), "CaveStory");
		Input GameInput;
		Audio GameAudio;
		SDL_Event Event;

		Level* GameLevel; 
		Player* Character;

		GameUpdateManager::Instance = new GameUpdateManager();

		// CurrentPlayerScript = Player(Vector2(280, 252));
		GameEntity CurrentPlayer = GameEntity();
		Character = CurrentPlayer.AddComponent<Player>(new Player(&CurrentPlayer, Vector2(280, 252)));

		// GameLevel = Level("Map 1", Vector2(100, 100), RenderGraphics);
		
		GameEntity LevelEntity = GameEntity();
		GameLevel = new Level(&LevelEntity, "../Data/Levels/PrtCave.json");
		LevelEntity.AddComponent<Level>(GameLevel);

		Time::Initialize();

		ThreadManager::Instance = new ThreadManager();

		GameEventSystem::EventSystem->CreateCollisionCells( Vector2(640, 480));

		while (true)
		{
			GameInput.BeginFrame();

			if (SDL_PollEvent(&Event))
			{
				if (Event.type == SDL_KEYDOWN)
				{
					if (Event.key.repeat == 0)
					{
						GameInput.KeyDownEvent(Event);
					}
				}
				else if (Event.type == SDL_KEYUP)
				{
					GameInput.KeyUpEvent(Event);
				}
				else if (Event.type == SDL_QUIT)
				{
					return;
				}
			}

			if (GameInput.WasKeyPressed(SDL_SCANCODE_ESCAPE))
			{
				return;
			}

			if (GameInput.WasKeyPressed(SDL_SCANCODE_1))
			{
				GameAudio.PlayMusic("../Data/Music/sf1_intro_3.mid", false, true);
			}

			if (GameInput.WasKeyPressed(SDL_SCANCODE_SPACE))
			{
				Character->Jump();
			}

			if (GameInput.IsKeyHeld(SDL_SCANCODE_LEFT))
			{
				Character->Move(DIRECTION::LEFT);
			}
			else if (GameInput.IsKeyHeld(SDL_SCANCODE_RIGHT))
			{
				Character->Move(DIRECTION::RIGHT);
			}

			if (!GameInput.IsKeyHeld(SDL_SCANCODE_LEFT) && !GameInput.IsKeyHeld(SDL_SCANCODE_RIGHT))
			{
				Character->StopMove();
			}

			Vector2 Position = Character->EntityAttachedTo->EntTransform.Position;

			// Notify threads to update

			GameUpdateManager::Instance->OnGameUpdate();
			
			ThreadManager::Instance->Update();
			
			uint32 DelayAmount = (uint32)(Time::CurrentFPS / Time::FPS);
			SDL_Delay(DelayAmount);

			Time::Update();
		}
	}

};

#define KANTI2D_GAME

#endif
