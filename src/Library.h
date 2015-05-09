#pragma once

#include <vector>
#include <boost/filesystem/path.hpp>

#include "Song.h"

class Library {
	public:
		Library(
				const std::vector<Song>& songs,
				const boost::filesystem::path& mediaDir);

		const boost::filesystem::path& getMediaDir() const {
			return mediaDir;
		}

		const std::vector<Song>& getSongs() const {
			return songs;
		}
	
	private:
		std::vector<Song> songs;
		boost::filesystem::path mediaDir;
};
