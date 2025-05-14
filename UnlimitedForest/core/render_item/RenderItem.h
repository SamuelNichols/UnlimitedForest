#pragma once

#include "node/Node.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class App;        // forward
class Camera;

class RenderItem : public Node {
public:
	RenderItem(const uint8_t& id, 
		std::vector<GLfloat> vertexData, 
		std::vector<GLuint> vertexIdxs,
		const glm::vec3& worldPosition, 
		const glm::vec3& rotation, 
		const glm::vec3& scale
	);
	~RenderItem() {};

	void update(void) override;
	void predraw(void);
	void draw();

	void translate(const glm::vec3& translation);
	void rotate(const glm::vec3& eulerAngles);
	void scale(const glm::vec3& scale);

	void print(void);

private:
	void mesh_specification(void);
	std::vector<GLfloat> m_vertexData;
	std::vector<GLuint> m_vertexIdxs;

	// TODO: handle all of the opengl functionality for actually drawing on update tick here
	// private: TODO: these variables should be private but for now they are public since rendering is happening outside of this node
	//		later rendering will be moved within this node
	//uint8_t create_render_item(const glm::vec3& worldPosition, const glm::vec3& rotation, const glm::vec3& scale);
	glm::vec3 m_worldPosition;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;

	// OpenGL variables
	// OpenGL VAO \ VBO
	GLuint m_vertexArrayObject = 0;
	GLuint m_vertexBufferObject = 0;
	// index buffer for reusing shared indecies of triangles
	// TODO: this might make sense in a more abstracted sense to allow for more efficient rendering
	GLuint m_vertexElementBuffer = 0;
};