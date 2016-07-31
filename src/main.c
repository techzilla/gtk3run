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
 * @brief Removes leading and trailing whitespace from a string
 * @param string
 * @return
 */
static GString * g_string_strip(GString *string) {

	gint i1;
	gint pos2;

	/* Left side */
	for (i1 = 0; i1 < string->len; i1++) {
		if (!g_ascii_isspace(string->str[i1])) {
			break;
		}
	}
	if (i1 > 0) {
		g_string_erase(string, 0, i1);
	}

	/* Right side */
	for (pos2 = string->len - 1;; pos2--) {
		if (!g_ascii_isspace(string->str[pos2])) {
			break;
		}

	}
	if (pos2 + 1 < string->len) {
		g_string_set_size(string, pos2);
	}

	return string;

}

/**
 * @brief Function to truncate last word
 * @param string
 * @return
 */
static GString * g_string_trunword(GString *string) {
	gint pos2;

	/* Right side */
	for (pos2 = string->len - 1;; pos2--) {
		if (g_ascii_isspace(string->str[pos2])) {
			break;
		}
	}

	if (pos2 + 1 < string->len) {
		g_string_set_size(string, pos2);
	}

	return string;
}

/**
 * @brief Function to erase everything except last word
 * @param string
 * @return
 */
static GString * g_string_invtrunword(GString *string) {
	gint pos2;

	/* Right side */
	for (pos2 = string->len - 1;; pos2--) {
		if (g_ascii_isspace(string->str[pos2])) {
			break;
		}
	}

	if (pos2 + 1 < string->len) {
		g_string_erase(string, 0, pos2 + 1);
	}

	return string;
}

/**
 * @brief Function to search directory for commands and update list
 * @param dirname
 * @param liststore

 */
static void update_cmd_list(const gchar *dirname, GtkListStore *liststore) {
	GDir *dir;
	const gchar *basename;

	gchar *fullname;

	GtkTreeIter iter;

	if (!dirname) {
		return;
	}

	dir = g_dir_open(dirname, 0, NULL);
	if (!dir) {
		return;
	}

	while ((basename = g_dir_read_name(dir)) != NULL) {

		fullname = g_build_filename(dirname, basename, NULL);

		if (g_file_test(fullname, G_FILE_TEST_IS_EXECUTABLE)) {
			gtk_list_store_insert_with_values(liststore, &iter, -1, COL_NAME, (gchar *) basename, COL_FLAG, T_CMD, -1);

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
 * @brief Function to remove liststore directory row
 * @param model
 * @param path
 * @param iter
 * @param data
 * @return
 */
static gboolean remove_dir_rows(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data) {

	GtkListStore *liststore;
	gint flag;

	gtk_tree_model_get(model, iter, COL_FLAG, &flag, -1);

	if (flag == T_DIR) {

		liststore = GTK_LIST_STORE(model);
		gtk_list_store_remove(liststore, iter);
		gtk_list_store_iter_is_valid(liststore, iter);
	}

	return FALSE;
}

/**
 * @brief Function to search directory for contents and update list
 * @param dirname
 * @param liststore
 */
static void update_dir_list(const gchar *dirname, GtkListStore *liststore) {
	GDir *dir;
	const gchar *basename;

	gchar *fullname;

	GtkTreeIter iter;
	GtkTreeModel *model;

	if (!dirname) {
		return;
	}

	model = GTK_TREE_MODEL(liststore);

	gtk_tree_model_foreach(model, (GtkTreeModelForeachFunc) remove_dir_rows, NULL);

	dir = g_dir_open(dirname, 0, NULL);
	if (!dir) {
		return;
	}

	while ((basename = g_dir_read_name(dir)) != NULL) {
		fullname = g_build_filename(dirname, basename, NULL);

		if (g_file_test(fullname, G_FILE_TEST_IS_DIR)) {
			if (!g_str_has_suffix(fullname, G_DIR_SEPARATOR_S)) {
				fullname = g_realloc(fullname, ((strlen(fullname)) + 2));
				fullname = strcat(fullname, G_DIR_SEPARATOR_S);
			}
		}

		gtk_list_store_insert_with_values(liststore, &iter, -1, COL_NAME, (gchar *) fullname, COL_FLAG, T_DIR, -1);
		g_free(fullname);
	}

	g_dir_close(dir);

	return;
}

/**
 * @brief Function to search string for space
 * @param str
 * @return
 */
static gboolean str_contains_space(const gchar *str) {
	gchar *token;
	gboolean retval = FALSE;

	token = strchr(str, ' ');
	if (token) {
		retval = TRUE;
	}
	return retval;
}

/**
 * @brief Function to search string for path
 * @param str
 * @return
 */
static gboolean str_contains_path(const gchar *str) {
	gchar *token1;
	gchar *token2;
	gboolean retval = FALSE;

	token1 = strrchr(str, ' ');
	if (token1) {
		token2 = strchr(token1, G_DIR_SEPARATOR);
		{
			if (token2) {
				retval = TRUE;
			}
		}
	}

	return retval;
}

/**
 * @brief Function to search string for dir separator
 * @param str
 * @return
 */
static gboolean str_contains_separator(const gchar *str) {
	gchar *token;
	gboolean retval = FALSE;

	token = strchr(str, G_DIR_SEPARATOR);
	if (token) {
		retval = TRUE;
	}
	return retval;
}

/**
 * @brief Function to search string for alnum
 * @param str
 * @return
 */
static gboolean str_contains_alnum(const gchar *str) {
	gchar *token;
	gboolean retval = FALSE;

	token = strpbrk(str, "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789");
	if (token) {
		retval = TRUE;
	}
	return retval;
}

/**
 * @brief completion match function
 * @param completion
 * @param key
 * @param iter
 * @param user_data
 * @return
 */
static gboolean match_func(GtkEntryCompletion *completion, const gchar *key, GtkTreeIter *iter, gpointer user_data) {

	gboolean retval = FALSE;

	GString *keybuffer;
	GString *namebuffer;

	GtkTreeModel *model;
	gchar *name = NULL;

	gint flag;

	if (!str_contains_alnum(key)) {
		return retval;
	}

	model = gtk_entry_completion_get_model(completion);
	gtk_tree_model_get(model, iter, COL_NAME, &name, COL_FLAG, &flag, -1);
	if (!name) {
		return retval;
	}

	namebuffer = g_string_new(name);
	keybuffer = g_string_new(key);
	keybuffer = g_string_strip(keybuffer);

	if (!str_contains_space(keybuffer->str)) {
		if (flag == T_CMD) {
			if (g_str_has_prefix(namebuffer->str, keybuffer->str)) {
				retval = TRUE;
				goto ret1;
			}
		}
		goto ret1;
	}

	keybuffer = g_string_invtrunword(keybuffer);
	if (flag == T_DIR) {
		if (g_str_has_prefix(namebuffer->str, keybuffer->str)) {
			if (keybuffer->len < namebuffer->len) {
				g_string_erase(namebuffer, 0, keybuffer->len);
				if (namebuffer->str[namebuffer->len - 1] == G_DIR_SEPARATOR) {
					retval = TRUE;
					goto ret1;
				} else if (!str_contains_separator(namebuffer->str)) {
					retval = TRUE;
					goto ret1;
				}

			}
		}
	}

	ret1: g_string_free(keybuffer, TRUE);
	g_string_free(namebuffer, TRUE);
	g_free(name);
	return retval;
}

/**
 * @brief Function for entry match-selected signal callback
 * @param completion
 * @param model
 * @param iter
 * @param user_data
 * @return
 */
G_MODULE_EXPORT gboolean cb_matchselected(GtkEntryCompletion *completion, GtkTreeModel *model, GtkTreeIter *iter,
		gpointer user_data) {

	gboolean retval = TRUE;

	gchar *fullname = NULL;
	GString *entrybuffer;

	GtkEntry *entry;
	GtkEditable *editable;

	GtkListStore *liststore;

	entry = GTK_ENTRY(gtk_entry_completion_get_entry(completion));
	editable = GTK_EDITABLE(entry);

	entrybuffer = g_string_new(gtk_entry_get_text(entry));
	entrybuffer = g_string_strip(entrybuffer);

	gtk_tree_model_get(model, iter, COL_NAME, &fullname, -1);

	if (!str_contains_space(entrybuffer->str)) {
		gtk_entry_set_text(entry, fullname);
		gtk_editable_set_position(editable, -1);
		goto ret1;
	}

	entrybuffer = g_string_trunword(entrybuffer);
	entrybuffer = g_string_append_c(entrybuffer, ' ');
	entrybuffer = g_string_append(entrybuffer, fullname);

	gtk_entry_set_text(entry, entrybuffer->str);
	gtk_editable_set_position(editable, -1);

	liststore = GTK_LIST_STORE(model);
	update_dir_list(fullname, liststore);



	ret1: g_string_free(entrybuffer, TRUE);
	g_free(fullname);
	return retval;

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
	gboolean retval = TRUE;

	const gchar *entry_text;
	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

	if (g_spawn_command_line_async(entry_text, &err)) {
		gtk_main_quit();
	} else {
		context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "info");
		gtk_statusbar_push(GTK_STATUSBAR(statusbar), context_id, err->message);
		g_error_free(err);
		retval = FALSE;
	}
	return retval;
}

/**
 * @brief Function for entry insert-text signal callback
 * @param editable
 * @param new_text
 * @param new_text_length
 * @param position
 * @param liststore
 */
G_MODULE_EXPORT void cb_inserttext(GtkEditable *editable, gchar *new_text, gint new_text_length, gpointer position,
		GtkListStore *liststore) {

	/** @todo  properly escaped directory from entry **/

	const gchar *entry_text;
	gchar *token;
	gchar *dirname;

	entry_text = gtk_entry_get_text(GTK_ENTRY(editable));
	if (!str_contains_path(entry_text)) {
		return;
	}

	token = strrchr(entry_text, ' ');
	dirname = strchr(token, G_DIR_SEPARATOR);

	update_dir_list(dirname, liststore);

	return;

}

/**
 * @brief Function for entry delete-text signal callback
 * @param editable
 * @param start_pos
 * @param end_pos
 * @param liststore
 */
G_MODULE_EXPORT void cb_deletetext(GtkEditable *editable, gint start_pos, gint end_pos, GtkListStore *liststore) {

	return;

}

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
	gtk_widget_show(widgets->window1);

	gtk_main();

	g_slice_free(struct completions, completions);
	g_slice_free(struct widgets, widgets);
	g_slice_free(struct lists, lists);

	return EXIT_SUCCESS;
}
