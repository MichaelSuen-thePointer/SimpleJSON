#include "json.h"
#include "jerror.h"
namespace mq
{
json::json()
    : json(nullptr)
{
}

json::json(std::nullptr_t)
    : _node(jvalue::null_instance())
{
}

json::json(int d)
    : _node(jvalue::int_instance(d))
{
}

json::json(double d)
    : _node(jvalue::double_instance(d))
{
}

json::json(const std::string& s)
    : _node(jvalue::string_instance(s))
{
}

json::json(std::string&& s)
    : _node(jvalue::string_instance(std::move(s)))
{
}

json::json(const char* s)
    : _node(jvalue::string_instance(s))
{
}

json::json(const object& r)
    : _node(jvalue::object_instance(r))
{
}

json::json(object&& r)
    : _node(jvalue::object_instance(std::move(r)))
{
}

json::json(const array& r)
    : _node(jvalue::array_instance(r))
{
}

json::json(array&& r)
    : _node(jvalue::array_instance(std::move(r)))
{
}

json::json(bool b)
    : _node(b ? jvalue::true_instance() : jvalue::false_instance())
{
}

json::type json::value_type() const
{
    return _node->type();
}

bool json::as_bool() const
{
    if (value_type() != type::boolean)
    {
        throw bad_json_cast{};
    }
    return _node->as_bool_unsafe();
}

int json::as_int() const
{
    
}

double json::as_double() const
{
}

const std::string& json::as_string() const
{
    if (value_type() != type::string)
    {
        throw bad_json_cast{};
    }
    return _node->as_string_unsafe();
}

json::json(const json& r)
    : _node(r._node)
{
}

json& json::operator=(const json& r)
{
    if (this != std::addressof(r))
    {
        _node = r._node;
    }
    return *this;
}

json::json(json&& r) noexcept
    : _node(std::move(r._node))
{
}

json& json::operator=(json&& r) noexcept
{
    if (this != std::addressof(r))
    {
        _node = std::move(r._node);
    }
    return *this;
}

const json& json::operator[](const std::string& i) const
{
    if (_node->is_object())
    {
        auto r = _node->get_value_unsafe(i);
        if (r)
        {
            return *r;
        }
    }
    return null;
}

const json& json::operator[](size_t i) const
{
    if(_node->is_array())
    {
        auto r = _node->get_value_unsafe(i);
        if (r)
        {
            return *r;
        }
    }
    return null;
}

bool operator==(const json& l, const json& r)
{
    return l._node->equals_to(r._node.get());
}

bool operator!=(const json& l, const json& r)
{
    return !(l == r);
}

json json::null{};
}
