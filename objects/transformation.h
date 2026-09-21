
#pragma once
#include "../../../graphicsapi/graphicsapi.h"
#include "../math/trighelper.h"
#include "object.h"

namespace transformation {

void scale(instance *ins) {
  if (auto *pt = dynamic_cast<instanceTypes::basepart *>(ins)) {
    for (int i = 0; i < pt->dvertices.size(); i++) {
      vec3df v = pt->dvertices[i];

      vec3df nc2 = {v.x * pt->scale.x, v.y * pt->scale.y, v.z * pt->scale.z};

      pt->vertices[i] = nc2;
    };
  }
}

void rotate(instance *ins) {
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
