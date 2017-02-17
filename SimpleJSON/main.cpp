#define BOOST_TEST_MODULE SimpileJSON Test
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