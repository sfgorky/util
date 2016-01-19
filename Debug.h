#ifndef _myDebug_h_
#define _myDebug_h_

#include <assert.h>
#include <stdio.h>
#include <typeinfo>

namespace Util {

void waitForAttach( );


#define DEBUG(format, ...) \
    { fprintf(stdout, "[%s:%d]", __FILE__, __LINE__); \
      fprintf(stdout, format, ## __VA_ARGS__); \
    }; 

// TBD: add a a parameter to account for the ith tiem to pass, before attaching debugger....
#define WAIT_FOR_DEBUGGER waitForAttach( );

}

#endif


