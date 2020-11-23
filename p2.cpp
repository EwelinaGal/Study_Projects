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

//PROGRAM 2
using namespace std;

//funkcja do tworzenia laczy i ich otwarcia
int make_open_fifo(char const *myfifo)
{
    mkfifo(myfifo, 0666); // mkfifo funkcja tworzaca lacze

    int fd = open(myfifo, O_RDWR); //open funkcja otwierajaca lacze w danym mode_t

    return fd;
}

int main(int argc, char *argv[], char *envp[])
{
    // lacze1 jako arg1 otwarte do czytania
    // lacze2 otwarte do pisania --jako argument 2
    int fdRd = make_open_fifo(argv[1]); //lacze1 -- do czytania
    int fdWr = make_open_fifo(argv[2]); // lacze2 -- do pisania
    
    while (1)
    {
        fd_set fds; // zestaw fds

        FD_ZERO(&fds); //zestaw zerujacy
        FD_SET(STDIN_FILENO, &fds); // umiesc st din w zestawie
        FD_SET(fdRd, &fds); // umiesc fdrd w zestawie fds

        int maxfd = (fdWr > fdRd) ? fdWr : fdRd; // znajdz najwieksza wartosc deskryptora
        if (select(maxfd + 1, &fds, NULL, NULL, NULL))
        {
            // jestli z lacza stdin
            if (FD_ISSET(STDIN_FILENO, &fds))
            {
                char msg[80];
                if (read(STDIN_FILENO, msg, sizeof(msg))) //czytaj z stdin
                {
                    write(fdWr, msg, strlen(msg) + 1); // pisz do lacza fdwr
                }
            }
            if (FD_ISSET(fdRd, &fds))
            {
                char msg[80];
                if (read(fdRd, msg, sizeof(msg))) //czytaj z lacza fdrd
                {
                    printf("Program 1: %s\n", msg); // wyswietl
                }
            }
        }
    }

    //zamknij lacza
    close(fdRd);
    close(fdWr);

    return 0;
}
