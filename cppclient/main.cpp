//main.cpp
#include <enet/enet.h>
#include "client.hpp"
#include <cstdio>
#include <string.h>

int _getc();

int main()
{
	class MurkClient client;

	client.SetNonblocking(); // not needed?

	client.InitEnet();

	// localhost target for testing
    ENetAddress address;
	enet_address_set_host(&address, "127.0.0.1");
    address.port = 1234;
	
	client.Connect(&address);

	ENetEvent event;

	char input[256];
	int input_ctr = 0;
	while (1) // Main loop 
	{
		while (enet_host_service(client.host, &event, 1) > 0)  // is there an event?
		{
			client.ProcessEvent(&event);
		}

		// Other processing (input etc:)
		int a = _getc();
        if(a < 127 && a > -1) {
            input[input_ctr++] = (char)a;
			printf("%d", a);
			// Key-by-key processing here: 

        }
        if(a == 10 || a == 13) { // ? 
            size_t _l = strlen(input);
            printf("\nString got: %s\n", input);
			// String input processing here: 
            
			// reset input 
			for(int i = 0; i < 256; i++) { input[i] = '\00'; }
            input_ctr = 0;
        }
	}

	client.Disconnect();// De-init 

	return 0;

}


//! Custom getc for non-blocking 
int _getc() 
{
    char ch;
    int error;
    static struct termios Otty, Ntty;

    fflush(stdout);
    tcgetattr(0, &Otty);
    Ntty = Otty;

    Ntty.c_iflag  =  0;     /* input mode       */
    Ntty.c_oflag  =  0;     /* output mode      */

    Ntty.c_cc[VMIN]  = 0;    /* minimum chars to wait for */
    Ntty.c_cc[VTIME] = 1;   /* minimum wait time 0.1s   */

    if ((error = tcsetattr(0, TCSANOW, &Ntty)) == 0) // if no error?
    {
        error = read(0, &ch, 1 );        	  /* get char from stdin */
        error += tcsetattr(0, TCSANOW, &Otty);   /* restore old settings */
    }
    // else return -1
    return (error == 1 ? (int) ch : -1 );
}