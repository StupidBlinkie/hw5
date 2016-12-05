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
string fakemove = "{\"action\": \"move\"}";


  GtkWidget *grid;
  GtkWidget *window;


void dosomething(gpointer user_data){

  cout << "(inside dosomething---) about to create socket" << endl;
  hw5_net::ClientSocket peerSocket = *(hw5_net::ClientSocket *)user_data; //create a new socket with same fd
  cout << "(inside dosomething---) about to send an action to client" << endl;
  //cout << "in dosomething, user_data is " << user_data << endl;
  //cout << "in dosomething, peerSocket address is " << &peerSocket << endl;
  peerSocket.WrappedWrite(fakemove.c_str(), fakemove.length());
  cout << "(inside dosomething---) I just sent an action" << endl;

   // int readCount = 0;
   // char buf[2048];
   // readCount = peerSocket.WrappedRead(buf, 2047);  //waits for after_helloack_message

   // buf[readCount] = '\0'; // make sure buf holds a c style string
   // cout << "(inside activate---)" << buf << "'" << endl;
   // cout << "(inside activate---) sending an action to client" << endl;
}


void view_construct_grid(hw5_net::ClientSocket peerSocket){
   int readCount1 = 0;
   char buff[2048];
   readCount1 = peerSocket.WrappedRead(buff, 2047);  //waits for after_helloack_message
   cout << "Read update >>>>" << buff  << endl;
   buff[readCount1] = '\0'; // make sure buf holds a c style string
       double timing = 350000;
    while (timing > 0){
      timing --;
      cout<< timing << endl;
    }
   //peerSocket.WrappedWrite(fakemove.c_str(), fakemove.length());
   //writes to the same buf in client
}


void activate (GtkApplication *app, gpointer user_data) {

   hw5_net::ClientSocket peerSocket = *(hw5_net::ClientSocket *)user_data;
   //cout << "in activate, user_data is " << *(int*)user_data << endl;
   cout << "in activate, user_data is " << user_data << endl;
   cout << "in activate, peerSocket address is " << &peerSocket << endl;

    //int readCount = 0;
    //char buf[2048];
    //readCount = peerSocket.WrappedRead(buf, 2047);  //waits for after_helloack_message
    
    // double timing = 350000;
    // while (timing > 0){
    //   timing --;
    //   cout<< timing << endl;
    // }

    //peerSocket.WrappedWrite(fakemove.c_str(), fakemove.length());

   // buf[readCount] = '\0'; // make sure buf holds a c style string
   // cout << "(inside activate---)" << buf << "'" << endl;
   // cout << "(inside activate---) sending an action to client" << endl;


   window = gtk_application_window_new (app);
   gtk_window_set_title (GTK_WINDOW (window), "Window");
   gtk_window_set_default_size (GTK_WINDOW (window), 40*6, 40*6);
   gtk_container_set_border_width (GTK_CONTAINER (window), 10);
   grid = gtk_grid_new();
   gtk_container_add (GTK_CONTAINER (window), grid);


   GtkButton *button;
   view_construct_grid(peerSocket);
   

   //four control buttons + 1 quit button
   button = (GtkButton*) gtk_button_new_with_label (NULL);
   gtk_button_set_image(button, gtk_image_new_from_file ("../images/direction/up.png"));
   g_signal_connect (button, "clicked", G_CALLBACK (dosomething), user_data);
   gtk_grid_attach (GTK_GRID (grid), (GtkWidget*) button, 6, 0, 1, 1); // need to fix these parameters

   gtk_widget_show_all (window);

    // double timing = 350000;
    // while (timing > 0){
    //   timing --;
    //   cout<< timing << endl;
    // }
    // cout << "(inside activate---) end of activate" << endl;
    // peerSocket.WrappedWrite(fakemove.c_str(), fakemove.length());


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

    cout << "in main, peerSocket address is " << &peerSocket << endl;

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


        // double timing = 350000;
        // while (timing > 0){
        //   timing --;
        //   cout<< timing << endl;
        // }
        peerSocket.WrappedWrite(helloack_message, 2047);  //TO-DO: not safe here if gameboard is large, convert to string then use str.length()?
      } 
      
    } //end of while }


   // int readCount1 = 0;
   // char buff[2048];
   // readCount1 = peerSocket.WrappedRead(buff, 2047);  //waits for after_helloack_message
   // cout << "Read helloack >>>>" << buff  << endl;
   // buf[readCount] = '\0'; // make sure buf holds a c style string

   cout << "in main, gtk check point 1" << endl;
   GtkApplication *app;
   int status;
   app = gtk_application_new ("candy.crush", G_APPLICATION_FLAGS_NONE);
   g_signal_connect (app, "activate", G_CALLBACK (activate), &peerSocket);
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
