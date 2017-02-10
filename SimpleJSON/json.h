#pragma once

#include "jnode.h"

using swallow = int[];

template<class T, std::enable_if_t<std::is_convertible<T, double>::value, int> = 0>
std::unique_ptr<json_value> make_value(T i)
{
    return std::make_unique<json_number>(i);
}

template<size_t N>
std::unique_ptr<json_value> make_value(const char(&str)[N])
{
    return std::make_unique<json_string>(str);
}

std::unique_ptr<json_value> make_value(const std::string& str)
{
    return std::make_unique<json_string>(str);
}

std::unique_ptr<json_value> make_value(bool b)
{
    return std::make_unique<json_bool>(b);
}

std::unique_ptr<json_value> make_value(std::unique_ptr<json_value>& v)
{
    return std::unique_ptr<json_value>(v->clone());
}

template<class... T>
std::unique_ptr<json_value> make_array(T&&... args)
{
    auto r = std::make_unique<json_array>();
    swallow{(r->add(make_value(std::forward<T>(args))), 0)...};
    return std::move(r);
}

template<class... T>
std::unique_ptr<json_value> make_object(std::pair<std::string, T>&&... args)
{
    auto r = std::make_unique<json_object>();
    swallow{(r->add(args.first, make_value(args.second)), 0)...};
    return r;
}

class json
{
public:
    template<class... T>
    static json from_array(T&&... args);

    template<class... T>
    static json from_object(const std::pair<std::string, T>&... args);
private:
    std::unique_ptr<json_value> _root;
};