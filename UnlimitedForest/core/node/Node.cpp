#include "Node.h"

Node::Node(const uint8_t& id) {
	m_id = id;
}

Node::~Node() {}

uint8_t Node::get_id() {
	return m_id;
}
