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

| Note: The following information is just an FYI, when doing FPGA work on CRNCH follow the steps in the [FGPA docs](fpga_setup.md).

Once you’ve connected to the CRNCH login node, you could use the Slurm scheduler to request an interactive job using `salloc`. This [page](https://gt-crnch-rg.readthedocs.io/en/main/general/using-slurm.html) explains why we use Slurm to request resources. Documentation for `salloc` can be found [here](https://gt-crnch-rg.readthedocs.io/en/main/general/using-slurm-examples.html).


To request 16 cores and 64GB of RAM for 8 hours on flubber1, a fpga dev node:
```bash
salloc -p rg-fpga --nodes=1 --ntasks-per-node=16 --mem=64G --nodelist flubber1 --time=08:00:00
```
