/* cfiles, an analysis frontend for the Chemfiles library
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/
#include <docopt/docopt.h>

#include "Convert.hpp"
#include "utils.hpp"

using namespace chemfiles;

static const char OPTIONS[] =
R"(cfiles convert: trajectory conversion

Convert trajectories from one format to another, automatically guessing the
format to use based on the files extension. The '--input-format' and
'--output-format' can be used to force the format.

Usage:
  cfiles convert [options] <input> <output>
  cfiles convert (-h | --help)

Options:
  -h --help                     show this help
  --input-format=<format>       force the input file format to be <format>
  --output-format=<format>      force the output file format to be <format>
  -t <path>, --topology=<path>  alternative topology file for the input
  -c <cell>, --cell=<cell>      alternative unit cell for the input. <cell>
                                should be formated using one of the
                                <a:b:c:α:β:γ> or <a:b:c> or <L> formats.
)";

struct convert_options {
    std::string infile;
    std::string outfile;
    std::string input_format;
    std::string output_format;
    bool custom_cell;
    chemfiles::UnitCell cell;
    std::string topology;
};

static convert_options parse_options(int argc, const char* argv[]) {
    convert_options options;
    auto args = docopt::docopt(OPTIONS, {argv, argv + argc}, true, "");

    options.infile = args["<input>"].asString();
    options.outfile = args["<output>"].asString();

    if (args["--input-format"]){
        options.input_format = args["--input-format"].asString();
    } else {
        options.input_format = "";
    }

    if (args["--output-format"]){
        options.output_format = args["--output-format"].asString();
    } else {
        options.output_format = "";
    }

    if (args["--topology"]){
        options.topology = args["--topology"].asString();
    } else {
        options.topology = "";
    }

    if (args["--cell"]) {
		options.cell = parse_cell(args["--cell"].asString());
        options.custom_cell = true;
	} else {
        options.custom_cell = false;
    }

    return options;
}


std::string Convert::description() const {
    return "Convert trajectories between formats";
}

std::string Convert::help() const {
    return OPTIONS;
}

int Convert::run(int argc, const char* argv[]) {
    auto options = parse_options(argc, argv);

    auto infile = Trajectory(options.infile, 'r', options.input_format);
    auto outfile = Trajectory(options.outfile, 'w', options.output_format);

    if (options.custom_cell) {
        infile.set_cell(options.cell);
    }

    if (options.topology != "") {
        infile.set_topology(options.topology);
    }

    for (size_t i=0; i<infile.nsteps(); i++) {
        auto frame = infile.read();
        outfile.write(frame);
    }

    return 0;
}