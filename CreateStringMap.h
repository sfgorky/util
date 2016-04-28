#ifndef _CreateStringMap_h_
#define _CreateStringMap_h_

#include <string>
#include <vector>
#include <ostream>

// Creates a 'map' table, in a tree format
//
// @param s                   output stream.
// @param keyArray            array with the strings to be compared.
// @param valueArray          array of with the return values of the switch.
// @param keyName             name of the variable that is compared.
// @param sDefault            default option.
// @param sReturnType         return type to be added to the function declaration. 
//                            ie. const char*
// @param functionName name of the function to be created - If empty, only the
//                     switch code will be generated.
// @param caseSensitive       if true, the map table is case sensitive.
// @param bReturnAsString     if true, the return type (from the return_value_array)
//                             are written between quotes "".
// @return true if file can be created.
bool createStringMap(std::ostream&                    s,
                     const std::vector<std::string>&  keyArray,
                     const std::vector<std::string>&  valueArray,
                     const std::string                sDefault,
                     std::string                      sReturnType,
                     std::string                      sFunctionName,
                     bool                             caseSensitive,
                     bool                             bQuoteValue);

//  Creates a map structure based on swich statements. The key values will mapped
//  into integers, starting from 0.  
//  @param s output stream
//  @param keyArray Array with all the vector entries.
//  @param functionName name of the function to be created.
//  @param caseSensitive if true, the map table is case sensitive  	
bool createStringMap(std::ostream&                      s,
                     const std::vector<std::string>&  keyArray,
                     const std::string                sFunctionName,
                     bool                             caseSensitive);


// Creates a map from a file, being each element of the file an entry.
// @param dataFilename input ascii filename, with 1 value per line
// @param outputFileName file to be generate
// @param sFunctionName function name
// @param caseSensitive
// @return true/false for success....
bool createStringMap(const std::string dataFilename,
                     const std::string outputFileName,
                     const std::string sFunctionName,
                     const bool caseSensitive);


bool testCreateStringMap(const int argc, const char** argv);;


#endif

