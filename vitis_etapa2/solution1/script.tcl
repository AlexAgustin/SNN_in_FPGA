############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
## Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
############################################################
open_project vitis_etapa2
set_top simulate_SNN
add_files includes/defines.h
add_files src/neuron.cpp
add_files includes/neuron.h
add_files src/snn.cpp
add_files includes/snn.h
add_files -tb src/first_network_tb.cpp
open_solution "solution1" -flow_target vivado
set_part {xczu7ev-ffvf1517-2LV-e}
create_clock -period 10 -name default
source "./vitis_etapa2/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -format ip_catalog
