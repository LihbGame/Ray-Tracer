#pragma once
#include <cstdlib>

inline float random_double() {
	return rand() / (RAND_MAX + 1.0f);
}