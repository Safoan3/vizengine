#pragma once
#include "../math/vectorm.h"

inline vec2df barycentric32(vec2df cord1, vec2df cord2, vec2df cord3,
                            vec2df point) {
  vec2df d1 = cord1;
  vec2df d2 = cord2;
  vec2df d3 = cord3;

  vec2df p1 = point - cord1;
  d1 = d1 - cord1;
  d2 = d2 - cord1;
  d3 = d3 - cord1;

  float Ax = (float)d2.x;
  float Ay = (float)d2.y;

  float Bx = (float)d3.x;
  float By = (float)d3.y;

  float px = (float)p1.x;
  float py = (float)p1.y;

  float determinate = Ax * By - Bx * Ay;
  float dx = px * By - Bx * py;
  float dy = Ax * py - Ay * px;

  float w1 = (By * px - Bx * py) / (By * Ax - Bx * Ay);
  float w2 = (py - (Ay * w1)) / By;

  w1 = dx / determinate;
  w2 = dy / determinate;

  return {w1, w2};
};
