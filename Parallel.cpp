#include <vector>

//http://antonym.org/2009/05/threading-with-boost---part-i-creating-threads.html

struct ClassReadData
{
////////////////////////////////////////////////////////////////////////////////
    public:
////////////////////////////////////////////////////////////////////////////////

                    ClassReadData     ( )                                      ;

    // set main data, read only

    void            setVector         (const std::vector<int>& v)              ;

    // access data (read only)
    
    int             point             (const size_t at)                   const
    { return (*m_vector)[at]; }

////////////////////////////////////////////////////////////////////////////////
    private:
////////////////////////////////////////////////////////////////////////////////

    // read only
    const std::vector<int>*       m_vector                                    ;

};

ClassReadData::ClassReadData( )
    :   m_vector(nullptr)
{
}

void ClassReadData::setVector(const std::vector<int>& v)
{
    m_vector = &v;
}


template<class T>
class Range
{
////////////////////////////////////////////////////////////////////////////////
    public:
////////////////////////////////////////////////////////////////////////////////
    
    Range(const T beg, const T end) 
        :   m_begin(beg)
        ,   m_end(end)
    { }
      
    T begin( )const
    { return m_begin; }
    
    T end( )const
    { return m_end; }
    
    
    const T m_begin;
    const T m_end;
};


class Class
{
////////////////////////////////////////////////////////////////////////////////
    public:
////////////////////////////////////////////////////////////////////////////////
    
                    Class               ( )                                    ;
                    Class               (ClassReadData* readData)              ;
                    Class               (const Class& other)                   ;
                    
    void            operator()          (const Range<size_t>& range)           ;
    void            join                (const Class& other)                   ;
    

    ////////////////////////////////////////////////////////////////////////////
    
    double          getSum              ( )                               const 
    { return m_sum; }
    
    // get data
    double          getDataSum          ( )                               const
    { return m_sum; }      
    
    size_t          getNbPts            ( )                               const
    { return m_nbPts; }

////////////////////////////////////////////////////////////////////////////////
    private:
////////////////////////////////////////////////////////////////////////////////

    bool            method              (const size_t from, const size_t to)   ;


    ClassReadData*             m_readData                                      ;
    
    // actual data
    double                      m_sum                                          ;
    size_t                      m_nbPts                                        ;    
};

////////////////////////////////////////////////////////////////////////////////

Class::Class( )
    :   m_readData(nullptr)
    ,   m_sum(0.0)
    ,   m_nbPts(0)
{

}

////////////////////////////////////////////////////////////////////////////////

Class::Class(const Class& other)
    :   m_readData(other.m_readData)
    ,   m_sum(0.0)
    ,   m_nbPts(0)    
{

}



////////////////////////////////////////////////////////////////////////////////

Class::Class(ClassReadData* data)
    :   m_readData(data)
    ,   m_sum(0.0)
    ,   m_nbPts(0)    
{

}

////////////////////////////////////////////////////////////////////////////////

bool Class::method(const size_t from, const size_t to)
{
    bool bOk(true);
    
    m_sum = 0.0;
    m_nbPts = (to-from)+1;
    
    for(size_t i=from; i<to; i++)
    {
        m_sum += m_readData->point(i);
    }
    return bOk;
}

////////////////////////////////////////////////////////////////////////////////

void Class::operator()(const Range<size_t>& range)
{
    const size_t begin = range.begin();
    const size_t end   = range.end();
    method(begin, end);
}

////////////////////////////////////////////////////////////////////////////////

void Class::join(const Class& other)
{
    // copy the data from other into this class.
    m_nbPts  += other.getNbPts( );
    m_sum    += other.getDataSum( );
}


////////////////////////////////////////////////////////////////////////////////

template<class T> 
void parallelReduce(Range<size_t>&  range, 
                    T&              mainClass, 
                    const size_t    nbthread)
{
    std::vector<T*> threads;
    for(size_t i=0; i<nbthread; i++)
    {
        // copies the read-only data from the 'main class'
        threads.push_back(new T(mainClass));
    }

    const size_t begin = range.begin();
    const size_t end   = range.end();
    
    const size_t nb    = end-begin+1;
    const size_t step  = (int)((double)nb/(double)nbthread);
    
    
    // Executes the individual threads
    for(size_t i=0; i<nbthread; i++)
    {
        const size_t this_begin = begin+i*step;
              size_t this_end   = this_begin+step;
        
        if(this_end>end) this_end = end;
    
        fprintf(stdout, "thread[%ld] %ld, %ld\n", i, this_begin, this_end);
        // run this thread - in parallel
        threads[i]->operator()(Range<size_t>(this_begin, this_end));
    }
        
    // wait for all finished to start joining....
        
    // after all threads are finished - reduce
    // data ownership may exist on this phase, since the main threads 
    // will be removed
    for(size_t i=0; i<nbthread; i++)
    {
        mainClass.join(*threads[i]);
    }
    
    // delete the thread classes
    for(size_t i=0; i<nbthread; i++)
    {
        delete threads[i];
    }
}

////////////////////////////////////////////////////////////////////////////////

int main( )
{
    std::vector<int>    v;
    
    size_t nb = 100;
    for(size_t i=1; i<=nb; i++)
    {
        v.push_back(i);
    }

    ClassReadData data;
    data.setVector(v);
    
    Class mainClass(&data);
    size_t nbThread = 4;
    
    Range<size_t> range(0, v.size());
    
    parallelReduce<Class>(range, mainClass, nbThread); 
    
    
    fprintf(stdout, "mainClass sum: %g\n", mainClass.getSum());
    
    return 0;
}

    


