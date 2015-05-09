#pragma once

#include <string>
#include <map>
#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>

struct Song {
	Song(const std::map<std::string,std::string>& properties);

	std::map<std::string, std::string> properties;

	std::string id;
	bool hasVideo;
	boost::optional<boost::filesystem::path> file;
	unsigned int rating;
	unsigned int bitRate;
};
