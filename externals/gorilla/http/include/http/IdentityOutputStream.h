#pragma once
#include "IOutputStream.h"

namespace gorilla
{
namespace http
{
/**
    IdentityOutputStream just forward output
*/
class IdentityOutputStream: public IOutputStream
{
};

} //http
} //gorilla
