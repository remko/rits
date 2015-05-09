#include <iostream>
#include <algorithm>
#include <set>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/core.hpp>
#include <boost/algorithm/string.hpp>

#include "Song.h"
#include "parser.h"
#include "tagger.h"
#include "encoder.h"

namespace fs = boost::filesystem;
namespace log = boost::log;

int main(int argc, const char* argv[]) {
	log::core::get()->set_filter(log::trivial::severity >= log::trivial::warning);

	auto library = parseLibrary();
	auto songs = library.getSongs();

	std::set<fs::path> songFiles;
	for (auto song : songs) {
		if (song.file) {
			songFiles.insert(*song.file); 
		}
	}

	// First pass: Check for pure equality. Fast
	std::vector<fs::path> notFoundFiles;
	for(fs::recursive_directory_iterator i(library.getMediaDir()); i != fs::recursive_directory_iterator(); ++i) {
		if (fs::is_directory(i->path()) || i->path().filename() == ".DS_Store" || i->path().extension() == ".plist" || i->path().extension() == ".m4r") {
			continue;
		}
		if (!songFiles.erase(i->path())) {
			notFoundFiles.push_back(i->path());
		}
	}

	// Second pass: Check for equivalence. Slow.
	for (auto notFoundFile : notFoundFiles) {
		auto equivalentFile = std::find_if(songFiles.begin(), songFiles.end(), [&](const fs::path& path) {
			return fs::equivalent(notFoundFile, path);
		});
		if (equivalentFile != songFiles.end()) {
			songFiles.erase(equivalentFile);
		}
		else {
			BOOST_LOG_TRIVIAL(warning) << "File not in library: " << notFoundFile;
		}
	}

	for (auto songFile : songFiles) {
		BOOST_LOG_TRIVIAL(warning) << "File not on filesystem: " << songFile;
	}

	return 0;
}
