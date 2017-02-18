#include "jparser.h"
#include <cerrno>
#include <cctype>
#include <cassert>
#include <cstdlib>

namespace mq
{
json jparser::parse(const std::string& s, std::string& err)
{
    try
    {
        jparser parser(s);
        return parser.parse_value();
    }
    catch (std::runtime_error& errorMsg)
    {
        err = errorMsg.what();
        return json::null;
    }
}

json jparser::parse(const std::string& s)
{
    std::string err;
    return parse(s, err);
}

jparser::jparser(const std::string& s)
    : s(s.c_str())
    , p(s.c_str())
{
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
            return parse_null();
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
    if (strncmp(p, "true", 4) == 0)
    {
        p += 4;
        return true;
    }
    if (strncmp(p, "false", 5) == 0)
    {
        p += 5;
        return false;
    }
    throw std::runtime_error(("Expected `true` or `false` at position ") + std::to_string(p - s));
}

std::string jparser::parse_string()
{
    skip_space();
    assert(*p == '\"');
    ++p;
    if (*p == '\"')
    {
        ++p;
        return{};
    }
    std::string str;
    while (*p)
    {
        if (*p == '\\')
        {
            ++p;
            switch (*p)
            {
                case'\"':
                    ++p;
                    str.push_back('\"');
                    continue;
                case '\\':
                    ++p;
                    str.push_back('\\');
                    continue;
                case '/':
                    ++p;
                    str.push_back('/');
                    continue;
                case 'b':
                    ++p;
                    str.push_back('\b');
                    continue;
                case 'f':
                    ++p;
                    str.push_back('\f');
                    continue;
                case 'n':
                    ++p;
                    str.push_back('\n');
                    continue;
                case 'r':
                    ++p;
                    str.push_back('\r');
                    continue;
                case 't':
                    ++p;
                    str.push_back('\t');
                    continue;
                case 'u':
                    str += parse_utf16_escape_sequence();
                    continue;
                default:; //fall through
            }
        }
        if (*p == '\"')
        {
            ++p;
            return str;
        }
        else
        {
            str.push_back(*p);
            ++p;
        }
    }
    throw std::runtime_error(("Unexpected end of input"));
}

json jparser::parse_object()
{
    skip_space();
    assert(*p == '{');
    ++p;
    json::object obj;
    skip_space();
    if (*p == '}')
    {
        ++p;
        return obj;
    }
    while (*p)
    {
        auto str = parse_string();
        if (obj.find(str) != obj.end())
        {
            throw std::runtime_error(("Duplicated key at position ") + std::to_string(p - s - str.size()));
        }
        skip_space();
        if (*p != ':')
        {
            throw std::runtime_error(("Expected `:` at position ") + std::to_string(p - s));
        }
        ++p;
        auto val = parse_value();
        obj.emplace(std::move(str), std::move(val));
        skip_space();
        if (*p == ',')
        {
            ++p;
        }
        else if (*p == '}')
        {
            ++p;
            return obj;
        }
        else
        {
            throw std::runtime_error(("Expected `}` or `,` at position ") + std::to_string(p - s));
        }
    }
    throw std::runtime_error(("Unexpected end of input"));
}

json jparser::parse_null()
{
    if (strncmp(p, "null", 4) == 0)
    {
        p += 4;
        return json::null;
    }
    throw std::runtime_error(("Expected string `null` at position ") + std::to_string(p - s));
}

json jparser::parse_array()
{
    skip_space();
    json::array arr;
    assert(*p == '[');
    ++p;
    skip_space();
    if (*p == ']')
    {
        ++p;
        return arr;
    }
    while (*p)
    {
        arr.push_back(parse_value());
        skip_space();
        if (*p == ',')
        {
            ++p;
        }
        else if (*p == ']')
        {
            ++p;
            return arr;
        }
        else
        {
            throw std::runtime_error(("Expected `,` or `]` at position ") + std::to_string(p - s));
        }
    }
    throw std::runtime_error(("Unexpected end of input"));
}

json jparser::parse_number()
{
    skip_space();
    const char* c = p;
    int64_t integer = 0;
    double fraction = 0;
    char* e;
    if (*c == '-')
    {
        ++c;
    }
    if (*c == '0')
    {
        ++c;
    }
    else if (isdigit(*c)) //1-9
    {
        integer = std::strtoll(p, &e, 10);
        if (*e != '.' || *e != 'e' || *e != 'E') // Number is integer.
        {
            if (errno == ERANGE)
            {
                throw std::runtime_error(("Number too big at position ") + std::to_string(e - s));
            }
            p = e;
            return integer;
        }
    }
    else
    {
        throw std::runtime_error(("Expected digit at position ") + std::to_string(c - s));
    }
    if (*c == '.')
    {
        ++c;
        if (!isdigit(*c))
        {
            throw std::runtime_error(("Expected digit at position ") + std::to_string(c - s));
        }
        for (++c; isdigit(*c); ++c);
    }
    if (*c == 'e' || *c == 'E')
    {
        ++c;
        if (*c == '-' || *c == '+')
        {
            ++c;
        }
        if (!isdigit(*c))
        {
            throw std::runtime_error(("Expected digit at position ") + std::to_string(c - s));
        }
        for (++c; isdigit(*c); ++c);
    }
    fraction = std::strtod(p, &e);
    if (errno == ERANGE)
    {
        throw std::runtime_error(("Number too big at position ") + std::to_string(e - s));
    }
    p = e;
    return fraction;
}

std::string jparser::parse_utf16_escape_sequence()
{
    std::string str;
    assert(*p == 'u');
    ++p;
    uint32_t h;
    uint32_t ch;
    if (sscanf(p, "%04x", &h) != 1)
    {
        throw std::runtime_error(("Expected 4 hexadecimal digits at position ") + std::to_string((p - s)));
    }
    p += 4;
    if ((h & 0xFC00) == 0xD800) //utf-16 BE
    {
        ch = h & 0x3FF;
        if (strncmp(p, "\\u", 2) != 0)
        {
            throw std::runtime_error("Expected `\\uxxxx` escape sequence at position " + std::to_string(p - s));
        }
        p += 2;
        if (sscanf(p, "%04x", &h) != 1 || ((h & 0xFC00) != 0xDC00))
        {
            throw std::runtime_error(("Encoding error at position ") + std::to_string((p - s)));
        }
        p += 4;
        ch = (ch << 10) | (h & 0x3FF);
        ch += 0x10000;
    }
    else
    {
        ch = h;
    }
    if (ch <= 0x7F)
    {
        str.push_back(static_cast<char>(ch));
        return str;
    }
    if (ch <= 0x7FF)
    {
        str.push_back(0b1100'0000 | (ch >> 6));
        str.push_back(0b1000'0000 | (ch & 0x3F));
        return str;
    }
    if (ch <= 0xFFFF)
    {
        str.push_back(0b1110'0000 | (ch >> 12));
        str.push_back(0b1000'0000 | ((ch >> 6) & 0x3F));
        str.push_back(0b1000'0000 | (ch & 0x3F));
        return str;
    }
    if (ch <= 0x1FFFFF)
    {
        str.push_back(0b1111'0000 | (ch >> 18));
        str.push_back(0b1000'0000 | ((ch >> 12) & 0x3F));
        str.push_back(0b1000'0000 | ((ch >> 6) & 0x3F));
        str.push_back(0b1000'0000 | (ch & 0x3F));
        return str;
    }
    if (ch <= 0x3FFFFFF)
    {
        str.push_back(0b1111'1000 | (ch >> 24));
        str.push_back(0b1000'0000 | ((ch >> 18) & 0x3F));
        str.push_back(0b1000'0000 | ((ch >> 12) & 0x3F));
        str.push_back(0b1000'0000 | ((ch >> 6) & 0x3F));
        str.push_back(0b1000'0000 | (ch & 0x3F));
        return str;
    }
    if (ch <= 0x7FFFFFFF)
    {
        str.push_back(0b1111'1100 | (ch >> 30));
        str.push_back(0b1000'0000 | ((ch >> 24) & 0x3F));
        str.push_back(0b1000'0000 | ((ch >> 18) & 0x3F));
        str.push_back(0b1000'0000 | ((ch >> 12) & 0x3F));
        str.push_back(0b1000'0000 | ((ch >> 6) & 0x3F));
        str.push_back(0b1000'0000 | (ch & 0x3F));
        return str;
    }
    return str;
}

void jparser::skip_space()
{
    for (; *p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'; ++p);
}

}
