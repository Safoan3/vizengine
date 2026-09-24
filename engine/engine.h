#pragma once
#include "../../../graphicsapi/graphicsapi.h"

#include "../input/backends/linuxkernel.h"
#include "../input/input.h"
#include "../objects/object.h"
#include "../rendering/camera.h"

#include <sys/types.h>
#include <thread>
#include <vector>

#include <typeindex>
#include <typeinfo>
#include <unordered_map>

enum inputMethods {
  linuxlts,
  glfw,
  win32,
  cocoa,
};

enum RenderingMethods {
  REND_dotted,
  REND_wireframe,
  REND_rasterize,
  REND_raytrace
};

struct enj {
  vec2 resolution = {32, 32};

  Xspace *env; // render space

  space *TotalRenderImage;

  inputData input;
  u_int8_t inputMeth = linuxlts;
  std::vector<instance *> instances;
  std::unordered_map<std::type_index, std::vector<instance *>> typeinstances;

  // RenderingMethods rendering = REND_wireframe;

  camera currentCam;
  float focalpoint = 300.0f;

  enj(vec2 res) {
    resolution = res;
    env = new Xspace(res);
    TotalRenderImage = new space(res);
    xframe *rf = new xframe(env);
    TotalRenderImage->addlayer(rf);

    env->setCharacterDensityList();

    TotalRenderImage->finalrenderimg.listC.clear();
    TotalRenderImage->finalrenderimg.listC.push_back({"  "});
    TotalRenderImage->finalrenderimg.listC.push_back({"██"});
    TotalRenderImage->finalrenderimg.listC.push_back({"██"});
    TotalRenderImage->finalrenderimg.listC.push_back({"██"});
    TotalRenderImage->finalrenderimg.listC.push_back({"██"});
    TotalRenderImage->finalrenderimg.listC.push_back({"██"});
    TotalRenderImage->finalrenderimg.listC.push_back({"██"});

    // TotalRenderImage->finalrenderimg.setCharacterDensityList();
  }

  void write(RenderingMethods rendering = REND_rasterize);
  void render();
  instance *findobj(std::string name);
  void initPoll();
  void printDir();

  struct Instance {
    enj &selfEnj;

    Instance(enj &p) : selfEnj(p) {};

    template <typename T> T *New(instance *parent);

    void del(instance *p);
    void add(instance *ob);
  };

  Instance ins = Instance(*this);
};
