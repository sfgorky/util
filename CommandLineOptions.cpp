#include <vector>
#include <string>
#include <stdlib.h>


void generateFile(const std::string fin, const size_t nb)
{

}

void filterWaveform(const std::string fin, const std::string fout, const size_t nb, bool verbose)
{
    
}


class Options
{
////////////////////////////////////////////////////////////////////////////////
    public:
////////////////////////////////////////////////////////////////////////////////

    Options( )
    {
        setDefault( );
    }

    Options(const int argc, const char** argv)
    {
        setDefault( );
        parse(argc, argv);
    }

    bool generate( )const
    { return m_generate; }
    
    bool verbose( )const
    { return m_verbose; }
    
    size_t nb( )const
    { return m_nb; }

    const std::string& fin( )const
    { return m_fin; }
    
    const std::string& fout( )const
    { return m_fout; }
    
    void help( )const
    {
        fprintf(stdout, "%s: usage -i <inputFile> -o <outputFile> -n <int> -g [-V] [-h]\n", m_appname.c_str());
        fprintf(stdout,
                        "    -i  [inputFile]   sets input file\n"
                        "    -o  [outputFile]  sets output file\n"
                        "    -g                Generates sample code with nb samples (given by -nb par)\n"
                        "    -n                Number of samples"
                        "    -V                verbose mode\n"
                        "    -h                help\n"
                        "\n\n");
        fprintf(stdout, "Examples:\n"
                        "%s -i <inputFile> -o <outputFile> -n 10 # create output with 10 columns\n"
                        "%s -i in.txt -g -n 10000                # create sample file 'in.txt' with 10000 samples\n"
                , m_appname.c_str(), m_appname.c_str());

    }
    
////////////////////////////////////////////////////////////////////////////////
    private:
////////////////////////////////////////////////////////////////////////////////

    void setDefault( )
    {
        m_nb = false;
        m_verbose = false;
        m_fin = "in.txt";
        m_fout = "out.txt";
    }
    void parse(const int argc, const char** argv)
    {
        m_appname = argv[0];
        if(argc == 1)
        {
            help();
            return;
        }
        for(int i=0; i<argc; i++)
        {
            m_list.push_back(argv[i]);
        }
        parse( );
    }
    ////////////////////////////////////////////////////////////////////////////
    void parse( )
    {
        const size_t nbarg = m_list.size();
        for(size_t i=1; i<nbarg; i++)
        {
            const std::string arg  = m_list[i];
            const std::string arg1 = (i<nbarg-1 ? m_list[i+1] : std::string());
            //fprintf(stdout, "[%ld] %s %s\n", i, arg.c_str(), arg1.c_str());
            if(0) { }
            else if(arg == "-h")        help( );
            else if(arg == "-V")        m_verbose = true;
            else if(arg == "-i")        m_fin   = arg1;
            else if(arg == "-o")        m_fout  = arg1;
            ////////////////////////////////////////////////////////////////////
            // specific options
            else if(arg == "-nb")       m_nb = atoi(arg1.c_str());
            else if(arg == "-g")        m_generate = true;
            
        }
    }
    ////////////////////////////////////////////////////////////////////////////
    // variables
    std::string                  m_appname                                     ;
    std::vector<std::string>     m_list                                        ;
    bool                         m_verbose                                     ;
    std::string                  m_fin                                         ;
    std::string                  m_fout                                        ;
    bool                         m_generate                                    ;
    size_t                       m_nb                                          ;
};




int main(const int argc, const char** argv)
{
    Options options(argc, argv);
    if(options.generate( ))
    {
        fprintf(stdout, "generating data: %s\n", options.fin().c_str());
        generateFile(options.fin(), 1000000);
    }

    if(!options.fin().empty() && !options.fout().empty())
    {
        filterWaveform(options.fin(), options.fout(), options.nb(), options.verbose());
    }
    else
    {
        options.help();
    }
    return 0;
}


