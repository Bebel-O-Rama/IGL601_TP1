#include <iostream>
#include <string>
#include <cstring>

#include <boost/uuid/detail/sha1.hpp>


void init();
void add(std::string);
void commit(std::string, std::string);
void checkout(std::string);