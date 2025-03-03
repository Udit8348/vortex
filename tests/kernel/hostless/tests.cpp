#include "tests.h"
#include <VX_config.h>
#include <stdio.h>
#include <vx_intrinsics.h>
#include <vx_print.h>
#include <vx_spawn.h>

int __attribute__((noinline)) check_error(const int *buffer, int offset, int size) {
  int errors = 0;
  for (int i = offset; i < size; i++) {
    int value = buffer[i];
    int ref_value = 65 + i;
    if (value == ref_value) {
      // PRINTF("[%d] %c\n", i, value);
    } else {
      PRINTF("*** error: [%d] 0x%x, expected 0x%x\n", i, value, ref_value);
      ++errors;
    }
  }
  return errors;
}

#define ST_BUF_SZ 8
typedef struct {
  int *src;
  int *dst;
} st_args_t;

int st_buffer_src[ST_BUF_SZ];
int st_buffer_dst[ST_BUF_SZ];

void st_kernel(const st_args_t *__UNIFORM__ arg) {
  arg->dst[blockIdx.x] = arg->src[blockIdx.x];
}

int test_spawn_tasks() {
  PRINTF("SpawnTasks Test\n");

  st_args_t arg;
  arg.src = st_buffer_src;
  arg.dst = st_buffer_dst;

  for (int i = 0; i < ST_BUF_SZ; i++) {
    st_buffer_src[i] = 65 + i;
  }

  uint32_t num_tasks(ST_BUF_SZ);
  vx_spawn_threads(1, &num_tasks, nullptr, (vx_kernel_func_cb)st_kernel, &arg);

  return check_error(st_buffer_dst, 0, ST_BUF_SZ);
}