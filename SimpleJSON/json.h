#pragma once
#include <map>
#include <string>
#include <vector>
#include "jnode.h"
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

    bool as_bool() const;
    bool& as_bool();
    int as_int() const;
    int& as_int();
    double as_double() const;
    double& as_double();
    const std::string& as_string() const;
    std::string& as_string();

    bool is_object() const { return type() == json::OBJECT; }
    bool is_array() const { return type() == json::ARRAY; }
    bool is_number() const { return type() == json::NUMBER; }
    bool is_string() const { return type() == json::STRING; }
    bool is_boolean() const { return type() == json::BOOLEAN; }
    bool is_null() const { return type() == json::NUL; }

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

    virtual enum type type() const = 0;
    const json* get_value_unsafe(const std::string& key) const;
    const json* get_value_unsafe(size_t i) const;
    bool as_bool_unsafe() const;
    int as_int_unsafe() const;
    const std::string& as_string_unsafe() const;
    double as_double_unsafe() const;


    virtual bool equals_to(const jvalue*) const = 0;

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

}
