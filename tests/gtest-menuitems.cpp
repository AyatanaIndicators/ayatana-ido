
#include <glib-object.h>
#include <gtest/gtest.h>

TEST(StartTesting, DummyTest) {
	g_type_init();
	EXPECT_TRUE(true);
}
