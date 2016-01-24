#ifndef _MemoryReport_h_
#define _MemoryReport_h_

#include <string>

namespace MemoryReport {

// initialize the memory counter. All allocated memory before this call
// will be ignored.
void init( );

// returns true if the package was initialized
bool isInit( );
    
// Stores the high water mark of used memory - instrumented sampling
size_t sample( );

// Gets the maximum memory among all samples
size_t getPeak( );

// gets the current memmory usage, minus the offset
size_t getUsage( );

// set a global offset 
void setOffset(size_t mem);

// returns a status strung
std::string  getStatus(const char* msg="");

// print status 
void printStatus(const char* msg="");

// formats a memory value, using Kb,Mb,Gb
std::string toString(long mem);

// test case for the memory report
void test( );


} // namespace MemoryReport


#endif


