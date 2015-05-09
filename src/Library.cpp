#include "Library.h"

Library::Library(const std::vector<Song>& songs, const boost::filesystem::path& mediaDir) : songs(songs), mediaDir(mediaDir) {
}
