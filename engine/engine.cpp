#include "engine.h"
#include "../rendering/render.h"
#include <chrono>
#include <thread>

void pollthread(enj *p) {
  if (p->inputMeth == linuxlts) {
    startlinuxkernelinput(p->input);

    std::cout << "input method is linux lts \n";

    while (true) {
      std::this_thread::sleep_for(std::chrono::hours(1));
    }
  }
}

void enj::initPoll() {
  std::thread ne808w(pollthread, this);
  ne808w.join();
  std::cout << "thread is finished!";
}

void enj::write() {
  // renderingmethods::wireframe(*this);

  // env->clear();

  renderingmethods::rasterize(*this);
  // renderingmethods::raytracer(*this);

  // renderingmethods::dotted(*this);
  // renderingmethods::debugs::zbuffdebug(*this);

  TotalRenderImage->render();
  //
  //
};

void enj::render() { TotalRenderImage->finalrenderimg.render(); };

object *enj::findobj(std::string name) {
  for (object *ob : objects) {
    if (ob->name == name) {
      return ob;
    }
  }
  return nullptr;
};
