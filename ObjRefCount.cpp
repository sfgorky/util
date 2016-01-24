#include <iostream>
#include <assert.h>

#ifndef nullptr
#define nullptr 0
#endif


#include "ObjRefCount.h"

////////////////////////////////////////////////////////////////////////////////

ObjRefCount::Data::Data( )
    : m_count(1)
{
}

////////////////////////////////////////////////////////////////////////////////

ObjRefCount::Data::Data(const Data& d)
    : m_count(1)
{
}

////////////////////////////////////////////////////////////////////////////////

bool ObjRefCount::Data::operator==(const ObjRefCount::Data& other)const 
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool ObjRefCount::Data::operator<(const ObjRefCount::Data& other)const 
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool ObjRefCount::Data::operator>(const ObjRefCount::Data& other)const 
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////

ObjRefCount::ObjRefCount( )
    :   m_data(defaultData())
{
}

////////////////////////////////////////////////////////////////////////////////

ObjRefCount::ObjRefCount(const ObjRefCount& other)
    :   m_data(other.m_data)
{
    ++m_data->m_count;
}

////////////////////////////////////////////////////////////////////////////////

ObjRefCount::~ObjRefCount( )
{
    if (--m_data->m_count == 0) delete m_data;
}

////////////////////////////////////////////////////////////////////////////////

ObjRefCount::Data* ObjRefCount::defaultData( )
{
    static ObjRefCount::Data* p = nullptr;
    if (p == nullptr)
    {
        p = new Data( );
        ++p->m_count;    // Make sure it never goes to zero
    }
    return p;
}

////////////////////////////////////////////////////////////////////////////////

void ObjRefCount::createNewDataIfNeeded( )
{
    // This method might need to change things in *m_data
    // Thus it first checks if this is the only pointer to *m_data
    if (m_data->m_count > 1)
    {
        Data* d = new Data(*m_data);    // Invoke Fred::Data's copy ctor
        --m_data->m_count;
        m_data = d;
    }
    assert(m_data->m_count == 1);
}

////////////////////////////////////////////////////////////////////////////////

ObjRefCount& ObjRefCount::operator=(const ObjRefCount& f)
{
    // DO NOT CHANGE THE ORDER OF THESE STATEMENTS!
    // (This order properly handles self-assignment)
    // (This order also properly handles recursion, e.g., if a
    // Fred::Data contains Freds)

    Data* const old(m_data);
    m_data = f.m_data;
    ++m_data->m_count;
    if (--old->m_count == 0)
    {
        delete old;
    }
    return *this;
}

////////////////////////////////////////////////////////////////////////////////

bool ObjRefCount::operator==(const ObjRefCount& other)const
{
    return m_data&&other.m_data ? m_data->operator==(*other.m_data) : false;
}

////////////////////////////////////////////////////////////////////////////////

bool ObjRefCount::operator>(const ObjRefCount& other)const
{
    return m_data&&other.m_data ? m_data->operator>(*other.m_data) : false;
}

////////////////////////////////////////////////////////////////////////////////

bool ObjRefCount::operator<(const ObjRefCount& other)const
{
    return m_data&&other.m_data ? m_data->operator<(*other.m_data) : false;
}

////////////////////////////////////////////////////////////////////////////////






