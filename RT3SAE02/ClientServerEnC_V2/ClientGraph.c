#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <gtk/gtk.h>

// Callback function for button click
static void on_button_clicked(GtkWidget *widget, gpointer data) {
    g_print("Button clicked\n");
}

int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create a new window
    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Simple GTK Application");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    // Create a new button
    GtkWidget *button = gtk_button_new_with_label("Click Me");

    // Connect the button's "clicked" signal to the callback function
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);

    // Add the button to the window
    gtk_window_set_child(GTK_WINDOW(window), button);

    // Show the window and all its child widgets
    gtk_widget_show(window);

    // Run the GTK main loop
    gtk_main();

    return 0;
}