#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
//Napisać dwa programy, które komunikują się ze sobą za pomocą łączy:

//Program łącze1 łącze2

//Każdy z programów otwiera łącza lub tworzy je, jeśli nie istnieją. Procesy odczytują wiadomości z terminala (standardowe wejście stdin) oraz z łącza.
//Wiadomości odczytane z terminala przesyłane są przez łącze do drugiego procesu. Wiadomości odczytane z łącza są wyświetlane na terminalu (standardowe wyjście stdout).
//Zalecane jest wykorzystanie funkcji select().

using namespace std;


int make_open_wrfifo(char const *myfifo)
{
    mkfifo(myfifo, 0666);

    int fd = open(myfifo, O_WRONLY);

    return fd; // zwraca otwarty file descriptor[]
}

int make_open_rdfifo(char const *myfifo)
{
    mkfifo(myfifo, 0666);

    int fd = open(myfifo, O_RDONLY);

    return fd; // zwraca otwarty file descriptor[]
}

char msg1[100], msg2[100];

int main(int argc, char *argv[], char *envp[])
{

    printf("Beginning of the program \n");
    printf("PID=%d  RODZIC \n", getpid());

    //char msg1[10];

    fd_set fds;
    int fd1 = make_open_rdfifo(argv[1]);
    int fd2 = make_open_wrfifo(argv[2]);

    cout << msg1 << endl;
    cout << "fd2" << fd2 << endl;

    //write(fd, msg1, strlen(msg1));

    // petla sprawdzajaca lacza
    while (1)
    {

        FD_ZERO(&fds); // set do czyszczenia zestawu
        FD_SET(fd1, &fds);
        FD_SET(STDIN_FILENO, &fds);

        cout << fd1 << fd2 << endl;

        int maxfds = (fd1 > STDIN_FILENO) ? fd1 : STDIN_FILENO;

        // select() = number of max stdin / fds, fd_set if read, fd_set if write, fd_set except, time)
        select(maxfds + 1, &fds, &fds, NULL, NULL);
        {
            printf("Argument 1: %s \n", msg1);
            // FD_ISSET sprawdza czy fd jest w zestawie &fds
            if (FD_ISSET(fd1, &fds))
            {
                read(fd1, msg1, sizeof(msg1));
                printf("Message received from fd1: %s \n", msg1);
            }

            if (FD_ISSET(STDIN_FILENO, &fds))
            {
                read(STDIN_FILENO, msg1, sizeof(msg1));

                printf("Message received from stdin: %s \n", msg1);
            }
            
            write(fd2, msg1, strlen(msg1));
        }
    }

    return 0;
}
