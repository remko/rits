#include "encoder.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/log/trivial.hpp>
#include <iostream>

static std::string bashEscape(std::string s) {
	return std::string("\"") + boost::algorithm::replace_all_copy(boost::algorithm::replace_all_copy(s, "\"", "\\\""), "$", "\\$") + std::string("\"");
}

void encode(const boost::filesystem::path& source, const boost::filesystem::path& target, unsigned int bitrate) {
	BOOST_LOG_TRIVIAL(info) << "Encoding " << source << " -> " << target;

	auto temporaryFile = boost::filesystem::unique_path().replace_extension(boost::filesystem::extension(target));

	std::vector<std::string> command { 
		"ffmpeg", "-y", 
		"-i", bashEscape(source.native()), 
		"-vn", "-b:a", std::to_string(bitrate) + "k", 
		bashEscape(temporaryFile.native()) 
	};
	auto commandString = boost::algorithm::join(command, " ");

	BOOST_LOG_TRIVIAL(debug) << "+ " << commandString;
	int ret = system(commandString.c_str());
	if (ret) {
		BOOST_LOG_TRIVIAL(error) << "Error: " << ret;
		exit(-1);
	}

	BOOST_LOG_TRIVIAL(debug) << "Moving " << temporaryFile << " -> " << target;
	boost::filesystem::rename(temporaryFile, target);
}
