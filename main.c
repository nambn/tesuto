#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#define BUFF_SIZE 100

typedef struct
{
	int quesID;
	char question[BUFF_SIZE];
	char answerA[BUFF_SIZE];
	char answerB[BUFF_SIZE];
	char answerC[BUFF_SIZE];
	char answerD[BUFF_SIZE];
} exam;

// window = gtk_application_window_new(app);
GtkBuilder *builder;

// main window
GtkWidget *window;

// grid list
GtkWidget *grd_main, *grd_test;

// widget list
GtkWidget *btn_exit, *btn_practice, *btn_test, *box_quizlist, *grd_list_w_scrollbar, *btn_back_in_test;

// create new GtkWidget (box) from exam struct
GtkWidget *create_box_from_exam(exam e, char *title)
{
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	// add question
	GtkWidget *label = gtk_label_new(title);
	gtk_label_set_xalign(GTK_LABEL(label), 0.0);
	gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);

	// add question
	GtkWidget *quiz = gtk_label_new(e.question);
	gtk_label_set_xalign(GTK_LABEL(quiz), 0.0);
	gtk_box_pack_start(GTK_BOX(box), quiz, TRUE, TRUE, 0);

	GtkWidget *btnbox_ans = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	GtkWidget *radio1, *radio2, *radio3, *radio4;

	// add answer 1
	radio1 = gtk_radio_button_new_with_label(NULL, e.answerA);
	gtk_box_pack_start(GTK_BOX(btnbox_ans), radio1, FALSE, FALSE, 0);

	// add answer 2
	radio2 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), e.answerB);
	gtk_box_pack_start(GTK_BOX(btnbox_ans), radio2, FALSE, FALSE, 0);

	// add answer 3
	radio3 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), e.answerC);
	gtk_box_pack_start(GTK_BOX(btnbox_ans), radio3, FALSE, FALSE, 0);

	// add answer 4
	radio4 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), e.answerD);
	gtk_box_pack_start(GTK_BOX(btnbox_ans), radio4, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(box), btnbox_ans, FALSE, FALSE, 0);
	return box;
}

// call when Practice button clicked
static void practice(GtkWidget *widget, gpointer data)
{
	gtk_widget_destroy(grd_main);
	gtk_container_add(GTK_CONTAINER(window), grd_test);
	gtk_widget_show_all(window);
}

// call when btn_back_in_test clicked
static void load_main(GtkWidget *widget, gpointer data)
{
	gtk_widget_destroy(grd_test);
	gtk_container_add(GTK_CONTAINER(window), grd_main);
	gtk_widget_show_all(window);	
}

// create gtk window
static void activate(GtkApplication *app, gpointer user_data)
{
	// Load container window
	gtk_builder_add_from_file(builder, "window.glade", NULL);
	window = GTK_WIDGET(gtk_builder_get_object(builder, "wdw_main"));
	g_object_set(window, "application", app, NULL);
	g_signal_connect_swapped(window, "destroy", G_CALLBACK(g_application_quit), app);

	/********** MAIN MENU *************/
	// load main menu from file
	gtk_builder_add_from_file(builder, "menu.glade", NULL);
	grd_main = GTK_WIDGET(gtk_builder_get_object(builder, "grd_main"));
	
	// add menu to window
	gtk_container_add(GTK_CONTAINER(window), grd_main);

	// load button in main menu
	btn_practice = GTK_WIDGET(gtk_builder_get_object(builder, "btn_practice"));
	g_signal_connect(btn_practice, "clicked", G_CALLBACK(practice), NULL);
	btn_test = GTK_WIDGET(gtk_builder_get_object(builder, "btn_test"));
	btn_exit = GTK_WIDGET(gtk_builder_get_object(builder, "btn_exit"));
	g_signal_connect_swapped(btn_exit, "clicked", G_CALLBACK(g_application_quit), app);


	/*********** PRACTICE  *************/
	// load test menu
	gtk_builder_add_from_file(builder, "test.glade", NULL);
	grd_test = GTK_WIDGET(gtk_builder_get_object(builder, "grd_test"));

	// load grid with scrollbar
	grd_list_w_scrollbar = GTK_WIDGET(gtk_builder_get_object(builder, "grd_list_w_scrollbar"));

	// create new box for quizlist and attach to grid with scrollbar
	box_quizlist = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_size_request(box_quizlist, 580, -1);
	gtk_grid_attach(GTK_GRID(grd_list_w_scrollbar), box_quizlist, 0, 0, 1, 1);

	// load buttons
	btn_back_in_test = GTK_WIDGET(gtk_builder_get_object(builder, "btn_back_in_test"));
	g_signal_connect(btn_back_in_test, "clicked", G_CALLBACK(load_main), NULL);

	exam q1;

	q1.quesID = 1;
	strcpy(q1.question, "Help please");
	strcpy(q1.answerA, "dap an a");
	strcpy(q1.answerB, "dap an b");
	strcpy(q1.answerC, "dap an c");
	strcpy(q1.answerD, "dap an d");

	// create exam-box from exam struct
	// TO DO
	GtkWidget *box1 = create_box_from_exam(q1, "Cau 1");

	// add exam-box to quizlist
	gtk_box_pack_start(GTK_BOX(box_quizlist), box1, FALSE, FALSE, 0);

	// show
	gtk_widget_show_all(window);
}

int main(int argc, char *argv[])
{
	GtkApplication *app;
	int status;

	builder = gtk_builder_new();

	app = gtk_application_new("com.tesuto", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
	return status;
}