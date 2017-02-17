#pragma once
#include <map>
#include <string>
#include <vector>
#include "jnode.h"
namespace mq
{

class json
{
public:
    using object = std::map<std::string, json>;
    using array = std::vector<json>;
    static json null;
    json();
    json(std::nullptr_t);
    json(int d);
    json(double d);
    json(const std::string& s);
    json(std::string&& s);
    json(const char* s);
    json(const object& r);
    json(object&& r);
    json(const array& r);
    json(array&& r);
    json(bool b);
    template<class T>
    json(T*) = delete; //delete all other ctors

    using type = enum jvalue::type;

    type value_type() const;
    bool as_bool() const;
    int as_int() const;
    double as_double() const;
    const std::string& as_string() const;

    json(const json& r);
    json& operator=(const json& r);
    json(json&& r) noexcept;
    json& operator=(json&& r) noexcept;

    const json& operator[](size_t i) const;
    const json& operator[](const std::string& i) const;

    friend bool operator==(const json& l, const json& r);
    friend bool operator!=(const json& l, const json& r);

private:
    std::shared_ptr<const jvalue> _node;
};

}
