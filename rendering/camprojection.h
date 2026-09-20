#pragma once

#include "../../../graphicsapi/graphicsapi.h"
#include "camera.h"

inline vec2df perspective(vec3df a) {
  vec3df p1 = a;
  float iz = 1.0f;
  float forcalLength = 240.0f;

  if (a.z < 0.001f) {
    iz = 0.001f;
  } else {
    iz = 1.0f / a.z;
  }

  vec2df ret = {p1.x * iz * forcalLength, p1.y * iz * forcalLength};
  return ret;
};

inline vec2df project(const camera &cam, vec3df p1, vec2 resolution) {
  vec3df pos = cam.position;
  vec3df dir = cam.direction;
  dir.normalize();

  vec3df left = dir.cross({0, 1.0f, 0});
  left = left.normalize();

  vec3df top = left.cross(dir);

  vec3df nP1 = p1 - pos;
  float zmag = nP1.magnitude();

  vec2df newProj = perspective({nP1.dot(left), nP1.dot(top), zmag});
  vec2df screenpos = {(float)resolution.x / 2.0f, (float)resolution.y / 2.0f};

  newProj = newProj + screenpos;

  return newProj;
};
