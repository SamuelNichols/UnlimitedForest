#pragma once

#include <glm/glm.hpp>

#include "node/Node.h"

class RenderItem : public Node {
public:
	RenderItem(const uint8_t& id, const glm::vec3& worldPosition, const glm::vec3& rotation, const glm::vec3& scale);
	~RenderItem() {};

	// TODO: handle all of the opengl functionality for actually drawing on update tick here
private:
	//uint8_t create_render_item(const glm::vec3& worldPosition, const glm::vec3& rotation, const glm::vec3& scale);
	glm::vec3 m_worldPosition;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;
};