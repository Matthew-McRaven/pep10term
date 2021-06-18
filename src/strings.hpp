#pragma once

#include <string>

const std::string application_name = "Pep10Term";
const std::string application_description = "Translate and run Pep/10 assembly language and microcode programs.";

const std::string asm_description = "Assemble a Pep/10 assembler source code program to object code.";
const std::string run_description = "Run a Pep/10 object code program.";
const std::string cpuasm_description = "Check a Pep/10 microcode program for syntax errors.";
const std::string cpurun_description = "Run a Pep/10 microcode program.";

const std::string asm_description_detailed = "The source_file must be a .pep file. \
The object_file must be a .pepo file. \
If there are assembly errors, an error log file named <source_file>_errLog.txt is created with the error messages. \
<source_file> is the name of source_file without the .pep extension. \
If there are no errors, the error log file is not created.";
const std::string run_description_detailed = "The object_file must be a .pepo file. \
If the program takes input, -i is required. \
If the program produces output, -o is required. \
As a guard against endless loops the program will abort after max_steps assembly instructions execute. \
The default value of max_steps is {}.";
const std::string cpuasm_description_detailed = "The microcode_file must be a .pepcpu file. \
If there are micro-assembly errors, an error log file named <microcode_file>_errLog.txt is created with the error messages. \
<microcode_file> is the name of microcode_file without the .pepcpu extension. \
If there are no errors, the error log file is not created. \
Supports the extended control section detailed in Pep9Micro.";
const std::string cpurun_description_detailed = "The microcode_file must be a .pepcpu file. \
If there are micro-assembly errors or UnitPost errors, an error log file named <microcode_file>_errLog.txt is created with the error messages. \
<microcode_file> is the name of source_file without the .pepcpu extension. \
If there are no errors the error log file is not created. \
If -p is specified, then all UnitPre and UnitPost statements in microcode_file are ignored. \
The UnitPre and UnitPost statments from precondition_file will be used instead. \
The precondition_file must be a .pepcpu file.";

const std::string asm_input_file_text = "Input Pep/10 source program for assembler.";
const std::string asm_output_file_text = "Output object code generated from source.";
const std::string asm_run_log = "Override the name of the default error log file.";
const std::string obj_input_file_text = "Input Pep/10 object code program for simulator.";
const std::string charin_file_text = "File buffered behind the charIn input port.";
const std::string charout_file_text = "File to which the charOut output port is streamed.";
const std::string charout_echo_text = "Echo data written to charOut to std::out.";
const std::string isaMaxStepText = "Override the default value of max_steps.";
const std::string microMaxStepText = "Override the default value of max_steps.";
const std::string cpuasm_input_file_text = "Input Pep/10 microcode source program for microassembler.";
const std::string cpu_asm_log = "Override the name of the default error log file.";

const std::string cpu_full_control = "Assemble the microprogram with the full control section (default is partial control section).";

const std::string cpu_preconditions = "Input Pep/10 microcode source program for microassembler.";
const std::string cpu_run_log = "Override the name of the default error log file.";