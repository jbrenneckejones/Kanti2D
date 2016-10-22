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

Collision::Collision(Collider* Caller, Collider* HitCollider)
{
	Collision Info = Caller->GetCollisionInfo(HitCollider);

	Hit = Info.Hit;
	OverlapAmount = Info.OverlapAmount;
	SideOverlapped = Info.SideOverlapped;
}
