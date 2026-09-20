#pragma once
#include <linux/input.h>

struct inputState {
  int value;
  int type;
  int code;
  int value1;

  int byteSize;
};

struct inputData {
  volatile inputState keyboard;
  volatile inputState mouse;
};
