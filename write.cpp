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


/*
void write_from_fifo(int fd)
{
    char msg1[100]; //msg1 do 100 znakow
    

    if (write(fd, msg1, sizeof(msg1)) < 0) // funkcja read z lacza, wiadomosc do 100 znakow, jesli mniejsze od zera, tzn stdin -> odczytaj z lacza
        perror("read");                          // jesli blad: Bad file descriptor
    
    else
    {
        printf("msg1 to send: %s \n", msg1);
    }
};

void read_from_fifo(int fd)
{
    char msg1[10];                           //msg1 do 100 znakow
    if (read(fd, msg1, sizeof(msg1)) < 0) // funkcja read z lacza
        perror("read");                         // jesli blad: Bad file descriptor
    
    else
    {
        printf("msg1 received: %s \n", msg1);
    }
};
*/

int make_open_wrfifo(char const *myfifo)
{
    mkfifo(myfifo, 0666);

    int fd = open(myfifo, O_WRONLY);

    return fd; // zwraca otwarty file descriptor[]
}

int make_open_rdfifo(char const *myfifo)
{
    mkfifo(myfifo, 0666);

    int fd = open(myfifo, O_RDONLY | O_NONBLOCK);

    return fd; // zwraca otwarty file descriptor[]
}

int main(int argc, char *argv[], char *envp[])
{

    printf("Beginning of the program \n");
    printf("PID=%d  RODZIC \n", getpid());

    char msg1[10], msg2[10];
    cin >> msg1;
    cin >> msg2;
    cout << "Msg1: " << msg1 << "Msg2: " << msg2 << endl;

    int fd1 = make_open_wrfifo(argv[1]);
    int fd2 = make_open_wrfifo(argv[2]);

    //int stdin = make_open_wrfifo(stdin);
    //int stdout = make_open_rdfifo(stdout);

    // petla sprawdzajaca lacza
    int maxfds = (fd1 > fd2) ? fd1 : fd2;

    fd_set fds;

    while (1)
    {

        FD_ZERO(&fds); // set do czyszczenia zestawu
        FD_SET(fd1, &fds);
        FD_SET(fd2, &fds);
        FD_SET(STDIN_FILENO, &fds);
        FD_SET(STDOUT_FILENO, &fds);

        // select() = number of max stdin / fds, fd_set if read, fd_set if write, fd_set except, time)
        select(maxfds + 1, &fds, &fds, NULL, NULL);
        // FD_ISSET sprawdza czy fd jest w zestawie &fds

        if (FD_ISSET(fd1, &fds))
        {
            printf("Msg1: \n", msg1);
            write(fd1, msg1, strlen(msg1));
        }

        if (FD_ISSET(fd2, &fds))
        {
            printf("Msg2: \n", msg2);
            write(fd2, msg2, sizeof(msg2));
        }

        if (FD_ISSET(STDIN_FILENO, &fds))
        {
            printf("Msg1: \n", msg1);
            write(STDIN_FILENO, msg1, strlen(msg1));
        }

        if (FD_ISSET(STDIN_FILENO, &fds))
        {
            printf("Msg1: \n", msg2);
            write(STDIN_FILENO, msg2, strlen(msg2));
        }
    }

    /* unlink usuwa potok
        if (unlink(fd1) < 0)
        perror("unlink");

        if (unlink(writefd2) < 0)
        perror("unlink");

        if (unlink(stdin) < 0)
        perror("unlink");
*/

    return 0;
}

