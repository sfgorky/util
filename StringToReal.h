#ifndef _StringToReal_h_
#define _StringToReal_h_

#include <string>

#ifndef nullptr
#define nullptr 0
#endif

double stringToReal(
                const std::string& s, 
                bool* bOk = nullptr);

std::string  realToString(
                const double value,
                const double n1,
                const double n2,
                const double bAddPlus);

// returns the string, in eletrical format.
// @pvalue the value to be converted
// @nbDec  number of decimal points, after the '.'
// @trim   if true, the returned string will be trimmed.
std::string realToStringElectric(
                const double value,
                const int    nbDec,
                const bool   bTrimString);

std::string realToString(
				const double value,
				const double n1,
				const double n2,
				const double bAddPlus);

int testStringToReal(const int argv, const char** argc);
    
#endif
