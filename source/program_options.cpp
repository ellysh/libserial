#include "program_options.h"

#include <iostream>
#include <stdexcept>
#include <set>

#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace serial;

ProgramOptions::ProgramOptions(string filename)
{
    file_.open(filename.c_str());

    if ( ! file_ )
        cerr << "File open failed: " << filename << endl;

    LoadOptions();
}

void ProgramOptions::LoadOptions()
{
    set<string> request;
    map<string, string> parameters;
    request.insert("*");

    try
    {
        using boost::program_options::detail::config_file_iterator;
        for (config_file_iterator i(file_, request), e ; i != e; ++i)
        {
            program_options_[i->string_key] = i->value[0];
        }
    }
    catch ( exception& e )
    {
        cerr << "Exception: " << e.what() << endl;
    }
}

ProgramOptions::~ProgramOptions()
{
    file_.close();
}

string ProgramOptions::GetString(string option_name)
{
    if ( program_options_.count(option_name) == 0 )
        return string();

    return program_options_[option_name];
}

int ProgramOptions::GetInt(string option_name)
{
    if ( program_options_.count(option_name) == 0 )
        return 0;

    return boost::lexical_cast<int>(program_options_[option_name]);
}
