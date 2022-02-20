#pragma once

namespace dae
{
	class InputManager;
}

class GameTime;

///
/// Holds general scene variables like gametime, input, and camera (later)
///
class SceneContext final
{
public:
	SceneContext();
	~SceneContext();
	SceneContext(const SceneContext& t) = delete;
	SceneContext& operator=(const SceneContext& t) = delete;

	GameTime* GetGameTime() const { return m_pGameTime; }
	dae::InputManager* GetInput() const { return m_pInput; }

private:
	GameTime* m_pGameTime;
	dae::InputManager* m_pInput;
};

