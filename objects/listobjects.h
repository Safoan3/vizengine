

#pragma once
#include "../math/trighelper.h"
#include "../math/vectorm.h"
#include "object.h"
#include <vector>

namespace list_objs {

instanceTypes::basepart *createTriangle(vec3df position, float length) {
  instanceTypes::basepart *newObj = new instanceTypes::basepart();

  newObj->position = position;
  newObj->col3 = {1.0f, 0.0f, 1.0f, 1.0f, 1.0f};
  newObj->name = "triangle";
  newObj->vertices.resize(4);
  newObj->edges.resize(6);

  std::cout << "cube TTT has been created on ";
  position.print();
  std::cout << "\n";

  std::vector<vec3df> vc = {
      {-1 * length, 1 * length, 1 * length},     // 0
      {1 * length, 1 * length, 1 * length},      // 1
      {-1 * length, -1 * length, 1.0f * length}, // 2
                                                 //
  };

  newObj->edges = {
      // Front face (z = +length), 2 triangles
      0,
      2,
      1,
  };

  newObj->vertices = vc;
  newObj->dvertices = vc;
  return newObj;
}
instanceTypes::basepart *createTriangleOctahedron(vec3df position,
                                                  float length) {
  instanceTypes::basepart *newObj = new instanceTypes::basepart();

  newObj->position = position;
  newObj->col3 = {1.0f, 0.0f, 1.0f}; // 3 components — adjust if yours is 4
  newObj->name = "octahedron";

  // Unit octahedron: poles on ±Y, ring on XZ plane. Scale by `length`.
  const float L = length;
  newObj->vertices = {
      {-L, 0, L},  // 0  ring
      {L, 0, L},   // 1  ring
      {-L, 0, -L}, // 2  ring
      {L, 0, -L},  // 3  ring
      {0, L, 0},   // 4  +Y pole
      {0, -L, 0},  // 5  -Y pole
  };

  // 8 triangular faces, CCW when viewed from outside.
  // Each triplet is (a, b, c) — 3 indices per face.
  newObj->edges = {
      // top half (+Y pole = 4)
      0,
      4,
      1,
      1,
      4,
      3,
      3,
      4,
      2,
      2,
      4,
      0,
      // bottom half (-Y pole = 5)
      1,
      5,
      0,
      3,
      5,
      1,
      2,
      5,
      3,
      0,
      5,
      2,
  };

  newObj->dvertices = newObj->vertices;
  return newObj;
}

struct objinf {
  std::vector<int> edges;
  std::vector<vec3df> vertices;
};

objinf reproduceTriangle(vec3df c1, vec3df c2, vec3df c3) {
  vec3df x0 = c1;
  vec3df x1 = (c1 + c2) * 0.5f;
  vec3df x2 = c2;
  vec3df x3 = (c2 + c3) * 0.5f;
  vec3df x4 = c3;
  vec3df x5 = (c3 + c1) * 0.5f;

  return {{0, 1, 5, 2, 3, 1, 5, 3, 4, 5, 1, 3}, {x0, x1, x2, x3, x4, x5}};
};

static void subdivideRecursive(const vec3df &v1, const vec3df &v2,
                               const vec3df &v3, int depth,
                               std::vector<vec3df> &outVerts,
                               std::vector<int> &outEdges) {
  if (depth <= 0) {
    int base = (int)outVerts.size();
    outVerts.push_back(v1);
    outVerts.push_back(v2);
    outVerts.push_back(v3);
    outEdges.push_back(base);
    outEdges.push_back(base + 1);
    outEdges.push_back(base + 2);
    return;
  }

  objinf lz = reproduceTriangle(v1, v2, v3);

  for (size_t i = 0; i + 2 < lz.edges.size(); i += 3) {
    vec3df a = lz.vertices[lz.edges[i]];
    vec3df b = lz.vertices[lz.edges[i + 1]];
    vec3df c = lz.vertices[lz.edges[i + 2]];
    subdivideRecursive(a, b, c, depth - 1, outVerts, outEdges);
  }
}

instanceTypes::basepart *createTriangleSphere(vec3df position, float length,
                                              int detail) {
  instanceTypes::basepart *newObj = new instanceTypes::basepart();
  newObj->position = position;
  newObj->col3 = {1.0f, 0.0f, 1.0f};
  newObj->name = "sphere";

  const float L = length;

  objinf vz;
  vz.vertices = {
      {L, 0, 0}, {-L, 0, 0}, {0, L, 0}, {0, -L, 0}, {0, 0, L}, {0, 0, -L},
  };

  vz.edges = {
      // +X pole (0)
      0,
      2,
      4,
      0,
      4,
      3,
      0,
      3,
      5,
      0,
      5,
      2,
      // -X pole (1)
      1,
      4,
      2,
      1,
      3,
      4,
      1,
      5,
      3,
      1,
      2,
      5,
  };

  for (size_t i = 0; i + 2 < vz.edges.size(); i += 3) {
    vec3df v1 = vz.vertices[vz.edges[i]];
    vec3df v2 = vz.vertices[vz.edges[i + 1]];
    vec3df v3 = vz.vertices[vz.edges[i + 2]];

    std::vector<vec3df> faceVerts;
    std::vector<int> faceEdges;
    subdivideRecursive(v1, v2, v3, detail, faceVerts, faceEdges);

    int base = (int)newObj->vertices.size();
    for (int e : faceEdges)
      newObj->edges.push_back(base + e);

    for (vec3df &pp : faceVerts) {
      float mag = pp.magnitude();
      if (mag > 0.f)
        pp = pp * (L / mag); // project to sphere radius L
      newObj->vertices.push_back(pp);
    }
  }

  newObj->dvertices = newObj->vertices;
  return newObj;
}

instanceTypes::basepart *createPlanerTriangle(vec3df position, float length,
                                              int depth) {
  instanceTypes::basepart *newObj = new instanceTypes::basepart();

  newObj->position = position;
  newObj->col3 = {1.0f, 1.0f, 0.5f, 0.2f, 1.0f};
  newObj->name = "plane";

  // depth now means "grid resolution" instead of recursion depth
  int gridRes = depth > 0 ? depth : 1;
  float step = (2.0f * length) / (float)gridRes; // plane spans -length..+length

  // 1. Generate vertices, row-major
  auto idx = [&](int x, int z) { return z * (gridRes + 1) + x; };

  for (int z = 0; z <= gridRes; z++) {
    for (int x = 0; x <= gridRes; x++) {
      float worldX = -length + x * step;
      float worldZ = -length + z * step;
      newObj->vertices.push_back({worldX, 0.0f, worldZ});
      newObj->dvertices.push_back({worldX, 0.0f, worldZ});
    }
  }

  // 2. Generate triangles per quad, alternating diagonal
  for (int z = 0; z < gridRes; z++) {
    for (int x = 0; x < gridRes; x++) {
      int a = idx(x, z);         // top-left
      int b = idx(x + 1, z);     // top-right
      int c = idx(x + 1, z + 1); // bottom-right
      int d = idx(x, z + 1);     // bottom-left

      bool flip = ((x + z) % 2 == 0);

      if (flip) {
        // diagonal a-c
        newObj->edges.push_back(a);
        newObj->edges.push_back(c);
        newObj->edges.push_back(b);

        newObj->edges.push_back(c);
        newObj->edges.push_back(a);
        newObj->edges.push_back(d);
      } else {
        // diagonal b-d
        newObj->edges.push_back(a);
        newObj->edges.push_back(d);
        newObj->edges.push_back(b);

        newObj->edges.push_back(c);
        newObj->edges.push_back(b);
        newObj->edges.push_back(d);
      }
    }
  }

  return newObj;
}

instanceTypes::basepart *createPlaneTrTriangle(vec3df position, float length,
                                               int depth) {
  instanceTypes::basepart *newObj = new instanceTypes::basepart();

  newObj->position = position;
  newObj->col3 = {1.0f, 1.0f, 0.5f, 0.2f, 1.0f};
  newObj->name = "plane";

  std::vector<vec3df> vzV = {
      {-1 * length, 0.0f * length, 1 * length},  // 0
      {1 * length, 0.0f * length, 1 * length},   // 1
      {-1 * length, 0.0f * length, -1 * length}, // 2
      {1 * length, 0.0f * length, -1 * length},  // 3

  };
  std::vector<int> vzE = {
      // Front face (z = +length), 2 triangles
      2, 0, 1, 3, 2, 1,
      // Back face (z = -length)
  };

  std::vector<vec3df> faceVerts;
  std::vector<int> faceEdges;

  for (size_t i = 0; i + 2 < vzE.size(); i += 3) {
    vec3df v1 = vzV[vzE[i]];
    vec3df v2 = vzV[vzE[i + 1]];
    vec3df v3 = vzV[vzE[i + 2]];

    std::vector<vec3df> faceVerts;
    std::vector<int> faceEdges;
    subdivideRecursive(v1, v2, v3, depth, faceVerts, faceEdges);

    int base = (int)newObj->vertices.size();
    for (int e : faceEdges)
      newObj->edges.push_back(base + e);

    for (vec3df &pp : faceVerts) {
      newObj->vertices.push_back(pp);
      newObj->dvertices.push_back(pp);
    }
  }

  //  newObj->edges = vzE;
  // newObj->vertices = vzV;
  // newObj->dvertices = vzV;
  return newObj;
}

instanceTypes::basepart *createCubeTrigangle(vec3df position, float length) {
  instanceTypes::basepart *newObj = new instanceTypes::basepart();

  newObj->position = position;
  newObj->col3 = {1.0f, 1.0f, 0};
  newObj->name = "Cube! ~//w//~";
  newObj->vertices.resize(8);
  newObj->edges.resize(24);

  std::cout << "cube TTT has been created on ";
  position.print();
  std::cout << "\n";

  std::vector<vec3df> vc = {
      {-1 * length, 1 * length, 1 * length},  // 0
      {1 * length, 1 * length, 1 * length},   // 1
      {-1 * length, -1 * length, 1 * length}, // 2
      {1 * length, -1 * length, 1 * length},  // 3

      {-1 * length, 1 * length, -1 * length},  // 4
      {1 * length, 1 * length, -1 * length},   // 5
      {-1 * length, -1 * length, -1 * length}, // 6
      {1 * length, -1 * length, -1 * length},  // 7
  };

  newObj->edges = {// Front face (z = +length), 2 triangles
                   0, 2, 1, 2, 3, 1,
                   // Back face (z = -length)
                   4, 5, 6, 5, 7, 6,
                   // Top face (y = +length)
                   0, 1, 5, 0, 5, 4,
                   // Bottom face (y = -length)
                   2, 6, 3, 6, 7, 3,
                   // Left face (x = -length)
                   0, 4, 2, 4, 6, 2,
                   // Right face (x = +length)
                   1, 3, 5, 3, 7, 5};
  newObj->vertices = vc;
  newObj->dvertices = vc;
  return newObj;
}

instanceTypes::basepart *createDonut(vec3df position, float rad, float innerrad,
                                     int iv, int cv) {
  instanceTypes::basepart *newObj = new instanceTypes::basepart();

  newObj->position = position;
  newObj->col3 = {0.5f, 1.0f, 1.0f, 1.0f};
  newObj->name = "wet donut ~//w//~";

  // newObj.vertices.resize(iv * cv);
  // newObj.edges.resize(iv * cv);

  float incPerstep = 2 * pi / (float)cv;
  float incPerstepI = 2 * pi / (float)iv;

  std::vector<vec3df> vecx;

  for (int i = 0; i < cv; i++) {
    vec3df ccc = {cosf(incPerstep * i) * rad, 0, sinf(incPerstep * i) * rad};
    vec3df c9 = ccc.cross({0, 1, 0}).normalize();

    for (int j = 0; j < iv; j++) {
      vec3df inerC = {0, innerrad * sinf(j * incPerstepI), 0};
      vec3df tempcity = (ccc.normalize() * innerrad * cosf(j * incPerstepI));

      inerC = inerC + tempcity;
      inerC = inerC + ccc;

      vecx.push_back(inerC);
      newObj->normals.push_back(inerC.normalize());
      newObj->dnormals.push_back(inerC.normalize());

      newObj->edges.push_back((i * iv) + j);
      if (j + 1 < cv) {
        newObj->edges.push_back((i * iv) + (j + 1));
      }
      newObj->edges.push_back((i * iv) + j);
      if (i + 1 >= iv) {
        newObj->edges.push_back(((0) * iv) + j);
      } else {
        newObj->edges.push_back(((i + 1) * iv) + j);
      }
    }
    newObj->edges.push_back((i * iv));
    newObj->edges.push_back((i * iv) + (cv - 1));
  }

  newObj->vertices = vecx;
  newObj->dvertices = vecx;
  return newObj;
}

} // namespace list_objs
