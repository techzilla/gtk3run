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
	GtkListStore *liststore2;
};

/**
 * @brief Structure for globally scoped completions
 */
struct completions {
	GtkEntryCompletion *entrycompletion1;
};

/**
 * @brief Enum for liststore columns
 */
enum {
	COL_NAME, COL_FLAG,
};

/**
 * @brief Enum for COL_FLAG
 */
enum {
	T_CMD, T_DIR,
};



/**
 * @brief Function to search directory for commands
 * @return void
 */
void create_command_list(gchar *i, GtkListStore *liststore) {
	GDir *path_dir;
	GError *err = NULL;

	const gchar *command_name;
	gchar *command_path;

	GtkTreeIter iter;

	path_dir = g_dir_open(i, 0, &err);

	if (!path_dir) {
		g_warning("%s", err->message);
		g_error_free(err);
		return;
	}

	while ((command_name = g_dir_read_name(path_dir)) != NULL) {

		command_path = g_build_filename(i, command_name, NULL);

		if (g_file_test(command_path, G_FILE_TEST_IS_EXECUTABLE)) {
			gtk_list_store_append(liststore, &iter);
			gtk_list_store_set(liststore, &iter, COL_NAME, (gchar *) command_name, -1);

		}
		g_free(command_path);
	}

	g_dir_close(path_dir);
	return;
}

/**
 * @brief Function to initialize PATH command completion
 * @return gboolean
 */
static gboolean init_path(GtkListStore *liststore) {

	gint i;
	gchar **path_string;
	gchar **env;
	GSList *path_dirs = NULL;

	env = g_get_environ();

	path_string = g_strsplit((g_environ_getenv(env, "PATH")), G_SEARCHPATH_SEPARATOR_S, -1);

	for (i = 0; path_string[i]; i++) {
		path_dirs = g_slist_prepend(path_dirs, path_string[i]);
	}

	g_slist_foreach(path_dirs, (GFunc) create_command_list, liststore);

	g_slist_free(path_dirs);
	g_strfreev(path_string);
	g_strfreev(env);

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

	if (g_spawn_command_line_async(entry_text, &err)) {
		gtk_main_quit();
	} else {
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
	struct completions *completions;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	if (!gtk_builder_add_from_string(builder, gladestring, -1, &err)) {
		g_warning("%s", err->message);
		exit(EXIT_FAILURE);
	}

	widgets = g_slice_new(struct widgets);
	lists = g_slice_new(struct lists);
	completions = g_slice_new(struct completions);


	widgets->window1 = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
	lists->liststore1 = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore1"));
	completions->entrycompletion1 = GTK_ENTRY_COMPLETION(gtk_builder_get_object(builder, "entrycompletion1"));


	gtk_builder_connect_signals(builder, NULL);
	g_object_unref(G_OBJECT(builder));

	init_path(lists->liststore1);
	gtk_widget_show(widgets->window1);


	gtk_main();


	g_slice_free(struct completions, completions);
	g_slice_free(struct widgets, widgets);
	g_slice_free(struct lists, lists);

	return EXIT_SUCCESS;
}
