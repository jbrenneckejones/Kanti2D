#ifndef KANTI2D_INPUT

class Input 
{
public:

	inline void BeginFrame()
	{
		PressedKeys.clear();
		ReleasedKeys.clear();
	}

	void KeyUpEvent(const SDL_Event& Event)
	{
		ReleasedKeys[Event.key.keysym.scancode] = true;
		HeldKeys[Event.key.keysym.scancode] = false;
	}

	void KeyDownEvent(const SDL_Event& Event)
	{
		PressedKeys[Event.key.keysym.scancode] = true;
		HeldKeys[Event.key.keysym.scancode] = true;
	}

	bool32 WasKeyPressed(SDL_Scancode Key)
	{
		return PressedKeys[Key];
	}

	bool32 WasKeyReleased(SDL_Scancode Key)
	{
		return ReleasedKeys[Key];
	}

	bool32 IsKeyHeld(SDL_Scancode Key)
	{
		return HeldKeys[Key];
	}

	std::map<SDL_Scancode, bool32> HeldKeys;
	std::map<SDL_Scancode, bool32> PressedKeys;
	std::map<SDL_Scancode, bool32> ReleasedKeys;
};

#define KANTI2D_INPUT

#endif
