#define _CRT_SECURE_NO_WARNINGS
#include <SFML/Graphics.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SLEEP               1000
#define SIZE_TAB            10
#define NB_TAB              5
#define SIZE_BLOC           34
#define NB_BLOCS_WIDTH      15
#define NB_BLOCS_HEIGHT     14
#define SIZE_MARGIN_QUEUE   SIZE_BLOC * 2
#define SIZE_MARGIN_WAIT    SIZE_BLOC * 4
#define WIDTH_WINDOW        SIZE_BLOC * NB_BLOCS_WIDTH
#define HEIGHT_WINDOW       SIZE_BLOC * NB_BLOCS_HEIGHT

typedef struct {

    //Queue table fr
    int queue_fr_tab[NB_TAB][SIZE_TAB];
    float queue_fr_time[NB_TAB][SIZE_TAB];

    //Queue table en
    int queue_en_tab[NB_TAB][1];
    int queue_wait_en_tab[SIZE_TAB];
    float queue_en_time[NB_TAB][1];
    int queue_wait_en_time[SIZE_TAB];

    //Mean table
    int mean_fr_tab[1000];
    int mean_en_tab[1000];

}
queue;

//Prototype function
float meanTimeFr(HANDLE handle, COORD coord, queue queue);
float meanTimeEn(HANDLE handle, COORD coord, queue queue);
float openGraphic(int duration, float mean_fr, float mean_en);
float traceCourbe(int duration, float mean_fr, float mean_en);

int main()
{
    queue queue;
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    coord.X = 0;
    coord.Y = 0;

    int queue_fr, queue_en, mean_fr_i = 0, mean_en_i = 0;
    int duration = 0;
    float mean_time_fr = 0, mean_time_en = 0;

    sfVideoMode mode = { WIDTH_WINDOW, HEIGHT_WINDOW, 32 };
    sfRenderWindow* window;
    sfTexture* void_texture, * desk_texture, * client_texture, * seller_texture, * queue_fr_text, * queue_en_text;
    sfSprite* sprite;
    sfEvent event;

    //Create window
    window = sfRenderWindow_create(mode, "Mise a niveau", sfResize | sfClose, NULL);
    if (!window)
        return EXIT_FAILURE;

    //Charge all texture
    void_texture = sfTexture_createFromFile("src/img/void.png", NULL);
    if (!void_texture)
        return EXIT_FAILURE;

    desk_texture = sfTexture_createFromFile("src/img/desk.png", NULL);
    if (!desk_texture)
        return EXIT_FAILURE;

    seller_texture = sfTexture_createFromFile("src/img/seller.png", NULL);
    if (!seller_texture)
        return EXIT_FAILURE;

    client_texture = sfTexture_createFromFile("src/img/client.png", NULL);
    if (!desk_texture)
        return EXIT_FAILURE;

    queue_fr_text = sfTexture_createFromFile("src/img/queue_fr_text.png", NULL);
    if (!desk_texture)
        return EXIT_FAILURE;

    queue_en_text = sfTexture_createFromFile("src/img/queue_en_text.png", NULL);
    if (!desk_texture)
        return EXIT_FAILURE;

    sprite = sfSprite_create();

    //Draw in the console
    consoleDraw(handle, coord);

    //Initialise queue fr table
    for (queue_fr = 0; queue_fr < NB_TAB; queue_fr++) {
        for (int i = 0; i < SIZE_TAB; i++) {
            queue.queue_fr_tab[queue_fr][i] = 0;
            queue.queue_fr_time[queue_fr][i] = 0;
        }
        for (int i = 0; i < 1000; i++) {
            queue.mean_fr_tab[i] = 0;
        }
    }

    //Initialise queue en table
    for (queue_en = 0; queue_en < NB_TAB; queue_en++) {

        queue.queue_en_tab[queue_en][0] = 0;
        queue.queue_en_time[queue_en][0] = 0;

    }

    for (int i = 0; i < SIZE_TAB; i++) {
        queue.queue_wait_en_tab[i] = 0;
        queue.queue_wait_en_time[i] = 0;
    }

    //Initialise mean table
    for (int i = 0; i < 1000; i++) {
        queue.mean_en_tab[i] = 0;
    }

    while (sfRenderWindow_isOpen(window)) {

        //All event in the window
        while (sfRenderWindow_pollEvent(window, &event)) {

            //Event close window
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);

        }

        //Draw in the window
        windowDraw(window, sprite, queue_fr_text, queue_en_text);

/* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv Queue fr vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
        
        if ("Queue fr") {

            for (queue_fr = 0; queue_fr < NB_TAB; queue_fr++) {

                //Check number client in the queue
                int pos_add_client = verificateNbrClient(queue, queue_fr);

                //Add client with probability in the smallest queue
                int add_client = 0;
                if (pos_add_client == queue_fr) {

                    add_client = randomReturn(60);

                }

                //Actualise the queue
                for (int i = 0; i < SIZE_TAB; i++) {

                    if (queue.queue_fr_tab[queue_fr][i] == 0) {

                        //Add a client in the queue
                        if (add_client == 1) {

                            queue.queue_fr_tab[queue_fr][i] = 1;

                            if (i > 0) {

                                for (int j = i; queue.queue_fr_tab[queue_fr][j - 1] == 0 && j > 0; j--) {

                                    queue.queue_fr_tab[queue_fr][j - 1] = queue.queue_fr_tab[queue_fr][j];
                                    queue.queue_fr_tab[queue_fr][j] = 0;

                                }

                            }

                            add_client = 0;

                        }
                    }
                    else {

                        //Add time of the client on the queue
                        queue.queue_fr_time[queue_fr][i]++;

                    }

                }

                //Avance the queue with probability
                int move_queue = randomReturn(10);

                for (int i = 0; i < SIZE_TAB; i++) {

                    if (move_queue == 1) {

                        if (queue.queue_fr_tab[queue_fr][i] == 1) {

                            if (i == SIZE_TAB - 1) {

                                queue.queue_fr_tab[queue_fr][i]     = 0;
                                queue.queue_fr_time[queue_fr][i]    = 0;

                            }
                            else {

                                //Add the mean time of clients when a client go out of the queue
                                if (i == 0 && queue.queue_fr_time[queue_fr][i] != 0) {

                                    queue.mean_fr_tab[mean_fr_i]        = queue.queue_fr_time[queue_fr][i];
                                    queue.queue_fr_time[queue_fr][i]    = 0;
                                    mean_fr_i++;

                                }

                                queue.queue_fr_tab[queue_fr][i]     = queue.queue_fr_tab[queue_fr][i + 1];
                                queue.queue_fr_time[queue_fr][i]    = queue.queue_fr_time[queue_fr][i + 1];

                            }

                        }

                    }

                }

                //Actualise draw in the console
                consoleQueueFrDraw(queue, queue_fr, handle, coord);

                //Actualise draw in the window
                windowQueueFrDraw(queue, queue_fr, window, sprite, void_texture, desk_texture, client_texture, seller_texture);

            }

            //Actualise the mean time fr
            mean_time_fr = meanTimeFr(handle, coord, queue);

        }

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Queue fr ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

/* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv Queue En vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */

        if ("Queue en") {

            int add_client = randomReturn(60);

            //Actualise the queue
            for (int i = 0; i < SIZE_TAB; i++) {

                if (queue.queue_wait_en_tab[i] == 0) {

                    //Add a client in the queue
                    if (add_client == 1) {

                        queue.queue_wait_en_tab[i] = 1;

                        if (i > 0) {

                            for (int j = i; queue.queue_wait_en_tab[j - 1] == 0 && j > 0; j--) {

                                queue.queue_wait_en_tab[j - 1]  = queue.queue_wait_en_tab[j];
                                queue.queue_wait_en_tab[j]      = 0;

                            }

                        }

                        add_client = 0;

                    }
                }
                else {
                    //Add time of the client on the queue
                    queue.queue_wait_en_time[i]++;

                }
            }

            for (queue_en = 0; queue_en < NB_TAB; queue_en++) {

                //Avance the queue with probability
                int move_queue = randomReturn(10);

                if (move_queue == 1) {

                    if (queue.queue_en_tab[queue_en][0] == 1 && queue.queue_en_time[queue_en][0] != 0) {

                        queue.mean_en_tab[mean_en_i]        = queue.queue_en_time[queue_en][0];
                        queue.queue_en_time[queue_en][0]    = 0;
                        mean_en_i++;

                    }

                    queue.queue_en_tab[queue_en][0] = 0;

                }

                if (queue.queue_en_tab[queue_en][0] == 0) {

                    queue.queue_en_tab[queue_en][0]     = queue.queue_wait_en_tab[0];
                    queue.queue_en_time[queue_en][0]    = queue.queue_wait_en_time[0];

                    for (int i = 0; i < SIZE_TAB; i++) {

                        if (i == SIZE_TAB - 1) {

                            queue.queue_wait_en_tab[i]  = 0;
                            queue.queue_wait_en_time[i] = 0;

                        }
                        else {

                            queue.queue_wait_en_tab[i]  = queue.queue_wait_en_tab[i + 1];
                            queue.queue_wait_en_time[i] = queue.queue_wait_en_tab[i + 1];

                        }

                    }

                }
                else {

                    //Add time of the client on the queue
                    queue.queue_en_time[queue_en][0]++;

                }

                //Actualise draw in the console
                consoleQueueEnDraw(queue, queue_en, handle, coord);

                //Actualise draw in the window
                windowQueueEnDraw(queue, queue_en, window, sprite, void_texture, desk_texture, client_texture, seller_texture);

            }

            //Actualise the mean time en
            mean_time_en = meanTimeEn(handle, coord, queue);

        }
                
/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Queue En ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

        openGraphic(duration, mean_time_fr, mean_time_en);

        //Actualise the display of the window
        sfRenderWindow_display(window);

        duration++;

        Sleep(SLEEP);
    }

    return 0;

}

//Check number client in the queue fr
int verificateNbrClient(queue queue, int queue_fr) {

    int nbr_client_tab[5] ;

    for (queue_fr = 0; queue_fr < NB_TAB; queue_fr++) {
        int nbr_client = 0;
        for (int i = 0; i < SIZE_TAB; i++) {
            if (queue.queue_fr_tab[queue_fr][i] == 1) {
                nbr_client += 1;
            }
        }
        nbr_client_tab[queue_fr] = nbr_client;
    }

    return positionMin(nbr_client_tab);
}

//Return the queue with the least client
int positionMin(int nbr_client_tab[]) {

    int pos_min = 0;
    int val_min = nbr_client_tab[0];

    for (int i = 0; i < NB_TAB; i++) {
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

//Mean time fr
float meanTimeFr(HANDLE handle, COORD coord, queue queue) {
    
    float mean_fr = 0;
    int nbr_mean = 0;

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

    return mean_fr;
}

//Mean time en
float meanTimeEn(HANDLE handle, COORD coord, queue queue) {
    
    float mean_en = 0;
    int nbr_mean = 0;

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

    return mean_en;
}

//Open graphic file
float openGraphic(int duration, float mean_fr, float mean_en) {

    traceCourbe(duration, mean_fr, mean_en);

    return 1;
}

//Trace courbe
float traceCourbe(int duration, float mean_fr, float mean_en) {



    return 1;
}

//Draw in the console
int consoleDraw(HANDLE handle, COORD coord) {

    //Console draw queue fr
    coord.X = 10;
    coord.Y = 0;
    SetConsoleCursorPosition(handle, coord);
    printf("File Francaise");

    for (int i = 0; i < NB_TAB; i++) {

        coord.X = 0;
        coord.Y = i * 2 + 2;
        SetConsoleCursorPosition(handle, coord);
        printf("Guichet %d |", i + 1);

        coord.X = 0;
        coord.Y = i * 2 + 1;
        SetConsoleCursorPosition(handle, coord);
        for (int j = 0; j < SIZE_TAB * 3; j++) {
            printf("-");
        }
    }

    //Console draw queue en
    coord.X = 10;
    coord.Y = 14;
    SetConsoleCursorPosition(handle, coord);
    printf("File Anglaise");

    for (int i = 0; i < NB_TAB; i++) {

        coord.X = 0;
        coord.Y = i * 2 + 2 + 14;
        SetConsoleCursorPosition(handle, coord);
        printf("Guichet %d |", i + 1);

        coord.X = 0;
        coord.Y = i * 2 + 1 + 14;
        SetConsoleCursorPosition(handle, coord);
        for (int j = 0; j < SIZE_TAB; j++) {
            printf("-");
        }
    }
    return 1;
}

//Draw queue fr in the console
int consoleQueueFrDraw(queue queue, int queue_fr, HANDLE handle, COORD coord) {

    coord.Y = queue_fr * 2 + 2;

    for (int i = 0; i < SIZE_TAB; i++) {
        
        if (queue.queue_fr_tab[queue_fr][i] == 0) {
            coord.X = i + 12;
            SetConsoleCursorPosition(handle, coord);
            printf(" ");
        }
        else {
            coord.X = i + 12;
            SetConsoleCursorPosition(handle, coord);
            printf("*");
        }
    }
    return 1;
}

//Draw queue en in the console
int consoleQueueEnDraw(queue queue, int queue_en, HANDLE handle, COORD coord) {

    coord.Y = queue_en * 2 + 16;

    if (queue.queue_en_tab[queue_en][0] == 0) {
        coord.X = 12;
        SetConsoleCursorPosition(handle, coord);
        printf(" ");
    }
    else {
        coord.X = 12;
        SetConsoleCursorPosition(handle, coord);
        printf("*");
    }

    coord.Y = 20;

    for (int i = 0; i < SIZE_TAB; i++) {
        
        if (queue.queue_wait_en_tab[i] == 0) {
            coord.X = i + 15;
            SetConsoleCursorPosition(handle, coord);
            printf(" ");
        }
        else {
            coord.X = i + 15;
            SetConsoleCursorPosition(handle, coord);
            printf("*");
        }
    }
    return 1;
}

//Draw in the window
int windowDraw(sfRenderWindow* window, sfSprite* sprite, sfTexture* queue_fr_text, sfTexture* queue_en_text) {
    sfVector2f Position;

    Position.y = 0;
    Position.x = WIDTH_WINDOW / 2 - SIZE_BLOC * 2;
    sfSprite_setTexture(sprite, queue_fr_text, sfTrue);
    sfSprite_setPosition(sprite, Position);
    sfRenderWindow_drawSprite(window, sprite, NULL);

    Position.y = SIZE_BLOC * 7;
    Position.x = WIDTH_WINDOW / 2 - SIZE_BLOC * 2;
    sfSprite_setTexture(sprite, queue_en_text, sfTrue);
    sfSprite_setPosition(sprite, Position);
    sfRenderWindow_drawSprite(window, sprite, NULL);
    
    return 1;
}

//Draw queue fr in the window
int windowQueueFrDraw(queue queue, int queue_fr, sfRenderWindow* window, sfSprite* sprite, sfTexture* void_texture, sfTexture* desk_texture, sfTexture* client_texture, sfTexture* seller_texture) {
    sfVector2f Position;

    for (int i = 0; i < SIZE_TAB; i++) {
        
        Position.y = queue_fr * SIZE_BLOC + SIZE_BLOC;

        Position.x = 0;
        sfSprite_setTexture(sprite, seller_texture, sfTrue);
        sfSprite_setPosition(sprite, Position);
        sfRenderWindow_drawSprite(window, sprite, NULL);

        Position.x = SIZE_BLOC;
        sfSprite_setTexture(sprite, desk_texture, sfTrue);
        sfSprite_setPosition(sprite, Position);
        sfRenderWindow_drawSprite(window, sprite, NULL);

        if (queue.queue_fr_tab[queue_fr][i] == 0) {
            Position.x = i * SIZE_BLOC + SIZE_MARGIN_QUEUE;
            sfSprite_setTexture(sprite, void_texture, sfTrue);
            sfSprite_setPosition(sprite, Position);
            sfRenderWindow_drawSprite(window, sprite, NULL);
        }
        else {
            Position.x = i * SIZE_BLOC + SIZE_MARGIN_QUEUE;
            sfSprite_setTexture(sprite, client_texture, sfTrue);
            sfSprite_setPosition(sprite, Position);
            sfRenderWindow_drawSprite(window, sprite, NULL);
        }
    }
    return 1;
}

//Draw queue en in the window
int windowQueueEnDraw(queue queue, int queue_en, sfRenderWindow* window, sfSprite* sprite, sfTexture* void_texture, sfTexture* desk_texture, sfTexture* client_texture, sfTexture* seller_texture) {
    sfVector2f Position;

    Position.y = queue_en * SIZE_BLOC + SIZE_BLOC * 8;

    Position.x = 0;
    sfSprite_setTexture(sprite, seller_texture, sfTrue);
    sfSprite_setPosition(sprite, Position);
    sfRenderWindow_drawSprite(window, sprite, NULL);

    Position.x = SIZE_BLOC;
    sfSprite_setTexture(sprite, desk_texture, sfTrue);
    sfSprite_setPosition(sprite, Position);
    sfRenderWindow_drawSprite(window, sprite, NULL);

    if (queue.queue_en_tab[queue_en][0] == 0) {
        Position.x = SIZE_MARGIN_QUEUE;
        sfSprite_setTexture(sprite, void_texture, sfTrue);
        sfSprite_setPosition(sprite, Position);
        sfRenderWindow_drawSprite(window, sprite, NULL);
    }
    else {
        Position.x = SIZE_MARGIN_QUEUE;
        sfSprite_setTexture(sprite, client_texture, sfTrue);
        sfSprite_setPosition(sprite, Position);
        sfRenderWindow_drawSprite(window, sprite, NULL);
    }

    for (int i = 0; i < SIZE_TAB; i++) {
        Position.y = SIZE_BLOC * 10;

        if (queue.queue_wait_en_tab[i] == 0) {
            Position.x = i * SIZE_BLOC + SIZE_MARGIN_WAIT;
            sfSprite_setTexture(sprite, void_texture, sfTrue);
            sfSprite_setPosition(sprite, Position);
            sfRenderWindow_drawSprite(window, sprite, NULL);
        }
        else {
            Position.x = i * SIZE_BLOC + SIZE_MARGIN_WAIT;
            sfSprite_setTexture(sprite, client_texture, sfTrue);
            sfSprite_setPosition(sprite, Position);
            sfRenderWindow_drawSprite(window, sprite, NULL);
        }
    }
    return 1;
}