#define MINIAUDIO_IMPLEMENTATION

#include "../engine/engine.h"
#include "../input/backends/linuxkernel.h"
#include "../input/input.h"
#include "../objects/listobjects.h"
#include "../objects/transformation.h"

#include "../raycasting/raycast.h"
#include <cstdlib>
#include <linux/input-event-codes.h>
#include <sstream>

#include "../libs/miniaud/miniaudio.h"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <list>
#include <thread>

////////////////

enj newEngine({64, 32});
ma_engine AudioEngine;
inputData *newData = new inputData;
camera &cam = newEngine.currentCam;

double delta = 1.0f;
float camspeed = 10.5f;

instanceTypes::basepart *spawnCube(vec3df pos) {
  instanceTypes::basepart *cube = list_objs::createCubeTrigangle(pos, 0.5f);

  cube->scale = {1, 1, 1};
  cube->col3 = {1.0f, 0.5f, 0.5f, 0.5f, 0.0f};
  transformation::scale(cube);
  transformation::rotate(cube);

  newEngine.ins.add(cube);

  return cube;
}

// triggers

bool jumpTrigger = false;
bool shootTrigger = false;
// game functions

void shoot(vec3df dir) {}

//

void inputmanager() {
  if (newData->mouse.type == EV_REL) {
    cam.pitch = cam.pitch + newData->mouse.value * 0.025f * delta;
    cam.yaw = cam.yaw - newData->mouse.value1 * 0.025f * delta;
    cam.direction = EulerCord(cam.pitch, cam.yaw);
  }
  if (newData->mouse.type == EV_KEY) {
    if (newData->mouse.code == BTN_LEFT && newData->mouse.value == 1) {
    }
  }

  if (newData->keyboard.code == KEY_W) {
    cam.position = cam.position + cam.direction * camspeed * delta * 2.0f;
  } else if (newData->keyboard.code == KEY_S) {
    cam.position = cam.position - cam.direction * camspeed * delta * 2.0f;
  } else if (newData->keyboard.code == KEY_A) {
    vec3df dir = cam.direction.cross({0, 1, 0}).normalize();
    cam.position = cam.position + dir * -camspeed * delta;
  } else if (newData->keyboard.code == KEY_D) {
    vec3df dir = cam.direction.cross({0, 1, 0}).normalize();
    cam.position = cam.position + dir * camspeed * delta;
  } else if (newData->keyboard.code == KEY_E) {
    vec3df dir = cam.direction.cross({0, 1, 0}).normalize();
    dir = dir.cross(cam.direction).normalize();
    cam.position = cam.position - dir * -camspeed * delta;
  } else if (newData->keyboard.code == KEY_Q) {
    vec3df dir = cam.direction.cross({0, 1, 0}).normalize();

    dir = dir.cross(cam.direction).normalize();
    cam.position = cam.position - dir * camspeed * delta;
  }
  if (newData->keyboard.code == KEY_SPACE) {
  }

  newData->keyboard.code = 0;
  newData->mouse.value = 0;
  newData->mouse.value1 = 0;
}

// generate grid

void genGrid(int x, int y) {
  float gridL = 4.0f;

  instanceTypes::group *gc = new instanceTypes::group();
  gc->name = "grid space";

  for (int j = 0; j < y; j++) {
    for (int i = 0; i < x; i++) {
      float col = (sinf(((float)j + (float)i) * 42.0f) + 1.0f) * 0.5f;

      vec3df pos = {i * 1.0f, col, j * 1.0f};

      auto *part = spawnCube(pos);
      part->col3 = {1.0f, col, col, 1.0f, 1.0f};
      part->setParent(gc);
    }
  }
  newEngine.instances.push_back(gc);
  newEngine.printDir();
}

int main() {
  ma_engine_init(NULL, &AudioEngine);

  //  ma_engine_play_sound(&AudioEngine, "../testing/sfx/jump.wav", NULL);

  startlinuxkernelinput(*newData);

  cam.position = {-14.7888, -1.19973, -6.23827};
  cam.pitch = 0.4005;
  cam.yaw = 0.13075;
  cam.direction = EulerCord(cam.pitch, cam.yaw);
  cam.position = cam.position - cam.direction * 1.0f;

  newEngine.instances.push_back(spawnCube({0, 0, 0}));
  genGrid(113, 113);

  while (true) {
    auto start = std::chrono::steady_clock::now();
    inputmanager();
    newEngine.write();
    newEngine.render();

    std::this_thread::sleep_for(std::chrono::milliseconds(8));
    auto end = std::chrono::steady_clock::now();
    delta = std::chrono::duration<double>(end - start).count();
    std::cout << "FPS: " << 1 / delta << "\n";
  }
}
