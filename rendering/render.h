#pragma once
#include "../../../graphicsapi/graphicsapi.h"
#include "../engine/engine.h"
#include "../objects/object.h"

// #include "camera.h"
#include "camprojection.h"

inline vec3df lightDirX = {0.0f, -1.0f, 1.0f};

namespace renderingmethods {
void wireframe(enj &engine);
void dotted(enj &engine);
void rasterize(enj &engine);
void raytracer(enj &engine);

namespace debugs {
void zbuffdebug(enj &engine); // namespace debugs
}
} // namespace renderingmethods
//
