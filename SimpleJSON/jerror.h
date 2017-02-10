#pragma once
#include <stdexcept>

class json_exception : public std::runtime_error
{
public:
    json_exception(const std::string& message = "json exception")
        : runtime_error(message)
    {
    }
};

class null_json_value : public json_exception
{
};

class json_parse_error : public json_exception
{
};