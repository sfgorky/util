#ifndef _ObjRefCount_h_
#define _ObjRefCount_h_

#include <vector>
#include <string>

class ObjRefCount
{
////////////////////////////////////////////////////////////////////////////////
    public:
////////////////////////////////////////////////////////////////////////////////

                        ObjRefCount         ( )                                ;
                        ObjRefCount         (const ObjRefCount& other)         ;
                       ~ObjRefCount         ( )                                ;


    bool                operator==          (const ObjRefCount& other)   const ;
    bool                operator>           (const ObjRefCount& other)   const ;
    bool                operator<           (const ObjRefCount& other)   const ;
    
    ObjRefCount&        operator=           (const ObjRefCount& rhs)           ;


////////////////////////////////////////////////////////////////////////////////
    private:
////////////////////////////////////////////////////////////////////////////////

    // The actual data container.
    class Data
    {
    ////////////////////////////////////////////////////////////////////////////
        public:
    ////////////////////////////////////////////////////////////////////////////

                         Data       ( )                                        ;
                         Data       (const Data& d)                            ;
                        ~Data       ( )                                        ;
                        
        bool            operator==  (const Data& other)                  const ;
        bool            operator>   (const Data& other)                  const ;
        bool            operator<   (const Data& other)                  const ;
        
        unsigned        m_count                                                ;
        // Adds specific data here.

        friend class    ObjRefCount;
    };
    ////////////////////////////////////////////////////////////////////////////

    static Data*        defaultData                 ( )                        ;
    void                createNewDataIfNeeded       ( )                        ;
    Data*               m_data                                                 ;
};


#endif


