#include "RenderItem.h"

RenderItem::RenderItem(const uint8_t& id, const glm::vec3& worldPosition, const glm::vec3& rotation, const glm::vec3& scale) 
	: Node(id),
	m_worldPosition(worldPosition),
	m_rotation(rotation),
	m_scale(scale)
{}
