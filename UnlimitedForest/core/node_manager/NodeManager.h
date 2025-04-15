#pragma once

#include <camera/Camera.h>
#include <node/Node.h>

class NodeManager {
public:
	NodeManager();
	~NodeManager();

	bool update(void);
};
