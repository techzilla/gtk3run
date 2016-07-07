/** @file main.c
 *  @brief Main
 *
 *  Main Function with GTK Event Loop
 *
 *  @author J. M. Becker
 *  @date 6/23/16
 */

#include <stdlib.h>

#include <glib.h>
#include <glib/gstdio.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "main.h"

/**
 * @brief Structure for globally scoped widgets
 */
struct widgets {
    GtkWidget *window1;
    GtkWidget *entry1;
    GtkWidget *statusbar1;
};

/**
 * @brief Structure for globally scoped lists
 */
struct lists {
    GtkListStore *liststore1;
};

/**
 * @brief Enum for liststore columns
 */
enum {
    COL_NAME,
    COL_PATH
};

/**
 * @brief Function to acquire widgets from builder
 * @return gboolean
 */
static gboolean get_widgets(struct widgets *widgets, GtkBuilder *builder) {
    widgets->window1 = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
    widgets->entry1 = GTK_WIDGET(gtk_builder_get_object(builder, "entry1"));
    widgets->statusbar1 = GTK_WIDGET(gtk_builder_get_object(builder, "statusbar1"));

    return TRUE;
}

/**
 * @brief Function to acquire lists from builder
 * @return gboolean
 */
static gboolean get_lists(struct lists *lists, GtkBuilder *builder) {
    lists->liststore1 = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore1"));

    return TRUE;
}


/**
 * @brief Function to initialize shell history
 * @return gboolean
 */
static gboolean init_history(GtkListStore *liststore) {

    GtkTreeIter iter;

    gchar *history_filename;
    FILE *history_file;

    char *line;
    size_t length;
    ssize_t read;

    history_filename = g_build_filename(g_get_home_dir(),
                                        ".bash_history",
                                        NULL);
    if (!g_file_test(history_filename, G_FILE_TEST_EXISTS))
    {
        g_free(history_filename);
        return FALSE;
    };

    history_file = g_fopen(history_filename, "r");

    line = NULL;
    read = getline(&line, &length, history_file);
    for (; read != -1; read = getline(&line, &length, history_file))
    {
        gtk_list_store_append(liststore, &iter);
        gtk_list_store_set(liststore, &iter, COL_NAME, g_strchomp(line), -1);
        g_free(line);
        line = NULL;
    };
    fclose(history_file);
    g_free(history_filename);

    return TRUE;
}

/**
 * @brief Function for entry match-selected signal callback
 * @return void
 */
G_MODULE_EXPORT void cb_matchselected(GtkWidget *entry) {

}

/**
 * @brief Function for entry destroy signal callback
 * @return void
 */
G_MODULE_EXPORT void cb_destroy(GtkWidget *window) {
    gtk_main_quit();
}


/**
 * @brief Function for entry activation signal callback
 * @return gboolean
 */
G_MODULE_EXPORT gboolean cb_activate(GtkWidget *entry, GtkStatusbar *statusbar) {
    GError *err = NULL;
    guint context_id;

    const gchar *entry_text;
    entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

    if (g_spawn_command_line_async(entry_text, &err))
    {
        gtk_main_quit();
    }
    else
    {
        context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "info");
        gtk_statusbar_push(GTK_STATUSBAR(statusbar), context_id, err->message);
        g_error_free(err);
        return FALSE;
    }
    return TRUE;
}


/**
 * @brief Function for entry changed signal callback
 * @return void
 */
G_MODULE_EXPORT void cb_changed(GtkWidget *entry) {

}


/**
 * @brief Function for entry key-press-event signal callback
 * @return void
 */
G_MODULE_EXPORT gboolean cb_keypress(GtkWidget *entry, GdkEventKey *event) {

    switch (event->keyval) {
        case GDK_KEY_uparrow:
            break;
        case GDK_KEY_downarrow:
            break;
        case GDK_KEY_Tab:
            break;
            }
    return GDK_EVENT_PROPAGATE;

}

/**
 * @brief Function for entry activation signal callback
 * @return gboolean
 */
G_MODULE_EXPORT gboolean cb_pushed(GtkStatusbar *statusbar, guint context_id, gchar *text, gpointer data) {

    gtk_statusbar_pop(GTK_STATUSBAR(statusbar), context_id);

    return TRUE;
}


/**
 * @brief Function for main
 */
int main(int argc, char **argv) {
    GtkBuilder *builder;

    GError *err = NULL;
    struct widgets *widgets;
    struct lists *lists;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    if (!gtk_builder_add_from_string(builder, gladestring, -1, &err))
    {
        g_warning("%s", err->message);
        exit(EXIT_FAILURE);
    }

    widgets = g_slice_new(struct widgets);
    lists = g_slice_new(struct lists);

    get_lists(lists, builder);
    get_widgets(widgets, builder);

    gtk_builder_connect_signals(builder, widgets);
    g_object_unref(G_OBJECT(builder));


    init_history(lists->liststore1);

    gtk_widget_show(widgets->window1);
    gtk_main();

    g_slice_free(struct widgets, widgets);
    g_slice_free(struct lists, lists);


    return EXIT_SUCCESS;
}
