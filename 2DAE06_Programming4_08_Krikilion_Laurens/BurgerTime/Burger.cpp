#include "BigginPCH.h"
#include "Burger.h"
#include "BoxCollider2d.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Logger.h"
#include "MapLoader.h"
#include "RenderComponent.h"
#include "SpriteRenderComponent.h"
#include "Subject.h"

Burger::Burger(biggin::GameObject* go, BurgerIngredients ingredient, const std::vector<Observer*>& observers)
	: Component(go)
	, m_pNotifier(go->GetComponent<biggin::Subject>())
	, m_Ingredient(ingredient)
	, m_GameTimeRef{ GameTime::GetInstance() }
{
	m_FallDelayed.interval = GameTime::GetFixedTimeStep();
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
	for (auto child : m_Childeren)
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
		const auto tag = static_cast<const biggin::BoxCollider2d*>(entity)->GetTag();
		if (tag == "Platform")
		{
			Logger::GetInstance().LogDebug("ovelap started with platform and burger");

			if (!m_IsFalling) return;

			if (m_EnemiesOnBurger > 0)
			{
				--m_EnemiesOnBurger;
				m_FallDelayed.Reset();
			}

			std::fill(std::begin(m_Touched), std::end(m_Touched), false);
			//for (size_t i{ 0 }; i < m_BurgerSize; ++i)
			//{
			//	m_Touched[i] = false;
			//}

			m_AmntTouchedParts = 0;
			m_IsFalling = false;
		}
		else if(tag == "Player")
		{
			if (m_IsFalling) return;
			Logger::GetInstance().LogDebug("ovelap started with player and burger");

			//the gameobject of the collider that we are subscribed to and just hit the player
			const auto owner = static_cast<biggin::BoxCollider2d*>(entity)->GetOther()->GetOwningGameObject();

			const auto it = std::find(m_Childeren.begin(), m_Childeren.end(), owner);
			if (it != m_Childeren.end())
			{
				const __int64 index = std::distance(m_Childeren.begin(), it);
				if (m_Touched[index]) return;

				m_Touched[index] = true;
				(*it)->TranslateLocalPosition({ 0, static_cast<int>(burgerTime::MapLoader::GetGridSize() / 2.f) });
			}
			else
			{
				Logger::GetInstance().LogDebug("No matching child found");
			}

			++m_AmntTouchedParts;
			if (m_AmntTouchedParts == m_BurgerSize)
			{
				m_IsFalling = true;
				m_pNotifier->notify(this, "BurgerFalling");
			}
		}
		else if (tag == "Burger")
		{
			DropBurger();
		}
		else if (tag == "Catcher")
		{
			//set the tag of our burger to catcher so other burgers land on top of it
			static_cast<biggin::BoxCollider2d*>(entity)->GetOther()->SetTag("Catcher");
			m_ReachedBottom = true;
			m_pNotifier->notify(this, "BurgerReachedEnd");
		}
		else if (tag == "Enemy")
		{
			if (!m_IsFalling)
				++m_EnemiesOnBurger;
		}
	}
	else if (event == "EndContact")
	{
		const auto tag = static_cast<const biggin::BoxCollider2d*>(entity)->GetTag();
		if (tag == "Enemy")
		{
			if (!m_IsFalling)
				--m_EnemiesOnBurger;
		}
	}
}

void Burger::DropBurger()
{
	m_IsFalling = true;
	m_pNotifier->notify(this, "BurgerFalling");

	for (size_t i{ 0 }; i < m_BurgerSize; ++i)
	{
		if (m_Touched[i]) continue;;

		m_Touched[i] = true;
		m_Childeren[i]->TranslateLocalPosition({ 0, static_cast<int>(burgerTime::MapLoader::GetGridSize() / 2.f) });
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
		auto deltaVelo = glm::vec2{ 0, m_Velocity } * GameTime::GetFixedTimeStep();
		GetGameObject()->TranslateLocalPosition(deltaVelo);
	}
}

void Burger::Update()
{
	m_FallDelayed.Update(m_GameTimeRef.GetDeltaT());
}

void Burger::InitializeBurger()
{
	m_Childeren = GetGameObject()->GetAllChilderen();

	if (m_Childeren.size() != m_BurgerSize)
	{
		Logger::GetInstance().LogWarning("Expected " + std::to_string(m_BurgerSize) + "burger but got " + std::to_string(m_Childeren.size()));
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
	for (int i{ 0 }; i < m_Childeren.size(); ++i)
	{
		biggin::RenderComponent* renderComponent = m_Childeren[i]->GetComponent<biggin::RenderComponent>();
		renderComponent->SetTexture("BurgerTimeSpriteSheet.png");
		renderComponent->SetSourceRect({ (14 + i) * static_cast<int>(tileSize), collumnIdx * static_cast<int>(tileSize), static_cast<int>(tileSize),static_cast<int>(tileSize) });

		glm::vec2 localPos = glm::vec2{ tileSize * i, -tileSize };
		m_Childeren[i]->SetLocalPosition(localPos);
	}
}