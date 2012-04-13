#ifndef PROGRAM_OPTIONS_H
#define PROGRAM_OPTIONS_H

#include <string>
#include <fstream>
#include <map>

namespace planar
{

class ProgramOptions
{
public:
    ProgramOptions(std::string filename);
    ~ProgramOptions();

    std::string GetString(std::string option_name);
    int GetInt(std::string option_name);

private:
    std::ifstream file_;
    std::map<std::string, std::string> program_options_;

    void LoadOptions();
};

}

#endif
