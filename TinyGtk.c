#include "TinyScript.h"
#include <stdio.h>
#include <gtk/gtk.h>

static VM global_vm;
static GtkApplication *app;

static void activate(GtkApplication* app, gpointer user_data)
{
    GtkWidget *window;
    Object *attrs = (Object*)user_data;
    Function func = global_vm.FindFunc(attrs[1].i);

    window = gtk_application_window_new(app);
    memcpy(attrs + 2, &window, sizeof(GtkWidget*));
    
    global_vm.CallFunc(func, &attrs[0], 1);
    gtk_widget_show_all(window);
}

Object GtkButton_GtkButton(Object *args, int arg_size, VM vm)
{
    Object *button_obj = args - 1;
    Object text = args[0];
    GtkWidget *button = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    GtkWidget *label = gtk_button_new_with_label(text.p->str);
    gtk_container_add(GTK_CONTAINER(button), label);
    
    GtkWidget **attrs = (GtkWidget**)malloc(sizeof(GtkWidget*) * 2);
    attrs[0] = button;
    attrs[1] = label;
    button_obj->p = vm.AllocPointer(attrs);
    return (Object){vm.PrimType(INT), 0};
}

static void run_callback(GtkWidget *widget, gpointer user_data)
{
    int callback_id = (int)user_data;
    Function func = global_vm.FindFunc(callback_id);
    global_vm.CallFunc(func, NULL, 0);
}

Object GtkButton_signal_connect(Object *args, int arg_size, VM vm)
{
    Object button_obj = *(args - 1);
    Object signal = args[0];
    Object callback = args[1];
    GtkWidget *button = ((GtkWidget**)button_obj.p->v)[1];

    void *callback_id = (void*)callback.i;
    g_signal_connect(button, signal.p->str, G_CALLBACK(run_callback), callback_id);
    return (Object){vm.PrimType(INT), 0};
}

Object GtkWindow_GtkWindow(Object *args, int arg_size, VM vm)
{
    Object *window = args - 1;
    global_vm = vm;
    
    Object *attrs = (Object*)malloc(sizeof(Object) * 2 + sizeof(GtkWidget*));
    window->p = vm.AllocPointer(attrs);
    attrs[0] = *window;
    
    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), attrs);
    return (Object){vm.PrimType(INT), 0};
}

Object GtkWindow_on_activate(Object *args, int arg_size, VM vm)
{
    Object window = *(args - 1);
    Object callback = args[0];
    
    window.p->attrs[1] = callback;
    return (Object){vm.PrimType(INT), 0};
}

Object GtkWindow_set_title(Object *args, int arg_size, VM vm)
{
    Object window = *(args - 1);
    Object title = args[0];
    GtkWidget *win;
    memcpy(&win, window.p->attrs + 2, sizeof(GtkWidget*));
    
    gtk_window_set_title(GTK_WINDOW(win), title.p->str);
    return (Object){vm.PrimType(INT), 0};
}

Object GtkWindow_set_default_size(Object *args, int arg_size, VM vm)
{
    Object window = *(args - 1);
    Object width = args[0];
    Object height = args[1];
    GtkWidget *win;
    memcpy(&win, window.p->v + sizeof(Object) * 2, sizeof(GtkWidget*));
    
    gtk_window_set_default_size(GTK_WINDOW(win), width.i, height.i);
    return (Object){vm.PrimType(INT), 0};
}

Object GtkWindow_container_add(Object *args, int arg_size, VM vm)
{
    GtkWidget *win;
    Object window = *(args - 1);
    Object widget_obj = args[0];

    memcpy(&win, window.p->v + sizeof(Object) * 2, sizeof(GtkWidget*));
    GtkWidget *widget = ((GtkWidget**)widget_obj.p->v)[0];
    
    gtk_container_add(GTK_CONTAINER(win), widget);
}

Object GtkWindow_run(Object *args, int arg_size, VM vm)
{
    int status;
    
    status = g_application_run(G_APPLICATION(app), 0, NULL);
    g_object_unref(app);
    return (Object){vm.PrimType(INT), status};
}
