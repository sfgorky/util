////////////////////////////////////////////////////////////////////////////////
#ifndef _MLoop_h_
#define _MLoop_h_
////////////////////////////////////////////////////////////////////////////////

#include <ostream>
#include <iostream>
#include <string>
#include <vector>

/*
    Auxiliary class to perform multi-dimension sweeps.
    The class initializes and holds the internal state. It should be called 
    in an interactive loop.
    
    Example:
 
    {
        MLoop loop;
 
        // sets the initial and final point
        loop.addRange(0, 2);
        loop.addRange(0, 3);
 
        for(loop.begin(); !loop.end(); loop.next())
        {
            std::cout 
                << loop.loopIndex() << " "          // the current index of the loop
                << std::to_string(loop[0]) << " "   // value of 1st var
                << std::to_string(loop[1])          // value of 2nd var
                << std::endl;
        }
    }
 
    * Use function loop.str( ) to return a formated string.
        loop.str( )  will return : "[0, 4, 3]"
 
    * Alternative Init:
        MLoop loop( { MLoop::Range(0,2), MLoop::Range(0,3) } );
*/


class MLoop
{
////////////////////////////////////////////////////////////////////////////////
    public:
////////////////////////////////////////////////////////////////////////////////
    
    struct Range
    {
        Range( )
        { m_begin = 0; m_end = 0; m_step = 0; }
        
        Range(const int begin, const int end, const int step=1)
        { m_begin = begin; m_end = end; m_step = step; }
        
        Range(const Range& range)
        { m_begin = range.m_begin; m_end = range.m_end; m_step = range.m_step; }
        
        Range& operator=(const Range& range)
        { m_begin = range.m_begin; m_end = range.m_end; m_step = range.m_step; return *this;}
        
        int     m_begin;
        int     m_end;
        int     m_step;
    };
    ////////////////////////////////////////////////////////////////////////////
    
    MLoop( );
    MLoop(const int nbvar);
    MLoop(const std::vector<MLoop::Range>& rangeList);
   
    ~MLoop( );

    MLoop&  operator=(const MLoop& other);

    ////////////////////////////////////////////////////////////////////////////

    // clear the current data structure
    void clear( );
    
    void setRange(const size_t var, const int begin, const int end, const int step=1);
    
    // adds a new range to the current loop.
    // Returns the current variable index (added)
    size_t addRange(const int begin, const int end, const int step=1);
    
    // adds a new range to the current loop.
    // Returns the current variable index (added)
    size_t addRange(const MLoop::Range& range)
    { return addRange(range.m_begin, range.m_end, range.m_step); }
    
    // sets the step of a given variable.
    void setStep(const size_t var, const int step);
    
    // changes the sweep mode, to nested, or non nested
    void setNested(const bool nested);
    
    // returns the current sweep mode
    bool isNested( )const;
    
    // starts the internal state machine. Begin of the loop
    void begin( );
    
    // returns true if the loop got to its last state: end of loop
    bool end( )const;
    
    // moves to the next position
    void next( );
    
    // returns the value of the current index
    int operator[](const size_t idx)const;
    
    // returns the current loop index [0...size()-1]
    size_t loopIndex( )const;
    
    // total size (number of points)
    size_t size( )const;
    
    // a string representation of the current state
    std::string str(const char* left="[", const char* right="]", const char* sep=",")const;
    
    // current number of variables
    size_t nbVariable( )const;
    
    // the start point for the given variable var:[0,..., nbVariable()-1].
    // returns 0 if variable is out-of-range
    int getBegin(const size_t var)const;
    
    // the end point for the given variable var:[0,..., nbVariable()-1].
    // returns 0 if variable is out-of-range
    int getEnd(const size_t var)const;
    
    // the step point for the given variable var:[0,..., nbVariable()-1].
    // returns 0 if variable is out-of-range
    int getStep(const size_t var)const;

    // the current variable state vector on the loop
    const std::vector<int>& getState( )const;
    
    // Aux. function: loop internally and get all the points.
    void getAllPoints(std::vector< std::vector<int> >& data);

////////////////////////////////////////////////////////////////////////////////
    private:
////////////////////////////////////////////////////////////////////////////////

    void resize(const size_t nbvar);
    
    void nextNested( );
    void nextNonNested( );
    
    
    // total number of points in the sweep
    size_t computeSize( );
    
    ////////////////////////////////////////////////////////////////////////////
    
    std::vector<int>        m_index                                            ;
    std::vector<int>        m_end                                              ;
    std::vector<int>        m_begin                                            ;
    std::vector<int>        m_step                                             ;
    size_t                  m_loopIndex                                        ;
    size_t                  m_totalSize                                        ;
    bool                    m_finished                                         ;
    bool                    m_nested                                           ;

////////////////////////////////////////////////////////////////////////////////
};
////////////////////////////////////////////////////////////////////////////////


// Loop all points in the data array, and print it to stdout
void loopAllPoints(const std::vector< std::vector<double> >& data);


int testMLoop(const int argc, const char** argv);

////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
