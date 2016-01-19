#include <sys/types.h>
#include <iostream>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // win32

#include "Debug.h"

namespace Util {

void _sleep(int msec)
{
#ifdef WIN32
    Sleep(msec);
#else
    usleep(msec*1000);
#endif
}

///////////////////////////////////////////////////////////////////////////////

void waitForAttach( )
{
    fprintf(stdout, "*****************************************************************************");
    fprintf(stdout, "waiting to attache debugger [pid: %d]\n", getpid());
    bool bLoopMe = true;
    while(bLoopMe)
    {
        // sleep 1 sec, and try again....
        _sleep(1);
    }
    fprintf(stdout, "*****************************************************************************");

}


} // namespace Util
///////////////////////////////////////////////////////////////////////////////







