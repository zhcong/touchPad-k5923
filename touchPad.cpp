#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

// =====================  setting start =======================
#define FINGER3_DEVICE "/dev/input/event6"
#define KEY_MAP_LEN 7

char key_map[][KEY_MAP_LEN][256] = {
    {"104", "key Super_L+Up"},      //3 up
    {"109", "key Super_L+Down"},    //3 down
    {"105", "key Alt+Left"},        //3 right
    {"106", "key Alt+Right"},       //3 left
    {"125", ""},       //4 up
    {"32", "click 2"},       //4 down
    {"38", "key Super_L+Tab"},       //4 left
};
// =====================  setting over  =======================

void run_key(char *keys)
{
    char cmd[256];
    strcpy(cmd, "xdotool ");
    strcat(cmd, keys);
    system(cmd);
}

void run_key_map(short code)
{
    for (int i = 0; i != KEY_MAP_LEN; i++)
    {
        if (atoi(key_map[i][0]) == (int)code)
        {
            run_key(key_map[i][1]);
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    struct input_event ev[64];
    int fevdev = -1;
    int result = 0;
    int size = sizeof(struct input_event);
    int rd;
    int value;
    char name[256] = "Unknown";
    char *device = FINGER3_DEVICE;

    fevdev = open(device, O_RDONLY);
    if (fevdev == -1)
    {
        printf("Failed to open event device.\n");
        exit(1);
    }

    result = ioctl(fevdev, EVIOCGNAME(sizeof(name)), name);
    printf("%s\n%s\n", name, device);

    result = ioctl(fevdev, EVIOCGRAB, 1);
    printf("status=[%s]\n", (result == 0) ? "SUCCESS" : "FAILURE");

    while (1)
    {
        if ((rd = read(fevdev, ev, size * 64)) < size)
        {
            break;
        }

        value = ev[0].value;
        if (value != ' ' && ev[1].value == 1 && ev[1].type == 1)
        {
            printf ("Code[%d]\n", (ev[1].code));
            run_key_map(ev[1].code);
        }
    }

    printf("Exiting.\n");
    result = ioctl(fevdev, EVIOCGRAB, 1);
    close(fevdev);
    return 0;
}