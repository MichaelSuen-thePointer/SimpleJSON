#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define BOOST_TEST_MODULE SimpileJSON Test
#define BOOST_TEST_DETECT_MEMORY_LEAK 1
#include <boost/test/included/unit_test.hpp>
#include "json.h"
using namespace mq;

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