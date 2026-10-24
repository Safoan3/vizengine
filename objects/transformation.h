
#pragma once
#include "../../../graphicsapi/graphicsapi.h"
#include "../math/trighelper.h"
#include "object.h"
#include <cmath>
#include <utility>
#include <vector>

namespace transformation {

inline void updateBoundingBox(instanceTypes::basepart *ins) {
  if (!ins || ins->vertices.empty())
    return;

  const float inf = std::numeric_limits<float>::max();
  float extremeLeft = inf, extremeRight = -inf;
  float extremeTop = -inf, extremeBottom = inf;
  float extremeDepth = -inf, extremeNegDepth = inf;

  for (const auto &v : ins->vertices) {
    extremeLeft = std::min(extremeLeft, v.x);
    extremeRight = std::max(extremeRight, v.x);
    extremeBottom = std::min(extremeBottom, v.y);
    extremeTop = std::max(extremeTop, v.y);
    extremeNegDepth = std::min(extremeNegDepth, v.z);
    extremeDepth = std::max(extremeDepth, v.z);
  }

  ins->boundingBoxVert = {
      {extremeLeft, extremeTop, extremeDepth},
      {extremeRight, extremeTop, extremeDepth},
      {extremeRight, extremeBottom, extremeDepth},
      {extremeLeft, extremeBottom, extremeDepth},

      {extremeLeft, extremeBottom, extremeNegDepth},
      {extremeRight, extremeBottom, extremeNegDepth},
      {extremeLeft, extremeTop, extremeNegDepth},
      {extremeRight, extremeTop, extremeNegDepth},
  };
}
inline void scale(instanceTypes::basepart *ins) {
  if (auto *pt = dynamic_cast<instanceTypes::basepart *>(ins)) {
    for (int i = 0; i < pt->dvertices.size(); i++) {
      vec3df v = pt->dvertices[i];

      vec3df nc2 = {v.x * -pt->scale.x, v.y * pt->scale.y, v.z * pt->scale.z};

      pt->vertices[i] = nc2;
    };
  }
}

inline void rotate(instanceTypes::basepart *ins) {
  if (auto *pt = dynamic_cast<instanceTypes::basepart *>(ins)) {

    float rx = pt->rotX;
    float ry = pt->rotY;
    float rz = pt->rotZ;

    for (int i = 0; i < pt->dvertices.size(); i++) {
      vec3df v = pt->dvertices[i];
      vec3df nc = {v.x * pt->scale.x, v.y * pt->scale.y, v.z * pt->scale.z};

      vec3df nc2 = rotOperation(nc, rx, ry, rz);

      pt->vertices[i] = nc2;
    };
    for (int i = 0; i < pt->dnormals.size(); i++) {
      vec3df v = pt->dnormals[i];
      vec3df nc2 = rotOperation(v, rx, ry, rz);

      pt->normals[i] = nc2;
    };
  }
}
} // namespace transformation
