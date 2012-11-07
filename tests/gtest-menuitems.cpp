
#include <gtk/gtk.h>
#include <gtest/gtest.h>
#include "idocalendarmenuitem.h"
#include "idoentrymenuitem.h"
#include "idoscalemenuitem.h"
#include "idoswitchmenuitem.h"

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
	IdoCalendarMenuItem * c = IDO_CALENDAR_MENU_ITEM (cal);

	EXPECT_TRUE(cal != NULL);
	EXPECT_TRUE(IDO_IS_CALENDAR_MENU_ITEM(cal));
	EXPECT_TRUE(GTK_IS_MENU_ITEM(cal));

	const guint year_in = 1963;
	const guint month_in = 10;
	const guint day_in = 23;
	ido_calendar_menu_item_set_date (c, year_in, month_in, day_in);
	guint year_out = 0;
	guint month_out = 0;
	guint day_out = 0;
	ido_calendar_menu_item_get_date (c, &year_out, &month_out, &day_out);
	ASSERT_EQ (year_in, year_out);
	ASSERT_EQ (month_in, month_out);
	ASSERT_EQ (day_in, day_out);

	const GtkCalendarDisplayOptions options_in = GTK_CALENDAR_SHOW_DAY_NAMES;
	ido_calendar_menu_item_set_display_options (c, options_in);
	const GtkCalendarDisplayOptions options_out = ido_calendar_menu_item_get_display_options (c);
	ASSERT_EQ (options_in, options_out);

	GtkWidget * w;
	w = ido_calendar_menu_item_get_calendar (c);
	ASSERT_TRUE (w != NULL);
	ASSERT_TRUE (GTK_IS_CALENDAR (w));

	// test clear/mark/unmark days
	ido_calendar_menu_item_clear_marks (c);
	ido_calendar_menu_item_mark_day (c, 0);
	ido_calendar_menu_item_mark_day (c, 1);
	ido_calendar_menu_item_mark_day (c, 2);
	ido_calendar_menu_item_unmark_day (c, 0);
	ido_calendar_menu_item_unmark_day (c, 2);
	for (int i=0; i<28; i++)
		ASSERT_EQ (gtk_calendar_get_day_is_marked(GTK_CALENDAR(w), i), i==1);

	PutInMenu (cal);

	return;
}

TEST_F(TestMenuitems, BuildEntry)
{
	GtkWidget * entry = ido_entry_menu_item_new();
	EXPECT_TRUE (entry != NULL);
	EXPECT_TRUE (IDO_IS_ENTRY_MENU_ITEM(entry));
	EXPECT_TRUE (GTK_IS_MENU_ITEM(entry));

	GtkWidget * w = ido_entry_menu_item_get_entry (IDO_ENTRY_MENU_ITEM(entry));
	ASSERT_TRUE (w != NULL);
	ASSERT_TRUE (GTK_IS_ENTRY (w));

	PutInMenu (entry);
	return;
}

namespace
{
  void increment_the_userdata (GObject * scale, gpointer userdata)
  {
    ++*static_cast<int*>(userdata);
  }
}

TEST_F(TestMenuitems, BuildScaleDefault) {
	GtkWidget * scale = ido_scale_menu_item_new("Label", IDO_RANGE_STYLE_DEFAULT, gtk_adjustment_new(0.5, 0.0, 1.0, 0.01, 0.1, 0.1));

	EXPECT_TRUE(scale != NULL);
	EXPECT_TRUE(IDO_IS_SCALE_MENU_ITEM(scale));
	EXPECT_TRUE(GTK_IS_MENU_ITEM(scale));

	const gchar * str_in = "Primary Text";
	ido_scale_menu_item_set_primary_label (IDO_SCALE_MENU_ITEM(scale), str_in);
	const gchar * str_out = ido_scale_menu_item_get_primary_label (IDO_SCALE_MENU_ITEM(scale));
	ASSERT_TRUE (str_in != str_out);
	ASSERT_STREQ (str_in, str_out);

	str_in = "Secondary Text";
	ido_scale_menu_item_set_secondary_label (IDO_SCALE_MENU_ITEM(scale), str_in);
	str_out = ido_scale_menu_item_get_secondary_label (IDO_SCALE_MENU_ITEM(scale));
	ASSERT_TRUE (str_in != str_out);
	ASSERT_STREQ (str_in, str_out);

	int i = 0;
	g_signal_connect (scale, "primary-clicked", G_CALLBACK(increment_the_userdata), &i);
	g_signal_connect (scale, "secondary-clicked", G_CALLBACK(increment_the_userdata), &i);
	ido_scale_menu_item_primary_clicked (IDO_SCALE_MENU_ITEM (scale));
	ASSERT_EQ (1, i);
	ido_scale_menu_item_secondary_clicked (IDO_SCALE_MENU_ITEM (scale));
	ASSERT_EQ (2, i);

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


TEST_F(TestMenuitems, BuildSwitch) {
	GtkWidget * item = ido_switch_menu_item_new ();
	EXPECT_TRUE (item != NULL);
	EXPECT_TRUE (IDO_IS_SWITCH_MENU_ITEM(item));
	EXPECT_TRUE (GTK_IS_MENU_ITEM(item));

	GtkContainer * content_area = ido_switch_menu_item_get_content_area (IDO_SWITCH_MENU_ITEM(item));
	EXPECT_TRUE (content_area != NULL);
	EXPECT_TRUE (GTK_IS_CONTAINER (content_area));

	PutInMenu (item);
}
