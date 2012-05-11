#ifndef STATE_CLIENT_H
#define STATE_CLIENT_H

#include <string>

#include <types_mysql_state.h>

namespace serial
{

class StateClient
{
public:
    StateClient(std::string name) : name_(name) {};
    virtual ~StateClient() {};

    void SetState(mysql_state::ObjectState state);

protected:
    std::string name_;
    mysql_state::ObjectState* state_;
};

}

#endif
