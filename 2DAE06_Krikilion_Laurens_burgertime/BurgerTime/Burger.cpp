#include "BigginPCH.h"
#include "Burger.h"
#include "BoxCollider2d.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Logger.h"
#include "MapLoader.h"
#include "PeterPepper.h"
#include "RenderComponent.h"
#include "ScoreComponent.h"
#include "SoundServiceLocator.h"
#include "SpriteRenderComponent.h"
#include "Subject.h"

Burger::Burger(biggin::GameObject* go, BurgerIngredients ingredient, const std::vector<Observer*>& observers)
	: Component(go)
	, m_pNotifier(go->GetComponent<biggin::Subject>())
	, m_Ingredient(ingredient)
	, m_GameTimeRef{biggin::GameTime::GetInstance() }
{
	m_FallDelayed.interval = biggin::GameTime::GetFixedTimeStep();
	//According to Stephan T. Lavavej - "Avoid using bind(), ..., use lambdas"
	//https://www.youtube.com/watch?v=zt7ThwVfap0&t=32m20s
	m_FallDelayed.func = [this] {DropBurger(); };
	//m_FallDelayed.func = std::bind(&Burger::DropBurger, this);
	m_FallDelayed.finished = true;

	if (m_pNotifier == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing Subject Component");

	for (const auto observer : observers)
		m_pNotifier->AddObserver(observer);
}

Burger::~Burger()
{
	for (auto child : m_pChilderen)
	{
		child->RemoveObserver(this);
	}
}

void Burger::Initialize(biggin::GameObject* )
{

}

void Burger::Start()
{

}

//burger needs to check how many enemies are on top of it
void Burger::OnNotify(Component* entity, const std::string& event)
{
	if (m_ReachedBottom) return;

	if (event == "BeginContact")
	{
		const auto overlappingCollider = static_cast<const biggin::BoxCollider2d*>(entity);
		const auto tag = overlappingCollider->GetTag();
		if (tag == "Platform")
		{
			if (!m_IsFalling) return;

			SoundServiceLocator::GetSoundSystem().Play("burger_touching_floor.wav", 0.5f);

			if (m_EnemiesOnBurger > 0)
			{
				--m_EnemiesOnBurger;
				m_FallDelayed.Reset();
			}

			std::ranges::fill(m_Touched, false);

			m_AmntTouchedParts = 0;
			m_IsFalling = false;
		}
		else if(tag == "Player")
		{
			if (m_IsFalling) return;

			//the gameobject of the collider of one of our children and just hit the player
			const auto burgerPartGo = overlappingCollider->GetOther()->GetOwningGameObject();

			const auto it = std::find(m_pChilderen.begin(), m_pChilderen.end(), burgerPartGo);
			if (it != m_pChilderen.end())
			{
				const __int64 index = std::distance(m_pChilderen.begin(), it);
				if (m_Touched[index]) return;

				m_Touched[index] = true;
				(*it)->TranslateLocalPosition({ 0, static_cast<int>(burgerTime::MapLoader::GetGridSize() / 2.f) });
				SoundServiceLocator::GetSoundSystem().Play("stepping_on_burger.wav", 0.2f);
			}
			else
			{
				Logger::GetInstance().LogDebug("No matching child found");
			}

			++m_AmntTouchedParts;
			if (m_AmntTouchedParts == m_BurgerSize)
			{
				m_pPlayerGo = overlappingCollider->GetOwningGameObject();
				int score{ burgerTime::ScoreValues::burgerDropped };
				if (m_EnemiesOnBurger > 0)
					score = burgerTime::ScoreValues::burgerDroppedWithEnemy * static_cast<int>(pow(2, (m_EnemiesOnBurger - 1)));

				DropBurger(score);
			}
		}
		else if (tag == "Burger")
		{
			if(m_IsFalling)
				return;

			if (const auto* otherBurgerGo = overlappingCollider->GetOwningGameObject()->GetParent())
			{
				if (const auto* otherBurger = otherBurgerGo->GetComponent<Burger>())
					m_pPlayerGo = otherBurger->m_pPlayerGo;
			}

			DropBurger();
		}
		else if (tag == "Catcher")
		{
			SoundServiceLocator::GetSoundSystem().Play("burger_touching_floor.wav", 0.2f);

			//set the tag of our burger to catcher so other burgers land on top of it
			overlappingCollider->GetOther()->SetTag("Catcher");
			m_ReachedBottom = true;
			m_pNotifier->notify(this, "BurgerReachedEnd");
		}
	}
}

void Burger::DropBurger(int score)
{
	SoundServiceLocator::GetSoundSystem().Play("burger_going_down.wav", 0.2f);

	if (m_pPlayerGo != nullptr)
		if (auto* scoreComponent = m_pPlayerGo->GetComponent<burgerTime::ScoreComponent>())
			scoreComponent->IncreaseScore(score);

	m_IsFalling = true;
	m_pNotifier->notify(this, "BurgerFalling");

	if (m_AmntTouchedParts == m_BurgerSize)
		return;

	for (size_t i{ 0 }; i < m_BurgerSize; ++i)
	{
		if (m_Touched[i]) continue;;

		m_Touched[i] = true;
		m_pChilderen[i]->TranslateLocalPosition({ 0, static_cast<int>(burgerTime::MapLoader::GetGridSize() / 2.f) });
	}
}

void Burger::FixedUpdate()
{
	//as our we want to be sure our children are already initialized we initialize later
	//and as box 2d gets updated in fixed update we initialize in fixed update to prevent overlaps when things haven't been positioned yet
	if (!m_Initialized)
		InitializeBurger();

	if (m_ReachedBottom) return;

	if (m_IsFalling)
	{
		auto deltaVelo = glm::vec2{ 0, m_Velocity } * biggin::GameTime::GetFixedTimeStep();
		GetGameObject()->TranslateLocalPosition(deltaVelo);
	}
}

void Burger::Update()
{
	m_FallDelayed.Update(m_GameTimeRef.GetDeltaT());
}

void Burger::InitializeBurger()
{
	m_pChilderen = GetGameObject()->GetAllChilderen();

	if (m_pChilderen.size() != m_BurgerSize)
	{
		Logger::GetInstance().LogWarning("Expected " + std::to_string(m_BurgerSize) + "burger but got " + std::to_string(m_pChilderen.size()));
	}

	switch (m_Ingredient)
	{
	case BurgerIngredients::BurgerTop:
		InitRenderComp(6);
		break;
	case BurgerIngredients::Lettuce:
		InitRenderComp(11);
		break;
	case BurgerIngredients::Meat:
		InitRenderComp(9);
		break;
	case BurgerIngredients::BurgerBottom:
		InitRenderComp(7);
		break;
	case BurgerIngredients::Cheese:
		InitRenderComp(8);
		break;
	case BurgerIngredients::Tomato:
		InitRenderComp(10);
		break;
	default:;
	}

	m_Initialized = true;
}

void Burger::InitRenderComp(int collumnIdx) const
{
	constexpr auto tileSize = burgerTime::MapLoader::GetGridSize();
	for (int i{ 0 }; i < static_cast<int>(m_pChilderen.size()); ++i)
	{
		biggin::RenderComponent* renderComponent = m_pChilderen[i]->GetComponent<biggin::RenderComponent>();
		renderComponent->SetTexture("BurgerTimeSpriteSheet.png");
		renderComponent->SetSourceRect({ (14 + i) * static_cast<int>(tileSize), collumnIdx * static_cast<int>(tileSize), static_cast<int>(tileSize),static_cast<int>(tileSize) });

		glm::vec2 localPos = glm::vec2{ tileSize * i, -tileSize };
		m_pChilderen[i]->SetLocalPosition(localPos);
	}
}