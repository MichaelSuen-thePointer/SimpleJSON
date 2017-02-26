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

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <mutex>
#include <memory>

namespace mq
{

class jvalue;

class json_flat_deleter
{
public:
    void operator()(const jvalue* v) const noexcept;

    static void start_delete();
private:
    static bool is_started;
    static std::vector<const jvalue*> deferred_pool;
};

class json
{
private:
    using base = std::shared_ptr<jvalue>;
    json(jvalue* v);
    jvalue* get() const;
    std::shared_ptr<jvalue> _node;
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
    json(int64_t d);
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
    int64_t as_int() const;
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

    json(const json& r) = default;
    json& operator=(const json& r) = default;
    json(json&& r) noexcept = default;
    json& operator=(json&& r) noexcept = default;

    const json& operator[](size_t i) const;
    json& operator[](size_t i);
    const json& operator[](const std::string& i) const;
    json& operator[](const std::string& i);

    friend bool operator==(const json& l, const json& r);
    friend bool operator!=(const json& l, const json& r);

    static json parse(const std::string& s);
};

}
