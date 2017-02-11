#include "jnode.h"

namespace mq
{
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
