#define BOOST_TEST_MODULE SimpileJSON Test
#include <boost/test/included/unit_test.hpp>
#include "json.h"
using namespace mq;

BOOST_AUTO_TEST_CASE(json_value_test)
{
    json boolean = true;
    BOOST_TEST(boolean.as)
}