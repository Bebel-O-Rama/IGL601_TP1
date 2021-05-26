#pragma once

#include <boost/filesystem.hpp>

void TestDeTrucs() throw (boost::filesystem::filesystem_error);
std::string getSHA(std::string);
std::string getRandSHA();

