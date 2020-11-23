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

//PROGRAM 1
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
    // Program 1 czyta z lacza1 fdrd, jako pierwszy argument
    // lacze2 do pisania przesyla do drugiego wiadomosc -- argument 2
    int fdWr = make_open_fifo(argv[2]); // lacze2 -- do pisania
    int fdRd = make_open_fifo(argv[1]); // lacze1 -- do czytania

    while (1)
    {
        fd_set fds; //stworz zestaw laczy fds

        FD_ZERO(&fds); //zestaw zerujacy lacza
        FD_SET(STDIN_FILENO, &fds); // umiesc stdin do zestawu
        FD_SET(fdRd, &fds);// umiesc lacze fdrd  do zestawu

        int maxfd = (fdWr > fdRd) ? fdWr : fdRd; //wylicz najwiekszy deskryptor lacza
        if (select(maxfd + 1, &fds, NULL, NULL, NULL)) //sprawdz, ktore lacza sa gotowe do odczytu/zapisu
        {
            //jesli cos jest na laczu stdin
            if (FD_ISSET(STDIN_FILENO, &fds)) 
            {
                char msg[80];
                if (read(STDIN_FILENO, msg, sizeof(msg))) // czytaj z stdin
                { 
                    write(fdWr, msg, strlen(msg) + 1); //pisz do fdWr
                }
            }
            //jesli cos jest na laczu fdrd
            if (FD_ISSET(fdRd, &fds))
            {
                char msg[80];
                if (read(fdRd, msg, sizeof(msg))) // czytaj
                {
                    printf("Program 2: %s\n", msg); // wyswietl wiadomosc
                }
            }
        }
    }

    // zamknij lacza
    close(fdRd);
    close(fdWr);

    return 0;
}
