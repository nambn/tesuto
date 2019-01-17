#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#define BUFF_SIZE 100
#define DEBUG 1

typedef struct
{
	int quesID;
	char question[BUFF_SIZE];
	char answerA[BUFF_SIZE];
	char answerB[BUFF_SIZE];
	char answerC[BUFF_SIZE];
	char answerD[BUFF_SIZE];
} quiz_t;

GtkApplication *app;

// window = gtk_application_window_new(app);
GtkBuilder *builder;

// main window
GtkWidget *window;

// important grid list
GtkWidget *grd_main, *grd_test;

// component widget list
GtkWidget *btn_exit, *btn_practice, *btn_test, *box_quizlist, *grd_list_w_scrollbar, *btn_back_from_test, *btn_submit;

/*************** FUNCTION LIST ******************/
GtkWidget *new_grd_test(int num_of_quiz, int duration, quiz_t *quiz);
GtkWidget *new_grd_main();

// create new GtkWidget (box) from exam struct
GtkWidget *create_box_from_exam(quiz_t e, char *title)
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
static void show_practice(GtkWidget *widget, gpointer data)
{
	quiz_t q1[100];

	int i;
	for (i = 0; i < 100; i++)
	{
		q1[i].quesID = 1;
		strcpy(q1[i].question, "Help please, how to commit sudoku");
		strcpy(q1[i].answerA, "dap an a");
		strcpy(q1[i].answerB, "dap an b");
		strcpy(q1[i].answerC, "dap an c");
		strcpy(q1[i].answerD, "dap an d");
	}

	// destroy other grid
	gtk_widget_destroy(grd_main);

	// create grid test
	grd_test = new_grd_test(100, 5, q1);
	gtk_container_add(GTK_CONTAINER(window), grd_test);
	gtk_widget_show_all(grd_test);
}

// show main menu
static void show_main(GtkWidget *widget, gpointer data)
{
	gtk_widget_destroy(grd_test);
	grd_main = new_grd_main();
	gtk_container_add(GTK_CONTAINER(window), grd_main);
	gtk_widget_show_all(grd_main);
}

/********** MAIN MENU *************/
GtkWidget *new_grd_main()
{
	// construct builder from file "menu.glade"
	builder = gtk_builder_new_from_file("glade/menu.glade");

	// load component from builder file
	// main grid
	GtkWidget *return_grid = GTK_WIDGET(gtk_builder_get_object(builder, "grd_main"));

	// practice button
	btn_practice = GTK_WIDGET(gtk_builder_get_object(builder, "btn_practice"));
	g_signal_connect(btn_practice, "clicked", G_CALLBACK(show_practice), NULL);

	// test button
	btn_test = GTK_WIDGET(gtk_builder_get_object(builder, "btn_test"));

	// exit button
	btn_exit = GTK_WIDGET(gtk_builder_get_object(builder, "btn_exit"));
	g_signal_connect_swapped(btn_exit, "clicked", G_CALLBACK(g_application_quit), app);

	return return_grid;
}

/*********** TEST SITE (PRACTICE / EXAM) *************/
GtkWidget *new_grd_test(int num_of_quiz, int duration, quiz_t *quiz)
{
	// load grid test from builder "test.glade"
	builder = gtk_builder_new_from_file("glade/test.glade");
	GtkWidget *return_grid = GTK_WIDGET(gtk_builder_get_object(builder, "grd_test"));

	// load grid inside grd_test(with scrollbar)
	grd_list_w_scrollbar = GTK_WIDGET(gtk_builder_get_object(builder, "grd_list_w_scrollbar"));

	// create new box for quizlist and attach to this grid
	box_quizlist = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_size_request(box_quizlist, 580, -1);
	gtk_grid_attach(GTK_GRID(grd_list_w_scrollbar), box_quizlist, 0, 0, 1, 1);

	// submit button
	btn_submit = GTK_WIDGET(gtk_builder_get_object(builder, "btn_submit"));

	// quit button
	btn_back_from_test = GTK_WIDGET(gtk_builder_get_object(builder, "btn_back_from_test"));
	g_signal_connect(btn_back_from_test, "clicked", G_CALLBACK(show_main), app);

	// print each question out
	GtkWidget *quizbox[num_of_quiz];
	int i;
	char title[20];
	for (i = 0; i < num_of_quiz; i++)
	{
		sprintf(title, "Question %d", i + 1);
		quizbox[i] = create_box_from_exam(quiz[i], title);
		gtk_box_pack_start(GTK_BOX(box_quizlist), quizbox[i], FALSE, FALSE, 0);
	}

	return return_grid;
}

// create gtk window
static void activate(GtkApplication *app, gpointer user_data)
{
	// Load container window
	builder = gtk_builder_new_from_file("glade/window.glade");
	window = GTK_WIDGET(gtk_builder_get_object(builder, "wdw_main"));
	g_object_set(window, "application", app, NULL);
	g_signal_connect_swapped(window, "destroy", G_CALLBACK(g_application_quit), app);

	// load menu from file and add to window
	grd_main = new_grd_main();
	gtk_container_add(GTK_CONTAINER(window), grd_main);
	gtk_widget_show_all(grd_main);
}

int main(int argc, char *argv[])
{
	int status;

	app = gtk_application_new("com.tesuto", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
	return status;
}