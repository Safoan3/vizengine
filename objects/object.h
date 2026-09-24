#pragma once
#include "../math/vectorm.h"

#include "classification.h"
#include <vector>

struct object {
  std::string name;

  std::vector<vec3df> vertices;
  std::vector<vec3df> dvertices;
  std::vector<vec3df> normals;
  std::vector<vec3df> dnormals;
  std::vector<int> edges;

  int Type;

  color col3;
  vec3df position;
  vec3df scale = {1, 1, 1};

  float rotX, rotY, rotZ;
};

struct instance {
  std::string name;

  gameObjects::objtypes type;
  std::vector<instance *> children;
  instance *parent;
  virtual ~instance() = default; // makes it polymorphic

  void setParent(instance *p) {
    parent = p;
    p->children.push_back(this);
  }
};

namespace instanceTypes {

struct game : instance {
  bool physics = false;

  game() { type = gameObjects::game; };
};

struct basepart : instance {
  std::vector<vec3df> vertices;
  std::vector<vec3df> dvertices;
  std::vector<vec3df> normals;
  std::vector<vec3df> dnormals;
  std::vector<int> edges;

  color col3;
  vec3df position;
  vec3df scale = {1, 1, 1};
  std::string material = "normal";

  float rotX, rotY, rotZ;

  basepart() { type = gameObjects::basepart; }
};

struct camera : instance {
  vec3df position = {0, 0, 0};
  vec3df direction = {1, 0, 0};
  vec3df rotation = {0, 0, 0};
  int camType = 0;
};

struct lightsource : instance {
  vec3df position = {0, 0, 0};
  vec3df direction = {1, 0, 0};
  float spread;
  float brightness;
  float range;
  // color color;
};

struct group : instance {
  vec3df position = {0, 0, 0};
  vec3df rot = {0, 0, 0};

  // color color;
};

} // namespace instanceTypes
