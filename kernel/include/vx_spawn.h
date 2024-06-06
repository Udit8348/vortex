// Copyright © 2019-2023
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __VX_SPAWN_H__
#define __VX_SPAWN_H__

#include <VX_types.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*vx_spawn_tasks_cb)(int task_id, const void *arg);

typedef void (*vx_spawn_task_groups_cb)(int local_task_id, int group_id, int local_group_id, int warps_per_group, const void *arg);

typedef void (*vx_serial_cb)(const void *arg);

void vx_spawn_tasks(int num_tasks, vx_spawn_tasks_cb callback, const void * arg);

void vx_spawn_task_groups(int num_groups, int group_size, vx_spawn_task_groups_cb callback, const void * arg);

inline void* vx_local_malloc(int local_group_id, int size) {
  return (int8_t*)csr_read(VX_CSR_LOCAL_MEM_BASE) + local_group_id * size;
}

void vx_serial(vx_serial_cb callback, const void * arg);

#ifdef __cplusplus
}
#endif

#endif // __VX_SPAWN_H__
