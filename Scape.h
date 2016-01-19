#include <stdlib.h>
#include <stdio.h>
#include <string>
  
namespace Scape
{
/// \brief Unscape a string
/// \param dest[out] Destination string 
/// \param src[in] original string.
const char* getUnscape(std::string& dest, const std::string& src);


/// \brief Scape a string
/// \param dest[out] Destination string 
/// \param src[in] original string.
const char* getScape(std::string& dest, const std::string& src);


} // namescape Scape

