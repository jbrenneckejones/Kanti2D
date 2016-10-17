#ifndef KANTI2D_GAMEOBJECT

enum class DIRECTION
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

const real32 GravityRate = 0.002f;
const real32 GravityMax = 0.8f;
const real32 Friction = 0.01f;

class GameObject
{
public:

	Vector2 Position;
	Vector2 Velocity = Vector2(0, 0);
	Vector2 MaxVelocity = Vector2(10, 10);
	Vector2 Acceleration = Vector2(0, 0);
	real32 Mass = 1.0f;

	inline GameObject()
	{

	}

	inline GameObject(Graphics& RenderGraphics, Vector2 ObjectPosition)
	{
		Position = ObjectPosition;
		CurrentDirection = DIRECTION::RIGHT;
		ObjectSprite = new AnimatedSprite(RenderGraphics, "../Data/Images/MyChar.bmp", SDL_Rect{ 0, 0, 32, 32 }, ObjectPosition, 300);

		AnimatedSprite* AnimSprite = (AnimatedSprite *)ObjectSprite;

		if (AnimSprite)
		{

			AnimSprite->AddAnimation(1, 0, 0, "IdleLeft", 32, 32, Vector2(0, 0));
			AnimSprite->AddAnimation(1, 0, 32, "IdleRight", 32, 32, Vector2(0, 0));

			AnimSprite->AddAnimation(3, 0, 0, "RunLeft", 32, 32, Vector2(0, 0));
			AnimSprite->AddAnimation(3, 0, 32, "RunRight", 32, 32, Vector2(0, 0));

			AnimSprite->PlayAnimation("RunLeft");
		}
	}

	inline void Draw(Graphics& RenderGraphics)
	{
		ObjectSprite->Draw(RenderGraphics, (uint32)Position.X, (uint32)Position.Y);
	}

	inline void AddFriction()
	{
		if (Velocity.X > 0.0f)
		{
			Velocity.X -= Friction;
		}
		else if (Velocity.X < -0.0f)
		{
			Velocity.X += Friction;
		}

		if (Velocity.X < 0.01f && Velocity.X > -0.01f)
		{
			Velocity.X = 0.0f;
		}
	}

	inline void Update(uint32 ElapsedTime)
	{
		if (Velocity.Y <= GravityMax)
		{
			Velocity.Y += GravityRate * (real32)Time::DeltaTime;
		}

		Position.X += Velocity.X * (real32)Time::DeltaTime;

		Position.Y += Velocity.Y * (real32)Time::DeltaTime;

		/*
		
			if (Velocity.X != 0.0f && Velocity.Y != 0.0f)
		{
			AddFriction();
		}

		Velocity += Acceleration * (real32)Time::DeltaTime;

		int32 Factorial = Velocity.X > 0 ? 1 : -1;
		Velocity.X = Min(AbsoluteValue(Velocity.X), MaxVelocity.X) * Factorial;

		Position += Velocity;
		// Velocity.X = 0;

		// Position += (real32)Time::DeltaTime * (Velocity * Acceleration * 0.5f);
		// Velocity += Acceleration - (Friction * Velocity);

		*/
		
		ObjectSprite->Update(Position, ElapsedTime);
	}

	inline void Move(DIRECTION FacingDirection)
	{
		AnimatedSprite* AnimSprite = (AnimatedSprite *)ObjectSprite;
		CurrentDirection = FacingDirection;

		switch (FacingDirection)
		{
		case DIRECTION::LEFT:
			Velocity.X = -Speed;
			AnimSprite->PlayAnimation("RunLeft");
			break;
		case DIRECTION::RIGHT:
			Velocity.X = Speed;
			AnimSprite->PlayAnimation("RunRight");
			break;

		default:
			break;
		}
	}

	inline void StopMove()
	{
		AnimatedSprite* AnimSprite = (AnimatedSprite *)ObjectSprite;
		Velocity.X = 0.0f;
		switch (CurrentDirection)
		{
		case DIRECTION::LEFT:
			AnimSprite->PlayAnimation("IdleLeft");
			break;
		case DIRECTION::RIGHT:
			AnimSprite->PlayAnimation("IdleRight");
			break;

		default:
			break;
		}
	}

	inline void HandleTileCollisions(std::vector<Rectangle> Collisions)
	{
		for (uint32 Index = 0; Index < Collisions.size(); ++Index)
		{
			Rectangle CurrentCollision = Collisions[Index];

			SIDE CollisionSide = ObjectSprite->GetCollisionSide(CurrentCollision);

			if (CollisionSide != SIDE::NONE)
			{
				switch (CollisionSide)
				{
				case SIDE::TOP:
					Position.Y = (real32)CurrentCollision.GetBottom() + 1.0f;
					Velocity.Y = 0.0f;
					break;

				case SIDE::BOTTOM:
					Position.Y = (real32)CurrentCollision.GetTop() - (real32)ObjectSprite->BoundingBox.Height - 1.0f;
					Velocity.Y = 0.0f;
					IsGrounded = true;
					break;

				case SIDE::LEFT:
					Position.X = (real32)CurrentCollision.GetRight() + 1.0f;
					Velocity.X = 0.0f;
					break;

				case SIDE::RIGHT:
					Position.X = (real32)CurrentCollision.GetLeft() - (real32)ObjectSprite->BoundingBox.Width - 1.0f;
					Velocity.X = 0.0f;
					break;

				default:
					break;
				}
			}
		}
	}

	Sprite* ObjectSprite;
	DIRECTION CurrentDirection;

	real32 Speed = 0.2f;

	bool32 IsGrounded = 0;
};

#define KANTI2D_GAMEOBJECT

#endif

