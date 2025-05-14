#include "RenderItem.h"
#include <application/App.h>
#include <camera/Camera.h>
#include "log/Log.h"

#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float SCALEMIN = 0.1f;

RenderItem::RenderItem(const uint8_t& id, std::vector<GLfloat> vertexData, std::vector<GLuint> vertexIdxs, const glm::vec3& worldPosition, const glm::vec3& rotation, const glm::vec3& scale)
	: Node(id),
	m_worldPosition(worldPosition),
	m_rotation(rotation),
	m_scale(scale),
	m_vertexData(vertexData),
	m_vertexIdxs(vertexIdxs)
{
	mesh_specification();
}

void RenderItem::translate(const glm::vec3& tlate) {
	m_worldPosition += tlate;
	UF_LOG_DEBUG(glm::to_string(m_worldPosition));
}

void RenderItem::rotate(const glm::vec3& eulerAngles) {
	m_rotation += eulerAngles;
	UF_LOG_DEBUG(glm::to_string(m_rotation));
}

void RenderItem::scale(const glm::vec3& scale) {
	m_scale.x = std::max(m_scale.x + scale.x, SCALEMIN);
	m_scale.y = std::max(m_scale.y + scale.y, SCALEMIN);
	m_scale.z = std::max(m_scale.z + scale.z, SCALEMIN);
	UF_LOG_DEBUG(glm::to_string(m_scale));
}

// inherited from node object, will handle the render flow
void RenderItem::update() {
	predraw();
	draw();
}

// moving vertex data (vertecies / colors / etc) to gpu and telling opengl how data is stored for later drawing in shaders
void RenderItem::mesh_specification() {
	// generating VBO and VAO for loading vertex data
	glGenBuffers(1, &m_vertexBufferObject);
	glGenVertexArrays(1, &m_vertexArrayObject);
	CATCH_GL_ERROR("error generating vbo/vao");

	// Binding VAO for coming definitions
	glBindVertexArray(m_vertexArrayObject);
	CATCH_GL_ERROR("error binding vao");


	// binding buffer and loading data into VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER,
		m_vertexData.size() * sizeof(GLfloat),
		m_vertexData.data(),
		GL_STATIC_DRAW
	);
	CATCH_GL_ERROR("error generating vbo");


	// defining vertex positions for vao in vaa index 0
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,
		3,              // 3 components per vertex (x, y, z)
		GL_FLOAT,       // data type
		GL_FALSE,       // not normalized (correct)
		sizeof(GLfloat) * 6,              // stride (0 means tightly packed)
		(GLvoid*)0     // offset of the first component
	);
	CATCH_GL_ERROR("error setting vertex attributes idx: 0");


	// defining vertex color data for vao in vaa index 1
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(GLfloat) * 6,
		(GLvoid*)(sizeof(GLfloat) * 3)
	);
	CATCH_GL_ERROR("error setting vertex attributes idx: 1");


	// loading in indexcies of vertex position elements for later rendering
	// in this case a quant of 2 triangles
	glGenBuffers(1, &m_vertexElementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(GLfloat) * m_vertexIdxs.size(),
		m_vertexIdxs.data(),
		GL_STATIC_DRAW
	);
	CATCH_GL_ERROR("error loading VEO");

	// cleaning up vbo\vao as well
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void RenderItem::predraw() {
	App* app = App::get();
	EngineConfig cfg = app->get_engine_config();
	GLuint gp = app->get_graphics_pipeline_shader_program();

	glUseProgram(gp);

	// Constant rotation to object
	NodeManager* nm = app->get_node_manager();
	Camera* camera = nm->get_camera();

	// model transformation by translating our object into world space 
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(m_worldPosition.x, m_worldPosition.y, m_worldPosition.z));
	model = glm::rotate(model, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(m_scale.x, m_scale.y, 1.0f));

	GLint u_ModelMatrixLocation = glGetUniformLocation(gp, "u_ModelMatrix");
	if (u_ModelMatrixLocation >= 0) {
		glUniformMatrix4fv(u_ModelMatrixLocation, 1, GL_FALSE, &model[0][0]);
	}
	else {
		UF_LOG_ERROR("could not find u_ModelMatrix uniform");
		exit(EXIT_FAILURE);
	}

	glm::mat4 view = camera->get_view_matrix();

	GLint u_ViewMatrixLocation = glGetUniformLocation(gp, "u_ViewMatrix");
	if (u_ViewMatrixLocation >= 0) {
		glUniformMatrix4fv(u_ViewMatrixLocation, 1, GL_FALSE, &view[0][0]);
	}
	else {
		UF_LOG_ERROR("could not find u_ModelMatrix uniform");
		exit(EXIT_FAILURE);
	}

	// Projection matrix (in perspective)
	glm::mat4 perspective = glm::perspective(glm::radians(45.0f),
		(float)cfg.m_screenWidth / (float)cfg.m_screenHeight,
		0.1f,
		100.0f
	);

	GLint u_PerspectiveLocation = glGetUniformLocation(gp, "u_Perspective");
	if (u_PerspectiveLocation >= 0) {
		glUniformMatrix4fv(u_PerspectiveLocation, 1, GL_FALSE, &perspective[0][0]);
	}
	else {
		UF_LOG_ERROR("could not find u_Perspective uniform");
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
	CATCH_GL_ERROR("error drawing triangles");
	// cleanup graphics pipeline
	glUseProgram(0);
}

void RenderItem::print() {
	UF_LOG_DEBUG("id: {}\nwp: {}\nscale: {}\nrot: {}",
		get_id(),
		glm::to_string(m_worldPosition),
		glm::to_string(m_scale),
		glm::to_string(m_rotation)
	);
}
