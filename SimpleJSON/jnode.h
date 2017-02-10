#pragma once
#include <map>
#include <vector>
#include <memory>

class json_object;
class json_array;
class json_number;
class json_bool;
class json_string;

class json_value
{
public:
    virtual ~json_value() = default;

    virtual std::string to_string() const = 0;
    virtual json_object* as_object();
    virtual json_array* as_array();
    virtual json_number* as_number();
    virtual json_string* as_string();
    virtual json_bool* as_bool();
    virtual bool is_null();

    virtual json_value* clone() = 0;
};

class json_object : public json_value
{
public:
    json_object() = default;
    json_object(const json_object& rhs);
    json_object(json_object&& rhs) noexcept;
    json_object& operator=(const json_object& rhs);
    json_object& operator=(json_object&& rhs) noexcept;

    std::string to_string() const override;

    json_value* operator[](const std::string& key);
    const json_value* operator[](const std::string& key) const;

    void add(const std::string& key, std::unique_ptr<json_value>&& value);
    void remove(const std::string& key);
    size_t size();

    json_object* clone() override;
private:
    std::map<std::string, std::unique_ptr<json_value>> _values;
};

class json_array : public json_value
{
public:
    json_array() = default;
    json_array(const json_array& rhs);
    json_array(json_array&& rhs) noexcept;
    json_array& operator=(const json_array& rhs);
    json_array& operator=(json_array&& rhs) noexcept;

    std::string to_string() const override;

    json_value* operator[](size_t i);
    const json_value* operator[](size_t i) const;

    void add(std::unique_ptr<json_value>&& value);
    void remove(size_t i);
    size_t size();

    json_array* clone() override;
private:
    std::vector<std::unique_ptr<json_value>> _values;
};

class json_number : public json_value
{
public:
    json_number(double v);
    json_number(const json_number&) = default;


    std::string to_string() const override;

    double value();
    void value(double val);

    json_number* clone() override;
private:
    double _value = 0; //not accurate
};

class json_string : public json_value
{
public:
    json_string(const std::string& v);

    std::string to_string() const override;

    const std::string& value() const;
    void value(std::string&& val);

    json_string* clone() override;
private:
    std::string _value;
};

class json_bool : public json_value
{
public:
    explicit json_bool(bool b);

    std::string to_string() const override;

    bool value();
    void value(bool val);

    json_string* clone() override;
private:
    bool _value = false;
};

class json_null : public json_value
{
public:
    std::string to_string() const override;
};
