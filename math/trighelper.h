#pragma once
#include "../../../graphicsapi/graphicsapi.h"
const float pi = 3.14159265f;

vec3df EulerCord(float x, float y) {
  return {cosf(x) * cosf(y), sinf(y), sinf(x) * cosf(y)};
}

vec3df rotOperation(vec3df v, float rx, float ry, float rz) {
  float xx = v.x;
  float yy = v.y;
  float zz = v.z;
  float r = std::sqrt(xx * xx + yy * yy);

  float currentAngle = atan2(yy, xx);
  float newAngle = currentAngle + rx;
  vec3df nc = {cosf(newAngle) * r, sinf(newAngle) * r, zz};

  r = std::sqrt(nc.x * nc.x + nc.z * nc.z);
  currentAngle = atan2(nc.z, nc.x);
  newAngle = currentAngle + ry;
  vec3df nc1 = {sinf(newAngle) * r, nc.y, cosf(newAngle) * r};

  r = std::sqrt(nc1.z * nc1.z + nc1.y * nc1.y);

  currentAngle = atan2(nc1.z, nc1.y);

  newAngle = currentAngle + rz;
  vec3df nc2 = {nc1.x, cosf(newAngle) * r, sinf(newAngle) * r};

  return nc2;
}
