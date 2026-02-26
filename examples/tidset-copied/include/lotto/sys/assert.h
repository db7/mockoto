#ifndef LOTTO_ASSERT_H
#define LOTTO_ASSERT_H

#include <lotto/sys/abort.h>

#undef ASSERT
#define ASSERT(cond)                                                           \
    do {                                                                       \
        if (!(cond))                                                           \
            sys_assert_fail(#cond, __FILE__, (unsigned int)__LINE__,           \
                            __FUNCTION__);                                     \
    } while (0)

#endif
