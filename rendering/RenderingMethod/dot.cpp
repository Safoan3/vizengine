#include "../render.h"
#include "umbrella.h"

void renderingmethods::dotted(enj &engine) {

  engine.env->clear();
  camera &cam = engine.currentCam;

  for (instance *ob : engine.instances) {
    if (auto *pt = dynamic_cast<instanceTypes::basepart *>(ob)) {
      instanceTypes::basepart o = (*pt);

      for (vec3df vert : o.vertices) {
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
  }
  // engine.env->render();
}
