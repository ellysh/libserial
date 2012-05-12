#include "state_client_wrap.h"

using namespace std;
using namespace serial;

StateClientWrap::StateClientWrap(string name) : name_(name)
{
    /* FIXME: Implement this method */
}

StateClientWrap::~StateClientWrap()
{
    /* FIXME: Implement this method */
}

void StateClientWrap::SetState(mysql_state::ObjectState state)
{
    if ( name_.empty() )
        return;

    /* FIXME: Implement this method */
}
