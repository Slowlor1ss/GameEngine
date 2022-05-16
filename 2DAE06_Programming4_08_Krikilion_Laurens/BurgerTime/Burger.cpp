#include "BigginPCH.h"
#include "Burger.h"
#include "BoxCollider2d.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Logger.h"
#include "MapLoader.h"
#include "RenderComponent.h"
#include "SpriteRenderComponent.h"

Burger::Burger(biggin::GameObject* go, glm::vec2 pos, float tileSize, BurgerIngredients ingredient) : Component(go)
{
	m_RenderComponents.reserve(m_BurgerSize);

	m_RenderComponents = go->GetComponents<biggin::RenderComponent>();
	if (m_RenderComponents.empty())
		Logger::GetInstance().LogErrorAndBreak("Missing RenderComponent");

	switch (ingredient)
	{
	case BurgerIngredients::BurgerTop:
		InitRenderComp(6, pos, tileSize);
		break;
	case BurgerIngredients::Lettuce:
		InitRenderComp(11, pos, tileSize);
		break;
	case BurgerIngredients::Meat:
		InitRenderComp(9, pos, tileSize);
		break;
	case BurgerIngredients::BurgerBottom:
		InitRenderComp(7, pos, tileSize);
		break;
	default: ;
	}

	//auto playerSprite = new biggin::SpriteRenderComponent(go, { 6,{13* int(tileSize), 7* int(tileSize)},{int(tileSize),int(tileSize)} });

	b2Filter filter{};
	filter.maskBits = 0xFFFF ^ static_cast<unsigned short>(biggin::CollisionGroup::Group1); //Ignore group 1
	filter.categoryBits = static_cast<unsigned short>(biggin::CollisionGroup::Group2);//set self to group 2

	for (size_t i{0}; i < m_BurgerSize; ++i)
	{
		glm::vec2 localPos = pos + glm::vec2{tileSize*i, 0};

		m_ColliderComponents.emplace_back(new biggin::BoxCollider2d(
			go, { tileSize-5, tileSize-5 }, true, b2_dynamicBody,
			{this}, "Burger", localPos, false, filter));

		go->AddComponent(m_ColliderComponents.back());
	}
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

			const auto thisCollider = static_cast<biggin::BoxCollider2d*>(entity)->GetOther();

			const auto it = std::ranges::find(m_ColliderComponents, thisCollider);
			if (it != m_ColliderComponents.end())
			{
				const __int64 index = std::distance(m_ColliderComponents.begin(), it);
				if (m_Touched[index]) return;

				m_Touched[index] = true;
				m_RenderComponents[index]->TranslateDstRect({ 0, int(MapLoader::GetGridSize()/2.f) });
			}
				
			thisCollider->TranslateLocalOffset({0, MapLoader::GetGridSize()/2.f});


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
				m_RenderComponents[i]->TranslateDstRect({ 0, int(MapLoader::GetGridSize() / 2.f) });
				m_ColliderComponents[i]->TranslateLocalOffset({0, MapLoader::GetGridSize() / 2.f});
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
		for (const auto renderComponent : m_RenderComponents)
		{
			renderComponent->TranslateDstRect({ int(deltaVelo.x), int(deltaVelo.y) });
		}
	}
}

void Burger::InitRenderComp(int collumnIdx, glm::vec2 pos, float tileSize) const
{
	for (int i{ 0 }; i < m_RenderComponents.size(); ++i)
	{
		m_RenderComponents[i]->SetSourceRect({ (14 + i) * int(tileSize), collumnIdx * int(tileSize), int(tileSize),int(tileSize) });

		glm::vec2 localPos = pos + glm::vec2{ tileSize * i, -tileSize };
		m_RenderComponents[i]->SetDstRect({ int(localPos.x), int(localPos.y), int(tileSize),int(tileSize) });
	}
}
