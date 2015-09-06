#pragma once

#include <vector>
#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>

#include "Library.h"

boost::filesystem::path getLibraryPath();
Library parseLibrary(const boost::optional<std::string>& db = boost::optional<std::string>());
