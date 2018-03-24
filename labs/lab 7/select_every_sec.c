#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main ()
{
    struct timeval tv;
    
    tv.tv_sec = 1;
    
    while (1)
    {
        if( select(0, NULL, NULL, NULL, &tv) == -1 )
            perror("select()");
        
        fflush(stdout);
    }

    return 0;
}