#include "common.h"
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <vortex.h>

#define FLOAT_ULP 6

#define RT_CHECK(_expr)                                      \
  do {                                                       \
    int _ret = _expr;                                        \
    if (0 == _ret)                                           \
      break;                                                 \
    printf("Error: '%s' returned %d!\n", #_expr, (int)_ret); \
    cleanup();                                               \
    exit(-1);                                                \
  } while (false)

///////////////////////////////////////////////////////////////////////////////

template <typename Type>
class Comparator {};

template <>
class Comparator<int> {
public:
  static const char *type_str() {
    return "integer";
  }
  static int generate() {
    return rand();
  }
  static bool compare(int a, int b, int index, int errors) {
    if (a != b) {
      if (errors < 100) {
        printf("*** error: [%d] expected=%d, actxual=%d\n", index, b, a);
      }
      return false;
    }
    return true;
  }
};

template <>
class Comparator<float> {
private:
  union Float_t {
    float f;
    int i;
  };

public:
  static const char *type_str() {
    return "float";
  }
  static int generate() {
    return static_cast<float>(rand()) / RAND_MAX;
  }
  static bool compare(float a, float b, int index, int errors) {
    union fi_t {
      float f;
      int32_t i;
    };
    fi_t fa, fb;
    fa.f = a;
    fb.f = b;
    auto d = std::abs(fa.i - fb.i);
    if (d > FLOAT_ULP) {
      if (errors < 100) {
        printf("*** error: [%d] expected=%f(0x%x), actual=%f(0x%x), ulp=%d\n", index, b, fb.i, a, fa.i, d);
      }
      return false;
    }
    return true;
  }
};

const char *kernel_file = "kernel.vxbin";
uint32_t count = 16;

vx_device_h device = nullptr;
vx_buffer_h src0_buffer = nullptr;
vx_buffer_h src1_buffer = nullptr;
vx_buffer_h dst_buffer = nullptr;
vx_buffer_h krnl_buffer = nullptr;
vx_buffer_h args_buffer = nullptr;
kernel_arg_t kernel_arg = {}; // zero initialize fields of kernel args struct

static void show_usage() {
  std::cout << "Vortex Test." << std::endl;
  std::cout << "Usage: [-k: kernel] [-n words] [-h: help]" << std::endl;
}

static void parse_args(int argc, char **argv) {
  int c;
  while ((c = getopt(argc, argv, "n:k:h")) != -1) {
    switch (c) {
    case 'n':
      count = atoi(optarg);
      break;
    case 'k':
      kernel_file = optarg;
      break;
    case 'h': {
      show_usage();
      exit(0);
    } break;
    default:
      show_usage();
      exit(-1);
    }
  }
}

void cleanup() {
  if (device) {
    // vx_mem_free(src0_buffer);
    // vx_mem_free(src1_buffer);
    // vx_mem_free(dst_buffer);
    // vx_mem_free(krnl_buffer);
    // vx_mem_free(args_buffer);
    vx_dev_close(device);
  }
}

int main(int argc, char *argv[]) {
  // parse command arguments
  parse_args(argc, argv);

  std::srand(50);

  // open device connection
  std::cout << "open device connection" << std::endl;
  // RT_CHECK(vx_dev_open(&device));

  uint64_t num_cores, num_warps, num_threads;
  RT_CHECK(vx_dev_caps(device, VX_CAPS_NUM_CORES, &num_cores));
  RT_CHECK(vx_dev_caps(device, VX_CAPS_NUM_WARPS, &num_warps));
  RT_CHECK(vx_dev_caps(device, VX_CAPS_NUM_THREADS, &num_threads));

  uint32_t total_threads = num_cores * num_warps * num_threads;
  uint32_t num_points = count * 1; // for now just do 16 points so it matches our predefined binary size
  uint32_t buf_size = num_points * sizeof(TYPE);

  std::cout << "data type: " << Comparator<TYPE>::type_str() << std::endl;
  std::cout << "number of points: " << num_points << std::endl;
  std::cout << "buffer size: " << buf_size << " bytes" << std::endl;

  kernel_arg.num_tasks = total_threads;
  kernel_arg.task_size = count;

  // no longer allocate device memory, rather point to pre-allocated regions of elf
  std::cout << "allocate device memory" << std::endl;
  // RT_CHECK(vx_mem_alloc(device, buf_size, VX_MEM_READ, &src0_buffer));
  // RT_CHECK(vx_mem_address(src0_buffer, &kernel_arg.src0_addr));
  // RT_CHECK(vx_mem_alloc(device, buf_size, VX_MEM_READ, &src1_buffer));
  // RT_CHECK(vx_mem_address(src1_buffer, &kernel_arg.src1_addr));
  // RT_CHECK(vx_mem_alloc(device, buf_size, VX_MEM_WRITE, &dst_buffer));
  // RT_CHECK(vx_mem_address(dst_buffer, &kernel_arg.dst_addr));
  kernel_arg.src0_addr = 0xa0000000UL;
  kernel_arg.src1_addr = 0xa1000000UL;
  kernel_arg.dst_addr = 0xc0000000UL;
  krnl_buffer = (void *)0x80000094;

  std::cout << "dev_src0=0x" << std::hex << kernel_arg.src0_addr << std::endl;
  std::cout << "dev_src1=0x" << std::hex << kernel_arg.src1_addr << std::endl;
  std::cout << "dev_dst=0x" << std::hex << kernel_arg.dst_addr << std::endl;

  // allocate host buffers for testing
  // primarily used for comparing results
  // secondarily used to generate the .bin files on the fly
  std::cout << "allocate host buffers" << std::endl;
  std::vector<TYPE> h_src0(num_points);
  std::vector<TYPE> h_src1(num_points);
  std::vector<TYPE> h_dst(num_points);

  // generate source data
  for (uint32_t i = 0; i < num_points * 2; ++i) {
    // h_src0[i] = Comparator<TYPE>::generate();
    // h_src1[i] = Comparator<TYPE>::generate();
    if (i % 2 == 0) {
      h_src0[i] = static_cast<float>(i);
    } else {
      h_src1[i] = static_cast<float>(i);
    }
  }

  // upload source buffer0
  // std::cout << "upload source buffer0" << std::endl;
  // RT_CHECK(vx_copy_to_dev(src0_buffer, h_src0.data(), 0, buf_size));

  // upload source buffer1
  // std::cout << "upload source buffer1" << std::endl;
  // RT_CHECK(vx_copy_to_dev(src1_buffer, h_src1.data(), 0, buf_size));

  // upload program
  // std::cout << "upload program" << std::endl;
  // RT_CHECK(vx_upload_kernel_file(device, kernel_file, &krnl_buffer));

  // clear destination buffer
  std::cout << "clear destination buffer" << std::endl;
  // memset(h_dst.data(), 0, num_points * sizeof(TYPE));
  // RT_CHECK(vx_copy_to_dev(dst_buffer, h_dst.data(), 0, buf_size)); // buf_size = num_points * sizeof(TYPE)

  // upload kernel argument
  // std::cout << "upload kernel argument" << std::endl;
  // RT_CHECK(vx_upload_bytes(device, &kernel_arg, sizeof(kernel_arg_t), &args_buffer));

  // start device
  std::cout << "start device" << std::endl;
  RT_CHECK(vx_start(device, krnl_buffer, args_buffer));
  std::cout << "start device completed?" << std::endl;

  // wait for completion
  std::cout << "wait for completion" << std::endl;
  RT_CHECK(vx_ready_wait(device, VX_MAX_TIMEOUT));

  // download destination buffer
  std::cout << "download destination buffer" << std::endl;
  RT_CHECK(vx_copy_from_dev(h_dst.data(), dst_buffer, 0, buf_size));

  // verify result
  std::cout << "verify result" << std::endl;
  int errors = 0;
  for (uint32_t i = 0; i < num_points; ++i) {
    auto ref = h_src0[i] + h_src1[i];
    auto cur = h_dst[i];
    if (!Comparator<TYPE>::compare(cur, ref, i, errors)) {
      ++errors;
    }
  }

  // cleanup
  std::cout << "cleanup" << std::endl;
  cleanup();

  if (errors != 0) {
    std::cout << "Found " << std::dec << errors << " errors!" << std::endl;
    std::cout << "FAILED!" << std::endl;
    return errors;
  }

  std::cout << "PASSED!" << std::endl;

  return 0;
}