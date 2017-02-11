#pragma once

#include "jnode.h"

class jparser
{
public:
    
private:
    jparser();

    [[noreturn]]
    static void raise_parse_error();
};