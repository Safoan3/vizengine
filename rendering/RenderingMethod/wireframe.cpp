#include "../render.h"
#include "umbrella.h"

void renderingmethods::wireframe(enj &engine) {
  engine.env->clear();
  camera &cam = engine.currentCam;

  for (instance *ob : engine.instances) {

    if (auto *pt = dynamic_cast<instanceTypes::basepart *>(ob)) {
      instanceTypes::basepart o = (*pt);

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
              //  shading = shading + o.vertices[edge2].dot(lightDir * -1)
              //  / 2.0f;

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
    }
  };
  // engine.env->render();
}
