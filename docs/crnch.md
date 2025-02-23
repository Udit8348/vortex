### What does CRNCH Offer?

**The Rogues Gallery (RG)**: new concept focused on developing our understanding of next-generation hardware with a focus on unorthodox and uncommon technologies. **RG** will acquire new and unique hardware (ie, the aforementioned “*rogues*”) from vendors, research labs, and startups and make this hardware available to students, faculty, and industry collaborators within a managed data center environment

### Why are the Rouges Important?

By exposing students and researchers to this set of unique hardware, we hope to foster cross-cutting discussions about hardware designs that will drive future *performance improvements in computing long after the Moore’s Law era of “cheap transistors” ends*. Specifically, the Rouges Gallery contains FPGA's which can be synthesized into Vortex hardware.

### How is the Rouges Gallery Funded?

Rogues Gallery testbed is primarily supported by the National Science Foundation (NSF) under NSF Award Number [#2016701](https://www.nsf.gov/awardsearch/showAward?AWD_ID=2016701&HistoricalAwards=false)

### Rouges Gallery Documentation

You can read about RG in more detail on its official documentation [page](https://gt-crnch-rg.readthedocs.io/en/main/index.html#).

You can listen to a talk about RG [here](https://mediaspace.gatech.edu/media/Jeff%20Young%20-%20Rogues%20Gallery%20-%20CRNCH%20Summit%202021/1_lqlgr0jj)

[CRNCH Summit 2023](https://github.com/gt-crnch/crnch-summit-2023/tree/main)

### Request Access for Rouges Gallery

You should use [this form](https://crnch-rg.cc.gatech.edu/request-rogues-gallery-access/) to request access to RG’s reconfigurable computing (vortex fpga) resources. You should receive an email with your ticket item being created. Once it gets processed, you should get an email confirmed your access has been granted. It might take some time to get processed.

### How to Access Rouges Gallery?
There are two methods of accessing CRNCH's Rouges Gallery
1) Web-based GUI: [rg-ood.crnch.gatech.edu](http://rg-ood.crnch.gatech.edu/) with VPN connection
2) SSH: `ssh <your-gt-username>@rg-login.crnch.gatech.edu`


### Where should I keep my files?
The CRNCH servers have a folder called `USERSCRATCH` which can be found in your home directory: `echo $HOME`. You should keep all your files in this folder since it is available across all the Rouges Gallery Nodes.

## **What Machines are Available in the Rogues Gallery?**

Complete list of machines can be found [here](https://gt-crnch-rg.readthedocs.io/en/main/general/rg-hardware.html). Furthermore, you can find detailed information about the FPGA hardware [here](https://gt-crnch-rg.readthedocs.io/en/main/reconfig/xilinx/xilinx-getting-started.html).

## Allocate an FPGA Node

Once you’ve connected to the CRNCH login node, you need to use the Slurm scheduler to request an interactive job using `salloc`. This [page](https://gt-crnch-rg.readthedocs.io/en/main/general/using-slurm.html) explains why we use Slurm to request resources. Documentation for `salloc` can be found [here](https://gt-crnch-rg.readthedocs.io/en/main/general/using-slurm-examples.html).

Here is an example of how to format the `salloc` job in order to to request 4 cores and 16GB of RAM for 8 hours on flubber1, a fpga dev node:
```bash
salloc -p rg-fpga --nodes=1 --ntasks-per-node=4 --mem=16G --nodelist flubber1 --time=08:00:00
```

There are a number of factors that you should consider if you want to choose the correct parameters for `salloc`. 

1. Understand the workload you intend to run. If you need to check some files, complete basic tasks, or anything else that is not compute intensive try to allocate a limited number of cores and memory. If you are running more computationally intensive tasks such as running compilation, running synthesis, analyzing a Vivado project, etc you should allocate more resources accordingly
2. Be mindful of other users, often times deadlines can overlap for multiple users and the server will experience corresponding load. Please only allocate what you need, even if there is more available. After a certain point, allocating more resources will have diminishing returns, so its better to leave them available for other's usage.
3. Check on the node before you allocate it. If you make an allocation and it does not succeed, its possible that you requested for more resources than it has available. In this scenario it will just want until those resources are free, but there is not a straightforward way of checking how long this will take. The best way to check the status of SLURM nodes is to use the `scontrol show nodes` command from any SLURM node, including `rg-login`.

### `scontrol show nodes`
It will print a lot of information, so its a good idea to pipe the output to a file. If there is a node you want to learn about specifically you can use grep, for example `scontrol show nodes | grep -A 10 "flubber9"`. Which greps for a string, "flubber9" in this case along with the next 10 lines of output.

```
NodeName=flubber9 Arch=x86_64 CoresPerSocket=32
   CPUAlloc=126 CPUEfctv=128 CPUTot=128 CPULoad=139.60
   AvailableFeatures=(null)
   ActiveFeatures=(null)
   Gres=(null)
   NodeAddr=flubber9 NodeHostName=flubber9 Version=24.11.0
   OS=Linux 4.18.0-553.34.1.el8_10.x86_64 #1 SMP Mon Dec 16 04:25:43 EST 2024
   RealMemory=510000 AllocMem=0 FreeMem=500394 Sockets=2 Boards=1
   State=MIXED ThreadsPerCore=2 TmpDisk=0 Weight=1 Owner=N/A MCS_label=N/A
   Partitions=rg-fpga
   BootTime=2025-01-27T13:51:30 SlurmdStartTime=2025-02-10T17:42:29
   LastBusyTime=2025-02-23T13:40:12 ResumeAfterTime=None
   CfgTRES=cpu=128,mem=510000M,billing=128
   AllocTRES=cpu=126
   CurrentWatts=0 AveWatts=0
```
From the example above, you can see that flubber9 is under severe load (`CPULoad=139.60`) and (126/128) cores are being utilized. At this time, a user may seek another node to accomplish their task, or have to wait until more resources are available.