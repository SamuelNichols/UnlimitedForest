#include "RenderItem.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>
#include <algorithm>

const float SCALEMIN = 0.1f;

RenderItem::RenderItem(const uint8_t& id, const glm::vec3& worldPosition, const glm::vec3& rotation, const glm::vec3& scale) 
	: Node(id),
	m_worldPosition(worldPosition),
	m_rotation(rotation),
	m_scale(scale)
{}

void RenderItem::translate(const glm::vec3& tlate) {
	m_worldPosition += tlate;
	g_infoLogger->info(glm::to_string(m_worldPosition));
}

void RenderItem::rotate(const glm::vec3& eulerAngles) {
	m_rotation += eulerAngles;
	g_infoLogger->info(glm::to_string(m_rotation));
}

void RenderItem::scale(const glm::vec3& scale) {
	m_scale.x = std::max(m_scale.x + scale.x, SCALEMIN);
	m_scale.y = std::max(m_scale.y + scale.y, SCALEMIN);
	m_scale.z = std::max(m_scale.z + scale.z, SCALEMIN);
	g_infoLogger->info(glm::to_string(m_scale));
}

void RenderItem::print() {
	std::cout << "id: " << get_id() << "\n"
		<< "wp: " << glm::to_string(m_worldPosition) << "\n"
		<< "scale: " << glm::to_string(m_scale) << "\n"
		<< "rot: " << glm::to_string(m_rotation) << std::endl;
}
