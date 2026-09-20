
#pragma once
#include "../../../graphicsapi/graphicsapi.h"
#include "../math/trighelper.h"
#include "object.h"

namespace transformation {

void scale(object *obj) {
  for (int i = 0; i < obj->dvertices.size(); i++) {
    vec3df v = obj->dvertices[i];
    vec3df nc2 = {v.x * obj->scale.x, v.y * obj->scale.y, v.z * obj->scale.z};

    obj->vertices[i] = nc2;
  };
}

void rotate(object *obj) {
  float rx = obj->rotX;
  float ry = obj->rotY;
  float rz = obj->rotZ;

  for (int i = 0; i < obj->dvertices.size(); i++) {
    vec3df v = obj->dvertices[i];
    vec3df nc = {v.x * obj->scale.x, v.y * obj->scale.y, v.z * obj->scale.z};

    vec3df nc2 = rotOperation(nc, rx, ry, rz);

    obj->vertices[i] = nc2;
  };
  for (int i = 0; i < obj->dnormals.size(); i++) {
    vec3df v = obj->dnormals[i];
    vec3df nc2 = rotOperation(v, rx, ry, rz);

    obj->normals[i] = nc2;
  };
}
} // namespace transformation
