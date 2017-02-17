#pragma once
#include <stdexcept>

namespace mq
{

class json_exception : public std::runtime_error
{
public:
    json_exception(const std::string& message = "json exception")
        : runtime_error(message)
    {
    }
};

class bad_json_cast : public json_exception
{
public:
    bad_json_cast()
        : json_exception("bad json cast")
    { }
};

}