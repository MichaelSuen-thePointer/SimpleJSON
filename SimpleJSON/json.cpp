#include "json.h"
#include "jerror.h"
#include <cassert>
#include "jparser.h"

namespace mq
{

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

//TODO: make it thread-safe
class json_flat_deleter
{
public:
    static json_flat_deleter& instance()
    {
        static json_flat_deleter instance{};
        return instance;
    }
    json_flat_deleter(const json_flat_deleter&) = delete;
    json_flat_deleter& operator=(const json_flat_deleter&) = delete;
    json_flat_deleter(json_flat_deleter&&) = delete;
    json_flat_deleter& operator=(json_flat_deleter&&) = delete;
    void add_value(const jvalue* v)
    {
#ifdef _DEBUG
        if (std::find(jvalues.begin(), jvalues.end(), v) != jvalues.end())
        {
            assert(0);
        }
#endif
        jvalues.push_back(v);
        if (jvalues.size() > 16)
        {
            start_delete();
        }
    }
    void start_delete()
    {
        if (is_started)
        {
            return;
        }
        is_started = true;
        size_t i = 0;
        while (i != jvalues.size())
        {
            delete jvalues[i];
            i++;
        }
        jvalues.clear();
        is_started = false;
    }
    ~json_flat_deleter()
    {
        start_delete();
    }
private:
    json_flat_deleter() {}
    bool is_started = false;
    std::vector<const jvalue*> jvalues;
};

class jnumber : public jvalue
{
public:
    friend class json;
    friend class jvalue;
    json::type type() const override
    {
        return json::NUMBER;
    }
    virtual int get_int() const = 0;
    virtual double get_double() const = 0;
    bool equals_to_unsafe(const jvalue* r) const override
    {
        assert(reinterpret_cast<const jnumber*>(r) != nullptr);
        auto right = static_cast<const jnumber*>(r);
        return this->get_double() == right->get_double();
    }
};

class jint : public jnumber
{
public:
    friend class json;
    friend class jvalue;
    jint(int i) : _v(i) {}

    jvalue* clone() override
    {
        return new jint(_v);
    }
    int get_int() const override
    {
        return _v;
    }
    double get_double() const override
    {
        return _v;
    }
private:
    int _v;
};

class jdouble : public jnumber
{
public:
    friend class json;
    friend class jvalue;
    jdouble(double i) : _v(i) {}

    jvalue* clone() override
    {
        return new jdouble(_v);
    }
    int get_int() const override
    {
        return static_cast<int>(_v);
    }
    double get_double() const override
    {
        return _v;
    }
private:
    double _v;
};

class jboolean : public jvalue
{
public:
    friend class json;
    friend class jvalue;
    jboolean(bool b) : _v(b) {}

    json::type type() const override
    {
        return json::BOOLEAN;
    }
    jvalue* clone() override
    {
        return _v ? true_instance() : false_instance();
    }
    bool equals_to_unsafe(const jvalue* r) const override
    {
        assert(reinterpret_cast<decltype(this)>(r) != nullptr);
        return _v == static_cast<decltype(this)>(r)->_v;
    }
private:
    bool _v;
};

class jstring : public jvalue
{
public:
    friend class json;
    friend class jvalue;
    jstring(const std::string& s) : _v(s) {}
    jstring(std::string&& s) : _v(std::move(s)) {}

    json::type type() const override
    {
        return json::STRING;
    }
    jvalue* clone() override
    {
        return new jstring(_v);
    }
    bool equals_to_unsafe(const jvalue* r) const override
    {
        assert(reinterpret_cast<decltype(this)>(r) != nullptr);
        return _v == static_cast<decltype(this)>(r)->_v;
    }
private:
    std::string _v;
};

class jobject : public jvalue
{
public:
    friend class json;
    friend class jvalue;
    jobject(const json::object& s) : _v(s) {}
    jobject(json::object&& s) : _v(std::move(s)) {}
    json::type type() const override
    {
        return json::OBJECT;
    }
    jvalue* clone() override
    {
        return new jobject(_v);
    }
    bool equals_to_unsafe(const jvalue* r) const override
    {
        assert(reinterpret_cast<decltype(this)>(r) != nullptr);
        return _v == static_cast<decltype(this)>(r)->_v;
    }
private:
    json::object _v;
};

class jarray : public jvalue
{
public:
    friend class json;
    friend class jvalue;
    jarray(const json::array& s) : _v(s) {}
    jarray(json::array&& s) : _v(std::move(s)) {}

    json::type type() const override
    {
        return json::ARRAY;
    }
    jvalue* clone() override
    {
        return new jarray(_v);
    }
    bool equals_to_unsafe(const jvalue* r) const override
    {
        assert(reinterpret_cast<decltype(this)>(r) != nullptr);
        return _v == static_cast<decltype(this)>(r)->_v;
    }
private:
    json::array _v;
};

class jnull : public jvalue
{
public:
    json::type type() const override
    {
        return json::NUL;
    }
    jvalue* clone() override
    {
        return this;
    }
    bool equals_to_unsafe(const jvalue* r) const override
    {
        return this == r; //there should only 1 null instance
    }
};


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

json::~json()
{
    dispose_node(_node);
}

bool json::as_bool() const
{
    if (value_type() != BOOLEAN)
    {
        return false;
    }
    return _node->get_bool_unsafe();
}

int json::as_int() const
{
    if (value_type() != NUMBER)
    {
        return 0;
    }
    return _node->get_int_unsafe();
}

double json::as_double() const
{
    if (value_type() != NUMBER)
    {
        return 0;
    }
    return _node->get_double_unsafe();
}

const std::string& json::as_string() const
{
    static std::string empty;
    if (value_type() != STRING)
    {
        return empty;
    }
    return _node->get_string_unsafe();
}

const json::object& json::as_object() const
{
    static object empty;
    if (value_type() != OBJECT)
    {
        return empty;
    }
    return _node->get_object_unsafe();
}

const json::array& json::as_array() const
{
    static array empty;
    if (value_type() != ARRAY)
    {
        return empty;
    }
    return _node->get_array_unsafe();
}

json::type json::value_type() const
{
    return _node->type();
}

bool json::is_object() const
{
    return _node->type() == json::OBJECT;
}

bool json::is_array() const
{
    return _node->type() == json::ARRAY;
}

bool json::is_number() const
{
    return _node->type() == json::NUMBER;
}

bool json::is_string() const
{
    return _node->type() == json::STRING;
}

bool json::is_boolean() const
{
    return _node->type() == json::BOOLEAN;
}

bool json::is_null() const
{
    return _node->type() == json::NUL;
}

json::json(const json& r)
    : _node(r._node->clone())
{
}

json& json::operator=(const json& r)
{
    if (this != std::addressof(r))
    {
        dispose_node(_node);
        _node = r._node->clone();
    }
    return *this;
}

json::json(json&& r) noexcept
    : _node(r._node)
{
    r._node = nullptr;
}

json& json::operator=(json&& r) noexcept
{
    if (this != std::addressof(r))
    {
        dispose_node(_node);
        _node = r._node;
        r._node = nullptr;
    }
    return *this;
}

const json& json::operator[](const std::string& i) const
{
    if (value_type() == OBJECT)
    {
        return _node->get_value_unsafe(i);
    }
    return null;
}

json& json::operator[](const std::string& i)
{
    if (value_type() != OBJECT)
    {
        *this = object{};
    }
    return static_cast<jobject*>(_node)->_v[i];
}

json json::parse(const std::string& s)
{
    auto buf = s.c_str();
    auto p = buf;
    return jparser::parse_value(p);
}

void json::dispose_node(const jvalue* n)
{
    if (n == nullptr)
    {
        return;
    }
    switch (n->type())
    {
    case OBJECT:
    case ARRAY:
    case STRING:
    case NUMBER:
        json_flat_deleter::instance().add_value(n);
    default:;
    }
}

const json& jvalue::get_value_unsafe(const std::string& key) const
{
    assert(reinterpret_cast<const jobject*>(this) != nullptr);
    auto& obj = static_cast<const jobject*>(this)->_v;
    auto res = obj.find(key);
    if (res == obj.end())
    {
        return json::null;
    }
    return res->second;
}

const json& jvalue::get_value_unsafe(size_t i) const
{
    assert(reinterpret_cast<const jarray*>(this) != nullptr);
    auto& arr = static_cast<const jarray*>(this)->_v;
    if (i < arr.size())
    {
        return arr[i];
    }
    return json::null;
}

bool jvalue::get_bool_unsafe() const
{
    assert(reinterpret_cast<const jboolean*>(this) != nullptr);
    return static_cast<const jboolean*>(this)->_v;
}

int jvalue::get_int_unsafe() const
{
    assert(reinterpret_cast<const jnumber*>(this) != nullptr);
    return static_cast<const jnumber*>(this)->get_int();
}

const std::string& jvalue::get_string_unsafe() const
{
    assert(reinterpret_cast<const jstring*>(this) != nullptr);
    return static_cast<const jstring*>(this)->_v;
}

const json::object& jvalue::get_object_unsafe() const
{
    assert(reinterpret_cast<const jobject*>(this) != nullptr);
    return static_cast<const jobject*>(this)->_v;
}

const json::array& jvalue::get_array_unsafe() const
{
    assert(reinterpret_cast<const jarray*>(this) != nullptr);
    return static_cast<const jarray*>(this)->_v;
}

double jvalue::get_double_unsafe() const
{
    assert(reinterpret_cast<const jnumber*>(this) != nullptr);
    return static_cast<const jnumber*>(this)->get_double();
}

jvalue* jvalue::null_instance()
{
    static jnull instance;
    return &instance;
}

jvalue* jvalue::true_instance()
{
    static jboolean instance{true};
    return &instance;
}

jvalue* jvalue::false_instance()
{
    static jboolean instance{false};
    return &instance;
}

jvalue* jvalue::int_instance(int i)
{
    return new jint(i);
}

jvalue* jvalue::double_instance(double d)
{
    return new jdouble(d);
}

jvalue* jvalue::string_instance(const std::string& s)
{
    return new jstring(s);
}

jvalue* jvalue::string_instance(std::string&& s)
{
    return new jstring(std::move(s));
}

jvalue* jvalue::object_instance(const json::object& s)
{
    return new jobject(s);
}

jvalue* jvalue::object_instance(json::object&& s)
{
    return new jobject(std::move(s));
}

jvalue* jvalue::array_instance(const json::array& s)
{
    return new jarray(s);
}

jvalue* jvalue::array_instance(json::array&& s)
{
    return new jarray(std::move(s));
}

void cleanup()
{
    json_flat_deleter::instance().start_delete();
}

const json& json::operator[](size_t i) const
{
    if (value_type() == ARRAY)
    {
        return _node->get_value_unsafe(i);
    }
    return null;
}

json& json::operator[](size_t i)
{
    if (value_type() == ARRAY)
    {
        auto& arr = static_cast<jarray*>(_node)->_v;
        if (arr.size() > i)
        {
            return arr[i];
        }
        arr.insert(arr.end(), i - arr.size() + 1, json{});
        return arr[i];
    }
    (*this) = array(i + 1);
    return static_cast<jarray*>(_node)->_v[i];
}

bool operator==(const json& l, const json& r)
{
    return l._node->type() == r._node->type() && l._node->equals_to_unsafe(r._node);
}

bool operator!=(const json& l, const json& r)
{
    return !(l == r);
}

json json::null{};



}
