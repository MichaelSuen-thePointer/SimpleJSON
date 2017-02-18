#define BOOST_TEST_MODULE SimpileJSON Test
#define BOOST_TEST_DETECT_MEMORY_LEAK 1
#include <boost/test/included/unit_test.hpp>
#include "json.h"
#include "jparser.h"
using namespace mq;

BOOST_AUTO_TEST_CASE(json_ctor_dtor_test)
{
    json simple = 1;
    json copy = simple;
    json move = std::move(simple);

    json complex = json::object{{"key", "value"}};
    json ccopy = complex;
    json cmove = std::move(complex);

    ccopy = ccopy;
    ccopy = cmove;
    cmove = std::move(ccopy);
    cmove = std::move(cmove);
}

BOOST_AUTO_TEST_CASE(json_construction_and_readonly_test)
{
    json i = 1;
    BOOST_TEST(i.as_int() == 1);
    BOOST_TEST(i.as_double() == 1);

    json d = 1.2;
    BOOST_TEST(d.as_int() == 1);
    BOOST_TEST(d.as_double() == 1.2);

    json b = true;
    BOOST_TEST(b.as_bool());

    json nul;
    BOOST_TEST(nul.is_null());

    json str = "hello world";
    BOOST_TEST(str.as_string() == "hello world");

    json arr = json::array { 1, 1.2, true, json::null};
    BOOST_TEST(arr[0].as_int() == 1);
    BOOST_TEST(arr[1].as_double() == 1.2);
    BOOST_TEST(arr[2].as_bool());
    BOOST_TEST(arr[3].is_null());

    json obj = json::object{{"int", 1}, {"double", 2.3}, {"nul", json::null}, {"bool", false}};
    BOOST_TEST(obj["int"].as_int() == 1);
    BOOST_TEST(obj["double"].as_double() == 2.3);
    BOOST_TEST(obj["nul"].is_null());
    BOOST_TEST(obj["bool"].as_bool() == false);
}

BOOST_AUTO_TEST_CASE(json_copy_and_equality_test)
{
    json arr = json::array{json::object{
        {"int", 2},
        {"double", 1.2},
        {"arr", json::array{true, false, 1.2, json::null, "hello"}}
    }, 4.4, false, json::null};

    json copy = arr;
    BOOST_TEST((copy[0]["int"] == 2));
    BOOST_TEST((copy[0]["double"] == 1.2));
    BOOST_TEST((copy[0]["arr"][0] == true));
    BOOST_TEST((copy[0]["arr"][1] == false));
    BOOST_TEST((copy[0]["arr"][2] == 1.2));
    BOOST_TEST((copy[0]["arr"][3] == nullptr));
    BOOST_TEST((copy[0]["arr"][4] == "hello"));
    BOOST_TEST((copy[1] == 4.4));
    BOOST_TEST((copy[2] == false));
    BOOST_TEST((copy[3] == nullptr));
}

BOOST_AUTO_TEST_CASE(json_edit_test)
{
    json doc;
    BOOST_TEST((doc == nullptr));

    doc = json::array{1, 2, 3, 4, 5}; //reset null to array
    auto& arr = doc.as_array();
    for (int i = 0; i < 5; i++)
    {
        BOOST_TEST((arr[i] == i + 1));
    }

    doc[0] = json::object{{"int", 1},{"str", "2"}}; //reset int to object
    auto& obj = doc[0].as_object();
    BOOST_TEST((obj.find("int")->second == 1));
    BOOST_TEST((obj.find("str")->second == "2"));

    doc[0]["int"] = nullptr; //reset object to null
    BOOST_TEST((doc[0]["int"] == nullptr));

    doc[0]["not_exist_key"] = 2.3; //if key is not exist, add the key
    BOOST_TEST((doc[0]["not_exist_key"] == 2.3));

    doc[9] = 2.4; //if index is not exist, expand the array and fill the new entry with null
    for (int i = 5; i < 9; i++)
    {
        BOOST_TEST((doc[i] == nullptr));
    }
    BOOST_TEST((doc[9] == 2.4));

    doc[5]["new"] = 1; //use [] on a non-object/non-array term will convert it to the corresponding object
    BOOST_TEST((doc[5].is_object()));
    BOOST_TEST((doc[5]["new"] == 1));

    doc[5][1] = 2;
    BOOST_TEST((doc[5].is_array()));
    BOOST_TEST((doc[5][1] == 2));

    BOOST_TEST((doc[111] == nullptr)); //new initialized value is null
    BOOST_TEST((doc["obj"] == nullptr));

    doc = nullptr;
}

BOOST_AUTO_TEST_CASE(json_string_escape_sequence_test)
{
    std::string err;
    auto _1 = jparser::parse(R"("")", err);
    BOOST_TEST((_1.as_string() == ""));
    BOOST_TEST((err == ""));

    auto _2 = jparser::parse(R"("\" \\ \/ \b \f \n \r \t")", err);
    BOOST_TEST((_2.as_string() == "\" \\ / \b \f \n \r \t"));
    BOOST_TEST((err == ""));

    auto _3 = jparser::parse(R"("\u0024 \u20AC \uD801\uDC37 \uD852\uDF62")");
    BOOST_TEST((_3.as_string() == "\x24 \xe2\x82\xac \xf0\x90\x90\xb7 \xf0\xa4\xad\xa2"));
    BOOST_TEST((err == ""));
}

BOOST_AUTO_TEST_CASE(json_number_parse_test)
{
    auto _1 = jparser::parse("1.4");
    BOOST_TEST((_1 == 1.4));

    auto _2 = jparser::parse("-1.4");
    BOOST_TEST((_2 == -1.4));

    auto _3 = jparser::parse("-0");
    BOOST_TEST((_3 == 0));

    auto _4 = jparser::parse("0");
    BOOST_TEST((_4 == 0));

    auto _5 = jparser::parse("0.4");
    BOOST_TEST((_5 == 0.4));

    auto _6 = jparser::parse("-0.4");
    BOOST_TEST((_6 == -0.4));

    auto _7 = jparser::parse("-23.4e+10");
    BOOST_TEST((_7 == -23.4e+10));

    auto _8 = jparser::parse("0.1e-5");
    BOOST_TEST((_8 == 0.1E-5));

    auto _9 = jparser::parse("0.1e5");
    BOOST_TEST((_9 == 0.1e5));
}

BOOST_AUTO_TEST_CASE(json_big_number_test)
{
    auto j = jparser::parse("100000000000000000000000000"); //1e26 fallback to double
    BOOST_TEST((j == 1e26));
}

BOOST_AUTO_TEST_CASE(json_parser_test)
{
    auto _1 = jparser::parse(R"(
{
    "int" : 1,
    "double" : 1.2,
    "string" : "str123",
    "arr" : [1, 2.2, true, false, null, {}, []]
}
)");

    BOOST_TEST((_1["int"] == 1));
    BOOST_TEST((_1["double"] == 1.2));
    BOOST_TEST((_1["string"] == "str123"));
    BOOST_TEST((_1["arr"].is_array()));
    BOOST_TEST((_1["arr"][0] == 1));
    BOOST_TEST((_1["arr"][1] == 2.2));
    BOOST_TEST((_1["arr"][2] == true));
    BOOST_TEST((_1["arr"][3] == false));
    BOOST_TEST((_1["arr"][4] == nullptr));
    BOOST_TEST((_1["arr"][5].is_object()));
    BOOST_TEST((_1["arr"][6].is_array()));

}