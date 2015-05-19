#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/optional.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/core.hpp>

#include "Song.h"
#include "parser.h"
#include "tagger.h"
#include "encoder.h"

namespace fs = boost::filesystem;
namespace po = boost::program_options;
namespace logs = boost::log;

struct Options {
	unsigned int bitRate;
	bool syncRating;
	fs::path targetDir;
};

boost::optional<Options> parseOptions(int argc, const char** argv) {
	Options result;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Show this help message")
		("bitrate", po::value<unsigned int>(&result.bitRate)->default_value(192), "Set bitrate")
		("sync-rating", "Sync rating")
		("target-dir", po::value<std::string>(), "Target dir")
	;
	po::positional_options_description p;
	p.add("target-dir", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return boost::optional<Options>();
	}

	if (!vm.count("target-dir")) {
		std::cerr << "Missing target dir" << std::endl; 
		return boost::optional<Options>();
	}
	result.targetDir = vm["target-dir"].as<std::string>();
	if (!fs::exists(result.targetDir)) { 
		std::cerr << "Target does not exist" << std::endl;
		return boost::optional<Options>();
	}

	result.syncRating = vm.count("sync-rating");

	return result;
}

int main(int argc, const char* argv[]) {
	auto options = parseOptions(argc, argv);
	if (!options) {
		return -1;
	}

	logs::core::get()->set_filter(logs::trivial::severity >= logs::trivial::trace);

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

		auto targetFileDir = options->targetDir / song.id.substr(0, 2);
		if (!fs::is_directory(targetFileDir)) {
			fs::create_directory(targetFileDir);
		}
		auto targetFile = targetFileDir / (song.id + fs::extension(sourceFile));
		if (!fs::exists(targetFile)) {
			BOOST_LOG_TRIVIAL(debug) << targetFile << " does not exist";
			if (song.bitRate && song.bitRate > options->bitRate) {
				encode(sourceFile, targetFile, options->bitRate);
			}
			else {
				BOOST_LOG_TRIVIAL(info) << "Copying " << sourceFile << " -> " << targetFile;
				fs::copy_file(sourceFile, targetFile);
			}
		}
		else {
			BOOST_LOG_TRIVIAL(debug) << "Target file " << targetFile << " exists";
		}

		if (options->syncRating) {
			setRating(targetFile, song.rating);
		}

		songIndex++;
	}
	return 0;
}
