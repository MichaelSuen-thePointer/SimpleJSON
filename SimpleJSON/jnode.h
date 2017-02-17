#pragma once
#include <memory>
#include <string>

namespace mq
{
class json;

class jvalue
{
    friend json;
public:
    enum type
    {
        object = 1,
        array = 2,
        number = 4,
        string = 8,
        boolean = 16,
        null = 32
    };
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

    bool is_object() const { return type() == object; }
    bool is_array() const { return type() == array; }
    bool is_number() const { return type() == number; }
    bool is_string() const { return type() == string; }
    bool is_boolean() const { return type() == boolean; }
    bool is_null() const { return type() == null; }


    virtual bool equals_to(const jvalue*) const = 0;
private:
    static std::shared_ptr<jvalue> true_instance();
    static std::shared_ptr<jvalue> false_instance();
    static std::shared_ptr<jvalue> null_instance();
    template<enum type Type, class T>
    static std::shared_ptr<jvalue> get_instance(T&& obj);
    static std::shared_ptr<jvalue> get_instance();
};

template<enum jvalue::type Type, class T>
class json_value : public jvalue
{
    friend json;
    friend jvalue;
protected:
    explicit json_value(const T& v)
        : _value(v)
    {
    }
    explicit json_value(T&& v) noexcept
        : _value(v)
    {
    }
public:
    enum type type() const override
    {
        return Type;
    }

    bool equals_to(const jvalue* r) const override
    {
        auto real = dynamic_cast<const json_value*>(r);
        return real && real->_value == _value;
    }

protected:
    T _value;
};

template<>
class json_value<jvalue::null, void> : public jvalue
{
    friend json;
protected:
    json_value() {}
public:
    enum type type() const override
    {
        return null;
    }

    bool equals_to(const jvalue* r) const override
    {
        auto real = dynamic_cast<const json_value*>(r);
        return real;
    }
};

template <enum jvalue::type Type, class T>
std::shared_ptr<jvalue> jvalue::get_instance(T&& obj)
{
    using RmRefT = std::remove_reference_t<T>;
    struct make_shared_enabler : json_value<Type, RmRefT>
    {
        make_shared_enabler(RmRefT& v) : json_value<Type, RmRefT>(v) {}
        make_shared_enabler(RmRefT&& v) : json_value<Type, RmRefT>(std::move(v)) {}
    };
    return std::make_shared<make_shared_enabler>(std::forward<T>(obj));
}

}
