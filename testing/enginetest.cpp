#include "../engine/engine.h"
#include "../input/backends/linuxkernel.h"
#include "../input/input.h"
#include "../objects/listobjects.h"
#include "../objects/transformation.h"

#include "../raycasting/raycast.h"
#include <linux/input-event-codes.h>

#include <chrono>
#include <iostream>
#include <list>
#include <thread>

enj newEngine({132, 62});

camera &cam = newEngine.currentCam;

double delta = 1.0f;

// h in [0, 360), s and v in [0, 1]
vec3df hsvToRgb(float h, float s, float v) {
  float c = v * s;
  float x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f));
  float m = v - c;

  float r, g, b;
  if (h < 60.0f) {
    r = c;
    g = x;
    b = 0;
  } else if (h < 120.0f) {
    r = x;
    g = c;
    b = 0;
  } else if (h < 180.0f) {
    r = 0;
    g = c;
    b = x;
  } else if (h < 240.0f) {
    r = 0;
    g = x;
    b = c;
  } else if (h < 300.0f) {
    r = x;
    g = 0;
    b = c;
  } else {
    r = c;
    g = 0;
    b = x;
  }

  return {r + m, g + m, b + m};
}

inputData *newData = new inputData;

float camspeed = 10.5f;

void spawnCube(vec3df pos) {
  instanceTypes::basepart *Donut = list_objs::createCubeTrigangle(pos, 1.0f);

  Donut->scale = {1, 1, 1};
  Donut->col3 = {1.0f, 0.5f, 0.5f, 0.5f, 0.0f};

  transformation::scale(Donut);
  transformation::rotate(Donut);

  newEngine.instances.push_back(Donut);
}

void shoot() {
  instanceTypes::basepart *Donut = list_objs::createTriangleSphere(
      cam.position + (cam.direction * 1.0f), 0.1f, 1);
  //  Donut->scale = {10, 1, 1};

  Donut->col3 = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
  transformation::rotate(Donut);
  raycast::result res =
      raycast::raycastEnj(newEngine, cam.position, cam.direction);

  newEngine.instances.push_back(Donut);

  if (res.hit == true) {
    Donut->position = res.hitpos;
  }
}

instanceTypes::basepart *ReferenceSphere =
    list_objs::createTriangleSphere({0, 5, 0}, 0.4f, 1);

instanceTypes::basepart *Donut =
    list_objs::createCubeTrigangle({0, 0, 0}, 1.0f);
instanceTypes::basepart *Triangle =
    list_objs::createTriangle({-10.0f, 10, 10.0f}, 123.2f);

vec3df donutPos = {0, 0, 0};

void inputmanager() {
  // while (true) {

  if (newData->mouse.type == EV_REL) {
    cam.pitch = cam.pitch + newData->mouse.value * 0.025f * delta;
    cam.yaw = cam.yaw - newData->mouse.value1 * 0.025f * delta;

    //     std::cout << "mouse cord: " << x << ", " << y << "\n";
    cam.direction = EulerCord(cam.pitch, cam.yaw);
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
    vec3df dir = cam.direction.normalize();

    spawnCube(cam.position + dir * 10.0f);
  }

  if (newData->keyboard.code == KEY_F) {
    vec3df dir = cam.direction.normalize();
    donutPos = cam.position + (dir * 30.0f);
  }
  if (newData->keyboard.code == KEY_B) {
    vec3df dir = cam.direction.normalize();
    raycast::result res = raycast::lowCast(
        cam.position, dir, Triangle->vertices[0] + Triangle->position,
        Triangle->vertices[1] + Triangle->position,
        Triangle->vertices[2] + Triangle->position);

    if (res.bar.x > 0 && res.bar.y > 0 && res.bar.x + res.bar.y <= 1.0f) {

      ReferenceSphere->col3 = {1.0f, res.bar.x, res.bar.y,
                               1.0f - (res.bar.x + res.bar.y), 1.0f};

    } else {
      ReferenceSphere->col3 = {1.0f, 1.0f, 0.0f, 0.0f};
    }
    ReferenceSphere->position = res.hitpos;
  }

  if (newData->keyboard.code == KEY_L) {
    vec3df dir = cam.direction.normalize();
    ReferenceSphere->name = "light";
    ReferenceSphere->position = cam.position + (dir * 10.0f);
  }

  if (newData->keyboard.code == KEY_P) {
    shoot();
  }

  newData->keyboard.code = 0;
  newData->mouse.value = 0;
  newData->mouse.value1 = 0;

  //}
}

int main() {
  startlinuxkernelinput(*newData);
  cam.position = {-14.7888, -1.19973, -6.23827};

  cam.pitch = 0.4005;
  cam.yaw = 0.13075;
  cam.direction = EulerCord(cam.pitch, cam.yaw);

  cam.position = cam.position - cam.direction * 1.0f;

  // object *Donut = list_objs::createDonut({0, 0, 0}, 2.0f, 0.8f, 85, 85);
  transformation::rotate(Triangle);

  newEngine.instances.push_back(ReferenceSphere);

  newEngine.instances.push_back(Triangle);

  float step = 0.0f;

  newEngine.instances.push_back(Donut);
  newEngine.env->listC.clear();
  newEngine.env->listC.push_back({"██"});

  //  spawnSquare();

  // std::thread inputT(mouseStart);
  //  inputT.detach();
  int frame = 0;
  float dt = 1.0f;

  // hiearchy test

  instanceTypes::group *gunModel1 = new instanceTypes::group();
  gunModel1->name = "gun";

  instanceTypes::basepart *barrel1 = new instanceTypes::basepart();
  barrel1->name = "barrel";
  barrel1->setParent(gunModel1);

  newEngine.instances.push_back(gunModel1);

  while (true) {
    step = step + 0.015f * delta;
    auto start = std::chrono::steady_clock::now();
    ReferenceSphere->name = "light";
    ReferenceSphere->vertices.clear();
    ReferenceSphere->edges.clear();
    ReferenceSphere->dvertices.clear();

    inputmanager();
    Donut->rotX = Donut->rotX + 0.035 * (10.8f * delta) * dt;
    Donut->rotY = Donut->rotY + 0.020 * (10.8f * delta) * dt;
    Donut->rotZ = Donut->rotZ + 0.035 * (10.8f * delta) * dt;

    Donut->position =
        donutPos + vec3df{0, sinf(step * 100.0f) * dt * 0.3f + 0.3f, 0};
    //    Donut->position = Donut->position +
    //    vec3df{30.0f, 30.0f, 30.0f};
    //

    vec3df c =
        hsvToRgb((sinf(step * 10.0f) + 1.0f) * 360.0f / (2.0f), 1.0f, 1.0f);

    Donut->col3 = {1.0f, c.x, c.y, c.z};

    if (step > 2 * pi) {
      step = 0;
    }

    transformation::rotate(Donut);

    // house
    //
    //

    // Hollow rectangular house

    newEngine.write();
    newEngine.render();

    std::this_thread::sleep_for(std::chrono::milliseconds(8));
    auto end = std::chrono::steady_clock::now();
    delta = std::chrono::duration<double>(end - start).count();
    // std::cout << delta << "\n";
    frame++;
    // std::cout << 1 / delta << "\n";
  }
}
