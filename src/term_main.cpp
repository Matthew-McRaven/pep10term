#include <iostream>
#include <map>
#include <fstream>
#include <filesystem>


#include <fmt/core.h>
#include <magic_enum.hpp>
#include "CLI11.hpp"

#include "asmb/pep10/create_driver.hpp"
#include "assemble.hpp"
#include "formatter.hpp"
#include "gen_about.h"
#include "isa/pep10/from_elf.hpp"
#include "isa/pep10/local_machine.hpp"
#include "masm/conversion.hpp"
#include "masm/utils/listing.hpp"
#include "run.hpp"
#include "strings.hpp"
#include "term_version.hpp"
#include "utils.hpp"

struct command_line_values {
	bool had_version{false}, had_about{false}, had_d2{false}, had_full_control{false}, had_echo_output{false},
		enable_elf{false};
	std::string e{}, s{}, o{}, i{}, mc{}, p{}, os{}, fig{}, macro{}, obj, elf;
	uint64_t m{2500};
	uint64_t ch;
};

void handle_full_control(command_line_values&);
void handle_version(command_line_values&, int64_t);
void handle_about(command_line_values&, int64_t);
void handle_macro(command_line_values&);
void handle_ls_macros(command_line_values&);
void handle_ls_figures(command_line_values&);
void handle_figure(command_line_values&);
void handle_asm(command_line_values&);
void handle_run(command_line_values&);
//void handle_cpuasm(command_line_values&);
//void handle_cpurun(command_line_values&);

int main(int argc, char *argv[])
{
	// Perform any global inits here.
	// TODO: Determine global init's

	command_line_values values;

	CLI::App parser{application_description, application_name};

	// For each subcommand (key), mantain a list of flag names (sub-key) and the pretty-print name of the value
	// (sub-value). This allows for formatting as requested by Dr. Warford such as (-e error_file),
	// since the default CLI11 framework does not allow custom fields.
	std::map<std::string, std::map<std::string,std::string>> parameter_formatting;
	// For a given subcommand (key) add an additional lengthened description of the subcommand (value).
	// Must be passed to the custom formatter, since the formatter is responsible for "switching" descriptions.
	std::map<std::string, std::string> detailed_descriptions;

	parser.formatter(std::make_shared<TermFormatter>(parameter_formatting, detailed_descriptions));
	// Top level option flags
	auto help = parser.set_help_flag("--help,-h", "Show this help information.");
	// Setting help all flag to empty prevents it from being generated.
	auto help_all = parser.set_help_all_flag("", "");
	//auto help_all = parser.set_help_all_flag("--help-all", "Show help information for all subcommands.");
	std::string version_string =  "Display program version number.";
	auto version_flag = parser.add_flag("-v,--version", [&](int64_t flag){handle_version(values,flag);}, version_string);

	std::string about_string =  "Display information about licensing, Qt, and developers.";
	auto about_flag = parser.add_flag("--about", [&](int64_t flag){handle_about(values,flag);}, about_string);

	// Subcommands for MACRO
	// Must create map for flag value names.
	parameter_formatting.insert_or_assign("macro", std::map<std::string,std::string>());
	auto macro_subcommand = parser.add_subcommand("macro", macro_description);
	// Create detailed description.
	detailed_descriptions["macro"] = macro_description_detailed;
	// Name of macro
	macro_subcommand->add_option("--macro", values.macro, maco_name_text)->expected(1)->required(1);
	parameter_formatting["macro"]["macro"] = "macro_name";
	// Create a runnable application from command line arguments.
	macro_subcommand->callback(std::function<void()>([&](){handle_macro(values);}));

	// Subcommands for LS-MACRO
	// Must create map for flag value names.
	parameter_formatting.insert_or_assign("ls-macros", std::map<std::string,std::string>());
	auto ls_macros_subcommand = parser.add_subcommand("ls-macros", ls_macros_description);
	// Create detailed description.
	detailed_descriptions["ls-macros"] = ls_macros_description_detailed;
	ls_macros_subcommand->callback(std::function<void()>([&](){handle_ls_macros(values);}));


	// Subcommands for FIGURE
	// Must create map for flag value names.
	parameter_formatting.insert_or_assign("figure", std::map<std::string,std::string>());
	auto figure_subcommand = parser.add_subcommand("figure", figure_description);
	// Create detailed description.
	detailed_descriptions["figure"] = figure_description_detailed;
	// Chapter number for figure.
	figure_subcommand->add_option("--ch", values.ch, figure_chapter_number_text)->expected(1)->required(1);
	parameter_formatting["figure"]["ch"] = "chapter";
	// Figure name for figure.
	figure_subcommand->add_option("--fig", values.fig, figure_figure_name_text)->expected(1)->required(1);
	parameter_formatting["figure"]["fig"] = "figure_name";
	// Create a runnable application from command line arguments.
	figure_subcommand->callback(std::function<void()>([&](){handle_figure(values);}));

	// Subcommands for LS-FIGURE
	// Must create map for flag value names.
	parameter_formatting.insert_or_assign("ls-figures", std::map<std::string,std::string>());
	auto ls_figures_subcommand = parser.add_subcommand("ls-figures", ls_figures_description);
	// Create detailed description.
	detailed_descriptions["ls-figures"] = ls_figures_description_detailed;
	// Create a runnable application from command line arguments.
	ls_figures_subcommand->callback(std::function<void()>([&](){handle_ls_figures(values);}));


	// Subcommands for ASSEMBLE
	// Must create map for flag value names.
	parameter_formatting.insert_or_assign("asm", std::map<std::string,std::string>());
	auto asm_subcommand = parser.add_subcommand("asm", asm_description);
	// Create detailed description.
	detailed_descriptions["asm"] = asm_description_detailed;
	// Flag to enable dumping the object code as ELF in addition to .pepo.
	asm_subcommand->add_flag("--enable-elf", values.enable_elf, asm_enable_elf);
	// File where errors will be written. By default, will be written to a file based on object code file name.
	asm_subcommand->add_option("-e", values.e, asm_run_log)->expected(1);
	parameter_formatting["asm"]["e"] = "error_file";
	// File from which Pep/9 assembly code will be loaded.
	asm_subcommand->add_option("-s", values.s, asm_input_file_text)->expected(1)->required(1);
	parameter_formatting["asm"]["s"] = "source_file";
	// File from which Pep/9 os code will be loaded.
	asm_subcommand->add_option("--os", values.os, asm_input_os_text)->expected(1);
	parameter_formatting["asm"]["os"] = "operating_system";
	// File to which object code will be written.
	asm_subcommand->add_option("-o", values.o, asm_output_file_text)->expected(1)->required(1);
	parameter_formatting["asm"]["o"] = "object_file";
	// Create a runnable application from command line arguments
	asm_subcommand->callback(std::function<void()>([&](){handle_asm(values);}));

	// Subcommands for RUN
	parameter_formatting.insert_or_assign("run", std::map<std::string,std::string>());
	auto run_subcommand = parser.add_subcommand("run", run_description);
	// TODO: Fix!!
	detailed_descriptions["run"] = fmt::format(run_description_detailed, 1000);
	// Batch input that will be loaded into charIn.
	run_subcommand->add_option("-i", values.i, charin_file_text)->expected(1);
	parameter_formatting["run"]["i"] = "charin_file";
	// File where values written to charOut will be stored.
	run_subcommand->add_option("-o", values.o, charout_file_text)->expected(1);
	parameter_formatting["run"]["o"] = "charout_file";
	run_subcommand->add_flag("--echo-output", values.had_echo_output, charout_echo_text);
	//run_subcommand->add_option("-e", obj_input_file_text);
	// Maximum number of instructions to be executed.
	std::string max_steps_text = isaMaxStepText;
	// TODO: Set correctly
	run_subcommand->add_option("-m", values.m, max_steps_text)->expected(1)->check(CLI::PositiveNumber)
			->default_val(std::to_string(1000));
	parameter_formatting["run"]["m"] = "max_steps";
	// File from which object code will be loaded.
	auto opt_run_obj = run_subcommand->add_option("--obj", values.obj, obj_input_file_text)->expected(1);
	parameter_formatting["run"]["obj"] = "object_file";
	// File from which object code will be loaded.
	auto opt_run_elf = run_subcommand->add_option("--elf", values.elf, elf_input_file_text)->expected(1);
	parameter_formatting["run"]["elf"] = "elf_file";
	// Make options mutually exclusive.
	opt_run_obj->excludes(opt_run_elf);
	opt_run_elf->excludes(opt_run_obj);
	// Create a runnable application from command line arguments
	run_subcommand->callback(std::function<void()>([&](){handle_run(values);}));

	/* TODO: Restore CPU functionality.
	// Subcommands for CPUASM
	parameter_formatting.insert_or_assign("cpuasm", std::map<std::string,std::string>());
	auto cpuasm_subcommand = parser.add_subcommand("cpuasm", cpuasm_description);
	detailed_descriptions["cpuasm"] = cpuasm_description_detailed;
	// File where errors will be written. By default, will be written to a file based on the mc name.
	cpuasm_subcommand->add_option("-e", values.e, cpu_asm_log)->expected(1);
	parameter_formatting["cpuasm"]["e"] = "error_file";
	// Add flags to select 1-byte or 2-byte CPU data bus.
	auto cpuasm_d2_flag = cpuasm_subcommand->add_flag("--d2", [&](int64_t){handle_databus_size(values, true);}, cpu_2byte);
	// Microcode input file.
	cpuasm_subcommand->add_option("-s", values.mc, cpuasm_input_file_text)->expected(1)->required(true);
	parameter_formatting["cpuasm"]["s"] = "microcode_file";
	// Allow full control section to be enabled iff 2-byte data bus is enabled.
	// auto cpuasm_full_ctrl_flag = cpuasm_subcommand->add_flag("--full-control", [&](int64_t){handle_full_control(values, true);}, cpu_full_control);
	// cpuasm_full_ctrl_flag->needs(cpuasm_d2_flag);
	// Create a runnable application from command line arguments
	cpuasm_subcommand->callback(std::function<void()>([&](){handle_cpuasm(values, &run);}));

	// Subcommands for CPURUN
	parameter_formatting.insert_or_assign("cpurun", std::map<std::string,std::string>());
	auto cpurun_subcommand = parser.add_subcommand("cpurun", cpurun_description);
	detailed_descriptions["cpurun"] = cpurun_description_detailed;
	// File where errors will be written. By default, will be written to a file based on the mc name.
	cpurun_subcommand->add_option("-e", values.e, cpu_run_log)->expected(1);
	parameter_formatting["cpurun"]["e"] = "error_file";
	// Add flags to select 1-byte or 2-byte CPU data bus.
	auto cpurun_d2_flag = cpurun_subcommand->add_flag("--d2", [&](int64_t){handle_databus_size(values, true);}, cpu_2byte_run);
	// Allow full control section to be enabled iff 2-byte data bus is enabled.
	//auto cpurun_full_ctrl_flag = cpurun_subcommand->add_flag("--full-control",[&](int64_t){handle_full_control(values, true);}, cpu_full_control);
	//cpurun_full_ctrl_flag->needs(cpurun_d2_flag);
	// Maximum number of cycles to be executed.
	//std::string max_cycles_text = QString::fromStdString(microMaxStepText).arg(BoundExecMicroCpu::getDefaultMaxCycles()).toStdString();
	//cpurun_subcommand->add_option("-m", values.m, max_cycles_text)->expected(1)->needs(cpurun_full_ctrl_flag)->check(CLI::PositiveNumber)
			//->default_val(std::to_string(BoundExecMicroCpu::getDefaultMaxCycles()));
	// Precondition input file.
	cpurun_subcommand->add_option("-p", values.p, cpu_preconditions)->expected(1);
	parameter_formatting["cpurun"]["p"] = "precondition_file";
	// Microcode input file.
	cpurun_subcommand->add_option("-s", values.mc, cpuasm_input_file_text)->expected(1)->required(true);
	parameter_formatting["cpurun"]["s"] = "microcode_file";
	// Create a runnable application from command line arguments
	cpurun_subcommand->callback(std::function<void()>([&](){handle_cpurun(values, &run);}));
	*/

	// Require that one of the modes be used.
	parser.require_subcommand();

	try {
		parser.parse(argc, argv);
	} catch(const CLI::ValidationError &e){
		std::cout << e.what() << std::endl;
		return e.get_exit_code();
	} catch (const CLI::ParseError &e) {
		if(values.had_about || values.had_version) return 0;
		return parser.exit(e);
	}
	// All parsing was done in the try block. 
	// If no errors were raised, then by definition it succeeded.
	return 0;
}

void handle_full_control(command_line_values &values, bool use_full_control)
{
	values.had_full_control = use_full_control;
}

void handle_version(command_line_values &values, int64_t)
{
	values.had_version = true;
	std::cout << fmt::format("{} {}.{}", application_name, PepTerm_VERSION_MAJOR, PepTerm_VERSION_MINOR) << std::endl;
	std::cout << "Based on commit: "<< Version::GIT_SHA1 << std::endl;
}

void handle_about(command_line_values &values, int64_t)
{
	values.had_about = true;
	std::cout << fmt::format("{} {}.{}", application_name, PepTerm_VERSION_MAJOR, PepTerm_VERSION_MINOR) << std::endl;
	std::cout << "Based on commit: "<< Version::GIT_SHA1 << std::endl << std::endl;
	std::cout << about_txt << std::endl;
}

void handle_macro(command_line_values &values)
{
	auto figure = read_macro(values.macro);
	std::cout << fmt::format("Computer Systems, 6th edition.\nMacro {}", values.macro) << std::endl << std::endl;
	if(!figure) std::cout << "Figure not found!" << std::endl;
	else std::cout << *figure << std::endl;
}

void handle_ls_macros(command_line_values&)
{
	auto ex = registry::instance();
	std::cout << "Included macros:" << std::endl;
	for (const auto& macro : ex.macros()) {
		std::cout << fmt::format("Macro {} ", macro.name) << std::endl;
	}
	std::cout << std::endl;
}

void handle_ls_figures(command_line_values&)
{
	auto ex = registry::instance();
	std::cout << "Included figures: filetypes" << std::endl;
	for (const auto& figure : ex.figures()) {
		std::cout << fmt::format("Figure {}.{}: ", figure.chapter, figure.fig);
		for(auto type : figure.elements) {
			auto as_str = magic_enum::enum_name(type.first);
			std::cout <<  as_str.substr(1, as_str.size()) <<" ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	
}

void handle_figure(command_line_values &values)
{
	auto figure = read_figure(values.ch, values.fig, element_type::kPep);
	std::cout << fmt::format("Computer Systems, 6th edition.\nFigure {}.{}", values.ch, values.fig) << std::endl << std::endl;
	if(!figure) std::cout << "Figure not found!" << std::endl;
	else std::cout << *figure << std::endl;
}

void handle_asm(command_line_values &values)
{
	if(!std::filesystem::exists(values.s) && !is_resource(values.s)) throw CLI::ValidationError(fmt::format(err_fail_to_open, values.s), -1);

	auto text_source = read_file_or_resource(values.s);
	std::string text_os;
	if(!values.os.empty()) {
		if(!std::filesystem::exists(values.os)) throw CLI::ValidationError(fmt::format(err_fail_to_open, values.os), -1);
		text_os= read_file_or_resource(values.os);
	}
	else {
		text_os = read_default_os();
	}
	auto result = asmb::pep10::driver::assemble(text_source, text_os);
	auto project = std::get<1>(result);
	/*if(result.err_or_warning) {
		// TODO: Write warnings / errors to file.
	}*/
	if(!std::get<0>(result)) {
		throw std::logic_error("Assembly failed.");
	}
	auto listing = masm::utils::generate_listing(project->image->user);
	auto oc = masm::utils::generate_formatted_bytecode(project->image->user);
	std::cout << listing << std::endl;
	/*
	 * Open up listing file
	 */
	auto listing_file = std::filesystem::path(values.o).replace_extension(".pepl");
	save_listing(listing, listing_file);
	/*
	 * Open up object code file
	 */
	save_object_code(oc, {values.o});
	/*
	 * Open up ELF file
	 */
	auto elf_file = std::filesystem::path(values.o).replace_extension(".elf");
	if(values.enable_elf) project->as_elf->save(elf_file);


}

void handle_run(command_line_values &values)
{
	ELFIO::elfio reader;
	if(values.obj.empty() && values.elf.empty()) {
		throw CLI::ValidationError("Either --elf or --obj is required, but neither was supplied.", -1);
	}
	else if(!values.obj.empty()) throw std::logic_error("--obj is not yet implemented.");
	else reader.load(values.elf);

	std::string text_input = "";
	if(!values.i.empty()) {
		text_input = read_file_or_resource(values.i);
	}
	auto bytes_input = std::vector<uint8_t>(text_input.begin(), text_input.end());

	auto machine = isa::pep10::machine_from_elf<false>(reader).value();
	isa::pep10::load_user_program(reader, machine, isa::pep10::Loader::kDiskIn);

	// Load input.
	auto charIn = machine->input_device("charIn").value();
	components::storage::buffer_input(*charIn, bytes_input);

	// Initialize and run machine
	machine->begin_simulation();
	auto run_result = isa::pep10::run(machine, values.m);
	machine->end_simulation();

	// Must create a new endpoint so as to avoid modifying the state of charOut.
	auto charOut = machine->output_device("charOut").value();
	auto endpoint = charOut->endpoint()->clone();
	endpoint->set_to_head();

	// Accumulate bytes written to endpoint into bytes_output.
	std::vector<uint8_t> bytes_output;
	// next_value() will return std::nulllopt when no values remain;
	std::optional<uint8_t> temp = endpoint->next_value();
	while(temp) {
		bytes_output.emplace_back(uint8_t{*temp});
		if(values.had_echo_output) std::cout << (char) *temp;
		temp = endpoint->next_value();
	}

	if(values.had_echo_output) std::cout << std::endl;
	if(!values.o.empty()) save_runtime_output(bytes_output, {values.o});
	auto log_file = std::filesystem::path(values.o).replace_extension(".log");
	if(run_result.has_error()) {
		auto msg = run_result.error().message();
		std::ofstream log(log_file);
		log << msg.c_str();
		log.close();
	}
	else if(run_result.value() == true) {
		std::ofstream log(log_file);
		log << "Possible endless loop detected." << std::endl;
		log.close();
	}
}
/*
TODO: Switch to threads
void handle_asm(command_line_values &values)
{
	// Needs a assembler source program to be well defined.
	if(values.s.empty()) {
		//qDebug() << "Must set assembler input (-s).";
		throw CLI::ValidationError("Must set assembler input (-s).", -1);
	}
	// Needs an object code output to be well defined.
	else if(values.o.empty()) {
		//qDebug() << "Must set object code output (-o).";
		throw CLI::ValidationError("Must set object code output (-o).", -1);
	}

	// File names associated with cli parameters.
	QString sourceFileString = QString::fromStdString(values.s);
	QString objectFileString = QString::fromStdString(values.o);

	QFile sourceFile(sourceFileString);
	QString sourceText;
	// Read to assembler source, or return error that it could not be opened.
	if(!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		//qDebug().noquote() << errLogOpenErr.arg(sourceFile.fileName());
		throw CLI::ValidationError(errLogOpenErr.arg(sourceFile.fileName()).toStdString(), -1);
	}
	else {
		QTextStream sourceStream(&sourceFile);
		sourceText = sourceStream.readAll();
		sourceFile.close();

		ASMBuildHelper *helper = new ASMBuildHelper(sourceText, objectFileString,
													*AsmProgramManager::getInstance());
		if(!values.e.empty()) {
			helper->set_error_file(QString::fromStdString(values.e));
		}

		QObject::connect(helper, &ASMBuildHelper::finished, QCoreApplication::instance(), &QCoreApplication::quit);

		(*runnable) = helper;
	}
}

void handle_run(command_line_values &values)
{
	// Needs a source object code program to be well defined.
	if(values.s.empty()) {
		//qDebug() << "Must set object code input input (-s).";
		throw CLI::ValidationError("Must set object code input input (-s).", -1);
	}
	// Needs an output log.
	else if(values.o.empty()) {
		//qDebug() << "Must set output log file (-o).";
		throw CLI::ValidationError("Must set output log file (-o).", -1);
	}

	// File names associated with cli parameters.
	QString objCodeFileName = QString::fromStdString(values.s);
	QString textInputFileName = QString::fromStdString(values.i);
	QString textOutputFileName = QString::fromStdString(values.o);
	// Attempt to parse stepMax string as an integer.
	quint64 stepMaxValue = values.m;

	// Load object code string from file if possible, else print error log.
	QFile objFile(objCodeFileName);
	if(!objFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		//qDebug().noquote() << errLogOpenErr.arg(objFile.fileName());
		throw CLI::ValidationError(errLogOpenErr.arg(objFile.fileName()).toStdString(), -1);
	}

	QTextStream objStream(&objFile);
	QString objText = objStream.readAll();
	objFile.close();

	ASMRunHelper *helper = new ASMRunHelper(objText, stepMaxValue, textOutputFileName,
									textInputFileName, *AsmProgramManager::getInstance());
	helper->set_echo_charout(values.had_echo_output);
	QObject::connect(helper, &ASMRunHelper::finished, QCoreApplication::instance(), &QCoreApplication::quit);

	(*runnable) = helper;
}*/

/*
TODO: Restore CPU code
void handle_cpuasm(command_line_values &values, QRunnable **runnable)
{
	// Needs a microcode source program to be well defined.
	if(values.mc.empty()) {
		//qDebug() << "Must set microcode input (--mc).";
		throw CLI::ValidationError("Must set microcode input (--mc).", -1);
	}

	// Determine CPU type.
	Enu::CPUType type = Enu::CPUType::OneByteDataBus;
	if(values.had_d2) {
	type = Enu::CPUType::TwoByteDataBus;
	}

	// Enable or disable full control section features depending on passed flags.
	Pep::initMicroEnumMnemonMaps(type, values.had_full_control);

	// Setup parsing of microcode program.
	QString sourceFileString = QString::fromStdString(values.mc);
	QFile sourceFile(sourceFileString);
	QString sourceText;

	// Check that the file can be opened, otherwise log an error to the console.
	if(!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		//qDebug().noquote() << errLogOpenErr.arg(sourceFile.fileName());
		throw CLI::ValidationError(errLogOpenErr.arg(sourceFile.fileName()).toStdString(), -1);
	}
	else {
		// Otherwise read the file.
		QTextStream sourceStream(&sourceFile);
		// Must remove line numbers, on microassembler will raise spurious errors.
		sourceText = Pep::removeCycleNumbers(sourceStream.readAll());
		sourceFile.close();

		auto helper = new CPUBuildHelper(type, values.had_full_control, sourceText, sourceFile);
		if(!values.e.empty()) {
			helper->set_error_file(QString::fromStdString(values.e));
		}
		QObject::connect(helper, &CPUBuildHelper::finished, QCoreApplication::instance(), &QCoreApplication::quit);
		(*runnable) = helper;
	}

}

void handle_cpurun(command_line_values &values, QRunnable **run)
{
	// Needs a microcode source program to be well defined.
	if(values.mc.empty()) {
		//qDebug() << "Must set microcode input (--mc).";
		throw CLI::ValidationError("Must set microcode input (--mc).", -1);
	}

	// Determine CPU type.
	Enu::CPUType type = Enu::CPUType::OneByteDataBus;
	if(values.had_d2) {
	type = Enu::CPUType::TwoByteDataBus;
	}

	// Enable or disable full control section features depending on passed flags.
	Pep::initMicroEnumMnemonMaps(type, values.had_full_control);

	// Microcode input file and unit test output file.
	QString microcodeFileName = QString::fromStdString(values.mc);

	// Load object code string from file if possible, else print error log.
	QFile microcodeFile(microcodeFileName);
	if(!microcodeFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		//qDebug().noquote() << errLogOpenErr.arg(microcodeFile.fileName());
		throw CLI::ValidationError(errLogOpenErr.arg(microcodeFile.fileName()).toStdString(), -1);
	}

	QTextStream microprogramStream(&microcodeFile);
	QString microprogramText = Pep::removeCycleNumbers(microprogramStream.readAll());
	microcodeFile.close();

	// Load overriding preconditions if present.
	QString preconditionText;
	if(!values.mc.empty()) {
		QString preconditionFileName = QString::fromStdString(values.mc);
		QFile preconditionFile(preconditionFileName);
		// If passed precondition file that can't be opened, raise an error.
		if(!preconditionFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			//qDebug().noquote() << errLogOpenErr.arg(preconditionFile.fileName());
			throw CLI::ValidationError(errLogOpenErr.arg(preconditionFile.fileName()).toStdString(), -1);
		}

		QTextStream preconditionStream(&preconditionFile);
		preconditionText = Pep::removeCycleNumbers(preconditionStream.readAll());
		preconditionFile.close();
	}

	// If not using the full control section, use the non-branch enabled CPU simulator.
	if(!values.had_full_control) {
		CPURunHelper *helper = new CPURunHelper(type, microprogramText,
											QFileInfo(microcodeFile), preconditionText,
											nullptr);
		if(!values.e.empty()) {
			helper->set_error_file(QString::fromStdString(values.e));
		}

		QObject::connect(helper, &CPURunHelper::finished, QCoreApplication::instance(), &QCoreApplication::quit);
		(*run) = helper;
	}
	else {
		// Attempt to parse stepMax string as an integer.
		quint64 stepMaxValue = values.m;

		MicroStepHelper *helper = new MicroStepHelper(stepMaxValue,
													microprogramText,
													QFileInfo(microcodeFile),
													preconditionText,
													nullptr);
		if(!values.e.empty()) {
			helper->set_error_file(QString::fromStdString(values.e));
		}

		QObject::connect(helper, &MicroStepHelper::finished, QCoreApplication::instance(), &QCoreApplication::quit);
		(*run) = helper;

	}
}
*/