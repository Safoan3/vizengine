#include "engine.h"
#include "../rendering/render.h"
#include <algorithm>
#include <chrono>
#include <random>
#include <thread>
#include <typeinfo>

void pollthread(enj *p) {
  /*
  if (p->inputMeth == linuxlts) {
    startlinuxkernelinput(p->input);

    std::cout << "input method is linux lts \n";

    while (true) {
      std::this_thread::sleep_for(std::chrono::hours(1));
    }
  }
  */
}

void enj::initPoll() {
  /*
  std::thread ne808w(pollthread, this);
  ne808w.join();
  std::cout << "thread is finished!";
  */
}

void enj::write(RenderingMethods rendering) {
  if (rendering == REND_rasterize) {
    renderingmethods::rasterize(*this);
  } else if (rendering == REND_raytrace) {
    renderingmethods::raytracer(*this);
  } else if (rendering == REND_wireframe) {
    renderingmethods::wireframe(*this);
  }

  // renderingmethods::dotted(*this);
  // renderingmethods::debugs::zbuffdebug(*this);

  TotalRenderImage->render();
};

void enj::render() { TotalRenderImage->finalrenderimg.render(); };

instance *enj::findobj(std::string name, instance *target) {
  auto Ins = &instances;
  if (target != nullptr) {
    Ins = &target->children;
  }

  for (instance *ob : *Ins) {
    if (ob && ob->name == name) {
      return ob;
    }
  }
  return nullptr;
};

instance *enj::findchildfromtype(const std::type_info &typ, instance *target) {
  auto Ins = &instances;
  if (target != nullptr) {
    Ins = &target->children;
  }

  for (instance *ob : *Ins) {
    if (ob != nullptr && typ == typeid(*ob)) {
      return ob;
    }
  }
  return nullptr;
};

template <typename T> T *enj::Instance::New(instance *parent) {
  T *ob = new T();
  if (parent != nullptr) {
    return nullptr;
  }
  const auto &typeinfo = typeid(*ob);
  selfEnj.instances.push_back(ob);
  selfEnj.typeinstances[typeinfo].push_back(ob);
  return ob;
};

void enj::Instance::add(instance *ob, instance *parent) {
  if (ob != nullptr) {
    const auto &typeinfo = typeid(*ob);
    selfEnj.typeinstances[typeinfo].push_back(ob);
    if (parent == nullptr) {
      selfEnj.instances.push_back(ob);
    } else {
      ob->parent = parent;
      parent->children.push_back(ob);
    }
  }
}

void enj::Instance::del(instance *p) {
  for (instance *ob : p->children) {
    if (ob != nullptr) {
      del(ob);
    }
  }

  if (p->parent != nullptr) {
    auto &instances = p->parent->children;
    instances.erase(std::remove(instances.begin(), instances.end(), p),
                    instances.end());
  } else {
    auto &instances = selfEnj.instances;
    instances.erase(std::remove(instances.begin(), instances.end(), p),
                    instances.end());
  }
  p->parent = nullptr;
  auto &typeVec = selfEnj.typeinstances[typeid(*p)];
  typeVec.erase(std::remove(typeVec.begin(), typeVec.end(), p), typeVec.end());
  delete p;
};

void enj::printDir() {
  auto recursive = [](auto &self, const std::vector<instance *> array,
                      int depth) -> void {
    for (instance *ob : array) {
      std::string spacing = "";
      for (int i = 0; i < depth; i++) {
        spacing = spacing + " ";
      }
      std::string toPrint = spacing + "-|" + ob->name;
      std::cout << toPrint << "\n";

      self(self, ob->children, depth + 1);
    }
  };

  recursive(recursive, instances, 1);
}
