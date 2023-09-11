#pragma once

#include <cstdint>

class Field;
class LevelManager {
public:
	void Initialize();
	void Update();

	void GetFild(Field* field) { field_ = field; }
private:
	Field* field_;
	const uint32_t kInterval_ = 4;
	uint32_t intervalCount_;
};

