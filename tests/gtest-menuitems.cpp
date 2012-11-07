
#include <gtk/gtk.h>
#include <gtest/gtest.h>
#include "idocalendarmenuitem.h"
#include "idoentrymenuitem.h"
#include "idoscalemenuitem.h"

class TestMenuitems : public ::testing::Test
{
public:
	TestMenuitems()
	{
		gint argc = 0;
		gchar * argv[] = {NULL};
		gtk_init(&argc, (gchar ***)&argv);
		return;
	}

protected:
	void PutInMenu (GtkWidget * item)
	{
		GtkWidget * menu = gtk_menu_new();
		gtk_widget_show(menu);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

		gtk_widget_show(item);
		gtk_widget_realize(item);

		EXPECT_TRUE(gtk_widget_get_realized(item));

		g_object_ref_sink(menu);
		g_object_unref(menu);
	}
};

TEST_F(TestMenuitems, BuildCalendar) {
	GtkWidget * cal = ido_calendar_menu_item_new();

	EXPECT_TRUE(cal != NULL);
	EXPECT_TRUE(IDO_IS_CALENDAR_MENU_ITEM(cal));
	EXPECT_TRUE(GTK_IS_MENU_ITEM(cal));

	PutInMenu (cal);
	return;
}

TEST_F(TestMenuitems, BuildEntry) {
	GtkWidget * entry = ido_entry_menu_item_new();

	EXPECT_TRUE(entry != NULL);
	EXPECT_TRUE(IDO_IS_ENTRY_MENU_ITEM(entry));
	EXPECT_TRUE(GTK_IS_MENU_ITEM(entry));

	PutInMenu (entry);
	return;
}

TEST_F(TestMenuitems, BuildScaleDefault) {
	GtkWidget * scale = ido_scale_menu_item_new("Label", IDO_RANGE_STYLE_DEFAULT, gtk_adjustment_new(0.5, 0.0, 1.0, 0.01, 0.1, 0.1));

	EXPECT_TRUE(scale != NULL);
	EXPECT_TRUE(IDO_IS_SCALE_MENU_ITEM(scale));
	EXPECT_TRUE(GTK_IS_MENU_ITEM(scale));

	PutInMenu (scale);
	return;
}

TEST_F(TestMenuitems, BuildScaleSmall) {
	GtkWidget * scale = ido_scale_menu_item_new("Label", IDO_RANGE_STYLE_SMALL, gtk_adjustment_new(0.5, 0.0, 1.0, 0.01, 0.1, 0.1));

	EXPECT_TRUE(scale != NULL);
	EXPECT_TRUE(IDO_IS_SCALE_MENU_ITEM(scale));
	EXPECT_TRUE(GTK_IS_MENU_ITEM(scale));

	PutInMenu (scale);
	return;
}
