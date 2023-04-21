[![Build Status](https://travis-ci.com/vortexgpgpu/vortex.svg?branch=master)](https://travis-ci.com/vortexgpgpu/vortex)
[![codecov](https://codecov.io/gh/vortexgpgpu/vortex/branch/master/graph/badge.svg)](https://codecov.io/gh/vortexgpgpu/vortex)

# Vortex OpenGPU

Vortex is a full-system RISCV-based GPGPU processor.

## Specifications

- Support RISC-V RV32IMF ISA
- Performance: 
    - 1024 total threads running at 250 MHz
    - 128 Gflops of compute bandwidth
    - 16 GB/s of memory bandwidth
- Scalability: up to 64 cores with optional L2 and L3 caches
- Software: OpenCL 1.2 Support 
- Supported FPGAs: 
    - Intel Arria 10
    - Intel Stratix 10

## Directory structure

- `doc`: [Documentation](docs/index.md).
- `hw`: Hardware sources.
- `driver`: Host drivers repository.
- `runtime`: Kernel Runtime software.
- `sim`: Simulators repository.
- `tests`: Tests repository.
- `ci`: Continuous integration scripts.
- `miscs`: Miscellaneous resources.

# GPU-BF16: Design Exploration for bfloat16 FPU Extension for RISC-V GPGPU Vortex

Vortex is a full-system RISCV-based GPGPU processor. This feature aims to extend vortex with new floating point units that support **bfloat16**.

## Mentors 
Tine, Blaise <blaisetine@gatech.edu>

Saxena, Varun <vsaxena36@gatech.edu>

## Introduction
The popularization of non-standard floating-point formats by machine learning applications has been gaining support for hardware acceleration in recent years with accelerators like Google TPU and NVidia GPUs. bfloat16 reduced-precision floating-point format is heavily used for fast neural network inference for its storage density and for its hardware implementation efficiency while retaining the dynamic range of FP32

## What to implement:
You will be tasked to implement and evaluate bfloat16 in hardware using Verilog HDL. Vortex GPGPU (https://vortex.cc.gatech.edu/publications/vortex_micro21_final.pdf) currently supports RISC-V floating-point 32-bit ISA extension. You will be extending the current FPU pipeline to support the bfloat16 format. To enable bfloat16, applications will modify a specific CSR register to set the float-point precision for the next operations.

## Metrics:
latency, power consumption, and resource utilization on the FPGA.

## Keywords: Vector, GPGPU, Vortex

## Doc
[google doc](https://docs.google.com/document/d/1TKFeWRtvvfIE7FpOwqvBslruGUJESe4jnbEJp2bhVT4/edit#)
[bfloat-wikichip](https://en.wikichip.org/wiki/brain_floating-point_format)
[vortex](https://github.com/vortexgpgpu/vortex)
[canvas group](https://gatech.instructure.com/groups/305538)


## Deadlines
HW6 + Vortex Tutorials:	4/6 (T)
Milestone 1: 4/10 (M)
Milestone 2: 4/17(M) 
Presentation: 4/19 (W)  
Milestone 3:  4/ 28  (F)  
Final report: 5/5 (F)


## Milestones

- [X] **Pre-requisite task 1**: Become initially familiar with Vortex (Resources reading list, and https://vortex.cc.gatech.edu/publications/vortex_micro21_final.pdf)

- [ ] **Milestone 1**: Implement a functioning C++ emulation implementation of fmadd on Vortex (SimX).

- [ ] **Milestone 2**:  Implement a functioning Verilog implementation of the BF16 floating point operation, including enabling the use of a specific CSR register to be set to define the floating-point precision of the next operation.


- [ ] ** Milestone 3**: Obtain latency, power consumption, and resource utilization on the FPGA

---

## TODO Milestone 2
- finish the bfloat class
    - operators (basic ones for now)
    - edge cases (zero, NAN, inf)
    - place class in common ? && update the makefiles so that it can be found
- integrate into SimX
    - determine the RISC-V float operations (so we know which instrcutions to convert to bfloat)
    - figure out how simx code is structured (deocde, execute)
    - plan how to toggle between ieee-754 float and out bfloat (use macro?)
    - insert the bfloat class and use it where necessary
- create test cases that do the traditional floating point operations but with out bfloat data type
    - take inspo from dogfood
- think about slides
    - create the slides LOL

## Commands and Vortex Reference

./ci/blackbox.sh --driver=simx --cores=4 --app=bf_dogfood --args=”-n16”
./ci/blackbox.sh --driver=simx --cores=4 --app=bf_dogfood --bf16

(need to add an arg for float type)

## Questions
- why does the single precision version have boxed NaN?
- why doesn't the double precision version not have a boxed NaN?
- how and where does the precision get set? -- **


Contributions

- research bf16: Udit
- implement base bf16 base class: Udit
- implmement addition and subtraction: Udit
- implement multiplication and division: Nick
- implement iee Nan, inf, 0 edge cases: Nick
- integrate code into SimX: Udit and Nick
- create test suite for SimX (bf_dogfood): Udit
- baseline verilog: Nick

TODO: 2-3 page report, more details
maybe start the presentation slides
timeline so far, and future plan
