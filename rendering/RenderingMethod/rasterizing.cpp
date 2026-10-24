#include "../../objects/transformation.h"
#include "../render.h"
#include "umbrella.h"
#include <utility>
#include <vector>

float near = 0.25f;

struct Triangle {
  vec3df v1;
  vec3df v2;
  vec3df v3;
};

static std::pair<Triangle, Triangle> nearPlane(vec3df v1, vec3df v2, vec3df v3,
                                               float dot1, float dot2,
                                               float dot3, int count,
                                               camera camera) {

  // case 1

  std::pair<vec3df, vec3df> pair1;
  std::pair<vec3df, vec3df> pair2;
  vec3df share;

  std::pair<Triangle, Triangle> t;

  if (count == 1) {
    if (dot1 > 0.0f) {
      pair1.first = v1;
      pair1.second = v2;

      pair2.first = v1;
      pair2.second = v3;
      share = v1;
    } else if (dot2 > 0.0f) {
      pair1.first = v2;
      pair1.second = v1;

      pair2.first = v2;
      pair2.second = v3;
      share = v2;
    } else if (dot3 > 0.0f) {
      pair1.first = v3;
      pair1.second = v2;

      pair2.first = v3;
      pair2.second = v1;
      share = v3;
    }

  } else if (count == 2) {
    if (dot1 < 0.0f) {
      pair1.first = v1;
      pair1.second = v2;

      pair2.first = v1;
      pair2.second = v3;
      share = v1;
    } else if (dot2 < 0.0f) {
      pair1.first = v2;
      pair1.second = v1;

      pair2.first = v2;
      pair2.second = v3;
      share = v2;
    } else if (dot3 < 0.0f) {
      pair1.first = v3;
      pair1.second = v2;

      pair2.first = v3;
      pair2.second = v1;
      share = v3;
    }
  }
  /*
  std::cout << "dot: " << dot1 << ", " << dot2 << ", " << dot3 << "\n";
  v1.print();
  v2.print();
  v3.print();

  std::cout << "............... \n";
  pair1.first.print();
  pair1.second.print();
  pair2.first.print();
  pair2.second.print();
  std::cout << "\n";
  share.print();
  std::cout << "............... \n";
*/
  float c1 = (near - camera.direction.dot(pair1.first - camera.position)) /
             (pair1.second - pair1.first).dot(camera.direction.normalize());

  float c2 = (near - camera.direction.dot(pair2.first - camera.position)) /
             (pair2.second - pair2.first).dot(camera.direction.normalize());

  vec3df newVert1 = ((pair1.second - pair1.first) * c1) + pair1.first;
  vec3df newVert2 = ((pair2.second - pair2.first) * c2) + pair2.first;

  if (count == 1) {
    // 1 positive vertex (share) + 2 intersections → 1 triangle
    // Second triangle is empty
    return {{share, newVert1, newVert2}, {}};
  } else {
    // 2 positive vertices (pair1.second, pair2.second) + 2 intersections
    // → quad → 2 triangles
    vec3df pos1 = pair1.second;
    vec3df pos2 = pair2.second;
    return {{pos1, pos2, newVert2}, {pos1, newVert2, newVert1}};
  }
}

static void screenProject(vec3df v1, vec3df v2, vec3df v3, const enj engine,
                          const camera cam, vec2 &pz, vec2 &py, vec2 &px) {
  vec2df vp = project(cam, v1, engine.resolution, engine.focalpoint);
  vec2df vp2 = project(cam, v2, engine.resolution, engine.focalpoint);
  vec2df vp3 = project(cam, v3, engine.resolution, engine.focalpoint);
  pz = {(int)vp.x, (int)vp.y};
  py = {(int)vp2.x, (int)vp2.y};
  px = {(int)vp3.x, (int)vp3.y};
}

std::vector<int> boundingBoxEdges = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6,
                                     6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};

void renderingmethods::rasterize(enj &engine) {
  engine.env->clear();
  camera &cam = engine.currentCam;

  int trianglesOnScene = 0;

  for (instance *ob : engine.typeinstances[typeid(instanceTypes::basepart)]) {

    if (auto *pt = dynamic_cast<instanceTypes::basepart *>(ob)) {

      instanceTypes::basepart &o = (*pt);

      if (pt->showSelection) {

        transformation::updateBoundingBox(pt);
        for (int i = 0; i < o.boundingBoxVert.size(); i++) {
          if (i < pt->boundingBoxVert.size()) {
            /*
            int edge1 = boundingBoxEdges[i];
            int edge2 = boundingBoxEdges[i + 1];

            vec3df v1 = o.boundingBoxVert[edge1] + o.position;
            vec3df v2 = o.boundingBoxVert[edge2] + o.position;
*/
            vec3df v1 = pt->boundingBoxVert[i] + o.position;

            vec3df v1Rel = v1 - cam.position;
            // vec3df v2Rel = v2 - cam.position;

            vec3df camDir = engine.currentCam.direction.normalize();
            float dot1 = v1Rel.dot(camDir);
            // float dot2 = v2Rel.dot(camDir);

            vec2df vp = project(cam, v1, engine.resolution, engine.focalpoint);
            // vec2df vp2 = project(cam, v2, engine.resolution,
            // engine.focalpoint);
            vec2 pz = {(int)vp.x, (int)vp.y};
            // vec2 py = {(int)vp2.x, (int)vp2.y};

            // engine.env->line(pz, py, {1.0f, 0.0f, 0.0f, 1.0f, 1.0f}, dot1,
            // dot2);
            engine.env->setPixel(pz, {1.0f, 1.0f, 0, 1.0f});
          }
        }
      }
      if (pt->transparency == 1.0f)
        continue;

      for (int edgeIndex = 0; edgeIndex + 2 < o.edges.size();
           edgeIndex = edgeIndex + 3) {

        int edge1 = o.edges[edgeIndex];
        int edge2 = o.edges[edgeIndex + 1];
        int edge0 = o.edges[edgeIndex + 2];

        {
          vec3df v1 = o.vertices[edge1] + o.position;
          vec3df v2 = o.vertices[edge2] + o.position;

          vec3df v3 = o.vertices[edge0] + o.position;

          vec3df v1Rel = v1 - cam.position;
          vec3df v2Rel = v2 - cam.position;

          vec3df v3Rel = v3 - cam.position;

          vec3df normal_ = (v2 - v1).cross((v3 - v1)).normalize();

          vec3df camdir = cam.direction.normalize();

          float dot1 = v1Rel.dot(camdir);
          float dot2 = v2Rel.dot(camdir);

          float dot3 = v3Rel.dot(camdir);

          float dot = normal_.dot(camdir);

          int count = 0;
          if (dot1 > 0.0f)
            count++;
          if (dot2 > 0.0f)
            count++;
          if (dot3 > 0.0f)
            count++;

          vec2 pz, py, px;
          screenProject(v1, v2, v3, engine, cam, pz, py, px);

          float shading = 0.0f;
          float ambient = 0.35f;

          shading = normal_.dot(cam.direction);

          shading = ambient + (1 - ambient) * shading;
          shading = std::clamp(shading, 0.0f, 1.0f);
          if (pt->reflectance > 0.0f) {
            shading = std::pow(shading, 1.0f + (3.0f * pt->reflectance));
          }
          // std::cout << shading << "\n";
          if (pt->material == "neon") {
            shading = 1.0f;
          }
          pixel pix = {1.0f, o.col3.r * shading, o.col3.g * shading,
                       o.col3.b * shading, o.col3.alpha};

          if (count > 0 && count < 3) {
            auto ptTrig = nearPlane(v1, v2, v3, dot1, dot2, dot3, count, cam);
            if (count == 2) {
              vec2 pz, py, px;
              screenProject(ptTrig.first.v1, ptTrig.first.v2, ptTrig.first.v3,
                            engine, cam, pz, py, px);

              engine.env->drawTriangle(
                  py, px, pz, pix, (ptTrig.first.v2 - cam.position).dot(camdir),
                  (ptTrig.first.v3 - cam.position).dot(camdir),
                  (ptTrig.first.v1 - cam.position).dot(camdir));

              screenProject(ptTrig.second.v1, ptTrig.second.v2,
                            ptTrig.second.v3, engine, cam, pz, py, px);
              engine.env->drawTriangle(
                  py, px, pz, pix,
                  (ptTrig.second.v2 - cam.position).dot(camdir),
                  (ptTrig.second.v3 - cam.position).dot(camdir),
                  (ptTrig.second.v1 - cam.position).dot(camdir));
            } else {
              vec2 pz, py, px;
              screenProject(ptTrig.first.v1, ptTrig.first.v2, ptTrig.first.v3,
                            engine, cam, pz, py, px);
              engine.env->drawTriangle(
                  py, px, pz, pix, (ptTrig.first.v2 - cam.position).dot(camdir),
                  (ptTrig.first.v3 - cam.position).dot(camdir),
                  (ptTrig.first.v1 - cam.position).dot(camdir));
            }

          } else if (dot >= -0.1f && count >= 3) {
            engine.env->drawTriangle(py, px, pz, pix, dot2, dot3, dot1);

            trianglesOnScene++;
          } else {
            // std::cout << "out of sight! \n";
          }
        }
      }
    }
  };

  // std::cout << " \n triangles on scene : " << trianglesOnScene << "\n";

  // engine.env->render();
}
