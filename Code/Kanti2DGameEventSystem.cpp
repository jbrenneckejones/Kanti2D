#include "Kanti2DGameEventSystem.h"

void CollisionCell::Update()
{
	if (CollisionPairs.size() > 0)
	{
		int32 CollisionIndex = CollisionPairs.size() - 1;
		while (CollisionIndex >= 0)
		{
			if (!CollisionPairs[CollisionIndex].Caller || !CollisionPairs[CollisionIndex].Collider)
			{
				CollisionPairs.pop_back();
				continue;
			}

			if (!CollisionPairs[CollisionIndex].Caller->IsColliding(CollisionPairs[CollisionIndex].Collider) ||
				!CollisionPairs[CollisionIndex].Collider->IsColliding(CollisionPairs[CollisionIndex].Caller))
			{
				CollisionPairs[CollisionIndex].CallCollisionEnd();
				CollisionPairs.pop_back();
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
			if (Bounds.DoesOverlap(Colliders[ColliderIndex]->GetBoundsCombined()) == FALSE)
			{
				Colliders[ColliderIndex]->CellIndex = "NULL";
				Colliders.pop_back();
			}
			else
			{
				for (auto& PotentialCollider : Colliders)
				{
					if (PotentialCollider != Colliders[ColliderIndex])
					{
						if (Colliders[ColliderIndex]->IsColliding(PotentialCollider))
						{
							CollisionPair Pair = { (Colliders[ColliderIndex]), (PotentialCollider) };


							if (CollisionPairs.size() > 0)
							{
								if (std::find(CollisionPairs.begin(), CollisionPairs.end(), Pair) == CollisionPairs.end())
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

CollisionPair::CollisionPair(class Collider& A, class Collider& B)
{
	Collision InfoA = A.GetCollisionInfo(&B);
	Collision InfoB = B.GetCollisionInfo(&A);

	CollisionPair(InfoA, InfoB);
}

CollisionPair::CollisionPair(class Collider* A, class Collider* B)
{
	InfoCaller = new Collision( A->GetCollisionInfo(B));
	InfoCollider = new Collision(B->GetCollisionInfo(A));

	Caller = A;
	Collider = B;
}

CollisionPair::CollisionPair(Collision& A, Collision& B)
{
	Caller = B.Hit;
	InfoCaller = &A;

	Collider = A.Hit;
	InfoCaller = &B;
}

CollisionPair::CollisionPair(Collision* A, Collision* B)
{
	Caller = B->Hit;
	InfoCaller = A;

	Collider = A->Hit;
	InfoCaller = B;
}

void CollisionPair::CallCollisionStart()
{
	if (Caller)
	{
		Caller->OnCollisionStart(*InfoCaller);
	}

	if (Collider)
	{
		Collider->OnCollisionStart(*InfoCollider);
	}
}

void CollisionPair::CallCollisionStay()
{
	if (Caller)
	{
		Caller->OnCollisionStay(*InfoCaller);
	}

	if (Collider)
	{
		Collider->OnCollisionStay(*InfoCollider);
	}
}

void CollisionPair::CallCollisionEnd()
{
	if (Caller)
	{
		Caller->OnCollisionEnd(*InfoCaller);
	}

	if (Collider)
	{
		Collider->OnCollisionEnd(*InfoCollider);
	}
}
