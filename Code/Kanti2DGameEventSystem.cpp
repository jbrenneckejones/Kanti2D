#include "Kanti2DGameEventSystem.h"

void CollisionCell::Update()
{
	if (CollisionPairs.size() > 0)
	{
		int32 CollisionIndex = CollisionPairs.size() - 1;
		while (CollisionIndex >= 0)
		{
			CollisionPairs[CollisionIndex].Update();

			if (!CollisionPairs[CollisionIndex].InfoCaller.Caller->IsTouching(CollisionPairs[CollisionIndex].InfoCaller.Collider))
			{
				CollisionPairs[CollisionIndex].CallCollisionEnd();

				auto FoundPair = std::find(CollisionPairs.begin(), CollisionPairs.end(), CollisionPairs[CollisionIndex]);

				if (FoundPair != CollisionPairs.end())
				{
					CollisionPairs.erase(FoundPair);
				}

				// RemovedPairs.push_back(CollisionPairs[CollisionIndex]);
			}
			else
			{
				CollisionPairs[CollisionIndex].CallCollisionStay();
			}

			CollisionIndex--;
		}
	}

	if (Colliders.size() > 0)
	{
		int32 ColliderIndex = Colliders.size() - 1;
		while (ColliderIndex >= 0)
		{
			if (Bounds.Intersects(Colliders[ColliderIndex]->GetBounds()) == FALSE)
			{
				// TODO(Julian): This doesn't properly pop out the element we want to
				// Need to do a swap to the last element and then pop it
				// So check if it is the last element and if not then swap
				Colliders[ColliderIndex]->CellIndex = nullptr;

				auto Found = std::find(Colliders.begin(), Colliders.end(), Colliders[ColliderIndex]);

				if (Found != Colliders.end())
				{
					Colliders.erase(Found);
				}
			}
			else
			{
				for (auto& PotentialCollider : Colliders)
				{
					if (PotentialCollider != Colliders[ColliderIndex])
					{
						if (Colliders[ColliderIndex]->IsTouching(PotentialCollider))
						{
							CollisionPair Pair(Colliders[ColliderIndex], PotentialCollider);

							if (CollisionPairs.size() > 0)
							{
								auto Found = std::find(CollisionPairs.begin(), CollisionPairs.end(), Pair);
								if (Found == CollisionPairs.end())
								{
									Pair.CallCollisionStart();
									CollisionPairs.push_back(Pair);
								}
							}
							else
							{
								Pair.CallCollisionStart();
								CollisionPairs.push_back(Pair);
							}
						}
					}
				}
			}

			ColliderIndex--;
		}
	}
}

CollisionPair::CollisionPair(class Collider* A, class Collider* B)
{
	Collision::GetCollisionInfo(A, B, InfoCaller);
}

void CollisionPair::Update()
{
	InfoCaller = Collision(InfoCaller.Caller, InfoCaller.Collider);
}

void CollisionPair::CallCollisionStart()
{
	InfoCaller.Caller->OnCollisionStart(InfoCaller);
}

void CollisionPair::CallCollisionStay()
{
	InfoCaller.Caller->OnCollisionStay(InfoCaller);
}

void CollisionPair::CallCollisionEnd()
{
	InfoCaller.Caller->OnCollisionEnd(InfoCaller);
}
