#include <camera/Camera.h>
#include <glm/gtc/matrix_transform.hpp>

//default constructor places camera at z 2 (positive towards you) and view focus at -1 to ensure facing out to the negative
Camera::Camera(const uint8_t& id) : Camera(id, glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0, 0.0)) {}

Camera::Camera(const uint8_t& id, const glm::vec3& eye, const glm::vec3& viewDirection, const glm::vec3& up) : Node(id){
	m_eye = eye;
	m_viewDirection = viewDirection;
	m_upVector = up;
}

Camera::~Camera() {}

glm::mat4 Camera::get_view_matrix() const {
	return glm::lookAt(m_eye, m_viewDirection, m_upVector);
}

void Camera::set_location(glm::vec3 loc) {
	m_eye = loc;
}

void Camera::translate_x(float t) {
	m_eye.x += t;
}
void Camera::translate_y(float t) {
	m_eye.y += t;
}
void Camera::translate_z(float t) {
	m_eye.z += t;
}
