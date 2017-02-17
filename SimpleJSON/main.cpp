#define BOOST_TEST_MODULE SimpileJSON Test
#define BOOST_TEST_DETECT_MEMORY_LEAK 1
#include <boost/test/included/unit_test.hpp>
#include "json.h"
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
    BOOST_TEST((obj.find("int")->second == 0));
    BOOST_TEST((obj.find("str")->second == "2"));

    doc[0]["int"] = nullptr; //reset object to null
    BOOST_TEST((doc[0]["int"] == nullptr));

    doc[0]["not_exist_key"] = 2.3; //if key is not exist, add the key
    BOOST_TEST((doc[0]["not_exist_key"] == 2.3));

    doc[9] = 2.4; //if index is not exist, expand the array and fill the new entry with null
    for(int i = 5; i < 9; i++)
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
    cleanup();
}