#include "render.h"
#include "../raycasting/raycast.h"

#include "camera.h"
#include "iostream"
// vec3df lightDirX = {0.0f, -1.0f, 1.0f};
#include <algorithm>
#include <cstdlib>
#include <random>
#include <vector>

void renderingmethods::wireframe(enj &engine) {
  engine.env->clear();
  camera &cam = engine.currentCam;

  for (object *ob : engine.objects) {
    object o = (*ob);

    for (int edgeIndex = 0; edgeIndex < o.edges.size();
         edgeIndex = edgeIndex + 2) {

      int edge2 = -1;
      int edge1 = o.edges[edgeIndex];

      if (edgeIndex + 1 < o.edges.size()) {
        edge2 = o.edges[edgeIndex + 1];
      }

      if (edge2 > -1 && edge2 < o.vertices.size()) {
        vec3df v1 = o.vertices[edge1] + o.position;
        vec3df v2 = o.vertices[edge2] + o.position;

        vec3df v1Rel = v1 - cam.position;
        vec3df v2Rel = v2 - cam.position;

        float dot1 = v1Rel.dot(cam.direction);
        float dot2 = v2Rel.dot(cam.direction);
        //        std::cout << "{dot1, dot2} = " << dot1 << ", " << dot2 <<
        //        "\n";
        //
        if (dot1 > 0 && dot2 > 0) {
          vec2df vp = project(cam, v1, engine.resolution);
          vec2df vp2 = project(cam, v2, engine.resolution);

          vp = vp;
          vp2 = vp2;

          vec2 pz = {(int)vp.x, (int)vp.y};
          vec2 py = {(int)vp2.x, (int)vp2.y};

          float shading = 0.0f;
          float ambient = 0.35f;

          if (edge1 < o.normals.size()) {
            shading = o.vertices[edge1].dot(lightDirX * -1) / 1.0f;
            //  shading = shading + o.vertices[edge2].dot(lightDir * -1) / 2.0f;

            shading = std::clamp(shading, 0.0f, 1.0f);
          }

          float z1relcam = v1Rel.magnitude();
          float z2relcam = v2Rel.magnitude();

          shading = ambient + (1 - ambient) * shading;

          engine.env->line(pz, py,
                           {shading, o.col3.r * shading, o.col3.g * shading,
                            o.col3.b * shading},
                           z1relcam, z2relcam);

        } else {
          // std::cout << "out of sight! \n";
        }
      }
    }
  };
  // engine.env->render();
}

void renderingmethods::rasterize(enj &engine) {
  engine.env->clear();
  camera &cam = engine.currentCam;

  for (object *ob : engine.objects) {
    object o = (*ob);

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

        //        dot = 1.0f;

        //        std::cout << "{dot1, dot2} = " << dot1 << ", " << dot2 <<
        //        "\n";
        //
        //  std::cout << dot << "\n";
        //  dot = 1.0f;
        //

        bool onScreen = dot1 > 0 && dot2 > 0 && dot3 > 0;

        if (dot > -0.1f && onScreen) {

          vec2df vp = project(cam, v1, engine.resolution);
          vec2df vp2 = project(cam, v2, engine.resolution);
          vec2df vp3 = project(cam, v3, engine.resolution);

          vec2 pz = {(int)vp.x, (int)vp.y};
          vec2 py = {(int)vp2.x, (int)vp2.y};
          vec2 px = {(int)vp3.x, (int)vp3.y};

          float shading = 0.0f;
          float ambient = 0.35f;

          shading = normal_.dot(cam.direction);

          float z1relcam = v1Rel.magnitude();
          float z2relcam = v2Rel.magnitude();
          float z3relcam = v3Rel.magnitude();

          shading = ambient + (1 - ambient) * shading;
          shading = 1.0f;
          shading = std::clamp(shading, 0.0f, 1.0f);
          // std::cout << shading << "\n";
          engine.env->drawTriangle(py, px, pz,
                                   {shading, o.col3.r * shading,
                                    o.col3.g * shading, o.col3.b * shading},
                                   dot2, dot3, dot1);

        } else {
          // std::cout << "out of sight! \n";
        }
      }
    }
  };
  // engine.env->render();
}

void renderingmethods::dotted(enj &engine) {

  engine.env->clear();
  camera &cam = engine.currentCam;

  for (object *ob : engine.objects) {
    object o = (*ob);
    for (vec3df vert : ob->vertices) {
      vec3df v1 = vert + o.position;

      vec3df v1Rel = v1 - cam.position;

      float dot1 = v1Rel.dot(cam.direction);

      if (dot1 > 0) {
        vec2df vp = project(cam, v1, engine.resolution);

        vec2 pz = {(int)vp.x, (int)vp.y};

        float shading = 0.0f;
        float ambient = 0.15f;

        shading = vert.dot(lightDirX * -1);
        shading = std::clamp(shading, 0.0f, 1.0f);

        float z1relcam = v1Rel.magnitude();
        shading = ambient + (1 - ambient) * shading;

        engine.env->setPixel(pz,
                             {shading, o.col3.r * shading, o.col3.g * shading,
                              o.col3.b * shading},
                             z1relcam);

      } else {
        // std::cout << "out of sight! \n";
      }
    }
  }
  // engine.env->render();
}

void renderingmethods::debugs::zbuffdebug(enj &engine) {
  Xspace *env = engine.env;
  std::vector<pixel> tempBuff;

  for (int j = engine.resolution.y - 1; j >= 0; j--) {
    for (int i = 0; i < engine.resolution.x; i++) {
      float num = env->zbuffer[j * engine.resolution.x + i];
      float normalized = num / 70.0f;

      if (normalized > 1.0f)
        normalized = 1.0f;
      if (normalized < 0.0f)
        normalized = 0.0f;
      float anti = 1.0f - normalized;

      float alpha = 1.0f;
      if (num > 1000.0f) {
        alpha = 0.0f;
      }

      tempBuff.push_back({1.0f, anti, 0.0, normalized, alpha});
    }
  }

  int ind = 0;
  for (pixel pix : tempBuff) {
    env->buffer[ind] = pix;
    ind++;
  }
  tempBuff.clear();
}

// raytracer final molla boss
//

void lowraytrace(enj &engine, float noise) {
  // engine.env->clear();
  camera &cam = engine.currentCam;
  int resx = engine.resolution.x;
  int resy = engine.resolution.y;

  // SETTINGS //
  float fov = 0.5f; // radians, tune to taste
  float aspect = (float)resx / (float)resy;

  std::vector<pixel> tempBuff;
  vec3df forward = cam.direction.normalize();
  vec3df leftVec = vec3df{0, 1.0f, 0}.cross(forward).normalize() *
                   -1.0f; // actually "right" — see note
  vec3df topVec = leftVec.cross(forward).normalize();

  object *lightObj = engine.findobj("light");
  vec3df lightPos = cam.position + vec3df{0, 10.0f, 0};

  if (lightObj != nullptr) {
    lightPos = lightObj->position;
  }

  for (int j = 0; j < resy; j++) {
    for (int i = 0; i < resx; i++) {
      int precisionRand = rand() % 1001;
      float rand = (float)precisionRand / 1000.0f;

      //   std::cout << rand << "\n";
      //
      bool skip = false;
      if (rand < noise) {
        skip = true;
      }
      pixel newPix = {0.9f, rand, rand, rand, 1.0f};

      if (skip == false) {
        float ndcX =
            (2.0f * (i + 0.5f) / resx - 1.0f) * aspect * tanf(fov * 0.5f);
        float ndcY = (1.0f - 2.0f * (j + 0.5f) / resy) * tanf(fov * 0.5f);

        vec3df dir = (forward + leftVec * ndcX + topVec * ndcY).normalize();

        raycast::result newR = raycast::raycastEnj(engine, cam.position, dir);

        if (newR.hit == true) {
          newPix = {1.0f, newR.r, newR.g, newR.b, 1.0f};
          vec3df newOrigin = newR.hitpos + (newR.normal * 0.001f);

          raycast::result lightR = raycast::raycastEnj(
              engine, newOrigin, (lightPos - newOrigin).normalize());

          float ambient = 0.9f;

          if (lightR.hit) {
            ambient = 0.3;
          }
          newPix.a = 1.0f;

          newPix.r = newPix.r * ambient;
          newPix.g = newPix.g * ambient;
          newPix.b = newPix.b * ambient;
        } else {
          newPix = {1.0f, 0.3f, 0.3f, 0.7f, 1.0f};
        };
      }
      tempBuff.push_back(newPix);
    }
  }
  int ind = 0;
  for (pixel pix : tempBuff) {
    pixel np = engine.env->buffer[ind];
    int precisionRand = rand() % 1001;
    float rand = (float)precisionRand / 1000.0f;

    if (pix.a == 1.0f) {
      engine.env->buffer[ind] = pix;
    }
    ind++;
  }
  tempBuff.clear();

  // engine.env->render();
}

void renderingmethods::raytracer(enj &engine) { lowraytrace(engine, 0.1f); }
