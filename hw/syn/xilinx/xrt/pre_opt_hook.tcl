set tool_dir $::env(TOOL_DIR)
source ${tool_dir}/xilinx_async_bram_patch.tcl

report_utilization -file hier_utilization.rpt -hierarchical -hierarchical_percentages