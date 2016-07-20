/** @file main.c
 *  @brief Main
 *
 *  Main Function with GTK Event Loop
 *
 *  @author J. M. Becker
 *  @date 6/23/16
 */

#include <stdlib.h>
#include <string.h>


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
 * @brief Function to search directory for commands and update list
 * @param dirname
 * @param liststore

 */
static void update_cmd_list(gchar *dirname, GtkListStore *liststore) {
	GDir *dir;
	GError *err = NULL;

	const gchar *basename;
	gchar *fullname;

	GtkTreeIter iter;

	dir = g_dir_open(dirname, 0, &err);

	if (!dir) {
		g_warning("%s", err->message);
		g_error_free(err);
		return;
	}

	while ((basename = g_dir_read_name(dir)) != NULL) {

		fullname = g_build_filename(dirname, basename, NULL);

		if (g_file_test(fullname, G_FILE_TEST_IS_EXECUTABLE)) {
			gtk_list_store_append(liststore, &iter);
			gtk_list_store_set(liststore, &iter, COL_NAME, (gchar *) basename, COL_FLAG, T_CMD, -1);

		}
		g_free(fullname);
	}

	g_dir_close(dir);
	return;
}

/**
 * @brief Function to initialize command completion
 * @param liststore
 */
static void init_cmd(GtkListStore *liststore) {

	gint i;
	gchar **searchpath;
	gchar **env;
	GSList *dirs = NULL;

	env = g_get_environ();

	searchpath = g_strsplit((g_environ_getenv(env, "PATH")), G_SEARCHPATH_SEPARATOR_S, -1);

	for (i = 0; searchpath[i]; i++) {
		dirs = g_slist_prepend(dirs, searchpath[i]);
	}

	g_slist_foreach(dirs, (GFunc) update_cmd_list, liststore);

	g_slist_free(dirs);
	g_strfreev(searchpath);
	g_strfreev(env);

	return;

}

/**
 * @brief Function to search directory for contents and update list
 * @param dirname
 * @param liststore
 */
static void update_dir_list(gchar *dirname, GtkListStore *liststore) {
	GDir *dir;
	GError *err = NULL;
	const gchar *basename;
	gchar *fullname;

	GtkTreeIter iter;

	dir = g_dir_open(dirname, 0, &err);

	if (!dir) {
		g_warning("%s", err->message);
		g_error_free(err);
		return;
	}

	while ((basename = g_dir_read_name(dir)) != NULL) {

		fullname = g_build_filename(dirname, basename, NULL);

		gtk_list_store_append(liststore, &iter);
		gtk_list_store_set(liststore, &iter, COL_NAME, (gchar *) fullname, COL_FLAG, T_DIR, -1);

		g_free(fullname);
	}

	g_dir_close(dir);

	return;
}

/**
 * @brief Functions to initialize directory completion
 * @param liststore
 */
static void init_dir(GtkListStore *liststore) {
	gchar *dirname;

	dirname = "./";
	update_dir_list(dirname, liststore);

	return;
}

static gboolean str_contains_space(gchar *str) {
	gchar *token;

	token = strchr(str, ' ');
	if (token) {
		return TRUE;
	}
	return FALSE;
}

static gboolean str_contains_alnum(const gchar *str) {
	gchar *token;

	token = strpbrk(str, "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789");
	if (token) {
		return TRUE;
	}
	return FALSE;
}
static gboolean match_func(GtkEntryCompletion *completion, const gchar *key, GtkTreeIter *iter, gpointer user_data) {

	GtkTreeModel *model;
	gchar *fullname = NULL;
	gint flag;

	gchar **keytext;
	gchar *keybuffer;
	guint len;

	if (!str_contains_alnum(key)) {
		return FALSE;
	}

	model = gtk_entry_completion_get_model(completion);
	gtk_tree_model_get(model, iter, COL_NAME, &fullname, COL_FLAG, &flag, -1);

	keybuffer = g_strstrip(g_strdup_printf(key, "%s"));

	if (!str_contains_space(keybuffer)) {
		if (flag == T_CMD) {
			if (g_str_has_prefix(fullname, keybuffer)) {
				g_free(keybuffer);
				return TRUE;
			}
		}
		g_free(keybuffer);
		return FALSE;
	}

	keytext = g_strsplit(keybuffer, " ", -1);
	len = g_strv_length(keytext);

	if (flag == T_DIR) {
		if (g_str_has_prefix(fullname, keytext[len - 1])) {
			g_strfreev(keytext);
			g_free(keybuffer);
			return TRUE;
		}
	}

	g_strfreev(keytext);
	g_free(keybuffer);

	return FALSE;
}

/**
 * @brief Function for entry match-selected signal callback
 * @param completion
 */
/*
G_MODULE_EXPORT void cb_matchselected(GtkEntryCompletion *completion) {

}
 */

/**
 * @brief Function for entry insert-prefix signal callback
 * @param completion
 */
G_MODULE_EXPORT gboolean cb_insertprefix(GtkEntryCompletion *completion) {

	return TRUE;
}


/**
 * @brief Function for entry destroy signal callback
 * @param window
 */
G_MODULE_EXPORT void cb_destroy(GtkWidget *window) {
	gtk_main_quit();
}

/**
 * @brief Function for entry activation signal callback
 * @param entry
 * @param statusbar
 * @return
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
 * @param entry
 */
/*
G_MODULE_EXPORT void cb_changed(GtkWidget *entry) {

}
 */

/**
 * @brief Function for entry key-press-event signal callback
 * @param entry
 * @param event
 * @return
 */
/*G_MODULE_EXPORT gboolean cb_keypress(GtkWidget *entry, GdkEventKey *event, GtkEntryCompletion *entrycompletion) {

	switch (event->keyval) {
	case GDK_KEY_uparrow:
		break;
	case GDK_KEY_downarrow:
		break;
	case GDK_KEY_Tab:
		break;
	}

	return GDK_EVENT_PROPAGATE;

 }*/

/**
 * @brief Function for entry activation signal callback
 * @param statusbar
 * @param context_id
 * @param text
 * @param data
 * @return
 */
G_MODULE_EXPORT gboolean cb_textpushed(GtkStatusbar *statusbar, guint context_id, gchar *text, gpointer data) {

	gtk_statusbar_pop(GTK_STATUSBAR(statusbar), context_id);

	return TRUE;
}

/**
 * @brief Function for main
 * @param argc
 * @param argv
 * @return
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

	gtk_entry_completion_set_match_func(completions->entrycompletion1, match_func, NULL, NULL);

	gtk_builder_connect_signals(builder, NULL);
	g_object_unref(G_OBJECT(builder));

	init_cmd(lists->liststore1);
	init_dir(lists->liststore1);
	gtk_widget_show(widgets->window1);

	gtk_main();

	g_slice_free(struct completions, completions);
	g_slice_free(struct widgets, widgets);
	g_slice_free(struct lists, lists);

	return EXIT_SUCCESS;
}
