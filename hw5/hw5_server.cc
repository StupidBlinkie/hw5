#include <iostream>
#include <string>
#include <cstdlib>

#include "ServerSocket.h"
#include "ClientSocket.h"

//#include "../hw4/hw4/hw4_view.h"
//#include "hw4_controller.h"
extern "C"{
    #include "../jansson/include/jansson.h"
    #include <gtk/gtk.h>
}

using namespace std;
string fakemove = "{\"action\": \"move\" }";
string Message_game  = "from server: nothing for now";
string Message_helloack = "helloack";

  GtkWidget *grid;
  GtkWidget *window;


void dosomething(gpointer user_data){
  int socketFd = *(int*)user_data;
  hw5_net::ClientSocket peerSocket (socketFd); //create a new socket with same fd
}

void activate (GtkApplication *app, gpointer user_data) {

  int socketFd = *(int*)user_data;
  cout << "in activate, user_data is " << *(int*)user_data << endl;

  hw5_net::ClientSocket peerSocket (*(int*)user_data);
    cout << "in activate, peerSocket address is " << &peerSocket << endl;
   int readCount = 0;
   char buff[2048];
   readCount = peerSocket.WrappedRead(buff, 2047) ;
   buff[readCount] = '\0'; // make sure buf holds a c style string
   cout << "(inside activate---)" << buff << "'" << endl;
   cout << "(inside activate---) sending an action to client" << endl;


   //test.. may need to write the same message back here once to make things work
   //peerSocket.WrappedWrite(buff, 2047);  CANT PUT HERE

   window = gtk_application_window_new (app);
   gtk_window_set_title (GTK_WINDOW (window), "Window");
   gtk_window_set_default_size (GTK_WINDOW (window), 40*6, 40*6);
   gtk_container_set_border_width (GTK_CONTAINER (window), 10);
   grid = gtk_grid_new();
   gtk_container_add (GTK_CONTAINER (window), grid);


   GtkButton *button;
   //view_construct_grid();
   

   //four control buttons + 1 quit button
   button = (GtkButton*) gtk_button_new_with_label (NULL);
   gtk_button_set_image(button, gtk_image_new_from_file ("../images/direction/up.png"));
   g_signal_connect (button, "clicked", G_CALLBACK (dosomething), user_data);
   gtk_grid_attach (GTK_GRID (grid), (GtkWidget*) button, 6, 0, 1, 1); // need to fix these parameters

   // button = (GtkButton*) gtk_button_new_with_label (NULL);
   // gtk_button_set_image(button, gtk_image_new_from_file ("../images/direction/down.png"));
   // g_signal_connect (button, "clicked", G_CALLBACK (NULL), NULL);
   // gtk_grid_attach (GTK_GRID (grid),(GtkWidget*)  button, 6, 1, 1, 1);

   // button = (GtkButton*) gtk_button_new_with_label (NULL);
   // gtk_button_set_image(button, gtk_image_new_from_file ("../images/direction/left.png"));
   // g_signal_connect (button, "clicked", G_CALLBACK (NULL), NULL);
   // gtk_grid_attach (GTK_GRID (grid), (GtkWidget*) button, 6, 2, 1, 1);

   // button = (GtkButton*) gtk_button_new_with_label (NULL);
   // gtk_button_set_image(button, gtk_image_new_from_file ("../images/direction/right.png"));
   // g_signal_connect (button, "clicked", G_CALLBACK (NULL), NULL);
   // gtk_grid_attach (GTK_GRID (grid), (GtkWidget*) button, 6, 3, 1, 1);

   gtk_widget_show_all (window);


   cout << "(inside activate---) end of activate" << endl;
   // peerSocket.WrappedWrite(fakemove.c_str(), fakemove.length());
   peerSocket.WrappedWrite(buff, 2047);   //**NEED A write here

  // peerSocket.WrappedWrite(buff, 2047);  //write n times here,  client's loop will read n times
}








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


// void foo (hw5_net::ClientSocket ps){
//   hw5_net::ClientSocket peerSocket = ps;
// }


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
    void* peer_sock = &peerSocket;

    cout << "in main, peerSocket address is " << &peerSocket << endl;
    cout << "in main, peer_sock address is " << &peer_sock << endl;
    cout << "Reading" << endl;

    char buf[2048];
    int readCount;
    //bool received_hello = false;
    bool said_hello = 0;


    //keep reading until gets hello
    while (said_hello == 0) {
      // write to stdout
      readCount = peerSocket.WrappedRead(buf, 2047);
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
        peerSocket.WrappedWrite(helloack_message, 2048);  //TO-DO: not safe here if gameboard is large, convert to string then use str.length()?
      } 
      
    } //end of while }


    ////////*** following while loop mimics gtk activate() ****////////////////
    ////////*** and it is working as expected

    //TODO- we put gtk app/ run/ status stuff here, and do reads and writes inside activate
    //function, and it should work... if activate is called again and again until
    //gtk app terminates...
    
    // readCount = 0;
    // char buff[2048];
    // while(readCount = peerSocket.WrappedRead(buff, 2047) ){
    //       //readCount = peerSocket.WrappedRead(buff, 2047); 
    //       buff[readCount] = '\0'; // make sure buf holds a c style string
    //       cout << "(after gets hello) Got '" << buff << "'" << endl;
    //       cout << "sending an action to client" << endl;
    //       peerSocket.WrappedWrite(fakemove.c_str(), fakemove.length());
    // }

       //gtk

  cout << "in main, gtk check point 1" << endl;
   GtkApplication *app;
   int status;
  
   app = gtk_application_new ("candy.crush", G_APPLICATION_FLAGS_NONE);

   g_signal_connect (app, "activate", G_CALLBACK (activate), &acceptedFd);
 

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
// 