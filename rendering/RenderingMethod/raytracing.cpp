#include "../render.h"
#include "umbrella.h"
#include <algorithm>

#include <cmath>
#include <cstdlib>
#include <memory>
#include <new>
#include <sys/types.h>
#include <type_traits>
#include <typeinfo>
#include <vector>

bool hashInit = false;
int maxBounces = 4;

float DefaultAmbient = 0.8f;
int maxSamples = 2;
float sampleDeviation = 0.0005f;
int maxShadowSamples = 2;
float shadowsampleDeviation = 0.1f;

static int hashLoop = 1024;
static float hashRand[1024];
int hashIndex = 0;
int workLoad = 2;

void initHash() {
  for (int i = 0; i < hashLoop; i++) {
    float ppp = (float)(rand() % hashLoop) / (float)hashLoop;
    hashRand[i] = (ppp - 0.5f) * 2.0f;
  }
}

color colorGradient(color A, color B, float per) {
  color c1;
  float invPer = 1.0f - per;

  c1.alpha = 1.0f;

  c1.a = A.a * invPer + B.a * per;
  c1.r = A.r * invPer + B.r * per;
  c1.g = A.g * invPer + B.g * per;
  c1.b = A.b * invPer + B.b * per;
  return c1;
};

struct raytracecontx {
  float noise;
  float &cur;
  const float aspect;
  const float fov;
  const int resx;
  const int resy;

  std::vector<pixel> *brightPixels;
};

static vec3df reflect(vec3df current, vec3df normal) {
  vec3df vn = current.normalize();
  float mul = vn.dot(normal.normalize());
  vec3df Ref = vn - (normal * (2 * mul));
  return Ref.normalize();
}

static bool noiseTest(float noise) {
  hashIndex = hashIndex + 3;
  if (hashIndex > hashLoop)
    hashIndex = 0;

  float rand = hashRand[hashInit];
  bool skip = false;
  if (rand < noise) {
    skip = true;
  }
  return skip;
};

int totalLightBounces = 0;

static pixel shoot(vec3df ori, vec3df direc, enj &engine, raytracecontx &conx) {
  vec3df dir = direc;
  pixel newPix = {1.0f, 0, 0, 0, 1.0f};

  raycast::resultenj newR = raycast::raycastEnj(engine, ori, dir);
  // glare

  if (newR.hit == true) {
    instanceTypes::basepart *FirstRayObjectHit =
        dynamic_cast<instanceTypes::basepart *>(newR.hitobj);
    newPix = {1.0f, newR.r, newR.g, newR.b, 1.0f};
    vec3df LightShootOrigin = newR.hitpos + (newR.normal * -0.01f);

    float shadingDot = 0.0f;
    float ambient = DefaultAmbient;
    float reduction = 1.0f;
    float Intensity = 0.16f;
    for (auto *LightSourceInstance :
         engine.typeinstances[typeid(instanceTypes::lightsource)]) {
      instanceTypes::lightsource *lightSource =
          dynamic_cast<instanceTypes::lightsource *>(LightSourceInstance);
      instanceTypes::basepart *LightParent =
          dynamic_cast<instanceTypes::basepart *>(LightSourceInstance->parent);

      if (LightParent == nullptr)
        continue;

      vec3df lightPosition = LightParent->position + lightSource->position;

      float LightRaycastMaxDist =
          (lightSource->position - LightShootOrigin).magnitude();

      // insane shading ;

      {
        float TempshadingDot = newR.normal.normalize().dot(
            (lightPosition - newR.hitpos).normalize());
        shadingDot += (1.0f - (TempshadingDot + 1.0f) * 0.5f);
        shadingDot = std::clamp(shadingDot, 0.0f, 1.0f);

        for (int shadowSamples = 0; shadowSamples < maxShadowSamples;
             shadowSamples++) {
          vec3df randomDir = vec3df{hashRand[hashIndex % hashLoop],
                                    hashRand[(hashIndex + 1) % hashLoop],
                                    hashRand[(hashIndex + 2) % hashLoop]} *
                             shadowsampleDeviation * 0.1f;
          if (shadowSamples == 0) {
            randomDir = vec3df{0, 0, 0};
          }
          hashIndex += 3;
          raycast::resultenj ToLightRayResult = raycast::raycastEnj(
              engine, lightPosition,
              ((LightShootOrigin - lightPosition + randomDir).normalize())
                  .normalize(),
              {LightParent});

          bool occluded = ToLightRayResult.hit &&
                          ToLightRayResult.distance <
                              (newR.hitpos - lightPosition).magnitude();

          if (!occluded) {
            float tempInt = 0.95f;
            float distanceFromSourceToHit =
                (newR.hitpos - lightPosition).magnitude();
            float percentage = lightSource->range / (distanceFromSourceToHit);

            float IntensityPerSample =
                (tempInt * percentage) / (float)maxShadowSamples;

            Intensity += IntensityPerSample * lightSource->brightness;

            color np = lightSource->col3;
            color toCol = newPix.convToColor();
            toCol = toCol.colorGradient(np, IntensityPerSample * 0.4f);
            toCol.normalize();
            newPix = {Intensity, toCol.r, toCol.g, toCol.b, toCol.alpha};
          }
        }

        // shadow
      };
    }

    if (newR.hit && FirstRayObjectHit) {
      const std::type_info &typinf = typeid(instanceTypes::lightsource);
      instance *insideLightsource =
          engine.findchildfromtype(typinf, newR.hitobj);
      instanceTypes::lightsource *lightPt =
          dynamic_cast<instanceTypes::lightsource *>(insideLightsource);

      if (lightPt != nullptr) {
        // FirstRayObjectHit->col3 = lightPt->col3;
        shadingDot = 0.0f;
        Intensity = 30.0f;
      }

      // HardShadow = 1.0f - HardShadow;

      // glare reflectance

      //
      if (Intensity > 10.0f) {
      }

      reduction = reduction * Intensity;
      ambient = ambient * shadingDot + (1 - ambient);
      ambient = ambient * reduction;

      vec3df refD = reflect(dir, newR.normal);
      totalLightBounces++;

      float rr, gg, bb;

      if (totalLightBounces < maxBounces && FirstRayObjectHit) {
        float retain = 1.0f - FirstRayObjectHit->reflectance;
        float reflectance = FirstRayObjectHit->reflectance;

        pixel miki = shoot(LightShootOrigin, refD, engine, conx);
        rr = newPix.r * ambient * retain + miki.r * ambient * reflectance;
        gg = newPix.g * ambient * retain + miki.g * ambient * reflectance;
        bb = newPix.b * ambient * retain + miki.b * ambient * reflectance;
      }

      totalLightBounces = 0;
      newPix.a = Intensity;
      newPix.r = rr;
      newPix.g = gg;
      newPix.b = bb;
      newPix.alpha = 1.0f;
      newPix.normalize();
      newPix.a = Intensity;
    }

    return newPix;
  } else {

    // sky
    vec3df WorldUp = {0, 1, 0};
    float gradientDot = WorldUp.dot(direc.normalize());
    float graddotclipped = (gradientDot + 1.0f) / 2.0f;
    graddotclipped = graddotclipped * 1.5f;

    if (graddotclipped > 1.0f)
      graddotclipped = 1.0f;

    color t1 = colorGradient({1.0f, 0.2f, 0.2f, 0.2f, 1.0f},
                             {1.0f, 0.6f, 0.6f, 0.95f, 1.0f}, graddotclipped);
    pixel retPix = {t1.a, t1.r, t1.g, t1.b, t1.alpha};

    return retPix;
  }
}

static pixel literation(const int i, const int j, enj &engine,
                        raytracecontx &conx) {
  pixel newPix;
  // bool skip = noiseTest(conx.noise);
  bool skip = false;
  if (skip == false) {
    conx.cur = conx.cur + 1.0f;
    float ndcX = (2.0f * (i + 0.5f) / conx.resx - 1.0f) * conx.aspect *
                 tanf(conx.fov * 0.5f);
    float ndcY = (1.0f - 2.0f * (j + 0.5f) / conx.resy) * tanf(conx.fov * 0.5f);
    vec3df forward = engine.currentCam.direction.normalize();
    vec3df leftVec = vec3df{0, 1.0f, 0}.cross(forward).normalize() *
                     -1.0f; // actually "right" — see note
    vec3df topVec = leftVec.cross(forward).normalize();
    vec3df dir = (forward + leftVec * ndcX + topVec * ndcY).normalize();

    pixel Sampled = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    //   newPix = shoot(engine.currentCam.position, dir, engine, conx);

    for (int numSamp = 0; numSamp < maxSamples; numSamp++) {
      vec3df randomDir = vec3df{hashRand[hashIndex % hashLoop],
                                hashRand[(hashIndex + 1) % hashLoop],
                                hashRand[(hashIndex + 2) % hashLoop]} *
                         sampleDeviation * 0.01f;
      if (numSamp == 0) {
        randomDir = vec3df{0, 0, 0};
      }

      pixel rtPix =
          shoot(engine.currentCam.position, dir + randomDir, engine, conx);

      hashIndex = hashIndex + 3;

      Sampled.a += (rtPix.a / (float)maxSamples);
      Sampled.r += (rtPix.r / (float)maxSamples);
      Sampled.g += (rtPix.g / (float)maxSamples);
      Sampled.b += (rtPix.b / (float)maxSamples);
      Sampled.alpha += (rtPix.alpha / (float)maxSamples);
    }
    newPix = Sampled;

    if ((int)conx.cur % 1000 == 0) {
      std::cout << "comp :" << conx.cur / (conx.resx * conx.resy) * 100.0f
                << "% \n";
    }

  } else {
    newPix = {1.0f, 0.5f, 0.0f, 0.0f, 0.0f};
  };
  // conx.buff[j * conx.resx + i] = newPix;
  return newPix;
}

static void lowraytrace(enj &engine, float noise) {
  // engine.env->clear();

  std::vector<pixel> BrightPixels;

  if (hashInit == false) {
    hashInit = true;
    initHash();
  }

  camera &cam = engine.currentCam;
  int resx = engine.resolution.x;
  int resy = engine.resolution.y;
  BrightPixels.resize(resx * resy);
  BrightPixels.assign(resx * resy, pixel{0.0f, 0.0f, 0.0f, 0.0f, 1.0f});

  // SETTINGS //
  float fov = engine.focalpoint * 0.5f;
  float aspect = (float)resx / (float)resy;
  std::vector<pixel> tempBuff;

  vec3df forward = cam.direction.normalize();

  instance *lightObjAz = engine.findobj("light");
  auto *lightObj = dynamic_cast<instanceTypes::basepart *>(lightObjAz);
  vec3df lightPos = cam.position + vec3df{0, 0.0f, 0};
  if (lightObj != nullptr) {
    lightPos = lightObj->position;
  } else {
  }
  float progress = 0.0f;
  float total = resx * resy;
  float cur = 0.0f;
  uint8_t ns = (uint8_t)(noise * 255.0f);

  // settings update

  instanceTypes::game *gameObj = dynamic_cast<instanceTypes::game *>(
      engine.findchildfromtype(typeid(instanceTypes::game), nullptr));
  instanceTypes::lighting *lighjtingObj =
      dynamic_cast<instanceTypes::lighting *>(
          engine.findchildfromtype(typeid(instanceTypes::lighting), gameObj));
  if (lighjtingObj) {
    maxSamples = lighjtingObj->MaxAntiAlisaingSamples;
    sampleDeviation = lighjtingObj->AntiAlisaingDeviation;
    DefaultAmbient = lighjtingObj->Ambient;
    shadowsampleDeviation = lighjtingObj->ShadowSmoothingDeviation;
    maxBounces = lighjtingObj->MaxReflectionBounces;
    maxShadowSamples = lighjtingObj->MaxShadowSamples;
  }

  //

  for (int j = 0; j < resy; j++) {
    for (int i = 0; i < resx; i++) {
      int literationIndex = j * resx + i;

      raytracecontx conx = raytracecontx{noise, cur, aspect, fov, resx, resy};
      pixel pix = literation(i, j, engine, conx);
      //     engine.env->buffer[literationIndex] = pix;
      if (pix.a > 10.0f) {
        // std::cout << "wpahj thats bright \n";
        BrightPixels[literationIndex] = pix * pix.a;
      }

      engine.env->buffer[literationIndex] = pix;
    }
  }
  KernelFilter filter;
  filter.FilterWeight = std::vector<float>(5 * 5, 1.0f);
  filter.filterRes = {5, 5};

  for (int i = 0; i < 50; i++) {
    RawGraphicsHelperApi::FilterKernel(filter, BrightPixels.data(), resy, resx);
  }

  for (int j = 0; j < resy; j++) {
    for (int i = 0; i < resx; i++) {
      int literationIndex32 = j * resx + i;
      pixel found = engine.env->buffer[literationIndex32];
      engine.env->buffer[literationIndex32] =
          found + (BrightPixels[literationIndex32] * 1.0f);
      engine.env->buffer[literationIndex32].normalize();
    }
  }

  int ind = 0;
  //  for (pixel pix : tempBuff) {
  // pixel np = engine.env->buffer[ind];
  // engine.env->buffer[ind] = pix;

  ind++;
  //}
  //  tempBuff.clear();
};

void renderingmethods::raytracer(enj &engine) { lowraytrace(engine, 0.95f); };
