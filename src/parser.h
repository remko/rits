#pragma once

#include <vector>
#include <boost/filesystem/path.hpp>

#include "Library.h"

boost::filesystem::path getLibraryPath();
Library parseLibrary();
