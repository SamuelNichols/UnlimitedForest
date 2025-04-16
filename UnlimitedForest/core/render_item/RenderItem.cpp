#include "RenderItem.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>

RenderItem::RenderItem(const uint8_t& id, const glm::vec3& worldPosition, const glm::vec3& rotation, const glm::vec3& scale) 
	: Node(id),
	m_worldPosition(worldPosition),
	m_rotation(rotation),
	m_scale(scale)
{}

void RenderItem::translate(const glm::vec3& tlate) {
	m_worldPosition += tlate;
}

void RenderItem::rotate(const glm::vec3& eulerAngles) {
	m_rotation += eulerAngles;
}

void RenderItem::scale(const glm::vec3& scale) {
	m_scale += scale;
}

void RenderItem::print() {
	std::cout << "id: " << get_id() << "\n"
		<< "wp: " << glm::to_string(m_worldPosition) << "\n"
		<< "scale: " << glm::to_string(m_scale) << "\n"
		<< "rot: " << glm::to_string(m_rotation) << std::endl;
}
