#ifndef camera_h
#define camera_h

#include <glm/glm.hpp>
#include <cstdint>

#include <node/Node.h>

class Camera : public Node {
public:
	Camera(const uint8_t& id);
	Camera(const uint8_t& id, const glm::vec3 &eye,const glm::vec3 &viewDirectio, const glm::vec3 &up);
	~Camera();

	//ultimate view matrix
	glm::mat4 get_view_matrix() const;

	void set_location(glm::vec3 loc);
	void translate_x(float t);
	void translate_y(float t);
	void translate_z(float t);

private:
	glm::vec3 m_eye;
	glm::vec3 m_viewDirection;
	glm::vec3 m_upVector;
};

#endif // !camera_h
