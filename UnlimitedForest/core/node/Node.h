#pragma once

#include <cstdint>

class Node {
public:
	Node(const uint8_t &id);
	virtual ~Node();

	// game loop update function to be inherited by node parent classes
	// this is to avoid dynamically casting every time I want to reference a node
	virtual void update(void) {};

	uint8_t get_id(void);
private:
	uint8_t m_id;
};
