// MODULES / UTILS / INT.C

#include "int.h" // modules/utils/int.h

#include <stdlib.h>

int _utils_int_get_random(int min, int max) {
  return (rand() % (max - min + 1)) + min;
}
