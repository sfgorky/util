#include "MLoop.h"

//const bool bDebugThis = true;
const bool bDebugThis = false;

MLoop::MLoop( )
{
    m_nested      = true;
    m_finished    = false;
    m_loopIndex  = 0;
    m_totalSize  = 0;
}

////////////////////////////////////////////////////////////////////////////////

MLoop::MLoop(const std::vector< MLoop::Range >& rangeList)
{
    m_nested      = true;
    m_finished    = false;
    m_loopIndex  = 0;
    m_totalSize  = 0;
    for(size_t i=0; i<rangeList.size(); i++)
    {
        addRange(rangeList[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////

MLoop::MLoop(const int nbvar)
{
    m_nested     = true;
    m_finished   = false;
    m_loopIndex = 0;
    m_totalSize = 0;
    
    m_index.resize(nbvar);
    m_begin.resize(nbvar);
    m_end.resize(nbvar);
    m_step.resize(nbvar);
    
}

////////////////////////////////////////////////////////////////////////////////

MLoop::~MLoop( )
{
}

////////////////////////////////////////////////////////////////////////////////

MLoop& MLoop::operator=(const MLoop& other)
{
    m_index       = other.m_index;
    m_begin       = other.m_begin;
    m_end         = other.m_end;
    m_step        = other.m_step;
    m_nested      = other.m_nested;
    m_finished    = other.m_finished;
    m_loopIndex  = other.m_loopIndex;
    m_totalSize  = other.m_totalSize;
    
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
// clear the current data structure

void MLoop::clear( )
{
    m_index.resize(0);
    m_begin.resize(0);
    m_end.resize(0);
    m_step.resize(0);
    
    m_nested     = true;
    m_finished   = false;
    m_loopIndex = 0;
    m_totalSize = 0;
}

////////////////////////////////////////////////////////////////////////////////

void MLoop::setRange(const size_t var, const int begin, const int end, const int step)
{
    if(bDebugThis)
    {
        fprintf(stdout, "MLoop::setRange(var=%ld, range[%d, %d], step=%d\n",
                var, begin, end, step);
    }
    if(var<nbVariable())
    {
        m_begin[var]  = begin;
        m_end[var]    = end;
        m_step[var]   = step;
    }
}

////////////////////////////////////////////////////////////////////////////////

size_t MLoop::addRange(const int begin, const int end, const int step)
{
    if(bDebugThis)
    {
        fprintf(stdout, "MLoop::addRange(range[%d, %d], step=%d)\n", begin, end, step);
    }
    m_begin.push_back(begin);
    m_end.push_back(end);
    m_step.push_back(step);
    m_index.push_back((int)m_begin.size()-1);
    
    // update the size - since we are in interactive mode
    m_totalSize = computeSize();
    
    return m_begin.size();
}

////////////////////////////////////////////////////////////////////////////////
// sets the step of a given variable.

void MLoop::setStep(const size_t var, const int step)
{
    if(bDebugThis)
    {
        fprintf(stdout, "MLoop::set_step(var=%ld,step=%d)\n", var, step);
    }
    if(var<nbVariable())
    {
        m_step[var]  = step;
    }
    m_totalSize = computeSize();
}

////////////////////////////////////////////////////////////////////////////////
// changes the sweep mode, to nested, or non nested

void MLoop::setNested(const bool nested)
{
    m_nested = nested;
}

////////////////////////////////////////////////////////////////////////////////
// returns the current sweep mode

bool MLoop::isNested( )const
{
    return m_nested;
}

////////////////////////////////////////////////////////////////////////////////
// starts the internal state machine. Begin of the loop

void MLoop::begin( )
{
    m_totalSize = computeSize();
    m_loopIndex = 0;
    m_finished = true;
    if(nbVariable() > 0)
    {
        m_finished = false;
        m_index[0] = 0;
        for(size_t i=0; i<nbVariable(); i++)
        {
            m_index[i] =  m_begin[i];
            if(m_index[i] > m_end[i])
            {
                m_finished = true;
            }
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
// returns true if the loop got to its last state: end of loop

bool MLoop::end( )const
{
    return m_finished;
}

////////////////////////////////////////////////////////////////////////////////
// moves to the next position

void MLoop::next( )
{
    if(bDebugThis)
    {
        fprintf(stdout, "MLoop::next( )\n");
    }
    const size_t nbvar = nbVariable();
    if(nbvar == 0)
    {
        m_finished = true;
    }
    if(!m_finished)
    {
        if(isNested( ))
        {
            nextNested();
        }
        else
        {
            nextNonNested();
        }
        m_loopIndex++;
    }
    if(m_loopIndex>m_totalSize)
    {
        fprintf(stdout, "Error: loop exceeded total number of points\n");
        m_finished=true;
    }
}

////////////////////////////////////////////////////////////////////////////////
// returns the value of the current index

int MLoop::operator[](const size_t idx)const
{
    return (idx<nbVariable()) ? m_index[idx] : 0;
}

////////////////////////////////////////////////////////////////////////////////
// returns the current loop index [0...size()-1]

size_t MLoop::loopIndex( )const
{
    return m_loopIndex;
}

////////////////////////////////////////////////////////////////////////////////
// total size (number of points)

size_t MLoop::size( )const
{
    return m_totalSize;
}

////////////////////////////////////////////////////////////////////////////////
// a string representation of the current state

std::string MLoop::str(const char* left, const char* right, const char* sep)const
{
    char sBuffer[32];
    size_t nbvar = nbVariable();
    std::string s;
    if(left) s += left;
    if(nbvar == 1)
    {
        snprintf(sBuffer, sizeof(sBuffer), "%d", m_index[0]);
        s +=  std::string(sBuffer);
    }
    else if(nbvar>1)
    {
        snprintf(sBuffer, sizeof(sBuffer), "%d", m_index[0]);
        s += std::string(sBuffer);
        for(size_t i=1; i<nbvar; i++)
        {
            snprintf(sBuffer, sizeof(sBuffer), "%d", m_index[i]);
            s += std::string(sep) + std::string(sBuffer);
        }
    }
    if(right) s += right;
    return s;
}

////////////////////////////////////////////////////////////////////////////////
// current number of variables

size_t MLoop::nbVariable( )const
{
    return m_begin.size();
}

////////////////////////////////////////////////////////////////////////////////
// the start point for the given variable var:[0,..., nbVariable()-1].
// returns 0 if variable is out-of-range

int MLoop::getBegin(const size_t var)const
{
    return (var<nbVariable()) ? m_begin[var] : 0;
}

////////////////////////////////////////////////////////////////////////////////
// the end point for the given variable var:[0,..., nbVariable()-1].
// returns 0 if variable is out-of-range
int MLoop::getEnd(const size_t var)const
{
    return (var<nbVariable()) ? m_end[var] : 0;
}

////////////////////////////////////////////////////////////////////////////////
// the step point for the given variable var:[0,..., nbVariable()-1].
// returns 0 if variable is out-of-range

int MLoop::getStep(const size_t var)const
{
    return (var<nbVariable()) ? m_step[var] : 0;
}

////////////////////////////////////////////////////////////////////////////////
// the current variable state vector on the loop

const std::vector<int>& MLoop::getState( )const
{
    return m_index;
}

////////////////////////////////////////////////////////////////////////////////
// Aux. function: loop internally and get all the points.

void MLoop::getAllPoints(std::vector< std::vector<int> >& data)
{
    data.resize(0);
    for(begin(); !end(); next())
    {
        data.push_back(getState());
    }
}

////////////////////////////////////////////////////////////////////////////////

void MLoop::resize(const size_t nbvar)
{
    m_index.resize(nbvar);
    m_begin.resize(nbvar);
    m_end.resize(nbvar);
    m_step.resize(nbvar);
    
    for(size_t i=0; i<nbvar; i++)
    {
        m_begin[i] = 1;
        m_end[i]   = 1;
        m_step[i]  = 1;
    }
}

////////////////////////////////////////////////////////////////////////////////

void MLoop::nextNested( )
{
    // the indeces are backwards
    int last = (int)m_index.size()-1;
    m_index[last] += m_step[last];
    for(size_t i=last; i!=0; i--)
    {
        if(m_index[i] > m_end[i])
        {
            if(i==0)
            {
                m_finished = true;
                break;
            }
            else
            {
                m_index[i]    = m_begin[i];
                m_index[i-1] += m_step[i-1];
            }
        }
    }
    if(m_index[0] > m_end[0])
    {
        if(bDebugThis) fprintf(stdout, "finished\n");
        m_finished = true;
    }
}

////////////////////////////////////////////////////////////////////////////////

void MLoop::nextNonNested( )
{
    //TBD
}

////////////////////////////////////////////////////////////////////////////////
// total number of points in the sweep
size_t MLoop::computeSize( )
{
    size_t nbvar = nbVariable();
    size_t iTotalSize = 0;
    
    if(nbvar>0)
    {
        double dSize = 1.0;
        if(isNested( ))
        {
            for(size_t i=0; i<nbvar; i++)
            {
                if(m_step[i] != 0)
                {
                    dSize *= ((m_end[i]-m_begin[i]+1)/m_step[i]);
                }
            }
        }
        else
        {
            const int i=0;
            if(m_step[i] != 0)
            {
                dSize *= ((m_end[0]-m_begin[0]+1)/m_step[0]);
            }
            else
            {
                dSize = 0;
            }
        }
        iTotalSize = (size_t)dSize;
    }
    return iTotalSize;
}

////////////////////////////////////////////////////////////////////////////////
// Loop all points in the data array, and print it to stdout

void loopAllPoints(const std::vector< std::vector<double> >& data)
{
    MLoop loop;

    int nbVar = (int)data.size();
    for(int i=0; i<nbVar; i++)
    {
        loop.addRange(0, (int)data[i].size()-1);
    }

    std::cout << "size=" << loop.size() << std::endl;
    for(loop.begin(); !loop.end(); loop.next())
    {
        std::cout << loop.loopIndex() << ": ";
        
        for(int i=0; i<nbVar; i++)
        {
            double v  = data[i][ loop[i] ];
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////


template<typename T> 
std::vector<T> createVector(int iNb, ...)
{
    std::vector<T> v(iNb);
 
    va_list list;
    va_start(list, iNb);
 
    for (int nArg=0; nArg <iNb; nArg++)
    {
        v[nArg] = va_arg(list, T);
    }
    va_end(list);
     return v;
}

////////////////////////////////////////////////////////////////////////////////

int testMLoop(int testRank)
{
    switch(testRank)
    {
        case 0:
        {
            MLoop loop;
            loop.addRange(0, 2);
            loop.addRange(0, 3);
            for(loop.begin(); !loop.end(); loop.next())
            {
                std::cout << loop.loopIndex() << " " << loop.str( ) << "\n";
            }
            break;
        }
        case 1:
        {
            std::vector<MLoop::Range> rangeList;
            rangeList.push_back(MLoop::Range(0,2));
            rangeList.push_back(MLoop::Range(0,3));
            MLoop loop(rangeList);
            for(loop.begin(); !loop.end(); loop.next())
            {
                std::cout << loop.loopIndex() << " " << loop.str( ) << "\n";
            }
            break;
        }
        case 2:
        {
            MLoop loop;
            
            std::vector<double> v1 = createVector<double>(3,  1.0, 2.0, 3.0);
            std::vector<double> v2 = createVector<double>(2,  1.0, 2.0);
            std::vector<double> v3 = createVector<double>(3, 100.0, 200.0, 300.0 );
            std::vector< std::vector<double> > data;
            data.push_back( v1 );
            data.push_back( v2 );
            data.push_back( v3 );
            
            loopAllPoints(data);
            break;
        }
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////

int testMLoop(const int /* argc */, const char** /* argv */)
{
    testMLoop(1);
    testMLoop(2);
    testMLoop(3);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////

#ifdef _MLOOP_MAIN_

int main( )
{
    testMLoop(1);
    testMLoop(2);
    testMLoop(3);
}

#endif

