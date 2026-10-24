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
  std::vector<vec3df> boundingBoxVert;

  bool showSelection = false;

  color col3;
  vec3df position;
  vec3df scale = {1, 1, 1};
  float transparency = 0.0f;
  float reflectance = 0.0f;
  std::string material = "normal";
  float rotX, rotY, rotZ;

  basepart() { type = gameObjects::basepart; };
  ~basepart() { std::cout << "basepart destroyed \n"; };
};

struct line : instance {
  vec3df End = {0, 0, 0};
  vec3df Start = {0, 0, 0};
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
  float spread = 1.0f;
  float brightness = 1.0f;
  float range = 1;
  color col3 = {1, 1, 1, 1, 1};

  // color color;
};

struct group : instance {
  vec3df position = {0, 0, 0};
  vec3df rot = {0, 0, 0};

  ~group() { std::cout << "group destroyed \n"; };

  // color color;
};

struct lighting : instance {

  // raytracing
  bool AntiAlisaing;
  bool Shadows;

  /*


  int maxBounces = 4;

  int maxSamples = 2;
  float sampleDeviation = 0.0005f;
  int maxShadowSamples = 2;
  float shadowsampleDeviation = 0.1f;


    */
  float AntiAlisaingDeviation = 0.0005f;
  float RenderEffort;
  int MaxShadowSamples = 2;
  int MaxAntiAlisaingSamples = 2;
  float ShadowSmoothingDeviation = 0.1f;
  float ReflectionDeviation;
  int MaxReflectionSamples = 4;
  int MaxReflectionBounces = 4;
  float Ambient = 0.9;
  float IntensityMultiplier = 1.0f;

  // color color;
};

struct deformer : instance {
  vec3df LocalPosition;
  float strength = 1.0f;
  float radius = 0.5f;
};

struct scene : instance {
  vec3df gravity = {0, 0, 0};
};

} // namespace instanceTypes
