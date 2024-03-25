// getc.cpp
#include <cstdio>
#include <string.h>
#include <termios.h>
#include <unistd.h>

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