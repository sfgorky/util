#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "Scape.h"

namespace Scape
{

///////////////////////////////////////////////////////////////////////////////

static const char SCAPE_CHAR     = '\\';
static const char* SPECIAL_TOKEN = "{}Ω[].$#%@!()_-,;:<>=+-*/|&^";

///////////////////////////////////////////////////////////////////////////////

bool hasScape(const std::string& src)
{
    return (strrchr(src.c_str(), SCAPE_CHAR) == nullptr
                ? false
                : true);
}

///////////////////////////////////////////////////////////////////////////////

#define VALUE(X) *X
#define NEXT(X) X++

const char* getUnscape(std::string& dest, const std::string& src)
{
    const size_t count = src.size( );
    if(src.empty())
    {
        dest = "";
    }
    else
    {
        char* buffer  = (char*)calloc(count+1, sizeof(char));
        char* in      = const_cast<char*>(src.c_str());        
        char* out     = buffer;
        bool  inScape = false;

fprintf(stdout, ":::%s\n", in);
        while(VALUE(in) != 0)
        {
            if(!inScape && VALUE(in) == SCAPE_CHAR && VALUE(in+1) != 0)
            {
                inScape = true;
            }
            else 
            {
                inScape = false;
                VALUE(out) = VALUE(in);
                NEXT(out);
            }
            fprintf(stdout, ">>%s 'in:%c'\n", buffer, *in);

            NEXT(in);
        }
        VALUE(out) = 0;
        
        dest = buffer;
        free(buffer);
    }
    return dest.c_str();
}

////////////////////////////////////////////////////////////////////////////////

const char* getScape(std::string& dest, const std::string& src)
{
    if(src.empty())
    {
        dest = "";
    }
    else
    {
        char* in = nullptr;

        // if it has a scape character, unscape it first.
        if(hasScape(src))
        {
            getUnscape(dest, src);
            in = (char*)dest.c_str();
        }
        else
        {
            in = (char*)src.c_str();
        }

        char* buffer = new char[2*src.size()+1];
        char* out   = buffer;

        // test the first char....
        if(!(isalnum(*out) || strrchr(SPECIAL_TOKEN, *in)))
        {
            *out = SCAPE_CHAR;
            out++;
            *out = *in;
            out++;
            in++;
        }
        while(*out != 0)
        {
            // scape condition
            if(!(isalnum(*out) || strrchr(SPECIAL_TOKEN, *in)))
            {
                *out = SCAPE_CHAR;
                out++;
            }
            *out = *in;
            out++;
            in++;    
        };
        *out = 0;

        dest = buffer;
        delete[] buffer;
    }
    return dest.c_str();
}

///////////////////////////////////////////////////////////////////////////////

void test( )
{
    const char* things[] = {
        "\\\\a\\\\b\\\\c"
    };
    /*
        "1\\23\\45",
        " ",
        "\\\\a",
        "",
        "\\",
        "1",
        "hell",
        "I don't know",
        "what I am doing ...",
        "\\12345",
        "1234\\5",
        "1234\\" ,
        "#a.b.c",
        "\\a\b\\c"
    };
    */
    const int nb = sizeof things/sizeof(things[0]);


    fprintf(stdout, "| start                   | unscaped                | canonical \n");
    for(int i=0; i<nb; i++)
    {
        std::string s = things[i];
        std::string unscape;
        std::string scape;        
        
        fprintf(stdout, "[%s]", s.c_str());
        fprintf(stdout, "[%s]", Scape::getUnscape(unscape, s));
        fprintf(stdout, "[%s]\n",  Scape::getScape(scape, unscape));
    }
}


///////////////////////////////////////////////////////////////////////////////
} // namescape 


int main( )
{
    Scape::test();
    return 0;
}

