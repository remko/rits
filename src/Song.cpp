#include "Song.h"

#include <boost/algorithm/string/predicate.hpp>

#include "uridecode.h"

static unsigned int getUInt(const std::map<std::string, std::string>& values, const std::string& key) {
	auto i = values.find(key);
	if (i == values.end()) {
		return 0;
	}
	try {
		return std::stoul(i->second);
	}
	catch (const std::logic_error&) {
		return 0;
	}
}

Song::Song(const std::map<std::string,std::string>& properties) : 
		properties(properties), hasVideo(false), rating(0), bitRate(0) {
	id = properties.at("Persistent ID");
	rating = std::min(getUInt(properties, "Rating") / 20, 5U);
	bitRate = getUInt(properties, "Bit Rate");
	hasVideo = properties.find("Has Video") == properties.end();

	auto locationIt = properties.find("Location");
	if (locationIt != properties.end()) {
		std::string location = uridecode(locationIt->second);
		if (boost::starts_with(location, "file://")) {
			file = location.substr(7);
		}
	}
}
