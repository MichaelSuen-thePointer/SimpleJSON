#include "jparser.h"
#include <cctype>
#include <cassert>

namespace mq
{
json jparser::parse(const std::string& s, std::string& err)
{
    try
    {
        jparser parser(s);
        return parser.parse_value();
    }
    catch (std::string& errorMsg)
    {
        err = errorMsg;
        return json::null;
    }
}

json jparser::parse_value()
{
    skip_space();
    if (*p == '\0')
    {
        return json::null;
    }
    switch (*p)
    {
    case '{':
        return parse_object();
    case '[':
        return parse_array();
    case 't': case 'f':
        return parse_boolean();
    case 'n':
        return json::null; //whether success or fail return null, make it simple
    case '\"':
        return parse_string();
    default:
        if (isdigit(*p) || *p == '-')
        {
            return parse_number();
        }
        return json::null;
    }
}

json jparser::parse_boolean()
{
    skip_space();
    if (strcmp(p, "true"))
    {
        p += 4;
        return true;
    }
    if (strcmp(p, "false"))
    {
        p += 5;
        return false;
    }
    return json::null;
}

std::string jparser::parse_string()
{
    skip_space();
}

json jparser::parse_object()
{
    skip_space();
    assert(*p == '{');
    ++p;
    json::object obj;
    while (*p)
    {
        auto str = parse_string();
        skip_space();
        if (*p != ':')
        {
            return json::null;
        }

    }
}

json jparser::parse_array()
{
    skip_space();
}

json jparser::parse_number()
{
    skip_space();
}

void jparser::skip_space()
{
    for (;;)
    {
        switch (*p)
        {
        case ' ':case '\t':case '\r':case '\n':
            ++p;
        default:
            return;
        }
    }
}

}
