#pragma once

#include "../input.h"
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <linux/input-event-codes.h>
#include <linux/input.h>

#include <thread>
#include <unistd.h>

static int keyboardrd = open("/dev/input/event6", O_RDONLY);
static int mouseid = open("/dev/input/event5", O_RDONLY);

inline input_event keyboardInputEv;
inline input_event mouseInputEv;

inline void keyboardStart(inputData &state) {
  while (read(keyboardrd, &keyboardInputEv, sizeof(keyboardInputEv)) > 0) {
    if (keyboardInputEv.type == EV_KEY) {
      state.keyboard.code = keyboardInputEv.code;
      state.keyboard.type = keyboardInputEv.type;
      state.keyboard.value = keyboardInputEv.value;
    }
  };
}

inline void mouseStart(inputData &state) {
  while (read(mouseid, &mouseInputEv, sizeof(mouseInputEv)) > 0) {
    if (mouseInputEv.type == EV_REL) {

      if (mouseInputEv.code == REL_X) {
        state.mouse.code = mouseInputEv.code;
        state.mouse.type = mouseInputEv.type;
        state.mouse.value = mouseInputEv.value;
      } else if (mouseInputEv.code == REL_Y) {
        state.mouse.code = mouseInputEv.code;
        state.mouse.type = mouseInputEv.type;
        state.mouse.value1 = mouseInputEv.value;
      }
    }
  };
}

inline void startlinuxkernelinput(inputData &state) {
  std::thread mouseT(mouseStart, std::ref(state));
  std::thread kbT(keyboardStart, std::ref(state));
  kbT.detach();

  mouseT.detach();
}
