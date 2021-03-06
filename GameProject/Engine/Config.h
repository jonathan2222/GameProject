#pragma once

#include "glm/gtc/matrix_transform.hpp"

// Common defines
#define EPSILON 0.0000001

// Window defines
#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480
#define DEFAULT_TITLE "Little Game Project"

#define FRAMES_PER_SECOND 30

// Camera defines
#define STARTING_FOV 75;
#define ZFAR 100.0f;
#define ZNEAR 0.1f;
const glm::vec3 GLOBAL_UP_VECTOR = glm::vec3(0.0f, 1.0f, 0.0f);
