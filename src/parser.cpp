#include "parser.h"

#include <libxml/parser.h>
#include <map>
#include <set>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <algorithm>

#include "uridecode.h"

namespace fs = boost::filesystem;

class Parser {
	public:
		Parser();

		const std::vector<Song>& getSongs() const {
			return songs;
		}

		const std::string& getMediaDir() const {
			return mediaDir;
		}

		void handleStartElement(const std::string&);
		void handleEndElement(const std::string&);
		void handleCharacterData(const std::string&);
	
	private:
		int level;
		std::string lastText;
		bool inMediaDir;
		std::string mediaDir;
		bool inTracks;
		std::string currentKey;
		std::map<std::string, std::string> data;
		std::vector<Song> songs;

		// for verification only
		std::set<std::string> ids;
};

Parser::Parser() : level(0), inMediaDir(false), inTracks(false) {
}

void Parser::handleStartElement(const std::string& tag) {
	level++;
	lastText = "";
}

void Parser::handleEndElement(const std::string& tag) {
	level--;
	if (level == 2) {
		if (inMediaDir && tag != "key") {
			mediaDir = uridecode(lastText);
			boost::algorithm::replace_all(mediaDir, "file://localhost", "file://");
			boost::algorithm::replace_all(mediaDir, "file://", "");
		}
		if (tag == "key" && lastText == "Tracks") {
			inTracks = true;
		}
		else {
			inTracks = false;
		}
		if (tag == "key" && lastText == "Music Folder") {
			inMediaDir = true;
		}
		else {
			inMediaDir = false;
		}
	}
	if (inTracks) {
		if (level == 4) {
			if (tag == "key") {
				currentKey = lastText;
			}
			else if (!currentKey.empty()) {
				data[currentKey] = lastText;
			}
		}
		else if (level == 3 && data.size()) {
			Song song(data);
			int wasInserted = ids.insert(song.id).second;
			if (!wasInserted) {
				BOOST_LOG_TRIVIAL(error) << "Duplicate with ID " << song.id;
				exit(-1);
			}
			songs.push_back(song);
			data.clear();
		}
	}
	lastText = "";
}

void Parser::handleCharacterData(const std::string& text) {
	lastText = lastText + text;
}

static void handleStartElement(void* parser, const xmlChar* name, const xmlChar*, const xmlChar*, int, const xmlChar**, int, int, const xmlChar**) {
	static_cast<Parser*>(parser)->handleStartElement(reinterpret_cast<const char*>(name));
}

static void handleEndElement(void *parser, const xmlChar* name, const xmlChar*, const xmlChar*) {
	static_cast<Parser*>(parser)->handleEndElement(reinterpret_cast<const char*>(name));
}

static void handleCharacterData(void* parser, const xmlChar* data, int len) {
	static_cast<Parser*>(parser)->handleCharacterData(std::string(reinterpret_cast<const char*>(data), static_cast<size_t>(len)));
}

static void handleError(void*, const char* /*m*/, ... ) {
	/*
	va_list args;
	va_start(args, m);
	vfprintf(stdout, m, args);
	va_end(args);
	*/
}

static void handleWarning(void*, const char*, ... ) {
}


boost::filesystem::path getLibraryPath() {
	std::vector<std::string> libraries {
		std::string(getenv("HOME")) + "/Music/iTunes/iTunes Library.xml",
		std::string(getenv("HOME")) + "/Music/iTunes/iTunes Music Library.xml"
	};
	auto library = std::find_if(libraries.begin(), libraries.end(), [&](std::string l) {
		return fs::exists(l);
	});
	if (library == libraries.end()) {
		BOOST_LOG_TRIVIAL(error) << "Library not found";
		exit(-1);
	}
	return *library;
}

Library parseLibrary(const boost::optional<std::string>& userDB) {
	auto db = userDB ? boost::filesystem::path(*userDB) : getLibraryPath();
	Parser parser;
	xmlInitParser();
	xmlSAXHandler handler;
	memset(&handler, 0, sizeof(handler));
	handler.initialized = XML_SAX2_MAGIC;
	handler.startElementNs = &handleStartElement;
	handler.endElementNs = &handleEndElement;
	handler.characters = &handleCharacterData;
	handler.warning = &handleWarning;
	handler.error = &handleError;

	BOOST_LOG_TRIVIAL(debug) << "Parsing " << db;
	if (xmlSAXUserParseFile(&handler, &parser, db.native().c_str()) != XML_ERR_OK) {
		std::cerr << "PARSE ERROR" << std::endl;
	}

	return Library(parser.getSongs(), parser.getMediaDir());
}
