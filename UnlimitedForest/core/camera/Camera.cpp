#include <camera/Camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

//default constructor places camera at z 2 (positive towards you) and view focus at -1 to ensure facing out to the negative
Camera::Camera(const uint8_t& id) : Camera(id, glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0, 0.0)) {}

Camera::Camera(const uint8_t& id, const glm::vec3& eye, const glm::vec3& viewDirection, const glm::vec3& up) : Node(id) {
	m_eye = eye;
	m_viewDirection = glm::normalize(viewDirection);
	m_upVector = glm::normalize(up);
}

Camera::~Camera() {}

glm::mat4 Camera::get_view_matrix() {
	// TODO: add a scale variable to allow the view point to be further than 1 (since it is a unit vector)
	return glm::lookAt(m_eye, m_eye + m_viewDirection, m_upVector);
}

void Camera::set_location(const glm::vec3& loc) {
	m_eye = loc;
}

void Camera::translate(const glm::vec3& translation) {
	m_eye += translation;
}

void Camera::mouse_look(const glm::vec2& mouse) {
	g_infoLogger->info("x: {}, y: {}\n\n", mouse.x, mouse.y);
	std::call_once(m_oldMousePosInit, [this, mouse]() {
		m_oldMousePos = mouse;
		});

	glm::vec2 deltaMouse = m_oldMousePos - mouse;
	m_oldMousePos = mouse;

	// normalizing so scale of view doesn't affect rotation
	glm::vec3 direction = glm::normalize(m_viewDirection);
	// rotating then applying back to eye origin
	 direction = glm::rotate(direction, glm::radians(deltaMouse.x), m_upVector);
	 direction = glm::rotate(direction, glm::radians(deltaMouse.y), glm::vec3(1.0f, 0.0f, 0.0f));
	 m_viewDirection = glm::normalize(direction);

}

void Camera::move_forward(const float& speed) {
	m_eye += m_viewDirection * speed;
}

void Camera::move_backward(const float& speed) {
	m_eye -= m_viewDirection * speed;
}

glm::vec3 Camera::get_right_vector() {
	return glm::normalize(glm::cross(m_viewDirection, m_upVector));
}

void Camera::move_left(const float& speed) {
	m_eye -= get_right_vector() * speed;
}

void Camera::move_right(const float& speed) {
	m_eye += get_right_vector() * speed;
}

void Camera::move_up(const float& speed) {
	m_eye += glm::normalize(m_upVector) * speed;
}

void Camera::move_down(const float& speed) {
	m_eye -= glm::normalize(m_upVector) * speed;
}

void Camera::print() {
	g_infoLogger->info("eye: {} | view: {} | up: {}", glm::to_string(m_eye), glm::to_string(m_viewDirection), glm::to_string(m_upVector));
}
