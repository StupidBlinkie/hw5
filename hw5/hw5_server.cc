#include <iostream>
#include <string>
#include <cstdlib>

#include "ServerSocket.h"
#include "ClientSocket.h"
#include "hw5_server.h"


extern "C"{
    #include "../jansson/include/jansson.h"
    #include <gtk/gtk.h>
}
using namespace std;

const char* color_state1_files[7] = {"../images/regular/state1/blue.png", "../images/regular/state1/green.png", "../images/regular/state1/orange.png",
                              "../images/regular/state1/purple.png", "../images/regular/state1/red.png", "../images/regular/state1/yellow.png",
                              "../images/regular/state1/nocolor.png"};
const char* color_state0_files[7] = {"../images/regular/state0/blue.png", "../images/regular/state0/green.png", "../images/regular/state0/orange.png",
                              "../images/regular/state0/purple.png", "../images/regular/state0/red.png", "../images/regular/state0/yellow.png",
                              "../images/regular/state0/nocolor.png"};
long selected_row;
long selected_col;
int selected_candy_bool = 0;
int rows, cols;

string fakemove = "{\"action\": \"move\"}";


GtkWidget *grid;
GtkWidget *window;
GtkWidget *moves_label;
GtkWidget *score_label;

void* peerSocket_ptr;
// hw5_net::ClientSocket&  peerSocket_ref;

void dosomething(gpointer user_data){

//if selected candy, generate a move message and send to client

  if (selected_candy_bool == 1){

      cout << "(inside dosomething---) about to create socket" << endl;
      //hw5_net::ClientSocket peerSocket = *(hw5_net::ClientSocket *)user_data; 
      hw5_net::ClientSocket peerSocket = *(hw5_net::ClientSocket *)peerSocket_ptr;
      cout << "(inside dosomething---) about to send an action to client" << endl;
      //cout << "in dosomething, user_data is " << user_data << endl;
      //cout << "in dosomething, peerSocket address is " << &peerSocket << endl;
      //peerSocket.WrappedWrite(fakemove.c_str(), fakemove.length());
      cout << "(inside dosomething---) I just sent an action" << endl;
  }else{
    cout<< "please select a candy !" << endl;
  }
}





void activate (GtkApplication *app, gpointer user_data) {

   window = gtk_application_window_new (app);
   gtk_window_set_title (GTK_WINDOW (window), "Window");
   gtk_window_set_default_size (GTK_WINDOW (window), 40*6, 40*6);
   gtk_container_set_border_width (GTK_CONTAINER (window), 10);
   grid = gtk_grid_new();
   gtk_container_add (GTK_CONTAINER (window), grid);


   GtkButton *button;
   view_construct_grid();
   

   //four control buttons + 1 quit button
   button = (GtkButton*) gtk_button_new_with_label (NULL);
   gtk_button_set_image(button, gtk_image_new_from_file ("../images/direction/up.png"));
   g_signal_connect (button, "clicked", G_CALLBACK (dosomething), user_data);
   gtk_grid_attach (GTK_GRID (grid), (GtkWidget*) button, cols, 0, 1, 1); // need to fix these parameters

   gtk_widget_show_all (window);
   cout << "(inside activate---) end of activate" << endl;
}




int main(int argc, char *argv[]) {
  if ( argc != 2 ) usage(argv[0]);
  int port = 0;


  try {
    int socketFd;
    hw5_net::ServerSocket sock(port);
    sock.BindAndListen(AF_INET, &socketFd);
    cout << "Created bound socket. port = " << sock.port() << endl;

    // wait for client connection
    int acceptedFd;
    string clientAddr;
    uint16_t clientPort;
    string clientDNSName;
    string serverAddress;
    string serverDNSName;

    sock.Accept( &acceptedFd,
		 &clientAddr,
		 &clientPort,
		 &clientDNSName,
		 &serverAddress,
		 &serverDNSName );

    cout << "Got connection from: " << endl
	 << "\tacceptedFd\t" << acceptedFd << endl
	 << "\tclientPort\t" << clientPort << endl
	 << "\tclientDNSName\t" << clientDNSName << endl
	 << "\tserverAddress\t" << serverAddress << endl
	 << "\tserverDNSName\t" << serverDNSName << endl;

    // wrap connection to peer with a CientSocket
    hw5_net::ClientSocket peerSocket(acceptedFd);

    cout << "Reading" << endl;

    char buf[2048];
    int readCount;
    //bool received_hello = false;
    bool said_hello = 0;


    //keep reading until gets hello
    while (said_hello == 0) {
      // write to stdout
      readCount = peerSocket.WrappedRead(buf, 2047); //waits for client to write hello
      buf[readCount] = '\0'; // make sure buf holds a c style string
      cout << "Got '" << buf << "'" << endl;

      char* temp = "hello";
      bool match_hello = 1;
      for (int i = 0; i<6; i++){
        if(buf[i] != temp[i]){
          match_hello = 0;
        }
      }
      if (match_hello == 1){
        cout << "server says: received hello from client.." << endl;
        cout << "server says: sending helloack.." << endl;
        said_hello = 1;
        match_hello = 0;

        char* helloack_message = generate_helloack_message(argv[1]);
        peerSocket.WrappedWrite(helloack_message, 2047);  //TO-DO: not safe here if gameboard is large, convert to string then use str.length()?
      } 
      
    } //end of while }

    // hw5_net::ClientSocket & ref = peerSocket;

   peerSocket_ptr = &peerSocket;

   cout << "in main, gtk check point 1" << endl;
   GtkApplication *app;
   int status;
   app = gtk_application_new ("candy.crush", G_APPLICATION_FLAGS_NONE);
   g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
   cout << "in main, gtk check point 1.5" << endl;
   status = g_application_run (G_APPLICATION (app), 0, argv);
   cout << "in main, gtk check point 2" << endl;
 

  } catch (string errString) {         //end of try }
    cerr << errString << endl;
    return EXIT_FAILURE;
  }
  
  cout << "Closing" << endl;
  
  return EXIT_SUCCESS;
}



///---------------server helper methods----------------------///

void usage(const char *exeName) {
  cout << "Usage: " << exeName << " filename" << endl;
  exit(1);
}
char* generate_helloack_message(char* file){
  json_t* json_from_file = json_load_file(file, JSON_COMPACT, NULL);
  json_t* json_final = json_object();
  json_object_set(json_final, "action", json_string("helloack"));
  json_object_set(json_final, "gameinstance", json_from_file);

  return json_dumps(json_final, NULL); 
}


//----------------new view methods-------------------------///

//also constructes move and score labels
void view_construct_grid(){
   hw5_net::ClientSocket peerSocket = *(hw5_net::ClientSocket *)peerSocket_ptr;

   //reterive update message
   int readCount1 = 0;
   char buff[2048];
   readCount1 = peerSocket.WrappedRead(buff, 2047);
   cout << "Read update >>>>" << buff  << endl << endl;
   buff[readCount1] = '\0'; // make sure buf holds a c style string
   
   //encode to json object
   json_t* message_json = json_loads(buff, JSON_COMPACT, NULL);  //remeber to json_decref()
   json_t* gameinstance_json = json_object_get(message_json, "gameinstance");
   json_t* gamestate_json = json_object_get(gameinstance_json, "gamestate");
   json_t* boardcandies_json = json_object_get(gamestate_json, "boardcandies");
   json_t* boardstate_json = json_object_get(gamestate_json, "boardstate");

       // json_t* rows_json = json_object_get(boardcandies_json, "rows");
       // json_t* cols_json = json_object_get(boardcandies_json, "cols");
   rows = json_integer_value(json_object_get(boardcandies_json, "rows"));
   cols = json_integer_value(json_object_get(boardcandies_json, "cols"));
   cout<< "checking decoding gameinstance, row and col should be 66 -------output: "<<rows << cols<< endl;


   json_t* boardcandies_data_json = json_object_get(boardcandies_json, "data");
   cout<< "checking decoding gameinstance, boardcandies_data size should be 36 -------output: " << json_array_size(boardcandies_data_json)<< endl;
   for (int row = 0; row < rows; row++){
    for (int col = 0; col < cols; col++){
      int curr = row * rows + col;
      json_t* candy = json_array_get(boardcandies_data_json, curr);
      int color = json_integer_value(json_object_get(candy, "color"));
      int type = json_integer_value(json_object_get(candy, "type"));

      view_create_candy(color, type, row, col);
    }
   }

   int movesmade = json_integer_value(json_object_get(gamestate_json, "movesmade"));
   int score = json_integer_value(json_object_get(gamestate_json, "currentscore"));
   //draw score and moves label
   moves_label = gtk_label_new (NULL);
   view_update_moves_label(movesmade);
   gtk_grid_attach (GTK_GRID (grid), moves_label, cols, 4, 1, 1);

   score_label = gtk_label_new(NULL);
   view_update_score_label(score);
   gtk_grid_attach (GTK_GRID (grid), score_label, cols, 5, 1, 1);





   peerSocket.WrappedWrite(fakemove.c_str(), fakemove.length());
   //writes to the same buf in client
}


void view_create_candy(int candy, int boardStateNum, int row, int col){
   GtkButton *button = (GtkButton*) gtk_toggle_button_new();
   if (boardStateNum > 0) {
      gtk_button_set_image(button, gtk_image_new_from_file (color_state1_files[candy]));
   } else {
      gtk_button_set_image(button, gtk_image_new_from_file (color_state0_files[candy]));
   }
   gtk_button_set_relief (button, GTK_RELIEF_NONE);

   g_signal_connect (button, "clicked", G_CALLBACK (set_row), (gpointer) row);
   g_signal_connect (button, "clicked", G_CALLBACK (set_col), (gpointer) col);  
   gtk_grid_attach (GTK_GRID (grid), (GtkWidget*) button, col, rows - 1 - row, 1, 1);   //col,row <--> left offset, top offset

   gtk_widget_show_all (window);
}
void view_update_moves_label(int m){
   gchar *label_text;
   label_text = g_strdup_printf("moves left: %d", m);
   gtk_label_set_text ((GtkLabel*) moves_label, label_text);
   free(label_text);

}
void view_update_score_label(int s){
   gchar *label_text;
   label_text = g_strdup_printf("score: %d", s);
   gtk_label_set_text ((GtkLabel*) score_label, label_text);
   free(label_text);
}







//----------------new controller methods-------------------------///
void unselect_current_active_button(){
   gtk_toggle_button_set_active(
      (GtkToggleButton*)gtk_grid_get_child_at(GTK_GRID (grid), selected_col, rows - 1 - selected_row), 
      false);   
}

void set_row(GtkWidget *widget, gpointer data) {
   unselect_current_active_button();
   selected_row = (long) data;
}

// connected to each candy to set col
void set_col(GtkWidget *widget, gpointer data) {
   selected_col = (long) data;
   selected_candy_bool = 1;
   cout<< "selected row , col = " << selected_row << ", " << selected_col << endl;
}
