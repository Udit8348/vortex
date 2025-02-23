# FPGA Startup and Configuration Guide

## CRNCH

### **C**enter for **R**esearch into **N**ovel **C**omputing **H**ierarchies

If you are associated with Georgia Tech you can use CRNCH's server to gain remote access to FPGA's (detailed CRNCH documentation [here](crnch.md)). Otherwise, you will have to supply your own compatible FPGA with a machine running a compatible OS (Ubuntu). 
> If you are working in your own FPGA environment, you will need to adapt the CRNCH specific steps to your own system.

### Login to CRNCH FPGA Node

CRNCH users need first need to login to FPGA specific hardware.

First, `ssh` into `rg-login` and navigate to the root of your vortex repo. If it is your first time, you will need to clone the repo. It must be stored in some subdirectory of `USERSCRATCH` because the files are synchronized betwen the login node and the FPGA node. For example,
```
ssh <your-gt-username>@rg-login.crnch.gatech.edu
cd USERSCRATCH
git clone --depth=1 --recursive https://github.com/vortexgpgpu/vortex.git
cd vortex
```

While you are logged into the `rg-login`node, you need to setup the vortex environment.

### First Time Evironment Setup
There are some one-time configurations you need to complete. If you are logging back in, you can skip to the regular environment setup steps.
```
# from root of vortex repo run config script to set bit size and vortex toolchain installation path
# check if config.mk file was generated from config.mk.in file
./configure --xlen=32 --tooldir=$HOME/tools

# now create a build folder
mkdir build
cd build

# run the configure script again (inside build) to generate more files 
../configure --xlen=32 --tooldir=$HOME/tools

# from build folder, install the whole prebuilt toolchain (one-time, long download)
./ci/toolchain_install.sh --all
```

### Environment Setup
If you completed the first time setup, and you are logging back in, you just need to follow these steps.

```
# from root of vortex repo run config script to set bit size and vortex toolchain installation path
# check if config.mk file was generated from config.mk.in file
./configure --xlen=32 --tooldir=$HOME/tools

# enter build directory
cd build

# run the configure script again (inside build) to generate more files
../configure --xlen=32 --tooldir=$HOME/tools
```

Now, you can enter the a FPGA connected node. These are any of the `flubber<1-9>` nodes. We can use `flubber1` for now. When we use a node for executing workloads, we need to allocate them from SLURM using `salloc`.


```
salloc -p rg-fpga --nodes=1 --ntasks-per-node=16 --mem=64G --nodelist flubber1 --time=12:00:00
cd ~/USERSCRATCH/vortex
cd build
source ./ci/toolchain_env.sh
unset VERILATOR_ROOT
```


### Check environment setup

To check if the toolchain is installed and functioning, this command should run properly
`verilator --version`


Synthesis for Xilinx Boards
----------------------

### Source Configuration Scripts
```
# From any directory
source /opt/xilinx/xrt/setup.sh
source /tools/reconfig/xilinx/Vitis/2023.1/settings64.sh
```

### Check Installed FPGA Platforms
`platforminfo -l` which tells us the correct name of the platform installed on the current fpga node. It should be used for the `PLATFORM` environment variable below. Otherwise, if there is an error then there was an issue with the previous two commands.

### Build the FPGA Bitstream
Exit the `build` directory back where the root directory contains the path `hw/syn/xilinx/xrt` which has the makefile used to generate the Vortex bitstream. (These files also exist in the build directory which was created in a eariler step, however the synthesis steps will not work from there. This is a known bug)

```
cd hw/syn/xilinx/xrt
PREFIX=test1 PLATFORM=xilinx_u50_gen3x16_xdma_5_202210_1 TARGET=hw NUM_CORES=1 nohup make > build_u50_hw_1c.log 2>&1 &
```
> This will run the synthesis as a process in the background using `nohup`. It will create a directory with the following format in the current folder:` BUILD_DIR := "\<PREFIX>\_\<PLATFORM>\_\<TARGET>"` as well as a log file with the name you provided in the current directory. Even if you disconnect from the node, it will continue to run in the background until the bitstream generatation completes or it fails with an error. You can check log files for the status. Run the command `while true; do cat build_u50_hw_1c.log; sleep 3; done`

If it completes successfully, the generated bitstream will be located under `<BUILD_DIR>/bin/vortex_afu.xclbin`

#### MISC
You can configure other settings for example:
```bash
CONFIGS="-DL2_ENABLE -DDCACHE_SIZE=8192" PREFIX=build_4c_u280 NUM_CORES=4 TARGET=hw PLATFORM=xilinx_u280_gen3x16_xdma_1_202310_1 nohup make > build_u250_hw_4c.log 2>&1 &
```

### Running a Program on Xilinx FPGA

Follow the env setup steps from above, and also build the vortex libary as the main readme describes.

The [blackbox.sh](./simulation.md) script within the build directory can be used to run a test with Vortex's xrt driver using the following command. Currently, apps can be any subfolder test under regression or opencl

`FPGA_BIN_DIR=<path to bitstream directory> TARGET=hw|hw_emu PLATFORM=<platform baseName> ./ci/blackbox.sh --driver=xrt --app=<test name>`

For example:

```
FPGA_BIN_DIR=`realpath /hw/syn/xilinx/xrt/test10_xilinx_u50_gen3x16_xdma_5_202210_1_hw/bin` TARGET=hw PLATFORM=xilinx_u50_gen3x16_xdma_5_202210_1 ./ci/blackbox.sh --driver=xrt --app=demo
```

Synthesis for Intel (Altera) Boards
----------------------

### OPAE Environment Setup


    $ source /opt/inteldevstack/init_env_user.sh
    $ export OPAE_HOME=/opt/opae/1.1.2
    $ export PATH=$OPAE_HOME/bin:$PATH
    $ export C_INCLUDE_PATH=$OPAE_HOME/include:$C_INCLUDE_PATH
    $ export LIBRARY_PATH=$OPAE_HOME/lib:$LIBRARY_PATH
    $ export LD_LIBRARY_PATH=$OPAE_HOME/lib:$LD_LIBRARY_PATH

### OPAE Build

The FPGA has to following configuration options:
- DEVICE_FAMILY=arria10 | stratix10
- NUM_CORES=#n

Command line:

    $ cd hw/syn/altera/opae
    $ PREFIX=test1 TARGET=fpga NUM_CORES=4 make

A new folder (ex: `test1_xxx_4c`) will be created and the build will start and take ~30-480 min to complete.
Setting TARGET=ase will build the project for simulation using Intel ASE.


### OPAE Build Configuration

The hardware configuration file `/hw/rtl/VX_config.vh` defines all the hardware parameters that can be modified when build the processor.For example, have the following parameters that can be configured:
- `NUM_WARPS`:   Number of warps per cores
- `NUM_THREADS`: Number of threads per warps
- `PERF_ENABLE`: enable the use of all profile counters

You configure the syntesis build from the command line:

    $ CONFIGS="-DPERF_ENABLE -DNUM_THREADS=8" make

### OPAE Build Progress

You could check the last 10 lines in the build log for possible errors until build completion.

    $ tail -n 10 <build_dir>/build.log

Check if the build is still running by looking for quartus_sh, quartus_syn, or quartus_fit programs.

    $ ps -u <username>

If the build fails and you need to restart it, clean up the build folder using the following command:

    $ make clean

The file `vortex_afu.gbs` should exist when the build is done:

    $ ls -lsa <build_dir>/synth/vortex_afu.gbs


### Signing the bitstream and Programming the FPGA

    $ cd <build_dir>
    $ PACSign PR -t UPDATE -H openssl_manager -i vortex_afu.gbs -o vortex_afu_unsigned_ssl.gbs
    $ fpgasupdate vortex_afu_unsigned_ssl.gbs

### Sample FPGA Run Test
Ensure you have the correct opae runtime for the FPGA target

```
$ TARGET=FPGA make -C runtime/opae
```

Run the [blackbox.sh](./simulation.md) from your Vortex build directory

```
$ TARGET=fpga ./ci/blackbox.sh --driver=opae --app=sgemm --args="-n128"
```

### FPGA sample test running OpenCL sgemm kernel

You can use the `blackbox.sh` script to run the following from your Vortex build directory

    $ TARGET=fpga ./ci/blackbox.sh --driver=opae --app=sgemm --args="-n128"

### Testing Vortex using OPAE with Intel ASE Simulation
Building ASE synthesis

```$ TARGET=asesim make -C runtime/opae```

Building ASE runtime

```$ TARGET=asesim make -C runtime/opae```

Running ASE simulation

```$ ASE_LOG=0 ASE_WORKDIR=<build_dir>/synth/work TARGET=asesim ./ci/blackbox.sh --driver=opae --app=sgemm --args="-n16"```
