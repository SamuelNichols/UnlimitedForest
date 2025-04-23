#ifndef camera_h
#define camera_h

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>
#include <glm/glm.hpp>
#include <cstdint>
#include <mutex>

#include <../UnlimitedForest.h>
#include <node/Node.h>

//global logging object
#include <spdlog/spdlog.h>
extern std::shared_ptr<spdlog::logger> g_infoLogger;
extern std::shared_ptr<spdlog::logger> g_errorLogger;

struct App;

class Camera : public Node {
public:
	Camera(App& app, const uint8_t& id);
	Camera(App& app, const uint8_t& id, const glm::vec3 &eye,const glm::vec3 &viewDirectio, const glm::vec3 &up);
	~Camera();

	//ultimate view matrix
	glm::mat4 get_view_matrix(void);
	glm::mat4 get_perspective_matrix(void);

	void set_location(const glm::vec3& loc);
	void translate(const glm::vec3& translation);
	void move_forward(const float& speed);
	void move_backward(const float& speed);
	void move_left(const float& speed);
	void move_right(const float& speed);
	void move_up(const float& speed);
	void move_down(const float& speed);
	void mouse_look(const glm::vec2& mouse);

	void print(void);

private:
	App& m_app;

	glm::vec3 m_eye;
	glm::vec3 m_viewDirection;
	glm::vec3 m_upVector;

	std::once_flag m_oldMousePosInit;
	glm::vec2 m_oldMousePos;

	glm::vec3 get_right_vector(void);
};

#endif // !camera_h
