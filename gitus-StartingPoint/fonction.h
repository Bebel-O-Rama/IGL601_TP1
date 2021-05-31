#include <iostream>
#include <string>
#include <cstring>

#include <boost/uuid/detail/sha1.hpp>



bool init();
bool add(std::string);
bool commit(std::string, std::string);
void checkout(std::string);