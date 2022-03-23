#include "BigginPCH.h"
#include "Transform.h"

void biggin::Transform::SetLocalPosition(const float x, const float y, const float z)
{
	m_LocalPosition.x = x;
	m_LocalPosition.y = y;
	m_LocalPosition.z = z;
}

void biggin::Transform::SetLocalPosition(glm::vec3 pos)
{
	m_LocalPosition = pos;
}

void biggin::Transform::TranslateLocalPosition(glm::vec3 pos)
{
	m_LocalPosition += pos;
}

void biggin::Transform::SetWorldPosition(float x, float y, float z)
{
	m_WorldPosition.x = x;
	m_WorldPosition.y = y;
	m_WorldPosition.z = z;
}

void biggin::Transform::SetWorldPosition(glm::vec3 pos)
{
	m_WorldPosition = pos;
}

void biggin::Transform::TranslateWorldPosition(glm::vec3 pos)
{
	m_WorldPosition += pos;
}
