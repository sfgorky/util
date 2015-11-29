#include "StringMatch.h"

namespace StringMatch {

#ifndef NEGATE
#define NEGATE    '^'            // std cset negation char
#endif

// internal match function.
static bool _match(const std::string& pattern, const char* str)
{
    const char* pat = pattern.c_str();
    while(*pat)
    {
        if(!*str && *pat != '*')
        {
            return false;
        }
        int c = *pat++;
        switch(c)
        {
            ///////////////////////////////////////////////////////////////////
            case '*':
                while(*pat == '*')
                {
                    pat++;
                }
                if(!*pat)
                {
                    return true;
                }
                if(*pat != '?' && *pat != '[' && *pat != '\\')
                {
                    while(*str && *pat != *str)
                    {
                        str++;
                    }
                }
                while(*str)
                {
                    if(_match(pat, str))
                    {
                        return true;
                    }
                    str++;
                }
                return false;
            ///////////////////////////////////////////////////////////////////
            case '?':
                if(*str)
                {
                    break;
                }
                return false;
            ///////////////////////////////////////////////////////////////////
            //  set specification is inclusive, that is [a-z] is a, z and
            //  everything in between. this means [z-a] may be interpreted
            //  as a set that contains z, a and nothing in between.
            case '[':
            {
                bool negate;
                if(*pat != NEGATE) negate = false;
                else
                {
                    negate = true;
                    pat++;
                }
                bool match = false;
                while(!match && (c = *pat++))
                {
                    if(!*pat) return false;
                    if(*pat == '-') //  c-c
                    {
                        if(!*++pat) return false;
                        if(*pat != ']')
                        {
                            if(*str == c || *str == *pat || (*str > c && *str < *pat))
                                match = true;
                        }
                        else // c-]
                        {
                            if(*str >= c)  match = true;
                            break;
                        }
                    }
                    else  // cc or c]
                    {
                        if(c == *str) match = true;
                        if(*pat != ']')
                        {
                            if(*pat == *str)  match = true;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            if (negate == match)
            {
                return false;
            }
            // if there is a match, skip past the cset and continue on
            while(*pat && *pat != ']')
            {
                pat++;
            }
            if(!*pat++)    // oops!/
            {
                return false;
            }
        }
        break;
        ///////////////////////////////////////////////////////////////////////
        case '\\':
            if(*pat)  c = *pat++;
            break;
        default:
            if (c != *str)  return false;
            break;
        }
        str++;
    }
    return !*str;
}

////////////////////////////////////////////////////////////////////////////////

bool match(const std::string& str, const std::string& pattern)
{
    return _match(pattern, str.c_str());
}

////////////////////////////////////////////////////////////////////////////////

bool match(const std::string& sString, const std::vector<std::string>& patternList)
{
    bool bOk = false;
    for(size_t i=0; i<patternList.size( ); i++)
    {
        if(match(sString, patternList[i]))
        {
            bOk = true;
            break;
        }
    }
    return bOk;
}

////////////////////////////////////////////////////////////////////////////////

bool test(const char* string, const char* matchPattern, const bool expectedResult)
{
    const bool thisResult = match(string, matchPattern);
    fprintf(stdout, "match %-15s %-15s -> %d - good: %d\n", string, matchPattern, thisResult, (thisResult==expectedResult));
    return (thisResult == expectedResult);
}

////////////////////////////////////////////////////////////////////////////////

static
const char* stringMatchTestExamples[] =
{
    // str, pattern, good/bad
      "a",            "*",        "1"
    , "abc",          "*",        "1"
    , "a",            "?",        "1"
    , "a",            "[",        "0"
    , "-",            "[--z]",    "1"
    , "a",            "[--z]",    "1"
    , "-abc",         "[-]abc",   "1"
    , "zd",           "[-a-]?",   "1"
    , "dd",           "[-abc]?",  "0"
    , "-b",           "[-abc]?",  "1"
    , "abc",          "[a-c][a-c][a-c]", "1"
    , "a",            "[]",       "0"
    , "^abc",         "[]-z]abc", "1"
    , "]abc",         "[]]abc",   "1"
    , "a",            "[]abc[a]", "1"
    , "[",            "[]abc[de]","1"
    , "]",            "[]abc[de]","1"
    , "d",            "[]abc[de]","1"
    , "a",            "[^a-z]",   "0"
    , "A",            "[^a-z]",   "1"
    , "a",            "[a-z]",    "1"
    , "a",            "[ab",      "0"
    , "abca",         "[abc]??[def]", "0"
    , "abcg",         "[abc]??[def]", "0"
    , "abcd",         "[abc]??[def]", "1"
    , "a",            "[b-",      "0"
    , "a",            "[z-a]",    "1"
    , "z",            "[z-a]",    "1"
    , "??*![abc]",    "\\?\\?\\*!\\[abc\\]", "1"
    , "abCdaxxxxb",   "ab[a-z]?a*b",  "0"
    , "abc",          "ab[a-z]?a*b",  "0"
    , "abcdab",       "ab[a-z]?a*b",  "1"
    , "abcdaxxxxb",   "ab[a-z]?a*b",  "1"
    , "xxx",          "xxx",          "1"
    , "az",           "a*z",          "1"
    , "abz",          "a?z",          "1"
    , "aasdfsdjflzz", "a*z",          "1"
};

////////////////////////////////////////////////////////////////////////////////

bool internalTest( )
{
    bool bOk = true;

    fprintf(stdout, "String Match Examples\n\n\n");

    const size_t nb = sizeof(stringMatchTestExamples)/sizeof(stringMatchTestExamples[0]);
    for(size_t i=0; i<nb; i += 3)
    {
        const std::string s    = stringMatchTestExamples[i];
        const std::string pat  = stringMatchTestExamples[i+1];
        const bool        good = stringMatchTestExamples[i+2][0] == '1' ? true : false;

        bOk &= test(s.c_str(), pat.c_str(), good);
    }
    fprintf(stdout, "\n\n:Final status: %d\n", bOk);
    return bOk;
}

////////////////////////////////////////////////////////////////////////////////
} // namespace StringMatch

bool testStringMatch(const int /* argc */, const char** /* argv */)
{
    StringMatch::internalTest( );
    return true;
}

////////////////////////////////////////////////////////////////////////////////

#ifdef STRING_MATCH_MAIN

#include <cstdio>
#include <cstdlib>

int main(const int argc, const char*const* argv)
{
    if(argc == 1)
    {
        fprintf(stderr, "%s <pattern> <string> | test\n", argv[0]);
        exit(1);
    }
    if(argc ==3)
    {
        const std::string pat = argv[1];
        const std::string str = argv[2];

        if(StringMatch::match(pat, str))
            fprintf(stderr, "1\n");
        else
            fprintf(stderr, "0\n");
    }
    else
    {
        StringMatch::internalTest( );
    }
  return 0;
}
#endif



