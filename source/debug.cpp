#include "debug.h"

#include <iostream>

#include <stdio.h>
#include <stdarg.h>

using namespace std;
using namespace serial;

Debug* Debug::instance_ = NULL;

Debug* Debug::Instance()
{
    if (instance_ == NULL)
        instance_ = new Debug();

    return instance_;
}

Debug::Debug()
{
#ifdef __DEBUG__
    file_.open("./debug.txt", ios::out | ios::app);
#endif
}

Debug::~Debug()
{
#ifdef __DEBUG__
    file_.close();
#endif
}

ostream& Debug::Log()
{
    return file_;
}

void Debug::Log(const char* fmt, ...)
{
    char buf[4096];
    va_list arg_list;
    va_start( arg_list, fmt );
    vsprintf( buf, fmt, arg_list );

    Log() << buf << endl;
    va_end( arg_list );
}
