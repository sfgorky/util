#include <string>
#include <stdlib.h>
#include <string.h>
#include <cmath>

#include "StringToReal.h"


static int _myIsdigit(int c)
{
    return (c>0 ? std::isdigit(c) : false);
}


#define IS_IN(X, A, B) \
        (X >= A && X<=B)

static const char _InfString[]       = "inf";
static const char  _MinusInfString[] = "-inf";
static const char  _NaNString[]      = "NaN";

static const double  posDummyValue = 17.0;
static const double  negDummyValue = -56.49595438;
static const double  zeroValue  = 0.0;
static const double  _INFINITY   = posDummyValue / zeroValue;
static const double  _MINFINITY  = negDummyValue / zeroValue;

/////////////////////////////////////////////////////////////////////////////////

static bool isInfinity(const double aValue)
{
    return(aValue == _INFINITY);
}

static bool isMinusInfinity(const double aValue)
{
    return (aValue == _MINFINITY);
}

/* Not used.
static double getInf( )
{
    return  _INFINITY;
}

static double getMinusInf( )
{
    return  _MINFINITY;
}
*/
////////////////////////////////////////////////////////////////////////////////

#define SCAPE_CHAR '\\'

double stringToReal(const char* str, bool* bOk)
{
    double retValue = 0.0;
    
    if(str == nullptr || str[0] == SCAPE_CHAR)
    {
        if(bOk) *bOk = false;
        return retValue;
    }
    if(str[0] == 0)
    {
        if(bOk) *bOk = true;
        return retValue;
    }
    
    double          scale   = 1.0;
    char            buf[255];
    
    char*           buf_ptr = buf;
    char*           str_ptr = (char*)str;

    buf[0] = 0;
    while((*str_ptr == ' ' || *str_ptr == '\t') && *str_ptr != 0 )
    {
        str_ptr++;
    }
    while( _myIsdigit(*str_ptr)  ||
            *str_ptr == '+'      ||
            *str_ptr == '-'      ||
            *str_ptr == '.'      ||
            *str_ptr =='E'       ||
            *str_ptr == 'e')
    {
        *buf_ptr = *str_ptr;
        str_ptr++;
        buf_ptr++;
    }
    *buf_ptr = 0;
    //    val = atof(buf);
    
    if(buf[0] == 0)
    {
        if(bOk) *bOk = false;
        return retValue;
    }

    char*   endptr;
    double val = strtod(buf, &endptr);
    if(*endptr != 0)
    {
        retValue = val;
        return false;
    }
    switch(*str_ptr)
    {
        case 'T':
        case 't':
            scale = 1.E12;
            str_ptr++;
            break;
        case 'G':
        case 'g':
            scale = 1.E09;
            str_ptr++;
            break;
        case 'M':
            scale = 1.E06;
            str_ptr++;
            break;
        case 'K':
        case 'k':
            scale = 1.E03;
            str_ptr++;
            break;
        case 'm':
            scale = 1.E-03;
            str_ptr++;
            if(*str_ptr != 0 && *str_ptr == 'i')
            {
                str_ptr++;
                if(*str_ptr != 0 && *str_ptr == 'l')
                {
                    scale *= 2.54E-2;
                    str_ptr++;
                }
            }
            break;
        case 'u':
        case 'U':
            scale = 1.E-06;
            str_ptr++;
            break;
        case 'n':
        case 'N':
            scale = 1.E-09;
            str_ptr++;
            break;
        case 'p':
        case 'P':
            scale = 1.E-12;
            str_ptr++;
            break;
        case 'f':
        case 'F':
            scale = 1.E-15;
            str_ptr++;
            break;
        case 'a':
        case 'A':
            scale = 1.E-18;
            str_ptr++;
            break;
    };
    while((*str_ptr == ' ' || *str_ptr == '\t' || *str_ptr == '\r') && *str_ptr != 0)
    {
        str_ptr++;
    }
    if(*str_ptr != 0)
    {
        if(*str_ptr != '\n' && *str_ptr != '\r')
        {
            if(bOk) *bOk = false;
            return retValue;
        }
    }
    
    retValue = val * scale;
    if(bOk) *bOk = true;
    return retValue;
}

////////////////////////////////////////////////////////////////////////////////

static
const char* format_number
                       (char*  		 buffer,
                        const int    size,
                        const double v,
                        const int    n1,
                        const int    n2,
                        const bool   bAddPlus)
{
    char mask[15];
    if(std::isnan(v))
    {
        snprintf(buffer, size, _NaNString);
    }
    else if(isInfinity(v))
    {
        snprintf(buffer, size, _InfString);
    }
    else if(isMinusInfinity(v))
    {
        snprintf(buffer, size, _MinusInfString);
    }
    else
    {
        if(bAddPlus)
        {
            snprintf(mask, sizeof mask, "%%+-%d.%dE", n1, n2);
        }
        else
        {
            if(v >= 0)
                snprintf(mask, sizeof mask, " %%%d.%dE", n1, n2);
            else
                snprintf(mask, sizeof mask, "%%%d.%dE", n1, n2);
        }
        snprintf(buffer, size, mask, v);
        
#ifdef _MSC_VER
        for(char* p=buffer; p != 0; p++)
        {
            if(*p == 'E') // E+000
            {
                p++;    // signal
                p++;    // 1st zero
                *p = *(p+1);
                p++; *p = *(p+1);
                p++; *p = 0;
                return buffer;
            }
        }
#endif
    }
    return buffer;
}

////////////////////////////////////////////////////////////////////////////////

static
char* format_number_electric(char* 	            buffer,
                             const unsigned int size,
                             const double       n,       // the number
                             const int          nbDec,   // nb decimal points
                             const bool         bTrimString)
{
    struct Range { double min; double max; double mult; const char* mask; };
    static struct Range AllRanges[] = {
        { 0.99999999E-3,  0.99999999,      1e-3,  "m" },
        { 0.99999999E-6,  0.999999999E-3,  1e-6,  "u" },
        { 0.99999999E-9,  0.99999999E-6,   1e-9,  "n" },
        { 0.99999999E-12, 0.99999999E-9,   1e-12, "p" },
        { 0.99999999E-15, 0.99999999E-12,  1e-15, "f" },
        { 0.99999999,     0.99999999E3,    1.0,   " " },
        { 0.99999999E3,   0.99999999E6,    1e3,   "K" },
        { 0.99999999E6,   0.99999999E9,    1e6,   "M" },
        { 0.99999999E9,   0.99999999E12,   1e9,   "G" },
        { 0.99999999E12,  0.99999999E15,   1e12,  "T"}
    };
    
    const double absn = std::fabs(n);
    char mask[26];
    
    if(std::isnan(n))
    {
        snprintf(buffer, size, _NaNString);
    }
    else
    {
        snprintf(mask, sizeof mask, "%%%d.%df%%s", nbDec+6, nbDec);
        for(;;)
        {
            if(absn == 0.0)
            {
                snprintf(buffer, size, mask, n, " ");
                break;
            }
            
            for(size_t i=0; i<sizeof(AllRanges)/sizeof(AllRanges[0]); i++)
            {
                if(IS_IN(absn, AllRanges[i].min, AllRanges[i].max))
                {
                    snprintf(buffer, size, mask, n/AllRanges[i].mult, AllRanges[i].mask);
                    break;
                }
            }
            if(absn < 0.99999999E-15)
            {
                snprintf(mask, sizeof mask, "%%%d.%dE ", nbDec+6, nbDec);
                snprintf(buffer, size, mask, n);
                break;
            }
            if(absn > 0.99999999E15)
            {
                snprintf(mask, sizeof mask, "%%%d.%dE ", nbDec+6, nbDec);
                snprintf(buffer, size, mask, n);
                break;
            }
            break;
        }
    }
#ifdef  _MSC_VER
    for(char* p=buffer; *p != 0; p++)
    {
        if(*p == 'E') // E+000
        {
            p++;    // signal
            p++;    // 1st zero
            *p = *(p+1);
            p++; *p = *(p+1);
            p++; *p = 0;
            return buffer;
        }
    }
#endif
    // strip the output buffer.
    if(bTrimString)
    {
        char* p1 = buffer;
        // look for the 1st non-' ' char
        while(p1 && *p1 == ' ' && *p1 != 0)
        {
            p1++;
        }
        // look for the last non-' ' char
        char* p2 = p1;
        while(p2 && *p2 != 0 && *p2 != ' ')
        {
            p2++;
        }
        *p2 = 0;
        // copy
        for(p2=p1, p1=buffer; *p2 != 0; p2++, p1++)
        {
            *p1 = *p2;
        }
        *p1 = 0;
    }
    return buffer;
}


////////////////////////////////////////////////////////////////////////////////

double stringToReal(const std::string& s, bool* bOk)
{
    return stringToReal(s.c_str(), bOk);
}

////////////////////////////////////////////////////////////////////////////////

std::string realToStringElectric(const double value,    // the number
                                 const int    nbDec,    // nb decimal points
                                 const bool   bTrimString)
{
    char sBuffer[32];
    format_number_electric(sBuffer,sizeof(sBuffer), value, nbDec, bTrimString);
    return std::string(sBuffer);
}

////////////////////////////////////////////////////////////////////////////////

std::string realToString(const double value,
                         const double n1,
                         const double n2,
                         const double bAddPlus)
{
    char sBuffer[32];
    format_number(sBuffer, sizeof(sBuffer), value, n1, n2, bAddPlus);
    return std::string(sBuffer);
}

////////////////////////////////////////////////////////////////////////////////

#define TEST(SVAL, DVAL) \
    fprintf(stdout, "%-15s == %10.15E : ok: %d PASS:%d\n", SVAL, stringToReal(SVAL, &bOk), (int)bOk, stringToReal(SVAL, &bOk) == DVAL);

#define TEST2(SVAL, DVAL) \
    fprintf(stdout, "%s <- %s\n", realToStringElectric(DVAL, 3, true).c_str(), SVAL);

#define TEST3(SVAL, DVAL) \
fprintf(stdout, "%s <- %s\n", realToString(DVAL, 10, 5, true).c_str(), SVAL);


int testStringToReal(const int argv, const char** argc)
{
    bool bOk = true;
    
    TEST("1.m",        1e-3);
    TEST("1.23m",      1.23e-3);
    TEST("1.23M",      1.23e+6);
    TEST("343.43533G", 3.4343533e11);

    fprintf(stdout, "\n");

    
    TEST2("1.230n",  1.23e-9);
    TEST2("1.230u",  1.23e-6);
    TEST2("1.230m",  1.23e-3);
    TEST2("1.230M",  1.23e+6);
    TEST2("1.230G",  1.23e+9);

    fprintf(stdout, "\n");
    
    TEST3("1.230n",  1.23e-9);
    TEST3("1.230u",  1.23e-6);
    TEST3("1.230m",  1.23e-3);
    TEST3("1.230M",  1.23e+6);
    TEST3("1.230G",  1.23e+9);
    
    
    return 0;
}




