#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <gtk/gtk.h>

#define BUFF_SIZE 1024
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

//struct for exam room
typedef struct
{
	int roomID;
	int num_of_question;
	int time_of_test;
	int status;
} room_t;

char *file_name_questionlist = "client_exam.txt";
char *file_name_roomlist = "client_room_list.txt";

char send_message[BUFF_SIZE], recv_message[BUFF_SIZE];
char answers[BUFF_SIZE];

int bytes_sent;
int bytes_received;

quiz_t exam_test[100];
room_t list_of_room[100];

int client_sock;
struct sockaddr_in server_addr;

GtkApplication *app;

// window = gtk_application_window_new(app);
GtkBuilder *builder;

// main window
GtkWidget *window;

// important grid list
GtkWidget *grd_main, *grd_test, *grd_room, *wdn_input;

// component widget list
GtkWidget *btn_exit, *btn_practice, *btn_exam, *box_quizlist, *box_roomlist, *vpt_question, *vpt_roomlist, *btn_back_from_test, *btn_submit, *ent_num_of_question, *ent_duration;

/*************** FUNCTION LIST ******************/
GtkWidget *new_grd_test(int num_of_quiz, int duration, quiz_t *quiz);
GtkWidget *new_grd_main();
GtkWidget *new_grd_room();
static void btn_result_close_clicked(GtkWidget *widget, gpointer data);

/*************** QUESTION BOX *******************/
// create new GtkWidget (box) from quiz struct
GtkWidget *create_box_from_quiz(quiz_t e, char *title)
{
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width(GTK_CONTAINER(box), 10);

	// child 1: question title (label)
	GtkWidget *label = gtk_label_new(title);
	gtk_label_set_xalign(GTK_LABEL(label), 0.0);
	gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);

	// child 2: question text (label)
	GtkWidget *quiz = gtk_label_new(e.question);
	gtk_label_set_xalign(GTK_LABEL(quiz), 0.0);
	gtk_box_pack_start(GTK_BOX(box), quiz, TRUE, TRUE, 0);

	// child 3: question answer list (button box)
	GtkWidget *btnbox_ans = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start(GTK_BOX(box), btnbox_ans, FALSE, FALSE, 0);

	GtkWidget *radio1, *radio2, *radio3, *radio4;

	// child 3.1
	radio1 = gtk_radio_button_new_with_label(NULL, e.answerA);
	gtk_box_pack_start(GTK_BOX(btnbox_ans), radio1, FALSE, FALSE, 0);

	// child 3.2
	radio2 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), e.answerB);
	gtk_box_pack_start(GTK_BOX(btnbox_ans), radio2, FALSE, FALSE, 0);

	// child 3.3
	radio3 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), e.answerC);
	gtk_box_pack_start(GTK_BOX(btnbox_ans), radio3, FALSE, FALSE, 0);

	// child 3.4
	radio4 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), e.answerD);
	gtk_box_pack_start(GTK_BOX(btnbox_ans), radio4, FALSE, FALSE, 0);

	return box;
}

/*************** SINGLE QUESTION GRID *******************/
// create new GtkWidget (grid) from room struct
GtkWidget *create_grid_from_room(room_t r)
{
	GtkWidget *grid = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(grid), 10);

	GtkWidget *lbl_id, *lbl_num_of_question, *lbl_time_of_test, *lbl_status, *btn_enter;

	char temp_s[BUFF_SIZE];

	// child 1: room id (label)
	sprintf(temp_s, "%d", r.roomID);
	lbl_id = gtk_label_new(temp_s);
	gtk_grid_attach(GTK_GRID(grid), lbl_id, 0, 0, 1, 1);

	// child 2: status (label)
	sprintf(temp_s, "%d", r.roomID);
	lbl_status = gtk_label_new(temp_s);
	gtk_grid_attach(GTK_GRID(grid), lbl_status, 0, 1, 1, 1);

	// child 3: num_of_quiz (label)
	sprintf(temp_s, "%d", r.roomID);
	lbl_num_of_question = gtk_label_new(temp_s);
	gtk_grid_attach(GTK_GRID(grid), lbl_num_of_question, 1, 0, 1, 1);

	// child 4: time_of_test (label)
	sprintf(temp_s, "%d", r.roomID);
	lbl_time_of_test = gtk_label_new(temp_s);
	gtk_grid_attach(GTK_GRID(grid), lbl_time_of_test, 1, 1, 1, 1);

	// child 5: button enter
	btn_enter = gtk_button_new_with_label("Enter room");
	gtk_grid_attach(GTK_GRID(grid), btn_enter, 2, 0, 1, 2);

	return grid;
}

// call when Practice button clicked
static void show_practice(GtkWidget *widget, gpointer data)
{
	memset(send_message, '\0', strlen(send_message));
	strcpy(send_message, "PRACTICE- ");
	bytes_sent = send(client_sock, send_message, strlen(send_message), 0);

	memset(recv_message, 0, strlen(recv_message));
	bytes_received = recv(client_sock, recv_message, BUFF_SIZE - 1, 0);
	recv_message[bytes_received] = '\0';

	quiz_t q1[13];

	int i;
	for (i = 0; i < 13; i++)
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

	// show grid test
	grd_test = new_grd_test(13, 5, q1);
	gtk_container_add(GTK_CONTAINER(window), grd_test);
	gtk_widget_show_all(grd_test);
}

// show main menu
static void show_main(GtkWidget *widget, gpointer data)
{
	gtk_widget_destroy(grd_test);
	gtk_widget_destroy(grd_room);
	grd_main = new_grd_main();
	gtk_container_add(GTK_CONTAINER(window), grd_main);
	gtk_widget_show_all(grd_main);
}

// caculate test result and display
static void show_result(GtkWidget *widget, gpointer data)
{
	// data is box_quizlist
	GList *children = gtk_container_get_children(GTK_CONTAINER(data));
	GtkWidget *btnbox_ans;
	GList *ans_child;

	// send format "1-A:2-B:3-C:........"
	char answers[BUFF_SIZE] = "", answer[BUFF_SIZE];

	int quiz_count = 0;
	do
	{
		quiz_count++;

		// get last grandchildren of each child, it should be the radio button box
		btnbox_ans = g_list_last(gtk_container_get_children(GTK_CONTAINER(children->data)))->data;
		ans_child = gtk_container_get_children(GTK_CONTAINER(btnbox_ans));

		// check if answer A toggled
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ans_child->data)))
		{
			sprintf(answer, "%d-A:", quiz_count);
			strcat(answers, answer);
		}
		// check if answer B toggled
		else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ans_child->next->data)))
		{
			sprintf(answer, "%d-B:", quiz_count);
			strcat(answers, answer);
		}
		// check if answer C toggled
		else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ans_child->next->next->data)))
		{
			sprintf(answer, "%d-C:", quiz_count);
			strcat(answers, answer);
		}
		// check if answer D toggled
		else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ans_child->next->next->next->data)))
		{
			sprintf(answer, "%d-D:", quiz_count);
			strcat(answers, answer);
		}
		else
		{
			sprintf(answer, "%d-X:", quiz_count);
			strcat(answers, answer);
		}
	} while ((children = g_list_next(children)) != NULL);

	if (DEBUG)
	{
		printf("Number of quiz: %d\n", quiz_count);
		printf("Answer: %s\n", answers);
	}

	// gui dap an len server

	// nhan ket qua tu server
	int test_score = 10;

	// display a dialog to show result

	/******************* TEST SCORE ******************/
	// load dialog from file "result.glade"
	builder = gtk_builder_new_from_file("glade/result.glade");

	GtkWidget *dia_result = GTK_WIDGET(gtk_builder_get_object(builder, "dia_result"));

	GtkButton *btn_result_close = GTK_BUTTON(gtk_builder_get_object(builder, "btn_result_close"));
	g_signal_connect(btn_result_close, "clicked", G_CALLBACK(btn_result_close_clicked), dia_result);

	GtkLabel *lbl_test_result = GTK_LABEL(gtk_builder_get_object(builder, "lbl_test_result"));
	char score_inform[BUFF_SIZE];
	sprintf(score_inform, "Number of right answer: %d / %d", test_score, quiz_count);
	gtk_label_set_text(lbl_test_result, score_inform);

	gtk_widget_show_all(dia_result);
}

static void btn_result_close_clicked(GtkWidget *widget, gpointer data)
{
	gtk_widget_destroy(data);
	gtk_widget_destroy(grd_test);
	grd_main = new_grd_main();
	gtk_container_add(GTK_CONTAINER(window), grd_main);
	gtk_widget_show_all(grd_main);
}

static void show_roomlist()
{
	// destroy other grid
	gtk_widget_destroy(grd_main);

	// show grid test
	grd_room = new_grd_room();
	gtk_container_add(GTK_CONTAINER(window), grd_room);
	gtk_widget_show_all(grd_room);
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
	btn_exam = GTK_WIDGET(gtk_builder_get_object(builder, "btn_exam"));
	g_signal_connect(btn_exam, "clicked", G_CALLBACK(show_roomlist), NULL);

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

	// load viewport inside grd_test
	vpt_question = GTK_WIDGET(gtk_builder_get_object(builder, "vpt_question"));

	// create new empty BOX for quizlist and attach to this viewport
	box_quizlist = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	// gtk_widget_set_size_request(box_quizlist, 580, -1);
	gtk_container_add(GTK_CONTAINER(vpt_question), box_quizlist);

	// print each question in a quiz box then insert box to box list
	GtkWidget *box_quiz[num_of_quiz];
	int i;
	char title[20];
	for (i = 0; i < num_of_quiz; i++)
	{
		sprintf(title, "Question %d", i + 1);
		box_quiz[i] = create_box_from_quiz(quiz[i], title);
		gtk_box_pack_start(GTK_BOX(box_quizlist), box_quiz[i], FALSE, FALSE, 0);
	}

	// submit button
	btn_submit = GTK_WIDGET(gtk_builder_get_object(builder, "btn_submit"));
	g_signal_connect(btn_submit, "clicked", G_CALLBACK(show_result), box_quizlist);

	// quit button
	btn_back_from_test = GTK_WIDGET(gtk_builder_get_object(builder, "btn_back_from_test"));
	g_signal_connect(btn_back_from_test, "clicked", G_CALLBACK(show_main), NULL);

	return return_grid;
}

// refresh room list
void refresh()
{
	gtk_widget_destroy(grd_room);

	// show grid room
	grd_room = new_grd_room();
	gtk_container_add(GTK_CONTAINER(window), grd_room);
	gtk_widget_show_all(grd_room);
}

void on_btn_create_clicked()
{
	char *dur = gtk_entry_get_text(GTK_ENTRY(ent_duration));
	char *no = gtk_entry_get_text(GTK_ENTRY(ent_num_of_question));
	char mess[BUFF_SIZE];
	sprintf(mess, "NEW-%d-%d", no, dur);
}

/************ NEW ROOM ENTRY BOX ****************/
void new_room()
{
	builder = gtk_builder_new_from_file("glade/input.glade");
	wdn_input = GTK_WIDGET(gtk_builder_get_object(builder, "wdn_input"));

	ent_num_of_question = GTK_WIDGET(gtk_builder_get_object(builder, "ent_num_of_question"));
	ent_duration = GTK_WIDGET(gtk_builder_get_object(builder, "ent_duration"));

	// create button
	GtkButton *btn_create_in_input = GTK_BUTTON(gtk_builder_get_object(builder, "btn_create_in_input"));
	g_signal_connect(btn_create_in_input, "clicked", G_CALLBACK(on_btn_create_clicked), NULL);

	// quit button
	GtkButton *btn_back_in_input = GTK_BUTTON(gtk_builder_get_object(builder, "btn_back_in_input"));
	g_signal_connect_swapped(btn_back_in_input, "clicked", G_CALLBACK(gtk_widget_destroy), wdn_input);

	gtk_widget_show_all(wdn_input);
}

/*************** ROOM LIST GRID ***********************/
GtkWidget *new_grd_room()
{
	// lay danh sach phong tu server

	// fake data /////////////////////////
	int num_of_room = 14;
	room_t room[num_of_room];
	int i;
	for (i = 0; i < num_of_room; i++)
	{
		room[i].roomID = 21;
		room[i].num_of_question = 22;
		room[i].time_of_test = 23;
		room[i].status = 1;
	}
	////////////////////////////////////////

	// load grid test from builder "test.glade"
	builder = gtk_builder_new_from_file("glade/room.glade");
	GtkWidget *return_grid = GTK_WIDGET(gtk_builder_get_object(builder, "grd_room"));

	vpt_roomlist = GTK_WIDGET(gtk_builder_get_object(builder, "vpt_roomlist"));

	box_roomlist = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(vpt_roomlist), box_roomlist);

	// print each question in a quiz box then insert box to box list
	GtkWidget *grd_room[num_of_room];
	for (i = 0; i < num_of_room; i++)
	{
		grd_room[i] = create_grid_from_room(room[i]);
		gtk_box_pack_start(GTK_BOX(box_roomlist), grd_room[i], FALSE, FALSE, 0);
	}

	// refresh button
	GtkWidget *btn_refresh = GTK_WIDGET(gtk_builder_get_object(builder, "btn_refresh"));
	g_signal_connect(btn_refresh, "clicked", G_CALLBACK(refresh), NULL);

	// new room button
	GtkWidget *btn_newroom = GTK_WIDGET(gtk_builder_get_object(builder, "btn_newroom"));
	g_signal_connect(btn_newroom, "clicked", G_CALLBACK(new_room), NULL);

	// quit button
	GtkWidget *btn_back_from_rooms = GTK_WIDGET(gtk_builder_get_object(builder, "btn_back_from_rooms"));
	g_signal_connect(btn_back_from_rooms, "clicked", G_CALLBACK(show_main), NULL);

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
	// check number of arguments
	if (argc != 3)
	{
		puts("Wrong numbers of arguments!");
		puts("Usage: /.client <IP> <PORT>");
		return 1;
	}

	// Construct socket
	client_sock = socket(AF_INET, SOCK_STREAM, 0);

	// Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);

	// Request to connect server
	if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
	{
		puts("\nError!Can not connect to sever! Client exit immediately!");
		return 0;
	}

	if (DEBUG)
		puts("Connected to server!");

	// create gtk application
	int status;
	app = gtk_application_new("com.tesuto", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), 0, argv);

	g_object_unref(app);
	return status;
}