
#define SDL_MAIN_HANDLED
#include "External/SDL2/include/SDL.h"
#include "External/SDL2/include/SDL_mixer.h"
#include "External/SDL2/include/SDL_image.h"

#include <string>
typedef std::string string;
#include <vector>
#include <queue>

// multi threading


#include <unordered_set>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

// end multi

#include "Kanti2DPlatform.h"
#include "Kanti2DMath.h"
#include "Kanti2DTimer.h"
#include <map>
#include "Kanti2DInput.h"

#include "Kanti2DRectangle.h"
#include "Kanti2DRectangle.cpp"
#include "Kanti2DAudio.h"

#include "External/json.hpp"
#include "Kanti2DGameObject.h"

#include "Kanti2DRenderManager.h"
#include "Kanti2DGameEventSystem.h"

#include "Kanti2DThreadManager.h"

#include "Kanti2DGameObject.cpp"
#include "Kanti2DGameEventSystem.cpp"

#include "Kanti2DGame.h"

#include <Objbase.h>

k_internal inline UniqueID Win32GetUUID()
{
	UUID UID;
	CoCreateGuid(&UID);

	UniqueID Result;
	Result.Data1 = UID.Data1;
	Result.Data2 = UID.Data2;
	Result.Data3 = UID.Data3;

	for (uint32 Index = 0; Index < sizeof(Result.Data4); ++Index)
	{
		Result.Data4[Index] = UID.Data4[Index];
	}
	
	return (Result);
}

int main(int argc, const char* argv[])
{
	GetUUID = Win32GetUUID;

	Game KantiGame;

	return 0;
}
