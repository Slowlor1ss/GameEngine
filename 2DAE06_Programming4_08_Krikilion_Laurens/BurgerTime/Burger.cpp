#include "BigginPCH.h"
#include "Burger.h"
#include "BoxCollider2d.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Logger.h"
#include "MapLoader.h"
#include "RenderComponent.h"
#include "SpriteRenderComponent.h"

Burger::Burger(biggin::GameObject* go, BurgerIngredients ingredient)
	: Component(go)
	, m_Ingredient(ingredient)
{

}

void Burger::Start()
{
	//m_RenderComponents.reserve(m_BurgerSize);

//m_RenderComponents = go->GetComponents<biggin::RenderComponent>();
	m_Childeren = GetGameObject()->GetAllChilderen();
	//if (m_RenderComponents.empty())
	//	Logger::GetInstance().LogErrorAndBreak("Missing RenderComponent(s)");

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
	default:;
	}

	//auto playerSprite = new biggin::SpriteRenderComponent(go, { 6,{13* int(tileSize), 7* int(tileSize)},{int(tileSize),int(tileSize)} });

	//b2Filter filter{};
	//filter.maskBits = 0xFFFF ^ static_cast<unsigned short>(biggin::CollisionGroup::Group1); //Ignore group 1
	//filter.categoryBits = static_cast<unsigned short>(biggin::CollisionGroup::Group2);//set self to group 2

	//for (size_t i{ 0 }; i < m_BurgerSize; ++i)
	//{
	//	glm::vec2 localPos = pos + glm::vec2{ tileSize * i, 0 };

	//	m_ColliderComponents.emplace_back(new biggin::BoxCollider2d(
	//		go, { tileSize - 5, tileSize - 5 }, true, b2_dynamicBody,
	//		{ this }, "Burger", localPos, false, filter));

	//	go->AddComponent(m_ColliderComponents.back());
	//}
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

			const auto it = std::ranges::find(m_Childeren, owner);
			if (it != m_Childeren.end())
			{
				const __int64 index = std::distance(m_Childeren.begin(), it);
				if (m_Touched[index]) return;

				m_Touched[index] = true;
				(*it)->TranslateLocalPosition({ 0, int(MapLoader::GetGridSize() / 2.f) });
				//m_Childeren[index]->SetLocalPosition({});
				//m_RenderComponents[index]->TranslateDstRect({ 0, int(MapLoader::GetGridSize()/2.f) });
			}
			else
			{
				Logger::GetInstance().LogDebug("No matching child found");
			}
				
			//thisCollider->TranslateLocalOffset({0, MapLoader::GetGridSize()/2.f});


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
				//m_RenderComponents[i]->TranslateDstRect({ 0, int(MapLoader::GetGridSize() / 2.f) });
				//m_ColliderComponents[i]->TranslateLocalOffset({0, MapLoader::GetGridSize() / 2.f});
			}
		}
		else if (tag == "Catcher")
		{
			//set the tag of our burger to catcher so other burgers land on top of it
			static_cast<biggin::BoxCollider2d*>(entity)->GetOther()->SetTag("Catcher");
			m_ReachedBottom = true;
		}
	}
	else if (event == "EndContact")
	{
		Logger::GetInstance().LogDebug("ovelap ended Burger");
	}
}

void Burger::FixedUpdate()
{
	if (m_ReachedBottom) return;

	if (m_IsFalling)
	{
		auto deltaVelo = glm::vec2{ 0, m_Velocity } * GameTime::GetFixedTimeStep();
		GetGameObject()->TranslateLocalPosition(deltaVelo);
		//for (const auto renderComponent : m_RenderComponents)
		//{
		//	renderComponent->TranslateDstRect({ int(deltaVelo.x), int(deltaVelo.y) });
		//}
	}
}

void Burger::InitRenderComp(int collumnIdx) const
{
	//for (int i{ 0 }; i < m_RenderComponents.size(); ++i)
	//{
	//	m_RenderComponents[i]->SetSourceRect({ (14 + i) * int(tileSize), collumnIdx * int(tileSize), int(tileSize),int(tileSize) });

	//	glm::vec2 localPos = pos + glm::vec2{ tileSize * i, -tileSize };
	//	m_RenderComponents[i]->SetDstRect({ int(localPos.x), int(localPos.y), int(tileSize),int(tileSize) });
	//}

	constexpr auto tileSize = MapLoader::GetGridSize();
	for (int i{ 0 }; i < m_Childeren.size(); ++i)
	{
		biggin::RenderComponent* renderComponent = m_Childeren[i]->GetComponent<biggin::RenderComponent>();
		renderComponent->SetSourceRect({ (14 + i) * static_cast<int>(tileSize), collumnIdx * static_cast<int>(tileSize), static_cast<int>(tileSize),static_cast<int>(tileSize) });

		glm::vec2 localPos = glm::vec2{ tileSize * i, -tileSize };
		m_Childeren[i]->SetLocalPosition(localPos);
		//m_RenderComponents[i]->SetDstRect({ int(localPos.x), int(localPos.y), int(tileSize),int(tileSize) });
	}
}
