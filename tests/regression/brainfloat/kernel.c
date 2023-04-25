#include <stdint.h>
#include <math.h>
#include <vx_intrinsics.h>
#include <vx_print.h>
#include <vx_spawn.h>
#include "common.h"

typedef void (*PFN_Kernel)(int task_id, kernel_arg_t* arg);

void kernel_iadd(int task_id, kernel_arg_t* arg) {
	// vx_printf("IADD TEST CASE\n");
	uint32_t count    = arg->task_size;
	int32_t* src0_ptr = (int32_t*)arg->src0_addr;
	int32_t* src1_ptr = (int32_t*)arg->src1_addr;
	int32_t* dst_ptr  = (int32_t*)arg->dst_addr;	
	uint32_t offset = task_id * count;

	// float test = vx_bfadd_16(-1.0, 1.0);
	float e = 1.0;
	volatile float test = e + src0_ptr[0];


	for (uint32_t i = 0; i < count; ++i) {
		int32_t a = src0_ptr[offset+i];
		int32_t b = src1_ptr[offset+i];
		int32_t c = a + b;
		dst_ptr[offset+i] = c;
	}
}

void kernel_fadd(int task_id, kernel_arg_t* arg) {
	uint32_t count  = arg->task_size;
	float* src0_ptr = (float*)arg->src0_addr;
	float* src1_ptr = (float*)arg->src1_addr;
	float* dst_ptr  = (float*)arg->dst_addr;
	// float* dst_ptr  = (float*)arg->dst_addr;	
	uint32_t offset = task_id * count;

	for (uint32_t i = 0; i < count; ++i) {
		// compiles into float loads (flw)
		float a = src0_ptr[offset+i];
		float b = src1_ptr[offset+i];
		
		
		float c = a + b;

		// compiles into int add for offset then, float store (fsw)
		dst_ptr[offset+i] = c;
	}
}

static const PFN_Kernel sc_tests[] = {
	kernel_iadd,
	kernel_fadd
};

void main() {
	kernel_arg_t* arg = (kernel_arg_t*)KERNEL_ARG_DEV_MEM_ADDR;
	vx_spawn_tasks(arg->num_tasks, (vx_spawn_tasks_cb)sc_tests[arg->testid], arg);
}