#include "raycast.h"
#include <cmath>
#include <utility>

#include <iostream>

std::pair<vec3df, vec3df> getAxes(vec3df c1, vec3df c2, vec3df c3) {
  vec3df origin = c1;
  vec3df p1 = c2 - origin;
  vec3df p2 = c3 - origin;

  vec3df normal = p1.cross(p2).normalize();
  vec3df xaxis = p1.cross(normal).normalize();

  vec3df yaxis = normal.cross(xaxis).normalize();
  return {xaxis, yaxis};
};

vec2df convert(vec3df point, std::pair<vec3df, vec3df> axes) {
  return {point.dot(axes.first), point.dot(axes.second)};
}

raycast::result raycast::lowCast(vec3df Origin, vec3df Direction, vec3df c1,
                                 vec3df c2, vec3df c3) {
  raycast::result toRet;

  vec3df c1n = c2 - c1;
  vec3df c2n = c3 - c1;
  vec3df normal = c1n.cross(c2n).normalize();

  float dot = normal.dot(Direction.normalize());

  if (dot < 0.0001f) {
    return toRet;
  }

  float D = (normal.dot(c1));
  // Ax+By+Cz+D = 0

  float t = 0.0f;
  vec3df P = Origin + (Direction * t);
  t = (D - normal.x * Origin.x - normal.y * Origin.y - normal.z * Origin.z) /
      (normal.x * Direction.x + normal.y * Direction.y +
       normal.z * Direction.z);

  toRet.hitpos = (Origin + (Direction * t));
  toRet.normal = normal;

  auto axes = getAxes(c1, c2, c3);
  vec2df localC1 = {0, 0};
  vec2df localC2 = convert(c1n, axes);
  vec2df localC3 = convert(c2n, axes);
  vec2df localHit = convert(toRet.hitpos - c1, axes);

  vec2df bary = barycentric32(localC1, localC2, localC3, localHit);

  toRet.bar = bary;
  if (bary.x > 0 && bary.y > 0 && bary.x + bary.y <= 1.0f) {
    toRet.hit = true;

    toRet.distance = t;
  }
  if (t < 1e-4f) {
    toRet.hit = false;
  }

  return toRet;
}

raycast::resultenj
raycast::raycastEnj(enj &enjine, vec3df origin, vec3df dir,
                    std::vector<instanceTypes::basepart *> iignoreList) {

  std::pair<raycast::result, float> Prev;
  Prev.second = INFINITY;

  instanceTypes::basepart *ho = nullptr;

  for (instance *ob : enjine.typeinstances[typeid(instanceTypes::basepart)]) {
    if (auto *pt = dynamic_cast<instanceTypes::basepart *>(ob)) {
      bool ignoreFound = false;

      for (instanceTypes::basepart *ok : iignoreList) {
        if (ok != nullptr && ok == pt) {
          ignoreFound = true;
          break;
        }
      }
      if (ignoreFound == true)
        continue;

      for (int i = 0; i + 2 < pt->edges.size(); i = i + 3) {
        vec3df vert1 = pt->vertices[pt->edges[i]] + pt->position;
        vec3df vert2 = pt->vertices[pt->edges[i + 1]] + pt->position;
        vec3df vert3 = pt->vertices[pt->edges[i + 2]] + pt->position;

        raycast::result result =
            raycast::lowCast(origin, dir, vert1, vert2, vert3);

        if (result.hit == true) {
          if (result.distance < Prev.second) {
            result.r = pt->col3.r;
            result.g = pt->col3.g;
            result.b = pt->col3.b;

            Prev.first = result;
            Prev.second = result.distance;
            ho = pt;
          }
        }
      }
    }
  }

  return {Prev.first.normal, Prev.first.r,        Prev.first.g, Prev.first.b,
          Prev.first.hitpos, Prev.first.distance, ho,           Prev.first.bar,
          Prev.first.hit

  };
};
