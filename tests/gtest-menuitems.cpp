
#include <gtk/gtk.h>
#include <gtest/gtest.h>
#include "idocalendarmenuitem.h"

class TestMenuitems : public ::testing::Test
{
public:
	TestMenuitems()
	{
		g_type_init();
		return;
	}
};

TEST_F(TestMenuitems, BuildCalendar) {
	GtkWidget * cal = ido_calendar_menu_item_new();

	EXPECT_TRUE(cal != NULL);
	EXPECT_TRUE(IDO_IS_CALENDAR_MENU_ITEM(cal));
	EXPECT_TRUE(GTK_IS_MENU_ITEM(cal));

	g_object_ref_sink(cal);
	g_object_unref(cal);
	return;
}
