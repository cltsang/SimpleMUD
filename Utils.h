#ifndef UTILS
#define UTILS

//from http://www.codeguru.com/forum/showthread.php?t=231054

#include <string>
#include <sstream>
#include <iostream>
#include <vector>

namespace Utils
{

template <class T>
bool stringToNumber(T& t, 
                 const std::string& s, 
                 std::ios_base& (*f)(std::ios_base&))
{
  std::istringstream iss(s);
  return !(iss >> f >> t).fail();
}

}

#endif