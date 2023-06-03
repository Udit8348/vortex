#include <stdint.h>
#include <math.h>
#include <vx_intrinsics.h>
#include <vx_print.h>
#include <vx_spawn.h>
#include "common.h"

#define CSR_BF 0x004

typedef void (*PFN_Kernel)(int task_id, kernel_arg_t* arg);

void kernel_fadd(int task_id, kernel_arg_t* arg) {
	uint32_t count  = arg->task_size;
	float* src0_ptr = (float*)arg->src0_addr;
	float* src1_ptr = (float*)arg->src1_addr;
	float* dst_ptr  = (float*)arg->dst_addr;
	uint32_t offset = task_id * count;

	// enable bf16 csr
	csr_write(CSR_BF, 1);
	for (uint32_t i = 0; i < count; ++i) {
		// compiles into float loads (flw)
		float a = src0_ptr[offset+i];
		float b = src1_ptr[offset+i];
		
		float c = a + b;

		// compiles into int add for offset then, float store (fsw)
		dst_ptr[offset+i] = c;
	}

	// preserve csr state
	csr_write(CSR_BF, 0);
}

void kernel_fsub(int task_id, kernel_arg_t* arg) {
	uint32_t count  = arg->task_size;
	float* src0_ptr = (float*)arg->src0_addr;
	float* src1_ptr = (float*)arg->src1_addr;
	float* dst_ptr  = (float*)arg->dst_addr;	
	uint32_t offset = task_id * count;

	csr_write(CSR_BF, 1);

	for (uint32_t i = 0; i < count; ++i) {
		float a = src0_ptr[offset+i];
		float b = src1_ptr[offset+i];
		float c = a - b;
		dst_ptr[offset+i] = c;
	}

	csr_write(CSR_BF, 0);
}

void kernel_fmul(int task_id, kernel_arg_t* arg) {
	uint32_t count  = arg->task_size;
	float* src0_ptr = (float*)arg->src0_addr;
	float* src1_ptr = (float*)arg->src1_addr;
	float* dst_ptr  = (float*)arg->dst_addr;	
	uint32_t offset = task_id * count;

	csr_write(CSR_BF, 1);

	for (uint32_t i = 0; i < count; ++i) {
		float a = src0_ptr[offset+i];
		float b = src1_ptr[offset+i];
		float c = a * b;
		dst_ptr[offset+i] = c;
	}

	csr_write(CSR_BF, 1);
}

void kernel_fdiv(int task_id, kernel_arg_t* arg) {
	uint32_t count  = arg->task_size;
	float* src0_ptr = (float*)arg->src0_addr;
	float* src1_ptr = (float*)arg->src1_addr;
	float* dst_ptr  = (float*)arg->dst_addr;	
	uint32_t offset = task_id * count;

	csr_write(CSR_BF, 1);

	for (uint32_t i = 0; i < count; ++i) {
		float a = src0_ptr[offset+i];
		float b = src1_ptr[offset+i];
		float c = a / b;
		dst_ptr[offset+i] = c;
	}

	csr_write(CSR_BF, 0);
}

void kernel_fdiv2(int task_id, kernel_arg_t* arg) {
	uint32_t count  = arg->task_size;
	float* src0_ptr = (float*)arg->src0_addr;
	float* src1_ptr = (float*)arg->src1_addr;
	float* dst_ptr  = (float*)arg->dst_addr;	
	uint32_t offset = task_id * count;

	csr_write(CSR_BF, 1);

	for (uint32_t i = 0; i < count; ++i) {
		float a = src0_ptr[offset+i];
		float b = src1_ptr[offset+i];
		float c = a / b;
		float d = b / a;
		float e = c + d;
		dst_ptr[offset+i] = e;
	}

	csr_write(CSR_BF, 0);
}

static const PFN_Kernel sc_tests[] = {
	kernel_fadd,
	kernel_fsub,
	kernel_fmul,
	kernel_fdiv,
	kernel_fdiv2
};

void main() {
	kernel_arg_t* arg = (kernel_arg_t*)KERNEL_ARG_DEV_MEM_ADDR;
	vx_spawn_tasks(arg->num_tasks, (vx_spawn_tasks_cb)sc_tests[arg->testid], arg);
}