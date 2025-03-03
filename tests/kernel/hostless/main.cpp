#include "tests.h"
#include <vx_print.h>

int main() {
  int errors = 0;

  errors += test_spawn_tasks();

  if (0 == errors) {
    PRINTF("Passed!\n");
  } else {
    PRINTF("Failed!\n");
  }

  return errors;
}