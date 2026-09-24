#pragma once
#include "../engine/engine.h"
#include "barycentric.h"

namespace raycast {

struct result {
  vec3df normal;
  float r, g, b;

  vec3df hitpos;
  float distance;

  vec2df bar;
  bool hit = false;
};

struct resultenj {
  vec3df normal;
  float r, g, b;
  vec3df hitpos;
  float distance;
  instance *hitobj;
  vec2df bar;
  bool hit;
};
result lowCast(vec3df Origin, vec3df Direction, vec3df c1, vec3df c2,
               vec3df c3);

resultenj raycastEnj(enj &engine, vec3df origin, vec3df dir,
                     std::vector<instanceTypes::basepart *> iignoreList = {});

} // namespace raycast
