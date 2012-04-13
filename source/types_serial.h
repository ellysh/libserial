#ifndef TYPES_PLANAR_H
#define TYPES_PLANAR_H

#include <vector>

namespace planar
{

enum ProtocolType
{
    kProtocolPui = 0x1,
};

typedef unsigned char Byte;
typedef std::vector<Byte> ByteArray;

}

#endif
