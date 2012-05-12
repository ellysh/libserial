#include "state_client_wrap.h"

#include <assert.h>

using namespace std;
using namespace serial;
using namespace mysql_state;

StateClientWrap::StateClientWrap(string name) : name_(name)
{
    /* FIXME: Read this parameters from the ini config file */
    client_ = new StateClient("localhost", "simunit", "simunit", "simunit");
    assert( client_ != NULL );
}

StateClientWrap::~StateClientWrap()
{
    delete client_;
}

void StateClientWrap::SetState(mysql_state::ObjectState state)
{
    if ( name_.empty() )
        return;

    client_->SetState(name_, state);
}
