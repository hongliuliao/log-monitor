#include "gtest/gtest.h"
#include "monitor_handler.h"

TEST(StdInHandlerTest, handle_single) {
    LMConfig conf;
    conf._is_stat = true;
    StdInHandler lm_handler(conf);
    time_t now = time(NULL);
    int ret = lm_handler.handle_single("aaa", now);
    ret = lm_handler.handle_single("aaa", now + 1);
    ASSERT_EQ(0, ret);
}

TEST(StdInHandlerTest, handle_single_num) {
    LMConfig conf;
    conf._is_stat = true;
    StdInHandler lm_handler(conf);
    time_t now = time(NULL);
    int ret = lm_handler.handle_single("123", now);
    ret = lm_handler.handle_single("899", now);
    ret = lm_handler.handle_single("456", now + 1);
    ASSERT_EQ(0, ret);
}
