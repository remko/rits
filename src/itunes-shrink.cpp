#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/core.hpp>

#include "Song.h"
#include "parser.h"
#include "tagger.h"
#include "encoder.h"

namespace fs = boost::filesystem;
namespace log = boost::log;

int main(int argc, const char* argv[]) {
	if (argc < 2) { std::cerr << "Missing target" << std::endl; return -1; }
	fs::path targetDir = argv[1];
	if (!fs::exists(targetDir)) { std::cerr << "Target does not exist" << std::endl; return -1; }

	log::core::get()->set_filter(log::trivial::severity >= log::trivial::trace);
	auto bitRate = 192U;
	auto syncRating = false;

	auto library = parseLibrary();
	auto songs = library.getSongs();
	songs.erase(std::remove_if(songs.begin(), songs.end(), [&](const Song& s) { 
		return !s.file || s.hasVideo;
	}), songs.end());

	auto songIndex = 0;
	for (auto song : songs) {
		auto sourceFile = *song.file;

		BOOST_LOG_TRIVIAL(debug) << "Processing " << sourceFile << " (" << songIndex << "/" << songs.size() << ")";

		if (!fs::exists(sourceFile)) {
			BOOST_LOG_TRIVIAL(warning) << "Song does not exist: " << sourceFile;
			continue;
		}

		auto targetFileDir = targetDir / song.id.substr(0, 2);
		if (!fs::is_directory(targetFileDir)) {
			fs::create_directory(targetFileDir);
		}
		auto targetFile = targetFileDir / (song.id + fs::extension(sourceFile));
		if (!fs::exists(targetFile)) {
			BOOST_LOG_TRIVIAL(debug) << targetFile << " does not exist";
			if (song.bitRate && song.bitRate > bitRate) {
				encode(sourceFile, targetFile, bitRate);
			}
			else {
				BOOST_LOG_TRIVIAL(info) << "Copying " << sourceFile << " -> " << targetFile;
				fs::copy_file(sourceFile, targetFile);
			}
		}
		else {
			BOOST_LOG_TRIVIAL(debug) << "Target file " << targetFile << " exists";
		}

		if (syncRating) {
			setRating(targetFile, song.rating);
		}

		songIndex++;
	}
	return 0;
}
