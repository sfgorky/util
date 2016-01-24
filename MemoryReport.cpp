#include <vector>
#include <string>
#include <istream>

#ifdef  _MSC_VER
#pragma warning(disable : 4005)
#include <windows.h>
#include <psapi.h>
#endif

#ifdef __APPLE_CC__
#include <mach/vm_statistics.h>
#include <mach/mach_types.h>
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#include <mach/mach.h>
#endif


#include "MemoryReport.h"


///////////////////////////////////////////////////////////////////////////////

// aux. helper class
class MemoryStateHelper
{
///////////////////////////////////////////////////////////////////////////////
    public:
///////////////////////////////////////////////////////////////////////////////

    MemoryStateHelper( )
    { initialize(); }
   
    virtual ~MemoryStateHelper( )
    { };

    void initialize( )
    { m_init = false; m_peak = 0; m_offset = 0; }
   
    bool isInit( )
    { return m_init; }
    
    void setOffset(size_t offset)
    { m_offset = offset; }
    
    void setPeak( )
    { m_offset = sample_current( ); m_init = true; }

    size_t getPeak( )const
    { return m_peak; }
    
    // return the current mem usage. (minus offset)
    size_t current( )
    { return sample_current( ); }
   
    size_t sample( )
    {
        size_t mem = sample_current( );
        if(mem > m_peak)
        {
            m_peak = mem;
        }
        return (size_t)mem;
    }

///////////////////////////////////////////////////////////////////////////////
    private:    
///////////////////////////////////////////////////////////////////////////////

    size_t                  sample_current( )                                ;
    bool                    m_init										     ;
    size_t					m_peak   										 ;
    size_t					m_offset										 ;
};


///////////////////////////////////////////////////////////////////////////////


size_t MemoryStateHelper::sample_current( )
{
    size_t mem_usage = 0;
#ifdef _MSC_VER
    PROCESS_MEMORY_COUNTERS process_memory_counter;
    
    HANDLE process_id = GetCurrentProcess();
    int iRet = GetProcessMemoryInfo(process_id, &process_memory_counter, sizeof(process_memory_counter));
    mem_usage = process_memory_counter.PeakWorkingSetSize;
    
    mem_usage = process_memory_counter.WorkingSetSize;
#elif __APPLE_CC__
   struct mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;

    if ( task_info( mach_task_self( ), MACH_TASK_BASIC_INFO,  (task_info_t)&info, &infoCount ) == KERN_SUCCESS )
    {
        mem_usage =  (size_t)info.resident_size;
    }
#else
    FILE *fp = fopen("/proc/self/stat", "r");
    if(fp)
    {
        char entries[64];
        // ignore the first 23 entries...
        for (int i = 0; i<22; i++)
        {
            fscanf(fp, "%s", entries);
        }
        fscanf(fp, "%s", entries);
        mem_usage = strtoul(entries, NULL, 10);
        fclose(fp);
    }
#endif
    return (mem_usage - m_offset);
}


///////////////////////////////////////////////////////////////////////////////


static MemoryStateHelper* _memoryState = 0;


namespace MemoryReport {

    
///////////////////////////////////////////////////////////////////////////////

void init( )
{
    if(!_memoryState) _memoryState = new MemoryStateHelper( );
    _memoryState->setPeak( );
}

///////////////////////////////////////////////////////////////////////////////

bool isInit( )
{
    return (_memoryState && _memoryState->isInit( ) ? true : false);
}

///////////////////////////////////////////////////////////////////////////////
// Stores the high water mark of used memory - instrumented sampling
//
size_t sample( )
{
    return _memoryState ? _memoryState->sample( ) : 0;
    
}

///////////////////////////////////////////////////////////////////////////////
// Gets the maximum memory among all samples
//
size_t getPeak( )
{
    return _memoryState ? _memoryState->getPeak() : 0;
}

///////////////////////////////////////////////////////////////////////////////
// Gets the maximum memory among all samples
//
size_t getUsage( )
{
    return _memoryState ? _memoryState->current( ) : 0;
}

///////////////////////////////////////////////////////////////////////////////
//
void setOffset(size_t mem)
{
    if(_memoryState) _memoryState->setOffset(mem);
}

///////////////////////////////////////////////////////////////////////////////
//
std::string toString(long val)
{
    static long _1K = 1024;
    static long _1M = _1K*_1K;
    static long _1G = _1M*_1K;

    char sBuffer[64];
    if(val < _1K)
    {
       snprintf(sBuffer, sizeof(sBuffer), "%d", (int)(val));
    }
	else if(val < _1M)
    {
       snprintf(sBuffer, sizeof(sBuffer), "%dK", (int)((double)val/(double)_1K));
    }
    else if(val <_1G)
    {
       snprintf(sBuffer, sizeof(sBuffer), "%4dM", (int)((double)val/(double)_1M));
    }
    else
    {
       snprintf(sBuffer, sizeof(sBuffer), "%4dG", (int)((double)val/(double)_1G));
	}
    return std::string(sBuffer);
}

///////////////////////////////////////////////////////////////////////////////
//
std::string getStatus(const char* msg)
{
    std::string s;
    MemoryReport::sample( );

    if(msg && msg[0] != 0)
    {
        s = msg + std::string(":");
    }
    char sBuffer[1024];
    snprintf(sBuffer, sizeof(sBuffer), "(cur:%sb, peak:%sb)", 
            toString(getUsage()).c_str(), 
            toString(getPeak()).c_str());

    s += std::string(sBuffer);
    return s;
}

///////////////////////////////////////////////////////////////////////////////

void printStatus(const char* msg)
{
    fprintf(stdout, "%s\n", getStatus(msg).c_str());
}

///////////////////////////////////////////////////////////////////////////////

void test( )
{
	size_t _1M = 1024*1024;

    // to set the internal buffer....
	fprintf(stdout, "Test of Memory report\n");

    // initialize the package
    MemoryReport::init( );

    MemoryReport::printStatus("init");
    MemoryReport::sample( );

    char* p = new char[150*_1M]; 
    MemoryReport::printStatus("allocated 150M");

    delete[] p;
    MemoryReport::printStatus("deleted");

    p = new char[50*_1M];
    MemoryReport::printStatus("allocated 50M");

    delete[] p;
    MemoryReport::printStatus("deleted");

    p = new char[200*_1M];
    MemoryReport::printStatus("allocated 200M");

    delete[] p;
    MemoryReport::printStatus("deleted");
}


} // namespace MemoryReport
///////////////////////////////////////////////////////////////////////////////

#ifdef _MYMEMORY_MAIN_

int main( )
{
    MyMemory::test( );
}

#endif





