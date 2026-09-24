#include "../render.h"
#include "umbrella.h"

void renderingmethods::rasterize(enj &engine) {
  engine.env->clear();
  camera &cam = engine.currentCam;

  for (instance *ob : engine.typeinstances[typeid(instanceTypes::basepart)]) {
    if (auto *pt = dynamic_cast<instanceTypes::basepart *>(ob)) {
      instanceTypes::basepart o = (*pt);

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

            vec2df vp = project(cam, v1, engine.resolution, engine.focalpoint);
            vec2df vp2 = project(cam, v2, engine.resolution, engine.focalpoint);
            vec2df vp3 = project(cam, v3, engine.resolution, engine.focalpoint);

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
            shading = std::clamp(shading, 0.0f, 1.0f);
            // std::cout << shading << "\n";
            if (pt->material == "neon") {
              shading = 1.0f;
            }

            engine.env->drawTriangle(py, px, pz,
                                     {1.0f, o.col3.r * shading,
                                      o.col3.g * shading, o.col3.b * shading,
                                      1.0f},
                                     dot2, dot3, dot1);

          } else {
            // std::cout << "out of sight! \n";
          }
        }
      }
    }
  };
  // engine.env->render();
}
