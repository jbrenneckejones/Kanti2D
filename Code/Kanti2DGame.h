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
		Graphics RenderGraphics;
		Input GameInput;
		Audio GameAudio;
		SDL_Event Event;

		Player = GameObject(RenderGraphics, Vector2(280, 252));
		Player.Speed = 0.2f;

		// GameLevel = Level("Map 1", Vector2(100, 100), RenderGraphics);
		GameLevel = Level("../Data/Levels/PrtCave.json", RenderGraphics);

		Time::Initialize();

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
				Player.Move(DIRECTION::LEFT);
			}
			else if (GameInput.IsKeyHeld(SDL_SCANCODE_RIGHT))
			{
				Player.Move(DIRECTION::RIGHT);
			}

			if (!GameInput.IsKeyHeld(SDL_SCANCODE_LEFT) && !GameInput.IsKeyHeld(SDL_SCANCODE_RIGHT))
			{
				Player.StopMove();
			}

			Time::Update();

			Update(Time::UpdateTime);

			Draw(RenderGraphics);
		}
	}

	inline void Draw(Graphics &RenderGraphics)
	{
		RenderGraphics.Clear();

		GameLevel.Draw(RenderGraphics);

		Player.Draw(RenderGraphics);

		RenderGraphics.Draw();
	}

	inline void Update(uint32 ElapsedTime)
	{
		Player.Update(ElapsedTime);
		GameLevel.Update(ElapsedTime);

		std::vector<Rectangle> Collisions = GameLevel.CheckTileCollisions(Player.ObjectSprite->BoundingBox);

		if (Collisions.size() > 0)
		{
			Player.HandleTileCollisions(Collisions);
		}
	}


	GameObject Player;

	Level GameLevel;

};

#define KANTI2D_GAME

#endif
