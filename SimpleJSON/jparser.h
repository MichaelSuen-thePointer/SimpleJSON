#pragma once

#include "jnode.h"

class jparser
{
public:
    static std::unique_ptr<json_value> parse_json(const std::string& content);
    
private:
    jparser();

    std::unique_ptr<json_object> parse_object();

    std::unique_ptr<json_array> parse_array();

    std::unique_ptr<json_value> parse_json_value();

    double parse_number();

    bool parse_bool();

    std::string parse_string();

    [[noreturn]]
    static void raise_parse_error();
};