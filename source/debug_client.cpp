#include "debug_client.h"

#include "debug.h"

using namespace std;
using namespace serial;

void DebugClient::SetLogFile(string log_file)
{
    Debug::SetLogFile(log_file);
}
