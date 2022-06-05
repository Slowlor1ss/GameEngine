#include "PepperShooter.h"
#include "BoxCollider2d.h"
#include "GameObject.h"
#include "Logger.h"
#include "MapLoader.h"
#include "PepperComponent.h"
#include "PeterPepper.h"
#include "PossessGameObjectComponent.h"
#include "RenderComponent.h"
#include "SoundServiceLocator.h"
#include "SpriteRenderComponent.h"

burgerTime::PepperShooter::PepperShooter(biggin::GameObject* go, const std::vector<biggin::Observer*>& observers, int amntPepper)
	: Component(go)
	, m_AmntPepper(amntPepper)
	, m_pNotifier(go->GetComponent<biggin::Subject>())
{
	if (m_pNotifier == nullptr)
		Logger::GetInstance().LogErrorAndBreak("Missing Subject Component");

	for (const auto observer : observers)
		m_pNotifier->AddObserver(observer);
}

void burgerTime::PepperShooter::Start()
{
	//Updating the initial pepper
	m_pNotifier->notify(this, "PepperChanged");
}

bool burgerTime::PepperShooter::Shoot(biggin::PossessGameObjectComponent::MoveDirection dir)
{
	if (m_AmntPepper <= 0)
		return false;

	SoundServiceLocator::GetSoundSystem().Play("pepper.wav", 0.2f);

	auto* ourGameObject = GetGameObject();

	glm::vec2 pos{ ourGameObject->GetLocalPosition() };

	switch (dir)
	{
	case biggin::PossessGameObjectComponent::MoveDirection::Left:
		pos += glm::vec2{ -MapLoader::GetGridSize() * 2, 0 };
		break;
	case biggin::PossessGameObjectComponent::MoveDirection::Right:
		pos += glm::vec2{MapLoader::GetGridSize() * 2, 0 };
		break;
	case biggin::PossessGameObjectComponent::MoveDirection::Up:
		pos += glm::vec2{ 0, -MapLoader::GetGridSize() * 2 };
		break;
	case biggin::PossessGameObjectComponent::MoveDirection::Down:
		pos += glm::vec2{ 0, MapLoader::GetGridSize() * 2 };
		break;
	}

	--m_AmntPepper;
	m_pNotifier->notify(this, "PepperChanged");

	//spawn pepper
	auto pepper = std::make_shared<biggin::GameObject>();
	pepper->SetLocalPosition(pos);

	constexpr int columns{ 4 };
	constexpr int cellSize{ 32 };
	auto renderComp = new biggin::RenderComponent(pepper.get(), "BurgerTimeSpriteSheet.png");
	pepper->AddComponent(renderComp);
	auto pepperSprite = new biggin::SpriteRenderComponent(pepper.get(), { columns,{11*cellSize,1*cellSize},{cellSize,cellSize} }, 0.2f);
	switch (dir)
	{
	case biggin::PossessGameObjectComponent::MoveDirection::Left:
		pepperSprite->AddAnimation(static_cast<int>(dir), { 4, columns * 0 });
		break;
	case biggin::PossessGameObjectComponent::MoveDirection::Right:
		pepperSprite->AddAnimation(static_cast<int>(dir), { 4, columns * 0 });
		renderComp->SetFlip(SDL_FLIP_HORIZONTAL);
		break;
	case biggin::PossessGameObjectComponent::MoveDirection::Up:
		pepperSprite->AddAnimation(static_cast<int>(dir), { 4, columns * 2 });
		break;
	case biggin::PossessGameObjectComponent::MoveDirection::Down:
		pepperSprite->AddAnimation(static_cast<int>(dir), { 4, columns * 1 });
		break;
	}
	pepperSprite->SetCurrentSprite(static_cast<int>(dir));
	pepper->AddComponent(pepperSprite);

	pepper->AddComponent(new biggin::Subject(pepper.get()));

	b2Filter filterPepperCollider{};
	filterPepperCollider.categoryBits = character::PeterPepper::PlayerCollisionGroup::playerCollisionGroup;
	pepper->AddComponent(new biggin::BoxCollider2d(pepper.get(), { 32, 32 }, true, b2_dynamicBody, {}
	, "Pepper", { 0, 0 }, true, filterPepperCollider));

	pepper->AddComponent(new PepperComponent(pepper.get()));

	ourGameObject->GetSceneRef()->AddPending(pepper);

	return true;
}
