#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main ()
{
    fd_set rfds;
    struct timeval tv;    
    int retval;
    char str[20];
    fflush(NULL);

    FD_ZERO(&rfds);
    FD_SET(fileno(stdin), &rfds);

    while (1)
    {             
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        retval = select(1, &rfds, NULL, NULL, &tv); 

        if (retval == -1)
        {
	    perror("");
            return 1;
        }
        else if(retval)
        {
            fgets(str, 20, stdin);
	    printf("%s", str); 
        }
        else
        {
            break;
        }
    }

    return 0;
}
