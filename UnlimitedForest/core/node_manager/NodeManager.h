#pragma once
#include <node/Node.h>

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

class Camera; // forward ref
class RenderItem; // forward ref

class NodeManager {
public:
	NodeManager();
	~NodeManager();
	// update functionality for game loop
	bool update(void);

	// child node constructors
	uint8_t create_camera(void);
	uint8_t create_camera(const glm::vec3& eye, const glm::vec3& viewDirectio, const glm::vec3& up);

	uint8_t create_render_item(
		std::vector<GLfloat> vertexData,
		std::vector<GLuint> vertexIdxs,
		const glm::vec3& worldPosition,
		const glm::vec3& rotation,
		const glm::vec3& scale
	);

	Camera* get_camera(void);
	RenderItem* get_render_item(void);

private:
	uint8_t create_id(void);
	bool select_camera(const uint8_t& id);
	bool select_render_item(const uint8_t& id);

	bool select_next_camera(void);
	bool select_next_render_item(void);

	std::vector<Node*> m_nodes;
	std::vector<uint8_t> m_cameras;
	std::vector<uint8_t> m_renderItems;

	Camera* m_selectedCamera = nullptr;
	RenderItem* m_selectedRenderItem = nullptr;
};
