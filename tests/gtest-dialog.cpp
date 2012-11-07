
#include <gtk/gtk.h>
#include <gtest/gtest.h>
#include "idomessagedialog.h"

class TestDialog : public ::testing::Test
{
  private:

    guint log_handler_id;

    int log_count_actual;

    static void log_count_func (const gchar *log_domain,
                                GLogLevelFlags log_level,
                                const gchar *message,
                                gpointer user_data)
    {
      reinterpret_cast<TestDialog*>(user_data)->log_count_actual++;
    }

  protected:

    int log_count_expected;

    GMainLoop * main_loop;

  protected:

    virtual void SetUp()
    {
      const GLogLevelFlags flags = GLogLevelFlags(G_LOG_LEVEL_CRITICAL|G_LOG_LEVEL_WARNING);
      log_handler_id = g_log_set_handler ("Gdk", flags, log_count_func, this);
      log_count_expected = 0;
      log_count_actual = 0;

      main_loop = NULL;

      static bool initialized = false;
      if (G_UNLIKELY (!initialized))
        {
          g_type_init();
          initialized = true;
        }

      main_loop = g_main_loop_new (NULL, FALSE);
    }

    virtual void TearDown()
    {
      ASSERT_EQ (log_count_expected, log_count_actual);

      g_clear_pointer (&main_loop, g_main_loop_unref);
    }

  public:

    TestDialog()
    {
      gint argc = 0;
      gchar * argv[] = {NULL};
      gtk_init (&argc, (gchar ***)&argv);
    }

  protected:

    static gboolean
    on_wait_timeout (gpointer main_loop)
    {
      g_main_loop_quit (static_cast<GMainLoop*>(main_loop));
      return G_SOURCE_REMOVE;
    }

    void
    WaitForSignal (gpointer instance, const gchar * detailed_signal)
    {
      guint timeout_id;
      gulong handler_id;
      const int timeout_seconds = 5;

      ASSERT_TRUE (instance != NULL);
      ASSERT_TRUE (main_loop != NULL);


      handler_id = g_signal_connect_swapped (instance,
                                             detailed_signal,
                                             G_CALLBACK(g_main_loop_quit),
                                             main_loop);

      timeout_id = g_timeout_add_seconds (timeout_seconds,
                                          on_wait_timeout,
                                          main_loop);

      // wait for the signal or for timeout, whichever comes first
      g_main_loop_run (main_loop);
      ASSERT_TRUE (g_main_context_find_source_by_id(NULL,timeout_id) != NULL);
      g_signal_handler_disconnect (instance, handler_id);
      g_source_remove (timeout_id);
    }

  protected:

    void ShowDialog (GtkWidget * dialog)
    {
      EXPECT_TRUE (dialog != NULL);
      EXPECT_TRUE (IDO_IS_MESSAGE_DIALOG (dialog));

      gtk_message_dialog_format_secondary_markup (GTK_MESSAGE_DIALOG (dialog),
                                                  "Some Secondary Text");

      GtkWidget * action_area;
      action_area = gtk_dialog_get_action_area (GTK_DIALOG (dialog));
      EXPECT_TRUE (GTK_IS_WIDGET (action_area));

      gtk_widget_show (dialog);

      // synthesize a focus-in event to activate the idotimeline
      GdkEventFocus focus;
      focus.type = GDK_FOCUS_CHANGE;
      focus.window = gtk_widget_get_window (dialog);
      focus.send_event = FALSE;
      focus.in = TRUE;
      gtk_main_do_event ((GdkEvent*)&focus);
      ++log_count_expected; // this will throw up a synthesized event warning

      g_timeout_add_seconds (1, on_wait_timeout, main_loop);
      g_main_loop_run (main_loop);
    }
};

TEST_F (TestDialog, BuildMessageDialog)
{
  GtkWidget * dialog = ido_message_dialog_new (NULL,
                                               GtkDialogFlags(0),
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_CLOSE,
                                               "%s",
                                               "Hello World");
  ShowDialog (dialog);
}

TEST_F (TestDialog, BuildMessageDialogWithMarkup)
{
  GtkWidget * dialog = ido_message_dialog_new_with_markup (NULL,
                                                           GtkDialogFlags(0),
                                                           GTK_MESSAGE_INFO,
                                                           GTK_BUTTONS_CLOSE,
                                                           "%s",
                                                           "<small>Hello World</small>");
  ShowDialog (dialog);
}

