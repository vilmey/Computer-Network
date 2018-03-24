#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
    int open_fd;
    int out_fd;
    char *out_file_name;
    char buffer[10];
    ssize_t written;
    ssize_t bytes_read;
    size_t nbytes;

    if((open_fd = open(argv[1], O_RDONLY)) == -1)
    {
        printf("Could not open file\n");
        perror("");
        return 1;
    }

    out_file_name = strcat(argv[1], ".up");
    
    nbytes = sizeof(buffer);
    
    out_fd = open(out_file_name, O_RDWR | O_CREAT, S_IRWXU);

    do
    {
        if((bytes_read = read(open_fd, buffer, nbytes)) == -1)
        {
            printf("Could not read the file\n");
            return 1;
        }

        if(bytes_read > 0)
        { 
            for(int i = 0; i < bytes_read; ++i)
            {
                buffer[i] = toupper(buffer[i]);
            }
           
            if((written = write(out_fd, buffer, bytes_read)) == -1)
            {
                printf("Unable to write to file\n");
                perror("");
                return 1;
            }
        }

    }while(bytes_read != 0);
    close(out_fd);
    close(open_fd);

    return 1;
}
