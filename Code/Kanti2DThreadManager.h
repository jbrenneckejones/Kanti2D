#ifndef KANTI2D_THREADMANAGER

k_internal inline void UpdateThreadFunction(void** PointerReference);

k_internal inline void FixedUpdateThreadFunction(void** PointerReference);

k_internal inline void PhysicsUpdateThreadFunction(void** PointerReference);

k_internal inline void CollisionUpdateThreadFunction(void** PointerReference);

k_internal inline void DrawThreadFunction(void** PointerReference);

k_internal inline void UpdateCollisionCellsFunction(void** PointerReference);

enum class TASK_STATUS : int32
{
	NOTSTARTED = -1,
	FINISHED = 0,
	RUNNING = 1
};

struct Task
{
	string Name;
	struct Worker* WorkerAssigned;
	std::function<void(void**)> TaskToComplete = nullptr;
	void** PassedData;

	bool32 Finished = FALSE;

	Task()
	{
		Name = "NULL";
	}

	Task(string TaskName)
	{
		Name = TaskName;
	}
};

struct Worker
{
	string WorkerName;
	// SDL_Thread* WorkerThread;
	std::thread WorkerThread;
	Task* AssignedTask = nullptr;
	bool32 IsBusy = FALSE;

	TASK_STATUS Status = TASK_STATUS::NOTSTARTED;

	inline Worker()
	{
		IsBusy = FALSE;
	}

	inline Worker(const Worker& Other)
	{
	}

	inline void Assign(Task* CurrentTask)
	{
		AssignedTask = CurrentTask;
		AssignedTask->WorkerAssigned = this;
		AssignedTask->Finished = FALSE;
	}

	inline void Update()
	{
		if (!AssignedTask)
		{
			Status = TASK_STATUS::NOTSTARTED;
			IsBusy = FALSE;
			return;
		}

		if (Status == TASK_STATUS::FINISHED)
		{
			IsBusy = FALSE;
			Status = TASK_STATUS::NOTSTARTED;
			AssignedTask->Finished = TRUE;
			AssignedTask = nullptr;
			return;
		}

		Status = TASK_STATUS::RUNNING;

		IsBusy = true;

		// WorkerThread = SDL_CreateThread(AssignedTask.TaskToComplete, WorkerName.c_str(), AssignedTask.PassedData);
		// SDL_WaitThread(WorkerThread, (int32 *)&Status);

		if (!WorkerThread.joinable())
		{
			WorkerThread = std::thread(AssignedTask->TaskToComplete, AssignedTask->PassedData);
		}
	}
};

class ThreadManager
{
public:

	// Variables

	k_internal ThreadManager* Instance;

	// need to keep track of threads so we can join them
	std::vector<Worker> Workers;
	// the task queue
	std::deque<Task*> Tasks;

	// SDL
	// synchronization
	// SDL_mutex* QueueMutex;
	// SDL_cond* Condition;
	// SDL_bool Stop;

	// synchronization
	std::mutex QueueMutex;
	std::condition_variable Condition;
	bool32 Stop;

public:

	// Functions

	ThreadManager()
	{
		if (!Instance)
		{
			Instance = this;

			Stop = SDL_FALSE;

			int32 CPUS = SDL_GetCPUCount();

			for (int32 Index = 0; Index < CPUS; ++Index)
			{
				Worker CPUWorker = Worker();
				CPUWorker.WorkerName = "CPUThread";
				CPUWorker.WorkerName.push_back((uint32)Index - '0');

				Workers.push_back(CPUWorker);
			}
		}
	}

	void AddTask(Task* Assignment)
	{
		/*
		std::function<void()> task;

		{
			std::unique_lock<std::mutex> lock(this->queue_mutex);
			this->condition.wait(lock,
				[this] { return this->stop || !this->tasks.empty(); });
			if (this->stop && this->tasks.empty())
				return;
			task = std::move(this->tasks.front());
			this->tasks.pop();
		}

		task();
		*/

		/*

			using return_type = typename std::result_of<F(Args...)>::type;

	auto task = std::make_shared< std::packaged_task<return_type()> >(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

	std::future<return_type> res = task->get_future();
	{
		std::unique_lock<std::mutex> lock(queue_mutex);

		// don't allow enqueueing after stopping the pool
		if (stop)
			throw std::runtime_error("enqueue on stopped ThreadPool");

		tasks.emplace([task]() { (*task)(); });
	}
	condition.notify_one();
	return res;
		*/

		if (Assignment->Finished)
		{
			Assignment->Finished = FALSE;
		}

		for (int Index = 0; Index < Tasks.size(); ++Index)
		{
			if (Tasks[Index]->Name == Assignment->Name)
			{
				return;
			}
		}

		Tasks.push_back(Assignment);
	}

	bool32 GetFirstWorker(Worker** FoundWorker)
	{
		for (auto& Work : Workers)
		{
			if (Work.IsBusy == FALSE && Work.Status == TASK_STATUS::NOTSTARTED)
			{
				if (!Work.AssignedTask)
				{
					*FoundWorker = &Work;
					return TRUE;
				}
			}
		}

		return FALSE;
	}

	bool32 TryAssignTask(Task* Assignment)
	{
		Worker* FoundWorker = 0;
		if (GetFirstWorker(&FoundWorker))
		{
			FoundWorker->Assign(Assignment);
			return TRUE;
		}

		return FALSE;
	}

	void UpdateWorkers()
	{
		for (auto& Work : Workers)
		{
			Work.Update();

			if (Work.WorkerThread.joinable())
			{
				Work.WorkerThread.join();
				Work.Status = TASK_STATUS::FINISHED;
				Work.AssignedTask->Finished = TRUE;
			}
		}
	}

	void UpdateThreads()
	{
		while (Tasks.size() != 0)
		{
			Task* FrontTask = Tasks.front();
			if (TryAssignTask(FrontTask))
			{
				Tasks.pop_front();
			}
			else
			{
				break;
			}
		}
	}

	void Update()
	{
		UpdateThreads();
		UpdateWorkers();
	}
};

ThreadManager* ThreadManager::Instance = nullptr;

class GameUpdateManager
{
public:

	// Variables

	k_internal GameUpdateManager* Instance;

	Task GameUpdateTask;
	Task GameFixedUpdateTask;
	Task GamePhysicsUpdateTask;
	Task GameCollisionUpdateTask;
	Task GameDrawUpdateTask;

public:

	GameUpdateManager()
	{
		if (!Instance)
		{
			Instance = this;
			SetupGameUpdate();
		}
	}

	void SetupGameUpdate()
	{
		// Setup to run the first time
		GameUpdateTask.Finished = TRUE;
		GameFixedUpdateTask.Finished = TRUE;
		GamePhysicsUpdateTask.Finished = TRUE;
		GameCollisionUpdateTask.Finished = TRUE;
		GameDrawUpdateTask.Finished = TRUE;
	}

	void OnGameUpdate()
	{
		// Update
		if (GameUpdateTask.Finished)
		{
			GameUpdateTask.TaskToComplete = UpdateThreadFunction;
			GameUpdateTask.PassedData = (void **)&GameEventSystem::EventSystem;
			GameUpdateTask.Finished = FALSE;
			GameUpdateTask.Name = "Update";
			ThreadManager::Instance->AddTask(&GameUpdateTask);
		}

		local_persist real64 FixedCount = 0.0;
		bool32 CanFixedUpdate = FALSE;
		FixedCount += Time::DeltaTime;

		if (FixedCount >= Time::FixedUpdate)
		{
			FixedCount = 0.0;
			CanFixedUpdate = TRUE;
		}

		// Fixed Update
		if (GameFixedUpdateTask.Finished && CanFixedUpdate)
		{
			GameFixedUpdateTask.TaskToComplete = FixedUpdateThreadFunction;
			GameFixedUpdateTask.PassedData = (void **)&GameEventSystem::EventSystem;
			GameFixedUpdateTask.Finished = FALSE;
			GameFixedUpdateTask.Name = "FixedUpdate";
			ThreadManager::Instance->AddTask(&GameFixedUpdateTask);
		}

		// Physics
		if (GamePhysicsUpdateTask.Finished && CanFixedUpdate)
		{
			GamePhysicsUpdateTask.TaskToComplete = PhysicsUpdateThreadFunction;
			GamePhysicsUpdateTask.PassedData = (void **)&GameEventSystem::EventSystem;
			GamePhysicsUpdateTask.Finished = FALSE;
			GamePhysicsUpdateTask.Name = "Physics";
			ThreadManager::Instance->AddTask(&GamePhysicsUpdateTask);
		}

		// Collision
		if (GameCollisionUpdateTask.Finished && CanFixedUpdate)
		{
			GameCollisionUpdateTask.TaskToComplete = CollisionUpdateThreadFunction;
			GameCollisionUpdateTask.PassedData = (void **)&GameEventSystem::EventSystem;
			GameCollisionUpdateTask.Finished = FALSE;
			GameCollisionUpdateTask.Name = "Collision";
			ThreadManager::Instance->AddTask(&GameCollisionUpdateTask);
		}

		// Draw
		if (GameDrawUpdateTask.Finished && CanFixedUpdate)
		{
			GameDrawUpdateTask.TaskToComplete = DrawThreadFunction;
			GameDrawUpdateTask.PassedData = (void **)&GameEventSystem::EventSystem;
			GameDrawUpdateTask.Finished = FALSE;
			GameDrawUpdateTask.Name = "Draw";
			ThreadManager::Instance->AddTask(&GameDrawUpdateTask);
		}
	}
};

GameUpdateManager* GameUpdateManager::Instance = nullptr;

k_internal inline void UpdateThreadFunction(void** PointerReference)
{
	GameEventSystem** EventSystem = (GameEventSystem **)PointerReference;

	for (auto const &Iterator : (*EventSystem)->UpdateCalls)
	{
		if (Iterator != nullptr)
		{
			Iterator->Update();
		}
	}

	return;
}

k_internal inline void FixedUpdateThreadFunction(void** PointerReference)
{
	GameEventSystem** EventSystem = (GameEventSystem **)PointerReference;

	// Fixed update calls

	for (auto const &Iterator : (*EventSystem)->FixedUpdateCalls)
	{
		if (Iterator != nullptr)
		{
			Iterator->FixedUpdate();
		}
	}

	return;
}

k_internal inline void PhysicsUpdateThreadFunction(void** PointerReference)
{
	GameEventSystem** EventSystem = (GameEventSystem **)PointerReference;

	// Physics update calls

	for (auto const &Iterator : (*EventSystem)->PhysicsCalls)
	{
		if (Iterator != nullptr)
		{
			Iterator->Update();
		}
	}

	return;
}

k_internal inline void CollisionUpdateThreadFunction(void** PointerReference)
{
	// Collision update calls

	GameEventSystem** EventSystem = (GameEventSystem **)PointerReference;

	for (const auto& Collider : (*EventSystem)->CollisionCalls)
	{
		if (Collider->CellIndex == nullptr)
		{
			for (auto& Cell : (*EventSystem)->CollisionCells)
			{
				if (Cell.second.Bounds.Intersects(Collider->GetBounds()))
				{
					Collider->CellIndex = &Cell.second.ID;

					Cell.second.Colliders.push_back(Collider);
				}
			}
		}
	}

	for (auto& Cell : (*EventSystem)->CollisionCells)
	{
		if (Cell.second.Colliders.size() == 0 && Cell.second.CollisionPairs.size() == 0)
		{
				continue;
		}

		Task CellUpdate;
		CellUpdate.Finished = FALSE;
		CellUpdate.Name = "CellUpdate " + Cell.second.ID.ToString();
		CellUpdate.PassedData = (void **)&Cell.second;
		CellUpdate.TaskToComplete = UpdateCollisionCellsFunction;
		ThreadManager::Instance->AddTask(new Task(CellUpdate));
	}

	/*

	// std::sort((*Pointer)->CollisionCalls.begin(), (*Pointer)->CollisionCalls.end(), SortCollision);

	std::vector<Collider2D*> ActiveList;

	for (auto const& Iterator : (*EventSystem)->CollisionCalls)
	{
		if (ActiveList.size() == 0)
		{
			ActiveList.push_back(Iterator);
		}
		else
		{
			if (Iterator->EntityAttachedTo->Name == "Player")
			{
				__debugbreak();
			}

			for (auto const& Active : ActiveList)
			{
				BoundingBox IterBounds = Iterator->GetBoundsCombined();
				BoundingBox ActiveBounds = Active->GetBoundsCombined();

				if (IterBounds.GetLeft() > ActiveBounds.GetRight())
				{
					ActiveList.erase(std::remove(ActiveList.begin(), ActiveList.end(), Active), ActiveList.end());
				}
				else
				{
					if (IterBounds.CollisionWith(ActiveBounds))
					{
						Collision Info = Active->GetCollisionSide(Iterator);

						// Iterator->OnCollision(Info);

						 Iterator->OnCollision(Info);
					}
				}
			}
		}
	}

	*/

	return;
}

k_internal inline bool32 LayerSort(const Renderer* A, const Renderer* B)
{
	return A->Layer < B->Layer;
}

k_internal inline void DrawThreadFunction(void** PointerReference)
{
	GameEventSystem** EventSystem = (GameEventSystem **)PointerReference;

	SDL_RenderClear(RenderManager::CurrentRenderer->SDLRenderer);

	std::sort((*EventSystem)->DrawCalls.begin(), (*EventSystem)->DrawCalls.end(), LayerSort);

	for (auto const &Renderer : (*EventSystem)->DrawCalls)
	{
		if (Renderer != nullptr)
		{
			Renderer->Update();
			Renderer->Draw();
		}
	}

	SDL_RenderPresent(RenderManager::CurrentRenderer->SDLRenderer);

	return;
}

k_internal inline void UpdateCollisionCellsFunction(void** PointerReference)
{
	CollisionCell* Cell = (CollisionCell *)PointerReference;

	Cell->Update();

	return;
}

#define KANTI2D_THREADMANAGER
#endif