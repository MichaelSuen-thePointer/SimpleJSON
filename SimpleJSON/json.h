#pragma once

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

private:
    static void dispose_node(const jvalue* n);
    jvalue* _node;
};

class jvalue
{
    friend json;
public:
    virtual ~jvalue() = default;

    jvalue() = default;
    jvalue(const jvalue&) = delete;
    jvalue& operator=(const jvalue&) = delete;
    jvalue(jvalue&&) = delete;
    jvalue& operator=(jvalue&&) = delete;

    virtual json::type type() const = 0;
    const json& get_value_unsafe(const std::string& key) const;
    const json& get_value_unsafe(size_t i) const;
    bool get_bool_unsafe() const;
    int get_int_unsafe() const;
    const std::string& get_string_unsafe() const;
    const json::object& get_object_unsafe() const;
    const json::array& get_array_unsafe() const;
    double get_double_unsafe() const;

    virtual jvalue* clone() = 0;
    virtual bool equals_to_unsafe(const jvalue* r) const = 0;

    static jvalue* null_instance();
    static jvalue* true_instance();
    static jvalue* false_instance();
    static jvalue* int_instance(int i);
    static jvalue* double_instance(double d);
    static jvalue* string_instance(const std::string& s);
    static jvalue* string_instance(std::string&& s);
    static jvalue* object_instance(const json::object& s);
    static jvalue* object_instance(json::object&& s);
    static jvalue* array_instance(const json::array& s);
    static jvalue* array_instance(json::array&& s);
};

void cleanup();

}
