#include "gtest/gtest.h"
#include "monitor_handler.h"

TEST(LMHandlerTest, handle_single) {
    LMConfig conf;
    conf._is_stat = true;
    LMHandler lm_handler(conf);
    int ret = lm_handler.handle_single("aaa");
    sleep(1);
    ret = lm_handler.handle_single("aaa");
    ASSERT_EQ(0, ret);
}
