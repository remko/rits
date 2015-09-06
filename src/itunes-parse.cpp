#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/core.hpp>

#include "parser.h"

namespace fs = boost::filesystem;
namespace log = boost::log;

int main(int argc, const char* argv[]) {
	log::core::get()->set_filter(log::trivial::severity >= log::trivial::trace);
	boost::optional<std::string> db;
	if (argc >= 2) {
		db = std::string(argv[1]);
	}

	auto songs = parseLibrary(db);
	std::cout << "parsed" << songs.getSongs().size() << "songs" << std::endl;
	return 0;
}

