#pragma once

#include "node/Node.h"

class RenderItem : public Node {
public:
	RenderItem(const uint8_t& id) : Node(id) {};
	~RenderItem() {};

};