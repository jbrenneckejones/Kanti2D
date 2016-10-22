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

	inline void GameLoop()
	{
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

		GameEventSystem::EventSystem->CreateCollisionCells(KRectangle(0, 0, 640, 480));

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
