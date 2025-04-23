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

// Render functionality
void RenderItem::mesh_specification() {
	// lives on CPU
	const std::vector<GLfloat> vertexData{
		//   x      y      z// quad
		// vec 1
		-0.5f, -0.5f, 0.0f, // x y z bottom left
		1.0f, 0.0f, 0.0f,   // r g b
		// vec 2
		0.5f, -0.5f, 0.0f,  // x y z bottom right
		0.0f, 1.0f, 0.0f,   // r g b
		// vec 3
		-0.5f,  0.5f, 0.0f, // x y z top left
		0.0f, 0.0f, 1.0f,   // r g b
		// vec 4
		0.5f, 0.5f, 0.0f,  // x y z top right
		0.0f, 1.0f, 0.0f,   // r g b
	};

	// generating VBO
	glGenBuffers(1, &m_vertexBufferObject);
	// binding buffer to current buffer object id
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
	// setting buffer pointer to current buffer object id to vertex values
	glBufferData(GL_ARRAY_BUFFER,
		vertexData.size() * sizeof(GLfloat),
		vertexData.data(),
		GL_STATIC_DRAW
	);
	catch_gl_error("error generating vbo");

	// enabling vertex attribute for positional data x y z
	// loading to GPU
	// generating VAO
	glGenVertexArrays(1, &m_vertexArrayObject);
	glBindVertexArray(m_vertexArrayObject);
	catch_gl_error("error generating vao");

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,
		3,              // 3 components per vertex (x, y, z)
		GL_FLOAT,       // data type
		GL_FALSE,       // not normalized (correct)
		sizeof(GLfloat) * 6,              // stride (0 means tightly packed)
		(GLvoid*)0     // offset of the first component
	);
	catch_gl_error("error setting vertex attributes idx: 0");

	// enabling vertex attribute for color data r g b (offset 3 floats on position)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(GLfloat) * 6,
		(GLvoid*)(sizeof(GLfloat) * 3)
	);
	catch_gl_error("error setting vertex attributes idx: 1");

	std::vector<GLuint> vectorIdxs = {
		0, 1, 2,
		1, 3, 2
	};
	glGenBuffers(1, &m_vertexElementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(GLfloat) * vectorIdxs.size(),
		vectorIdxs.data(),
		GL_STATIC_DRAW
	);
	catch_gl_error("error loading VEO");
}

void RenderItem::predraw() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, app.m_screenWidth, app.m_screenHeight);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(app.m_graphicsPipelineShaderProgram);

	// Constant rotation to object
	//g_selectedItemTransform.roty += 0.05f;

	Camera* camera = app.m_nodeManager.get_camera();
	RenderItem* ri = app.m_nodeManager.get_render_item();

	// model transformation by translating our object into world space 
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(ri->m_worldPosition.x, ri->m_worldPosition.y, ri->m_worldPosition.z));
	model = glm::rotate(model, glm::radians(ri->m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(ri->m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(ri->m_scale.x, ri->m_scale.y, 1.0f));
	g_infoLogger->info("1\n");

	GLint u_ModelMatrixLocation = glGetUniformLocation(app.m_graphicsPipelineShaderProgram, "u_ModelMatrix");
	if (u_ModelMatrixLocation >= 0) {
		glUniformMatrix4fv(u_ModelMatrixLocation, 1, GL_FALSE, &model[0][0]);
	}
	else {
		std::cout << "could not find u_ModelMatrix uniform" << std::endl;
		exit(EXIT_FAILURE);
	}

	glm::mat4 view = camera->get_view_matrix();
	g_infoLogger->info("1.5\n");

	GLint u_ViewMatrixLocation = glGetUniformLocation(app.m_graphicsPipelineShaderProgram, "u_ViewMatrix");
	if (u_ViewMatrixLocation >= 0) {
		glUniformMatrix4fv(u_ViewMatrixLocation, 1, GL_FALSE, &view[0][0]);
	}
	else {
		std::cout << "could not find u_ModelMatrix uniform" << std::endl;
		exit(EXIT_FAILURE);
	}

	GLint u_PerspectiveLocation = glGetUniformLocation(app.m_graphicsPipelineShaderProgram, "u_Perspective");
	if (u_PerspectiveLocation >= 0) {
		glUniformMatrix4fv(u_PerspectiveLocation, 1, GL_FALSE, &perspective[0][0]);
	}
	else {
		std::cout << "could not find u_Perspective uniform" << std::endl;
		exit(EXIT_FAILURE);
	}

}

void RenderItem::draw() {
	glBindVertexArray(m_vertexArrayObject);

	glDrawElements(
		GL_TRIANGLES,
		6,
		GL_UNSIGNED_INT,
		(GLvoid*)0
	);
	catch_gl_error("error drawing triangles");
	// cleanup graphics pipeline
	glUseProgram(0);
}

// Movement functionality

void RenderItem::translate(const glm::vec3& tlate) {
	m_worldPosition += tlate;
	g_loggers.m_infoLogger->info(glm::to_string(m_worldPosition));
}

void RenderItem::rotate(const glm::vec3& eulerAngles) {
	m_rotation += eulerAngles;
	g_loggers.m_infoLogger->info(glm::to_string(m_rotation));
}

void RenderItem::scale(const glm::vec3& scale) {
	m_scale.x = std::max(m_scale.x + scale.x, SCALEMIN);
	m_scale.y = std::max(m_scale.y + scale.y, SCALEMIN);
	m_scale.z = std::max(m_scale.z + scale.z, SCALEMIN);
	g_loggers.m_infoLogger->info(glm::to_string(m_scale));
}

void RenderItem::print() {
	std::cout << "id: " << get_id() << "\n"
		<< "wp: " << glm::to_string(m_worldPosition) << "\n"
		<< "scale: " << glm::to_string(m_scale) << "\n"
		<< "rot: " << glm::to_string(m_rotation) << std::endl;
}
