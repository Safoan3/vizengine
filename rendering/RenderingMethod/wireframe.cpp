#include "../render.h"
#include "umbrella.h"

void renderingmethods::wireframe(enj &engine) {
  engine.env->clear();
  camera &cam = engine.currentCam;

  for (instance *ob : engine.instances) {

    if (auto *pt = dynamic_cast<instanceTypes::basepart *>(ob)) {
      instanceTypes::basepart o = (*pt);

      for (int edgeIndex = 0; edgeIndex < o.edges.size() - 2;
           edgeIndex = edgeIndex + 3) {
        int edge1 = o.edges[edgeIndex];
        int edge2 = o.edges[edgeIndex + 1];
        int edge3 = o.edges[edgeIndex + 2];

        vec3df v1 = o.vertices[edge1] + o.position;
        vec3df v2 = o.vertices[edge2] + o.position;
        vec3df v3 = o.vertices[edge3] + o.position;

        vec3df v1Rel = v1 - cam.position;
        vec3df v2Rel = v2 - cam.position;
        vec3df v3Rel = v3 - cam.position;

        float dot1 = v1Rel.dot(cam.direction);
        float dot2 = v2Rel.dot(cam.direction);
        float dot3 = v3Rel.dot(cam.direction);

        //        std::cout << "{dot1, dot2} = " << dot1 << ", " << dot2 <<
        //        "\n";
        //
        if (dot1 > 0 && dot2 > 0 && dot3 > 0) {
          vec2df vp = project(cam, v1, engine.resolution, engine.focalpoint);
          vec2df vp2 = project(cam, v2, engine.resolution, engine.focalpoint);
          vec2df vp3 = project(cam, v3, engine.resolution, engine.focalpoint);

          vec2 pz = {(int)vp.x, (int)vp.y};
          vec2 py = {(int)vp2.x, (int)vp2.y};
          vec2 px = {(int)vp3.x, (int)vp3.y};

          float shading = 0.0f;
          float ambient = 0.35f;

          if (edge1 < o.normals.size()) {
            shading = o.vertices[edge1].dot(lightDirX * -1) / 1.0f;
            shading = std::clamp(shading, 0.0f, 1.0f);
          }

          shading = ambient + (1 - ambient) * shading;

          engine.env->line(pz, py,
                           {shading, o.col3.r * shading, o.col3.g * shading,
                            o.col3.b * shading},
                           dot1, dot2);
          engine.env->line(pz, px,
                           {shading, o.col3.r * shading, o.col3.g * shading,
                            o.col3.b * shading},
                           dot1, dot3);
          engine.env->line(px, py,
                           {shading, o.col3.r * shading, o.col3.g * shading,
                            o.col3.b * shading},
                           dot3, dot2);

        } else {
          // std::cout << "out of sight! \n";
        }
      }
    }
  };
  // engine.env->render();
}
