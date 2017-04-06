#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>           /* O_RDWR */
#include <unistd.h>             /* read/write */
#include <fcntl.h>              /* open */
#define MAXN 128

char buffer[MAXN];
int main(void)
{
    int fd = open("/dev/plypy_chrdev", O_RDWR);
    while (1) {
        printf("Write something:\n");
        memset(buffer, 0, sizeof(buffer));
        gets(buffer);
        write(fd, buffer, strlen(buffer)+1);
    }
    return 0;
}
