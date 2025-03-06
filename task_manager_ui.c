#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

enum {
    PID_COLUMN,
    NAME_COLUMN,
    USER_TIME_COLUMN,
    SYS_TIME_COLUMN,
    THREADS_COLUMN,
    VM_RSS_COLUMN,
    VM_SIZE_COLUMN,
    N_COLUMNS
};

// Function to read data from /proc/task_manager and populate the list store
static void populate_process_list(GtkListStore *store) {
    FILE *fp = fopen("/proc/task_manager", "r");
    if (!fp) {
        perror("Failed to open /proc/task_manager");
        return;
    }

    // Skip the header line
    char line[256];
    fgets(line, sizeof(line), fp);

    // Clear existing data
    gtk_list_store_clear(store);

    // Read each process and add to the GTK ListStore
    while (fgets(line, sizeof(line), fp)) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);

        int pid, threads;
        char name[50];
        long user_time, sys_time, vm_rss, vm_size;

        sscanf(line, "%d %s %ld %ld %d %ld %ld", &pid, name, &user_time, &sys_time, &threads, &vm_rss, &vm_size);

        gtk_list_store_set(store, &iter,
                           PID_COLUMN, pid,
                           NAME_COLUMN, name,
                           USER_TIME_COLUMN, user_time,
                           SYS_TIME_COLUMN, sys_time,
                           THREADS_COLUMN, threads,
                           VM_RSS_COLUMN, vm_rss,
                           VM_SIZE_COLUMN, vm_size,
                           -1);
    }
    fclose(fp);
}

// Callback to refresh data
static gboolean refresh_data(gpointer data) {
    GtkListStore *store = GTK_LIST_STORE(data);
    populate_process_list(store);
    return TRUE;  // Keep the timeout active
}

// Callback to stop (pause) a process
static void stop_process(GtkButton *button, gpointer user_data) {
    GtkTreeSelection *selection = GTK_TREE_SELECTION(user_data);
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int pid;
        gtk_tree_model_get(model, &iter, PID_COLUMN, &pid, -1);
        kill(pid, SIGSTOP);
        g_print("Stopped process %d\n", pid);
    }
}

// Callback to terminate (kill) a process
static void kill_process(GtkButton *button, gpointer user_data) {
    GtkTreeSelection *selection = GTK_TREE_SELECTION(user_data);
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int pid;
        gtk_tree_model_get(model, &iter, PID_COLUMN, &pid, -1);
        kill(pid, SIGKILL);
        g_print("Killed process %d\n", pid);
    }
}

// Main function
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Window setup
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Task Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // List Store and Tree View
    GtkListStore *store = gtk_list_store_new(N_COLUMNS, G_TYPE_INT, G_TYPE_STRING, G_TYPE_LONG, G_TYPE_LONG, G_TYPE_INT, G_TYPE_LONG, G_TYPE_LONG);
    GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);

    // Column headers with sorting enabled
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *col;

    col = gtk_tree_view_column_new_with_attributes("PID", renderer, "text", PID_COLUMN, NULL);
    gtk_tree_view_column_set_sort_column_id(col, PID_COLUMN);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col);

    col = gtk_tree_view_column_new_with_attributes("Process Name", renderer, "text", NAME_COLUMN, NULL);
    gtk_tree_view_column_set_sort_column_id(col, NAME_COLUMN);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col);

    col = gtk_tree_view_column_new_with_attributes("User Time", renderer, "text", USER_TIME_COLUMN, NULL);
    gtk_tree_view_column_set_sort_column_id(col, USER_TIME_COLUMN);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col);

    col = gtk_tree_view_column_new_with_attributes("System Time", renderer, "text", SYS_TIME_COLUMN, NULL);
    gtk_tree_view_column_set_sort_column_id(col, SYS_TIME_COLUMN);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col);

    col = gtk_tree_view_column_new_with_attributes("Threads", renderer, "text", THREADS_COLUMN, NULL);
    gtk_tree_view_column_set_sort_column_id(col, THREADS_COLUMN);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col);

    col = gtk_tree_view_column_new_with_attributes("VmRSS", renderer, "text", VM_RSS_COLUMN, NULL);
    gtk_tree_view_column_set_sort_column_id(col, VM_RSS_COLUMN);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col);

    col = gtk_tree_view_column_new_with_attributes("VmSize", renderer, "text", VM_SIZE_COLUMN, NULL);
    gtk_tree_view_column_set_sort_column_id(col, VM_SIZE_COLUMN);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col);

    // Refresh interval to update data
    g_timeout_add(2000, refresh_data, store);  // Refresh every 2 seconds

    // Selection and button actions
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    GtkWidget *stop_button = gtk_button_new_with_label("Stop");
    g_signal_connect(stop_button, "clicked", G_CALLBACK(stop_process), selection);
    gtk_box_pack_start(GTK_BOX(button_box), stop_button, TRUE, TRUE, 0);

    GtkWidget *kill_button = gtk_button_new_with_label("Kill");
    g_signal_connect(kill_button, "clicked", G_CALLBACK(kill_process), selection);
    gtk_box_pack_start(GTK_BOX(button_box), kill_button, TRUE, TRUE, 0);

    // Layout setup
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

