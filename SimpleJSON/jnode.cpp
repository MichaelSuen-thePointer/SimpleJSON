#include "jnode.h"
#include <vector>
#include <map>
#include <cassert>
namespace mq
{
const json* jvalue::get_value_unsafe(const std::string& key) const
{
    assert(dynamic_cast<const json_value<object, std::map<std::string, json>>*>(this) != nullptr);
    auto derived = static_cast<const json_value<object, std::map<std::string, json>>*>(this);
    auto res = derived->_value.find(key);
    if (res == derived->_value.end())
    {
        return nullptr;
    }
    return &(res->second);
}

const json* jvalue::get_value_unsafe(size_t i) const
{
    assert(dynamic_cast<const json_value<array, std::vector<json>>*>(this) != nullptr);
    auto derived = static_cast<const json_value<array, std::vector<json>>*>(this);
    if (derived->_value.size() > i)
    {
        return &(derived->_value[i]);
    }
    return nullptr;
}

bool jvalue::as_bool_unsafe() const
{
    assert(dynamic_cast<const json_value<boolean, bool>*>(this) != nullptr);
    auto derived = static_cast<const json_value<boolean, bool>*>(this);
    return derived->_value;
}

int jvalue::as_int_unsafe() const
{
    assert(dynamic_cast<const json_value<number, int>*>(this) != nullptr);
    auto derived = static_cast<const json_value<number, int>*>(this);
    return derived->_value;
}

const std::string& jvalue::as_string_unsafe() const
{
    assert(dynamic_cast<const json_value<string, std::string>*>(this) != nullptr);
    auto derived = static_cast<const json_value<string, std::string>*>(this);
    return derived->_value;
}

double jvalue::as_double_unsafe() const
{
    assert(dynamic_cast<const json_value<number, double>*>(this) != nullptr);
    auto derived = static_cast<const json_value<number, double>*>(this);
    return derived->_value;
}

std::shared_ptr<jvalue> jvalue::true_instance()
{
    static auto i = get_instance<boolean, bool>(true);
    return i;
}

std::shared_ptr<jvalue> jvalue::false_instance()
{
    static auto i = get_instance<boolean, bool>(false);
    return i;
}

std::shared_ptr<jvalue> jvalue::null_instance()
{
    static auto i = get_instance();
    return i;
}

std::shared_ptr<jvalue> jvalue::get_instance()
{
    struct make_shared_enabler : json_value<null, void> {};
    return std::make_shared<make_shared_enabler>();
}
}
