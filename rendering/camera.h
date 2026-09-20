#pragma once
#include "../../../graphicsapi/graphicsapi.h"

struct camera {
  vec3df position;
  vec3df direction;
  float pitch = 0.0f;
  float yaw = 0.0f;
  float roll = 0.0f;
};
