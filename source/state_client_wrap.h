#ifndef STATE_CLIENT_WRAP_H
#define STATE_CLIENT_WRAP_H

#include <string>

#include <types_mysql_state.h>
#include <state_client.h>

namespace serial
{

class StateClientWrap
{
public:
    StateClientWrap(std::string name);
    virtual ~StateClientWrap();

    void SetState(mysql_state::ObjectState state);

private:
    std::string name_;
    mysql_state::StateClient* client_;
};

}

#endif
