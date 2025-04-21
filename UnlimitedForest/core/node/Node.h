#pragma once

#include <iostream>
#include <cstdint>

class Node {
public:
	Node(const uint8_t &id);
	virtual ~Node();

	uint8_t get_id(void);
private:
	uint8_t m_id;
};
