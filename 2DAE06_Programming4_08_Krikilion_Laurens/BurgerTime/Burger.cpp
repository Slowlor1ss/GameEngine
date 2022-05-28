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
{
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

			for (size_t i{ 0 }; i < m_BurgerSize; ++i)
			{
				m_Touched[i] = false;
			}

			m_AmntTouchedParts = 0;
			m_IsFalling = false;
		}
		else if(tag == "Player")
		{
			Logger::GetInstance().LogDebug("ovelap started with player and burger");
			
			const auto owner = static_cast<biggin::BoxCollider2d*>(entity)->GetOther()->GetOwningGameObject();

			const auto it = std::find(m_Childeren.begin(), m_Childeren.end(), owner);
			if (it != m_Childeren.end())
			{
				const __int64 index = std::distance(m_Childeren.begin(), it);
				if (m_Touched[index]) return;

				m_Touched[index] = true;
				(*it)->TranslateLocalPosition({ 0, int(MapLoader::GetGridSize() / 2.f) });
			}
			else
			{
				Logger::GetInstance().LogDebug("No matching child found");
			}

			++m_AmntTouchedParts;
			if (m_AmntTouchedParts == m_BurgerSize)
				m_IsFalling = true;
		}
		else if (tag == "Burger")
		{
			m_IsFalling = true;

			for (size_t i{0}; i < m_BurgerSize; ++i)
			{
				if (m_Touched[i]) continue;;

				m_Touched[i] = true;
				m_Childeren[i]->TranslateLocalPosition({ 0, int(MapLoader::GetGridSize() / 2.f) });
			}
		}
		else if (tag == "Catcher")
		{
			//set the tag of our burger to catcher so other burgers land on top of it
			static_cast<biggin::BoxCollider2d*>(entity)->GetOther()->SetTag("Catcher");
			m_ReachedBottom = true;
			m_pNotifier->notify(this, "BurgerReachedEnd");
		}
	}
	else if (event == "EndContact")
	{
		//Logger::GetInstance().LogDebug("ovelap ended Burger");
	}
}

void Burger::FixedUpdate()
{
	if (!m_Initialized)
		InitializeBurger();

	if (m_ReachedBottom) return;

	if (m_IsFalling)
	{
		auto deltaVelo = glm::vec2{ 0, m_Velocity } * GameTime::GetFixedTimeStep();
		GetGameObject()->TranslateLocalPosition(deltaVelo);
	}
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
	constexpr auto tileSize = MapLoader::GetGridSize();
	for (int i{ 0 }; i < m_Childeren.size(); ++i)
	{
		biggin::RenderComponent* renderComponent = m_Childeren[i]->GetComponent<biggin::RenderComponent>();
		renderComponent->SetTexture("BurgerTimeSpriteSheet.png");
		renderComponent->SetSourceRect({ (14 + i) * static_cast<int>(tileSize), collumnIdx * static_cast<int>(tileSize), static_cast<int>(tileSize),static_cast<int>(tileSize) });

		glm::vec2 localPos = glm::vec2{ tileSize * i, -tileSize };
		m_Childeren[i]->SetLocalPosition(localPos);
	}
}
