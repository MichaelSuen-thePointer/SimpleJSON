#include "json.h"
#include <string>

int main()
{
    auto i = make_value(10);
    auto l = make_value(1l);
    auto d = make_value(1.0);
    auto s = make_value("str");
    auto b = make_value(true);

    auto arr = make_array(1, 1.0, "str", true);
    using namespace std::string_literals;
    auto obj = make_object({"1"s, 10}, {"2"s, 1.5}, {"3"s, "3.14"});
}
