#include <stdint.h>
#include <math.h>
#include <vx_intrinsics.h>
#include <vx_print.h>
#include <vx_spawn.h>
#include "common.h"

#define CSR_BF 0x004

typedef void (*PFN_Kernel)(int task_id, kernel_arg_t* arg);

void kernel_iadd(int task_id, kernel_arg_t* arg) {
	// vx_printf("IADD TEST CASE\n");
	uint32_t count    = arg->task_size;
	int32_t* src0_ptr = (int32_t*)arg->src0_addr;
	int32_t* src1_ptr = (int32_t*)arg->src1_addr;
	int32_t* dst_ptr  = (int32_t*)arg->dst_addr;	
	uint32_t offset = task_id * count;
	
	// forcing fadd with known data types and numbers so we can see what their intermediate representation is in simX pipeline
	csr_write(CSR_BF, 1);
	float e = 2.0;
	int32_t other =  src0_ptr[508];
	volatile float test = e + other;
	vx_printf(">>>> KERNEL: %d plus two is %f\n", other, test);

	for (uint32_t i = 0; i < count; ++i) {
		int32_t a = src0_ptr[offset+i];
		int32_t b = src1_ptr[offset+i];
		int32_t c = a + b;
		dst_ptr[offset+i] = c;
	}

	csr_write(CSR_BF, 0);
}

void kernel_fadd(int task_id, kernel_arg_t* arg) {
	uint32_t count  = arg->task_size;
	float* src0_ptr = (float*)arg->src0_addr;
	float* src1_ptr = (float*)arg->src1_addr;
	float* dst_ptr  = (float*)arg->dst_addr;
	uint32_t offset = task_id * count;

	// float bf_test = vx_bfadd_16(1, 2);
	// vx_printf(">>>> KERNEL: instrinsics call gives us: %f\n", bf_test);

	csr_write(CSR_BF, 1);
	uint32_t a = csr_read(CSR_BF);
	vx_printf(">>>> KERNEL: instrinsics csr read gives us: %f\n", a);

	for (uint32_t i = 0; i < count; ++i) {
		// compiles into float loads (flw)
		float a = src0_ptr[offset+i];
		float b = src1_ptr[offset+i];
		
		float c = a + b;
		// float c = vx_bfadd_16(a, b);

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