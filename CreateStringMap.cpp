#include <stdarg.h>
#include <memory>
#include <assert.h>
#include <fstream>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include "CreateStringMap.h"

#ifdef _MSC_VER
    #ifndef snprintf
    #define snprintf _snprintf
    #endif
#endif

#ifndef nullptr
#define nullptr 0
#endif


typedef std::vector<std::string> StringVector;


#define  AA printf("**** %d\n", __LINE__);


const char* ws = " \t\n\r\f\v";

// trim from end of string (right)
static 
std::string rtrim(std::string s, const char* t = ws)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
static 
std::string ltrim(std::string s, const char* t = ws)
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from both ends of string (left & right)
static 
std::string trim(const std::string s, const char* t = ws)
{
    return ltrim(rtrim(s, t), t);
}

////////////////////////////////////////////////////////////////////////////////

static
std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string>& elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

////////////////////////////////////////////////////////////////////////////////

static 
std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

////////////////////////////////////////////////////////////////////////////////

static std::string toString(const char* sMsg,  ...)
{
    char sBuffer[2048];
    
    va_list args; 
    va_start(args, sMsg); 
    vsnprintf(sBuffer, sizeof(sBuffer), sMsg, args); 
    va_end(args); 
    return std::string(sBuffer);
}

////////////////////////////////////////////////////////////////////////////////

static std::string getValue(const std::string& value, const bool quoteString)
{
    return quoteString
            ?   toString("\"%s\"", value.c_str())
            :   value;
}

////////////////////////////////////////////////////////////////////////////////

static void createInfoSection(
                       std::ostream&       s, 
                       const std::string&  keyName, 
                       const StringVector& keyArray, 
                       const StringVector& valueArray)
{
    const std::string sTab = "    ";

    s << sTab << "//------------------------------Options-------------------------------------" << std::endl;
    s << sTab << toString("//    switch(%s)\n", keyName.c_str());
    s << sTab << "//    {" << std::endl;
    const size_t iNb = keyArray.size();
    for(size_t i=0; i<iNb; i++)
    {
        const std::string& key    = keyArray[i];
        const std::string& value  = valueArray[i];
        s << sTab << toString("//        case %-20s : return %s\n", key.c_str(), value.c_str());
    }
    s << sTab << "//    };" << std::endl;
    s << sTab << "//--------------------------------------------------------------------------" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////

class SwitchGenerator
{
////////////////////////////////////////////////////////////////////////////////
    public:
////////////////////////////////////////////////////////////////////////////////
    
    SwitchGenerator(bool caseSensitive, int tab, bool bquoteValue)
    :   m_c(1)
    ,   m_state(new GlobalState(caseSensitive, tab, bquoteValue))
    ,   m_mainClass(true)
    { 
        m_switchTree.resize(256);
        std::fill(m_switchTree.begin(), m_switchTree.end(), nullptr);
    }


    ~SwitchGenerator( )
    {
        if (m_state && m_mainClass) delete m_state;
        for(size_t i=0; i<m_switchTree.size(); i++)
        { delete m_switchTree[i]; }
    }
    
    bool add(const StringVector& keyArray, const StringVector& valueArray)
    {
        bool bOk = true;
        if(keyArray.size() != valueArray.size()) return false;
       
        for(size_t i=0; i<keyArray.size(); i++)
        {
            const std::string& key     = keyArray[i];
            const std::string& value   = valueArray[i];
        
            bOk &= newEntry(key, value);
        }
        return bOk;
    }
    
    bool newEntry(const std::string& key, const std::string& value)
    {
        return addLine(key, value);
    }
    
    void setDefaultValue(const std::string& defaultValue)
    {
        m_state->m_defaultValue = defaultValue;
    }
    void setKeyName(const std::string& keyName)
    {
        m_state->m_keyValueName = keyName;
    }

    void write(std::ostream& s)const
    {
        assert(m_state);
        m_state->m_stream = &s;
       
        printChildren(0);
    }

///////////////////////////////////////////////////////////////////////////////
    private:
///////////////////////////////////////////////////////////////////////////////
    
    struct GlobalState
    {
        GlobalState( )
            :   m_caseSensitive(false)
            ,   m_quoteValue(0)
            ,   m_startTab(0)
            ,   m_deltaTab(0)
            ,   m_stream(&std::cout)
        { }
        
        GlobalState(const bool caseSensitive, const int startTab, const bool quoteValue)
            :   m_caseSensitive(caseSensitive)
            ,   m_quoteValue(quoteValue)
            ,   m_startTab(startTab)
            ,   m_deltaTab(4)
            ,   m_stream(&std::cout)
        { }
        
        bool                    m_caseSensitive                                ;
        bool                    m_quoteValue                                   ;
        int                     m_startTab                                     ;
        int                     m_deltaTab                                     ;
        std::string             m_defaultValue                                 ;
        std::string             m_keyValueName                                 ;
        std::string             m_sString                                      ;
        std::ostream*           m_stream                                       ;
    };
    
    ////////////////////////////////////////////////////////////////////////////

    SwitchGenerator(GlobalState* state, const char c)
    :   m_c(c)
    ,   m_state(state)
    ,   m_mainClass(false)
    {
        m_switchTree.resize(256);
        std::fill(m_switchTree.begin(), m_switchTree.end(), nullptr);        
    }
    
    SwitchGenerator(GlobalState* state, const std::string& value)
    :   m_c(0)
    ,   m_state(state)
    ,   m_mainClass(false)
    {
        m_returnString = value;
        m_switchTree.resize(256);
        std::fill(m_switchTree.begin(), m_switchTree.end(), nullptr);
    }
    
    
    SwitchGenerator& operator=(SwitchGenerator& /* other */)
    {
        // not used.
        return *this;
    }
    
    ////////////////////////////////////////////////////////////////////////////

    void printChildren(unsigned int depth)const
    {
        std::ostream& s = *m_state->m_stream;
        
        const std::string tab (m_state->m_startTab+1+((depth  )*m_state->m_deltaTab), ' ');
        const std::string tab2(m_state->m_startTab+1+((depth+1)*m_state->m_deltaTab), ' ');
        const std::string tab3(m_state->m_startTab+1+((depth+2)*m_state->m_deltaTab), ' ');
        const std::string tab4(m_state->m_startTab+1+((depth+3)*m_state->m_deltaTab), ' ');
                                        
        if (!m_c)
        {
            s << tab3 << toString("return %s;\n", getValue(m_returnString, m_state->m_quoteValue).c_str());
        }
        else
         {
            s << tab3 << toString("switch(%s[%u]) { \n", m_state->m_keyValueName.c_str(),  depth);
            s << tab4 << toString("default: return %s;\n", getValue(m_state->m_defaultValue, m_state->m_quoteValue).c_str());
            for(unsigned int i=0; i<m_switchTree.size(); i++)
            {
                const SwitchGenerator* p = m_switchTree[i];
                if(!p) continue;
                if(i)
                {
                    if(m_state->m_caseSensitive)
                    {
                        char cu = (char)i;
                        s << tab4 << toString("case '%c':\n", cu);
                    }
                    else
                    {
                        char cl = (char)tolower((char)i);
                        char cu = (char)toupper((char)i);
                        s << tab4 << toString("case '%c':\n", cl);
                        if(cl != cu)
                        {
                            s << tab4 << toString("case '%c':\n", cu);
                        }
                    }
                }
                else
                {
                    s << tab4 << "case 0:\n";
                }
                s << tab4 << "{\n";
                p->printChildren(depth+2);
                s << tab4 << "}\n";
            }
            s << tab3 << "}" ; // switc
         } 
        s << std::endl;
    }

    ///////////////////////////////////////////////////////////////////////////

    bool addLine(const std::string& key,
                 const std::string& value,
                 const unsigned int iPos=0)
    {
        bool bOk = true;
        assert(m_state);
        
        unsigned char c = m_state->m_caseSensitive
                            ?  (unsigned char)key[iPos]
                            :  (unsigned char)tolower(key[iPos]);

        if(!m_switchTree[c])
        {
            m_switchTree[c] = c 
                        ?   new SwitchGenerator(m_state, c)
                        :   new SwitchGenerator(m_state, value);
        }
        else
        {
            if (!c)
            {
                fprintf(stdout, "Error: '%s- %s' defined 2 times.", key.c_str(), value.c_str());
                bOk =  false;
            }
            else
            ;
        }
        if(c && bOk)
        {
             bOk = m_switchTree[c]->addLine(key, value, iPos+1);
        }
        return bOk;
    }

    ////////////////////////////////////////////////////////////////////////////

    const char                              m_c                                ;
    GlobalState*                            m_state                            ;
    bool                                    m_mainClass                        ;
    std::vector<SwitchGenerator*>           m_switchTree                       ;
    std::string                             m_returnString                     ;
};


///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

//#define     _DEBUG_CREATE_STRING_MAP
bool createStringMap(std::ostream&          s,
                       const StringVector&  keyArray,
                       const StringVector&  valueArray,
                       const std::string    sDefault,
                       const std::string    sReturnType,
                       const std::string    sFunctionName,
                        bool                caseSensitive,
                        bool                bQuoteValue)
{
    bool bOk = true;

    int iTab = 1;
    
    const std::string keyName = "key";
    bool bAddFunctionHeader = (sFunctionName.size()==0 ? false : true);
    
	const size_t iNb     = keyArray.size();
	const size_t iNbRet  = valueArray.size();
    if(iNb != iNbRet)
    {
        fprintf(stdout, "Error: wrong number of elements in keyArray(%ld) and valueArray(%ld)\n", 
                            keyArray.size(), valueArray.size());
        bOk = false;
    }
    if(bOk)
    {
        ////////////////////////////////////////////////////////////////////////
        // print the function synopsis
        //if(bPrintFunctionSynopsis)
        {
            if(bAddFunctionHeader)
            {
                s   << toString("%s %s(const char* %s)",
                        sReturnType.c_str(), 
                        sFunctionName.c_str(), 
                        keyName.c_str()) 
                    << std::endl
                    << "{"
                    << std::endl;
            }
            createInfoSection(s, keyName, keyArray, valueArray);
        }
        ////////////////////////////////////////////////////////////////////////
        // Generates the actual switch
        SwitchGenerator sSwitch(caseSensitive, iTab, bQuoteValue);

        bool bOk = sSwitch.add(keyArray, valueArray);
        
        if(!bOk)
        {
            fprintf(stdout, "Error: cannot add data to switch. Maybe duplicates...");
        }
        else
        {
            sSwitch.setDefaultValue(sDefault);
            sSwitch.setKeyName(keyName);
            sSwitch.write(s);
        }
        
        if(bAddFunctionHeader)
        {
            s << "}\n";
        }
    }
    return bOk;
}

////////////////////////////////////////////////////////////////////////////////
/*
bool createStringMap(std::ostream&        s,
                     const StringVector&  keyArray,
                     const std::string    sFunctionName,
                     bool                 caseSensitive)
{
    const size_t iNb = keyArray.size();

    StringVector valueArray;
    valueArray.resize(iNb);
    for(size_t i=0; i<iNb; i++)
    {
    	valueArray[i] = toString("%d", (int)i);
    }
    const bool bQuoteValue = false;
    const std::string sDefault = toString("%d", -1); 

    return createStringMap(s, 
                           keyArray, 
                           valueArray, 
                           sDefault, 
                           std::string("int"), 
                           sFunctionName,
                           caseSensitive,
                           bQuoteValue);
}
*/
////////////////////////////////////////////////////////////////////////////////

bool createStringMapFromFile(
                     const std::string  dataFilename,
                     const std::string  outputFileName,
                     const std::string  sFunctionName,
                     const bool         quoteOutput, 
                     const bool         caseSensitive)
{
    bool bOk = true;

    char sBuffer[1024];
    StringVector keyArray;
    StringVector valueArray;

    FILE* p = fopen(dataFilename.c_str(), "rt");
    while(p && !feof(p))
    {
        if(fgets(sBuffer, sizeof(sBuffer)-1, p))
        {
            std::string s = trim(std::string(sBuffer));
            if(s.size()>0)
            {
                std::vector<std::string> lst = split(s, ',');
                if(lst.size() == 2)
                {
                    keyArray.push_back(trim(lst[0]));
                    valueArray.push_back(trim(lst[1]));
                }
            }
        }
    }
    if(p) fclose(p);


    const std::string sDefault = toString("%d", -1); 

    if(keyArray.size()>0)
    {
        std::ofstream outfile(outputFileName.c_str(), std::ofstream::out);
        //bOk = createStringMap(outfile, keyArray, sFunctionName, caseSensitive);
        bOk = createStringMap(outfile, 
                              keyArray, 
                              valueArray, 
                              sDefault, 
                              std::string("int"), 
                              sFunctionName,
                              caseSensitive,
                              quoteOutput);
                           
                                   
    }
    return bOk;
}


////////////////////////////////////////////////////////////////////////////////
/*
bool dtestCreateStringMap(const int  argc, const char**  argv )
{
    StringVector strList;
    StringVector valList;
    
    
    strList.push_back("foo");
    valList.push_back("1");
    
    strList.push_back("bar");
    valList.push_back("2");
    
    strList.push_back("foobar");
    valList.push_back("3");
    
    bool caseSensitive = true;

    createStringMap(std::cout,
                    strList,
                    "myFunction",
                    caseSensitive);

    
    std::string dataFile = "data.txt";
    FILE* p = fopen(dataFile.c_str(), "wt");
    for(size_t i=0; i<strList.size(); i++)
    {
        fprintf(p, "%s\n", strList[i].c_str());
    }
    fclose(p);

    createStringMap(dataFile,
                    std::string("datamap.cpp"),
                    std::string("mymap"),
                    false);

    return true;
}
*/





#define VERSION "1.0"


///////////////////////////////////////////////////////////////////////////////

static void usage(FILE* out)
{
    fprintf(out, "createStringMap (%s) Usage\n", VERSION);
    fputs("createStringMap [-i<data file>] [-o<output file>] -f <functionName> -q\n"
          "   -i input file\n"
          "   -o output file\n"
          "   -f function name\n"
          "   -q quote output\n"
          "   -u case unsensitive\n"
          "\n"
          , out);
}

///////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    std::string command;
    std::string file_in;
    std::string file_out;
    std::string function_name;
    bool quote_output = false;
    bool case_unsensitive = true;

    int opt = 0;
    while((opt = getopt(argc, argv, "i:o:f:qu")) != -1)
    {
        switch(opt)
        {
            case 'i':
                file_in = optarg;
                break;
            case 'o':
                file_out = optarg;
                break;
            case 'f':
                function_name = optarg;
                break;
            case 'q':
                quote_output = true;
                break;
            case "u":
                case_unsensitive = true;
            default:
                usage(stdout);
                return 0;
        }
    }
    ///////////////////////////////////////////////////////////////////////////
    // this is a memory buffer, used on memory option (-m). 
  
    if(!file_in.empty() && !file_out.empty() && !function_name.empty())
    {
        createStringMapFromFile(file_in, file_out, function_name, quote_output, !case_unsensitive);
    }
    else
    {
        usage(stdout);
    }
    return 0;
}



/*


int main( )
{
testCreateStringMap(0, 0);
return 0;    
}

*/


