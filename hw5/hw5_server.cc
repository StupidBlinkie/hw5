#include <iostream>
#include <string>
#include <cstdlib>

#include "ServerSocket.h"
#include "ClientSocket.h"

#include "../hw4/hw4/hw4_view.h"
//#include "hw4_controller.h"
extern "C"{
    #include "../jansson/include/jansson.h"
    #include <gtk/gtk.h>
}

using namespace std;
string fake = "{\"action\": \"helloack\", \"gameinstance\": {\"gamedef\": {\"extensioncolor\": {\"rows\": 6, \"columns\": 6, \"data\": [0, 3, 1, 2, 2, 1, 0, 1, 3, 0, 0, 3, 1, 3, 2, 0, 1, 2, 3, 3, 3, 3, 0, 0, 0, 3, 1, 0, 1, 0, 1, 1, 3, 3, 3, 2]}, \"boardstate\": {\"rows\": 6, \"columns\": 6, \"data\": [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]}, \"movesallowed\": 24, \"gameid\": 66, \"colors\": 4}, \"gamestate\": {\"movesmade\": 0, \"currentscore\": 18, \"extensionoffset\": [12, 22, 11, 13, 8, 7], \"boardcandies\": {\"rows\": 6, \"columns\": 6, \"data\": [{\"color\": 0, \"type\": 0}, {\"color\": 3, \"type\": 0}, {\"color\": 1, \"type\": 0}, {\"color\": 2, \"type\": 0}, {\"color\": 2, \"type\": 0}, {\"color\": 1, \"type\": 0}, {\"color\": 0, \"type\": 0}, {\"color\": 1, \"type\": 0}, {\"color\": 3, \"type\": 0}, {\"color\": 2, \"type\": 0}, {\"color\": 0, \"type\": 0}, {\"color\": 3, \"type\": 0}, {\"color\": 1, \"type\": 0}, {\"color\": 1, \"type\": 0}, {\"color\": 2, \"type\": 0}, {\"color\": 0, \"type\": 0}, {\"color\": 1, \"type\": 0}, {\"color\": 2, \"type\": 0}, {\"color\": 1, \"type\": 0}, {\"color\": 3, \"type\": 0}, {\"color\": 1, \"type\": 0}, {\"color\": 3, \"type\": 0}, {\"color\": 1, \"type\": 0}, {\"color\": 0, \"type\": 0}, {\"color\": 0, \"type\": 0}, {\"color\": 1, \"type\": 0}, {\"color\": 2, \"type\": 0}, {\"color\": 0, \"type\": 0}, {\"color\": 2, \"type\": 0}, {\"color\": 2, \"type\": 0}, {\"color\": 1, \"type\": 0}, {\"color\": 3, \"type\": 0}, {\"color\": 1, \"type\": 0}, {\"color\": 2, \"type\": 0}, {\"color\": 0, \"type\": 0}, {\"color\": 1, \"type\": 0}]}, \"boardstate\": {\"rows\": 6, \"columns\": 6, \"data\": [1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1]}}}}";
string Message_game  = "from server: nothing for now";
string Message_helloack = "helloack";

void usage(const char *exeName) {
  cout << "Usage: " << exeName << " [port]" << endl;
  cout << "  Creates a server socket on port, if given," << endl
       << "  or on random port, if not." << endl;
  exit(1);
}

int main(int argc, char *argv[]) {

  if ( argc != 1 && argc != 2 ) usage(argv[0]);
  
  int port = 0;
  try {
    if ( argc == 2 ) port = stoi(argv[1]);
  } catch (...) {
    usage(argv[0]);
  }

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

    char buf[1024];
    int readCount;
    bool received_hello = false;
    bool said_hello = false;


    while ( readCount = peerSocket.WrappedRead(buf, 1023) ) {
      // write to stdout
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
        //cout << "received hello from client.." << endl;
        //peerSocket.WrappedWrite("message from server: helloack", 100);
        said_hello = true;
      } 
      
      if (said_hello){
        peerSocket.WrappedWrite(fake.c_str(), fake.length());
      }




      //if some bool indicates bye, clicking exit button
      //  peerSocket.WrappedWrite("bye", 100);
      

    } //end of while }


    


  } catch (string errString) {         //end of try }
    cerr << errString << endl;
    return EXIT_FAILURE;
  }
  
  cout << "Closing" << endl;
  
  return EXIT_SUCCESS;
}



// void sendToClient(hw5_net::ServerSocket sock, string message){
//       char buf[1024];
//       sock.WrappedWrite(message.c_str(), message.length());
//       int readCount = sock.WrappedRead(buf, 1023);
//       buf[readCount] = '\0';
//       cout << "\tSent to client '" << buf << "'" << endl;
// }



///---------------server helper methods----------------------///
char* constructHelloackMsg(){
  //deserialize test25by5.json
  //add filed  "action: helloack" 
}