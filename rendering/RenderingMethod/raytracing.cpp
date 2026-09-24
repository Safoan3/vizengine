#include "../render.h"
#include "umbrella.h"

void lowraytrace(enj &engine, float noise) {
  // engine.env->clear();
  camera &cam = engine.currentCam;
  int resx = engine.resolution.x;
  int resy = engine.resolution.y;

  // SETTINGS //
  float fov = 0.15f; // radians, tune to taste
  float aspect = (float)resx / (float)resy;

  std::vector<pixel> tempBuff;
  vec3df forward = cam.direction.normalize();
  vec3df leftVec = vec3df{0, 1.0f, 0}.cross(forward).normalize() *
                   -1.0f; // actually "right" — see note
  vec3df topVec = leftVec.cross(forward).normalize();

  instance *lightObjAz = engine.findobj("light");

  auto *lightObj = dynamic_cast<instanceTypes::basepart *>(lightObjAz);

  vec3df lightPos = cam.position + vec3df{0, 0.0f, 0};
  if (lightObj != nullptr) {
    lightPos = lightObj->position;
  } else {
  }

  lightPos.print();

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

        raycast::resultenj newR =
            raycast::raycastEnj(engine, cam.position, dir);

        if (newR.hit == true) {
          newPix = {1.0f, newR.r, newR.g, newR.b, 1.0f};
          vec3df newOrigin = newR.hitpos + (newR.normal * -0.001f);

          float maxDist = (lightPos - newOrigin).magnitude();
          float shadingDot =
              newR.normal.normalize().dot((lightPos - newR.hitpos).normalize());
          shadingDot = (shadingDot + 1.0f) * 0.5f;
          shadingDot = 1.0f - shadingDot;

          raycast::resultenj lightR = raycast::raycastEnj(
              engine, newOrigin, (lightPos - newOrigin).normalize(),
              {lightObj});

          float ambient = 0.9f;
          float reduction = 1.0f;
          if (lightR.hit && lightR.distance < maxDist) {
            reduction = 0.54;
          }

          ambient = ambient * shadingDot + (1 - ambient);

          ambient = ambient * reduction;

          newPix.a = 1.0f;
          newPix.r = newPix.r * ambient;
          newPix.g = newPix.g * ambient;
          newPix.b = newPix.b * ambient;
          newPix.alpha = 1.0f;

        } else {
          newPix = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
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

void renderingmethods::raytracer(enj &engine) { lowraytrace(engine, 0.98f); }
