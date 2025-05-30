#include "NodeManager.h"
#include <camera/Camera.h>
#include <render_item/RenderItem.h>
#include <log/Log.h>

NodeManager::NodeManager() {}

NodeManager::~NodeManager() {
	for (int i = 0, s = m_nodes.size(); i < s; i++) {
		delete m_nodes[i];
	}
}

bool NodeManager::update() {
	for (Node* node : m_nodes) {
		node->update();
	}
	return true;
}

uint8_t NodeManager::create_id() {
	return m_nodes.size();
}

uint8_t NodeManager::create_camera() {
	uint8_t newId = create_id();
	Camera* newCam = new Camera(newId);
	m_nodes.emplace_back(newCam);
	m_cameras.emplace_back(newId);
	if (!m_selectedCamera) {
		m_selectedCamera = newCam;
	}
	return newId;
}

uint8_t NodeManager::create_camera(const glm::vec3& eye, const glm::vec3& viewDirection, const glm::vec3& up) {
	uint8_t newId = create_id();
	Camera* newCam = new Camera(newId, eye, viewDirection, up);
	m_nodes.emplace_back(newCam);
	m_cameras.emplace_back(newId);
	if (!m_selectedCamera) {
		m_selectedCamera = newCam;
	}
	return newId;
}

uint8_t NodeManager::create_render_item(
	std::vector<GLfloat> vertexData, 
	std::vector<GLuint> vertexIdxs,
	const glm::vec3& worldPosition, 
	const glm::vec3& rotation, 
	const glm::vec3& scale
) {
	uint8_t newId = create_id();
	RenderItem* newRI = new RenderItem(newId, vertexData, vertexIdxs, worldPosition, rotation, scale);
	m_nodes.emplace_back(newRI);
	m_renderItems.emplace_back(newId);
	if (!m_selectedRenderItem) {
		m_selectedRenderItem = newRI;
	}
	return newId;
}

bool NodeManager::select_camera(const uint8_t& id) {
	Camera* selectedCamera = nullptr;
	try {
		selectedCamera = dynamic_cast<Camera*>(m_nodes.at(id));
	}
	catch (const std::out_of_range& e) {
		UF_LOG_ERROR("camera with id: {} not found", id);
		return false;
	}
	if (selectedCamera == nullptr) {
		UF_LOG_ERROR("selected node was not of type Camera");
		return false;
	}
	m_selectedCamera = selectedCamera;
	return true;
}

bool NodeManager::select_next_camera() {
	// no cameras currently initialized, break early
	if (m_cameras.size() == 0) {
		UF_LOG_ERROR("no camera to select");
		return false;
	}

	uint8_t cameraId = m_selectedCamera->get_id();
	int nextId = 0;

	for (int i = 0, s = m_cameras.size(); i < s; i++) {
		if (m_cameras[i] == cameraId) {
			// if at end of list, set camera to beginning cam
			if (i + 1 == s) {
				nextId = 0;
			}
			else {
				nextId = i + 1;
			}
		}
	}

	return select_camera(nextId);
}

bool NodeManager::select_render_item(const uint8_t& id) {
	RenderItem* selectedRenderItem = nullptr;
	try {
		selectedRenderItem = dynamic_cast<RenderItem*>(m_nodes.at(id));
	}
	catch (const std::out_of_range& e) {
		UF_LOG_ERROR("camera with id: {} not found", id);
		return false;
	}
	if (selectedRenderItem == nullptr) {
		UF_LOG_ERROR("selected node was not of type RenderItem");
		return false;
	}
	m_selectedRenderItem = selectedRenderItem;
	return true;
}

bool NodeManager::select_next_render_item() {
	// no renderItems currently initialized, break early
	if (m_renderItems.size() == 0) {
		UF_LOG_ERROR("no renderItem to select");
		return false;
	}

	uint8_t renderItemId = m_selectedRenderItem->get_id();
	int nextId = 0;

	for (int i = 0, s = m_cameras.size(); i < s; i++) {
		if (m_cameras[i] == renderItemId) {
			// if at end of list, set render_item to beginning render_item
			if (i + 1 == s) {
				nextId = 0;
			}
			else {
				nextId = i + 1;
			}
		}
	}

	return select_render_item(nextId);
}

Camera* NodeManager::get_camera() {
	return m_selectedCamera;
}

RenderItem* NodeManager::get_render_item() {
	return m_selectedRenderItem;
}
