#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int open_fd;
    char buffer[10];
    ssize_t bytes_read; 
    size_t nbytes;

    if((open_fd = open(argv[1], O_RDONLY)) == -1)
    {
        printf("Could not open file\n");
        return 1;
    }

    nbytes = sizeof(buffer);

    do
    {
        if((bytes_read = read(open_fd, buffer, nbytes)) == -1)
        {
            printf("Could not read the file\n");
            return 1;
        }
        
        if(bytes_read > 0)
        {
   
            for(int i = 0; i < nbytes; ++i)
            {
                printf("%c", buffer[i]);
            }
        }

    }while(bytes_read != 0);

    close(open_fd);

    return 1;
}
