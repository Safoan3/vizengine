#include "../render.h"
#include "umbrella.h"

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
