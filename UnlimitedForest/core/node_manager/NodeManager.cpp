#include "NodeManager.h"

#include <iostream>

NodeManager::NodeManager(){}

NodeManager::~NodeManager(){}

bool NodeManager::update() {
	std::cout << "nm update hit" << std::endl;
	return true;
}
