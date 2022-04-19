#include "BigginPCH.h"
#include "Burger.h"
#include "BoxCollider2d.h"
#include "GameObject.h"
#include "GameTime.h"

Burger::Burger(biggin::GameObject* go, glm::vec2 pos, float tileSize, BurgerIngredients ingredient) : Component(go)
{
	switch (ingredient)
	{
	case BurgerIngredients::BurgerTop: 
		break;
	case BurgerIngredients::Lettuce: 
		break;
	case BurgerIngredients::Meat: 
		break;
	case BurgerIngredients::BurgerBottom: 
		break;
	default: ;
	}

	for (size_t i{0}; i < m_BurgerSize; ++i)
	{
		glm::vec2 localPos = pos + glm::vec2{tileSize*i, 0};
		go->AddComponent(new biggin::BoxCollider2d(
			go, { tileSize, tileSize }, true, b2_dynamicBody,
			{}, "Burger", localPos, false));
	}
}

void Burger::OnNotify(const Component* entity, const std::string& event)
{
	if (event == "BeginContact")
	{
		std::cout << "ovelap started with burger\n";
		auto tag = static_cast<const biggin::BoxCollider2d*>(entity)->GetTag();
		if (tag == "Platform")
		{
			m_AmntTouchedParts = 0;
			m_IsFalling = false;
		}
		else if(tag == "Player")
		{
			++m_AmntTouchedParts;
			if (m_AmntTouchedParts == m_BurgerSize)
			{
				m_IsFalling = true;
			}
		}
	}
	else if (event == "EndContact")
	{
		std::cout << "ovelap ended\n";
		//if (static_cast<const biggin::BoxCollider2d*>(entity)->GetTag() == "DisableUp")
		//{
		//	m_VerticalMovDisabled = false;
		//}
	}
}

void Burger::FixedUpdate()
{
	if (m_IsFalling)
	{
		GetGameObject()->TranslateLocalPosition(glm::vec2{0, m_Velocity} * GameTime::GetFixedTimeStep());
	}
}
