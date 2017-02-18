#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif


#include <map>
#include <string>
#include <vector>

namespace mq
{

class jvalue;

class json
{
public:
    using object = std::map<std::string, json>;
    using array = std::vector<json>;
    static json null;

    enum type
    {
        OBJECT = 1,
        ARRAY = 2,
        NUMBER = 4,
        STRING = 8,
        BOOLEAN = 16,
        NUL = 32
    };

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
    ~json();

    bool as_bool() const;
    int as_int() const;
    double as_double() const;
    const std::string& as_string() const;
    const object& as_object() const;
    const array& as_array() const;

    type value_type() const;
    bool is_object() const;
    bool is_array() const;
    bool is_number() const;
    bool is_string() const;
    bool is_boolean() const;
    bool is_null() const;

    json(const json& r);
    json& operator=(const json& r);
    json(json&& r) noexcept;
    json& operator=(json&& r) noexcept;

    const json& operator[](size_t i) const;
    json& operator[](size_t i);
    const json& operator[](const std::string& i) const;
    json& operator[](const std::string& i);

    friend bool operator==(const json& l, const json& r);
    friend bool operator!=(const json& l, const json& r);

    static json parse(const std::string& s);
private:
    static void dispose_node(const jvalue* n);
    jvalue* _node;
};

}
