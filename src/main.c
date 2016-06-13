#include <stdlib.h>

#include <glib.h>

#include <gtk/gtk.h>




struct widgets {
	GtkWidget *window;
	GtkWidget *entry;
    GtkWidget *label;
};


static void get_widgets(struct widgets *widgets, GtkBuilder *builder)
{
	widgets->window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
	widgets->entry = GTK_WIDGET(gtk_builder_get_object(builder, "entry1"));
	widgets->label = GTK_WIDGET(gtk_builder_get_object(builder, "label1"));
}

G_MODULE_EXPORT gboolean cb_activated(GtkWidget *entry, gpointer data)
{
    GError *err = NULL;

    const gchar *entry1_text;
    entry1_text = gtk_entry_get_text(GTK_ENTRY(entry));

    if (g_spawn_command_line_async(entry1_text, &err)) {
        gtk_main_quit();
    } else {
        g_printerr("ERROR: %s\n", err->message);
        g_error_free(err);
        return FALSE;
    }
    return TRUE;
}


int main(int argc, char **argv)
{
	GtkBuilder *builder;
	GError *err = NULL;
	struct widgets *widgets;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	if (!gtk_builder_add_from_file(builder, "main.glade", &err)) {
		g_warning("%s", err->message);
		exit(EXIT_FAILURE);
	}

	widgets = g_slice_new(struct widgets);
	get_widgets(widgets, builder);
	gtk_builder_connect_signals(builder, widgets);
	g_object_unref(G_OBJECT(builder));

	gtk_widget_show(widgets->window);
	gtk_main();


	g_slice_free(struct widgets, widgets);

    return EXIT_SUCCESS;
}
