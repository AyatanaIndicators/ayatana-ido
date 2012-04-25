
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
};

TEST_F(TestMenuitems, BuildCalendar) {
	GtkWidget * cal = ido_calendar_menu_item_new();

	EXPECT_TRUE(cal != NULL);
	EXPECT_TRUE(IDO_IS_CALENDAR_MENU_ITEM(cal));
	EXPECT_TRUE(GTK_IS_MENU_ITEM(cal));

	GtkWidget * menu = gtk_menu_new();
	gtk_widget_show(menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), cal);

	gtk_widget_show(cal);
	gtk_widget_realize(cal);

	EXPECT_TRUE(gtk_widget_get_realized(cal));

	g_object_ref_sink(menu);
	g_object_unref(menu);
	return;
}

TEST_F(TestMenuitems, BuildEntry) {
	GtkWidget * entry = ido_entry_menu_item_new();

	EXPECT_TRUE(entry != NULL);
	EXPECT_TRUE(IDO_IS_ENTRY_MENU_ITEM(entry));
	EXPECT_TRUE(GTK_IS_MENU_ITEM(entry));

	GtkWidget * menu = gtk_menu_new();
	gtk_widget_show(menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), entry);

	gtk_widget_show(entry);
	gtk_widget_realize(entry);

	EXPECT_TRUE(gtk_widget_get_realized(entry));

	g_object_ref_sink(menu);
	g_object_unref(menu);
	return;
}

