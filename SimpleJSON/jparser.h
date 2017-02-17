#pragma once

#include "json.h"
namespace mq
{

class jparser
{
public:
    static json parse(std::string& err);
private:
    jparser(const std::string& s);

    json parse_value();
    json parse_boolean();
    std::string parse_string();
    json parse_object();
    json parse_array();
    json parse_number();
    void skip_space();

    const char* p;
    int line;
    int column;
};

}