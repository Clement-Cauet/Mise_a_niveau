#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE_TAB 10;

typedef struct {
    int queue_fr_tab[5][10];
    int queue_fr_tab_tmp[5][10];
    float queue_fr_time[5][10];

    int queue_en_tab[5][1];
    int queue_wait_en_tab[10];
    float queue_en_time[5][1];
    int queue_wait_en_time[10];

    int mean_fr_tab[1000];
    int mean_en_tab[1000];
}
queue;

int main()
{
    queue queue;
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;

    int queue_fr, queue_en, add_client = 0, mean_fr_i = 0, mean_en_i = 0, nbr_mean = 0;
    int sleep = 1000, duration = 0;
    float waiting_time = 0;
    int size_tab = 10, nbr_tab = 5;

    //Initialise queue fr table
    for (queue_fr = 0; queue_fr < nbr_tab; queue_fr++) {
        for (int i = 0; i < size_tab; i++) {
            queue.queue_fr_tab[queue_fr][i] = 0;
            queue.queue_fr_tab_tmp[queue_fr][i] = queue.queue_fr_tab[queue_fr][i];
            queue.queue_fr_time[queue_fr][i] = 0;
        }
        for (int i = 0; i < 1000; i++) {
            queue.mean_fr_tab[i] = 0;
        }
    }

    //Initialise queue en table
    for (queue_en = 0; queue_en < nbr_tab; queue_en++) {

        queue.queue_en_tab[queue_en][0] = 0;
        queue.queue_en_time[queue_en][0] = 0;

    }

    for (int i = 0; i < size_tab; i++) {
        queue.queue_wait_en_tab[i] = 0;
        queue.queue_wait_en_time[i] = 0;
    }

    for (int i = 0; i < 1000; i++) {
        queue.mean_en_tab[i] = 0;
    }

    coord.X = 0;
    coord.Y = 0;
    consoleDraw(handle, coord, nbr_tab, size_tab);

    while (1) {
        
        waiting_time = (duration * sleep) / 1000;

        //Queue fr
        for (queue_fr = 0; queue_fr < nbr_tab; queue_fr++) {

            coord.Y = queue_fr * 2 + 2;

            //Check number client in the queue
            int pos_add_client = verificateNbrClientFr(queue, queue_fr, nbr_tab, size_tab);

            //Add client with probability in the smallest queue
            if (pos_add_client == queue_fr) {
                add_client = randomReturn(60);
            }

            //Actualise the queue
            for (int i = 0; i < size_tab; i++) {

                coord.X = i + 12;
                SetConsoleCursorPosition(handle, coord);

                if (queue.queue_fr_tab[queue_fr][i] == 0) {

                    //Add a client in the queue
                    if (add_client == 1) {
                        queue.queue_fr_tab[queue_fr][i] = 1;
                        printf("*");

                        if (i > 0) {
                            for (int j = i; queue.queue_fr_tab[queue_fr][j - 1] == 0 && j > 0; j--) {
                                queue.queue_fr_tab[queue_fr][j - 1] = queue.queue_fr_tab[queue_fr][j];
                                queue.queue_fr_tab[queue_fr][j] = 0;

                                coord.X = j + 12 - 1;
                                SetConsoleCursorPosition(handle, coord);
                                printf("*");

                                coord.X = j + 12;
                                SetConsoleCursorPosition(handle, coord);
                                printf(" ");
                            }
                        }
                        add_client = 0;
                    }
                }
                else {
                    queue.queue_fr_time[queue_fr][i]++;
                }
            }

            //Avance the queue with probability
            int move_queue = randomReturn(10);

            for (int i = 0; i < size_tab; i++) {
                if (move_queue == 1) {
                    if (queue.queue_fr_tab[queue_fr][i] == 1) {
                        if (i > 0) {
                            coord.X = i + 12 - 1;
                            SetConsoleCursorPosition(handle, coord);
                            printf("*");
                        }

                        coord.X = i + 12;
                        SetConsoleCursorPosition(handle, coord);
                        printf(" ");

                        if (i == size_tab - 1) {
                            queue.queue_fr_tab[queue_fr][i] = 0;
                            queue.queue_fr_time[queue_fr][i] = 0;
                        }
                        else {
                            if (i == 0 && queue.queue_fr_time[queue_fr][i] != 0) {
                                queue.mean_fr_tab[mean_fr_i] = queue.queue_fr_time[queue_fr][i];
                                mean_fr_i++;
                                queue.queue_fr_time[queue_fr][i] = 0;
                            }
                            queue.queue_fr_tab[queue_fr][i] = queue.queue_fr_tab[queue_fr][i + 1];
                            queue.queue_fr_time[queue_fr][i] = queue.queue_fr_time[queue_fr][i + 1];
                        }
                    }
                }
            }
        }

        //Mean time fr
        float mean_fr = 0;
        for (nbr_mean = 0; queue.mean_fr_tab[nbr_mean] != 0; nbr_mean++) {
            mean_fr += queue.mean_fr_tab[nbr_mean];
        }

        if (nbr_mean != 0) {
            mean_fr /= nbr_mean;
        }

        coord.X = 0;
        coord.Y = 12;
        SetConsoleCursorPosition(handle, coord);
        printf("Temps moyen : %f s", mean_fr);

        //Queue en
        coord.Y = 20;

        add_client = randomReturn(60);

        //Actualise the queue
        for (int i = 0; i < size_tab; i++) {

            coord.X = i + 15;
            SetConsoleCursorPosition(handle, coord);

            if (queue.queue_wait_en_tab[i] == 0) {

                //Add a client in the queue
                if (add_client == 1) {
                    queue.queue_wait_en_tab[i] = 1;
                    printf("*");

                    if (i > 0) {
                        for (int j = i; queue.queue_wait_en_tab[j - 1] == 0 && j > 0; j--) {
                            queue.queue_wait_en_tab[j - 1] = queue.queue_wait_en_tab[j];
                            queue.queue_wait_en_tab[j] = 0;

                            coord.X = j + 15 - 1;
                            SetConsoleCursorPosition(handle, coord);
                            printf("*");

                            coord.X = j + 15;
                            SetConsoleCursorPosition(handle, coord);
                            printf(" ");
                        }
                    }
                    add_client = 0;
                }
            }
            else {
                queue.queue_wait_en_time[i]++;
            }
        }

        for (queue_en = 0; queue_en < nbr_tab; queue_en++) {

            coord.Y = queue_en * 2 + 16;

            //Avance the queue with probability
            int move_queue = randomReturn(10);

            if (move_queue == 1) {
                if (queue.queue_en_tab[queue_en][0] == 1 && queue.queue_en_time[queue_en][0] != 0) {
                    queue.mean_en_tab[mean_en_i] = queue.queue_en_time[queue_en][0];
                    mean_en_i++;
                    queue.queue_en_time[queue_en][0] = 0;
                }

                queue.queue_en_tab[queue_en][0] = 0;
            }

            if (queue.queue_en_tab[queue_en][0] == 1) {
                queue.queue_en_time[queue_en][0]++;
            }

            if (queue.queue_en_tab[queue_en][0] == 0) {
                coord.X = 12;
                SetConsoleCursorPosition(handle, coord);
                printf(" ");

                queue.queue_en_tab[queue_en][0] = queue.queue_wait_en_tab[0];
                queue.queue_en_time[queue_en][0] = queue.queue_wait_en_time[0];

                if (queue.queue_en_tab[queue_en][0] == 1) {
                    coord.X = 12;
                    SetConsoleCursorPosition(handle, coord);
                    printf("*");
                }

                coord.Y = 20;
                for (int i = 0; i < size_tab; i++) {

                    if (queue.queue_wait_en_tab[i] == 1) {
                        if (i > 0) {
                            coord.X = i + 15 - 1;
                            SetConsoleCursorPosition(handle, coord);
                            printf("*");
                        }

                        coord.X = i + 15;
                        SetConsoleCursorPosition(handle, coord);
                        printf(" ");
                    }

                    if (i == size_tab - 1) {
                        queue.queue_wait_en_tab[i] = 0;
                        queue.queue_wait_en_time[i] = 0;
                    }
                    else {
                        queue.queue_wait_en_tab[i] = queue.queue_wait_en_tab[i + 1];
                        queue.queue_wait_en_time[i] = queue.queue_wait_en_tab[i + 1];
                    }
                }
            }
            else {
                coord.X = 12;
                SetConsoleCursorPosition(handle, coord);
                printf("*");
            }
        }

        //Mean time en
        float mean_en = 0;
        for (nbr_mean = 0; queue.mean_en_tab[nbr_mean] != 0; nbr_mean++) {
            mean_en += queue.mean_en_tab[nbr_mean];
        }

        if (nbr_mean != 0) {
            mean_en /= nbr_mean;
        }
        
        coord.X = 0;
        coord.Y = 26;
        SetConsoleCursorPosition(handle, coord);
        printf("Temps moyen : %f s", mean_en);

        duration++;
        Sleep(sleep);
    }
    return 0;
}

//Check number client in the queue fr
int verificateNbrClientFr(queue queue, int queue_fr, int nbr_tab, int size_tab) {

    int nbr_client_tab[5] ;

    for (queue_fr = 0; queue_fr < nbr_tab; queue_fr++) {
        int nbr_client = 0;
        for (int i = 0; i < size_tab; i++) {
            if (queue.queue_fr_tab[queue_fr][i] == 1) {
                nbr_client += 1;
            }
        }
        nbr_client_tab[queue_fr] = nbr_client;
    }

    return positionMin(nbr_client_tab, nbr_tab);
}

//Check number client in the queue en
int verificateNbrClientEn(queue queue, int queue_en, int nbr_tab, int size_tab) {

    int nbr_client_tab[5];

    for (queue_en = 0; queue_en < nbr_tab; queue_en++) {
        int nbr_client = 0;
        for (int i = 0; i < size_tab; i++) {
            if (queue.queue_en_tab[queue_en][i] == 1) {
                nbr_client += 1;
            }
        }
        nbr_client_tab[queue_en] = nbr_client;
    }

    return positionMin(nbr_client_tab, nbr_tab);
}

//Return the queue with the least client
int positionMin(int nbr_client_tab[], int nbr_tab) {

    int pos_min = 0;
    int val_min = nbr_client_tab[0];

    for (int i = 0; i < nbr_tab; i++) {
        if (nbr_client_tab[i] < val_min)
        {
            val_min = nbr_client_tab[i];
            pos_min = i;
        }
    }

    return pos_min;
}

//Return true or false value
int randomReturn(int probability) {

    if (randomGenerate(100) <= probability) {
        return 1;
    }
    else {
        return 0;
    }
}

//Return a random value
int randomGenerate(int max) {

    int r = 1 + (rand() % max);

    return r;
}

int consoleDraw(HANDLE handle, COORD coord, int nbr_tab, int size_tab) {

    //Console draw queue fr
    coord.X = 10;
    coord.Y = 0;
    SetConsoleCursorPosition(handle, coord);
    printf("File Francaise");

    for (int i = 0; i < nbr_tab; i++) {

        coord.X = 0;
        coord.Y = i * 2 + 2;
        SetConsoleCursorPosition(handle, coord);
        printf("Guichet %d |", i + 1);

        coord.X = 0;
        coord.Y = i * 2 + 1;
        SetConsoleCursorPosition(handle, coord);
        for (int j = 0; j < size_tab * 3; j++) {
            printf("-");
        }
    }

    //Console draw queue en
    coord.X = 10;
    coord.Y = 14;
    SetConsoleCursorPosition(handle, coord);
    printf("File Anglaise");

    for (int i = 0; i < nbr_tab; i++) {

        coord.X = 0;
        coord.Y = i * 2 + 2 + 14;
        SetConsoleCursorPosition(handle, coord);
        printf("Guichet %d |", i + 1);

        coord.X = 0;
        coord.Y = i * 2 + 1 + 14;
        SetConsoleCursorPosition(handle, coord);
        for (int j = 0; j < size_tab; j++) {
            printf("-");
        }
    }
}