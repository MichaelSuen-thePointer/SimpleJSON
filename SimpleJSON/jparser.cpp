#include "jparser.h"
#include <cerrno>
#include <cctype>
#include <cassert>
#include <cstdlib>
#include <clocale>
#include <cuchar>
namespace mq
{
json jparser::parse(const std::string& s, std::string& err) noexcept
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

json jparser::parse(const std::string& s) noexcept
{
    std::string err;
    return parse(s, err);
}

jparser::jparser(const std::string& s)
    : s(s.c_str())
    , p(s.c_str())
{
}

/*
 * This method uses an ugly way to make the parsing not recursive.
 * Basically, it use vectors to simulate the stack and use macro
 * `CALL` and `RETURN` to simulate the invoking and returning of
 * the subroutine.
 */
json jparser::parse_value()
{
#define RETURN(val) if (recurHeight == 0) return val; returnValues.emplace_back(std::move(val)); recurHeight--; \
    _addr = returnAddr.back(); returnAddr.pop_back(); \
    switch(_addr) { case parse_value_object: goto VALUE_OBJECT_RETURN; case parse_value_array: goto VALUE_ARRAY_RETURN;\
    case parse_object_value: goto OBJECT_VALUE_RETURN; case parse_array_value: goto ARRAY_VALUE_RETURN; default: assert(0); } (void)0

#define CALL(call_label, back_addr, back_label, assign_val) \
    returnAddr.push_back(back_addr); recurHeight++; goto call_label; back_label: assign_val = pop_back(returnValues)

    enum return_addr { parse_value_object, parse_value_array, parse_object_value, parse_array_value };
    int recurHeight = 0;
    std::vector<json> returnValues;
    std::vector<return_addr> returnAddr;
    std::vector<json::object> obj;
    std::vector<json::array> arr;
    auto pop_back = [](auto& vec) { auto t = vec.back(); vec.pop_back();  return t; };
    return_addr _addr;
    std::string str;
PARSE_VALUE:
    skip_space();
    if (*p == '\0')
    {
        RETURN(json::null);
    }
    switch (*p)
    {
    case '{': //OBJECT
    {
        CALL(PARSE_OBJECT, parse_value_object, VALUE_OBJECT_RETURN, auto _theobj);
        RETURN(_theobj);
    }
    case '[':
    {
        CALL(PARSE_ARRAY, parse_value_array, VALUE_ARRAY_RETURN, auto _thearray);
        RETURN(_thearray);
    }
    case 't': case 'f':
        RETURN(parse_boolean());
    case 'n':
        RETURN(parse_null());
    case '\"':
        RETURN(parse_string());
    default:
        if (isdigit(*p) || *p == '-')
        {
            RETURN(parse_number());
        }
        RETURN(json::null);
    }
    { //PARSE OBJECT
PARSE_OBJECT:
        skip_space();
        assert(*p == '{');
        ++p;
        skip_space();
        if (*p == '}')
        {
            ++p;
            RETURN(json::object{});
        }
        obj.emplace_back();
        while (*p)
        {
            str = parse_string();
            if (obj.back().find(str) != obj.back().end())
            {
                throw std::runtime_error(("Duplicated key at position ") + std::to_string(p - s - str.size()));
            }
            skip_space();
            if (*p != ':')
            {
                throw std::runtime_error(("Expected `:` at position ") + std::to_string(p - s));
            }
            ++p;
            CALL(PARSE_VALUE, parse_object_value, OBJECT_VALUE_RETURN, auto val);

            obj.back().emplace(std::move(str), std::move(val));
            skip_space();
            if (*p == ',')
            {
                ++p;
            }
            else if (*p == '}')
            {
                ++p;
                RETURN(pop_back(obj));
            }
            else
            {
                throw std::runtime_error(("Expected `}` or `,` at position ") + std::to_string(p - s));
            }
        }
        throw std::runtime_error(("Unexpected end of input"));
    }
    { //PARSE ARRAY
PARSE_ARRAY:
        skip_space();
        assert(*p == '[');
        ++p;
        skip_space();
        if (*p == ']')
        {
            ++p;
            RETURN(json::array{});
        }
        arr.emplace_back();
        while (*p)
        {
            CALL(PARSE_VALUE, parse_array_value, ARRAY_VALUE_RETURN, auto _val);
            arr.back().push_back(std::move(_val));
            skip_space();
            if (*p == ',')
            {
                ++p;
            }
            else if (*p == ']')
            {
                ++p;
                RETURN(pop_back(arr));
            }
            else
            {
                throw std::runtime_error(("Expected `,` or `]` at position ") + std::to_string(p - s));
            }
        }
        throw std::runtime_error(("Unexpected end of input"));
    }
}
#undef CALL
#undef RETURN
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
                --p; //step back, give full `\uXXXX` sequence to parse function, for loop invariance
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
        do
        {
            ++c;
        } while (isdigit(*c));
    }
    else
    {
        throw std::runtime_error(("Expected digit at position ") + std::to_string(c - s));
    }
    if (*c != '.' && *c != 'e' && *c != 'E')
    {
        errno = 0;
        int64_t integer = std::strtoll(p, &e, 10);
        if (errno != ERANGE)
        {
            p = e;
            return integer;
        }
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
        do
        {
            ++c;
        } while (isdigit(*c));
    }
    errno = 0;
    double fraction = std::strtod(p, &e);
    if (errno == ERANGE)
    {
        throw std::runtime_error(("Number too big at position ") + std::to_string(e - s));
    }
    p = e;
    return fraction;
}

/*
 * Use `c16rtomb` to convert utf16 sequence to utf-8
 */
std::string jparser::parse_utf16_escape_sequence()
{
    char u8str[7]{}; //support the origin version of utf-8: at most 6 chars
    uint16_t char16;
    std::mbstate_t convState = {};
    size_t convSize;
    do //this loop will loop for at most 2 cycles
    {
        if (strncmp(p, "\\u", 2) != 0)
        {
            throw std::runtime_error("Expected `\\uXXXX` escape sequence at position " + std::to_string((p - s)));
        }
        p += 2;
        if (sscanf(p, "%04hx", &char16) != 1)
        {
            throw std::runtime_error("Expected 4 hexadecimal digit sequence at position " + std::to_string((p - s)));
        }
        convSize = std::c16rtomb(u8str, char16, &convState);
        if (convSize == -1)
        {
            throw std::runtime_error("Bad utf-16 code point at position " + std::to_string(p - s));
        }
        p += 4;
    } while (convSize == 0); // convSize == 0, meet multibyte utf-16 char, continue loop for next
    return u8str; //copy to return
}

void jparser::skip_space()
{
    for (; *p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'; ++p);
}

}
