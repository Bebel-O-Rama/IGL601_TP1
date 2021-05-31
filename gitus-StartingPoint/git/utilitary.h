#pragma once

#include <boost/filesystem.hpp>


std::string getSHA(std::string);
std::string getRandSHA();
std::string getCurrTime();

void writeFile(std::string filePath, std::string fileText, bool isAppend = false);
bool writeIndexFile(std::string fileText, bool isCommit = false);
std::string readFile(std::string filePath);

std::string getMergedTree(std::string currentIndex);

bool pathExists(std::string path);
bool fileExists(std::string filePath);
bool indexExists();

static const std::string GIT_PATH = boost::filesystem::current_path().append(".git/").string();