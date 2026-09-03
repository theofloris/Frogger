#include "main.h"

msg buffer[BUFFER_DIM];
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; // init mutex
int buff_counter; 

msg mex;
game_status gs;

struct sockaddr_in server_addr;
int server_socket;
int client_socket;

int main(){

    printf("\e[8;%d;%dt", HEIGHT_MAP + 2, WIDTH_MAP + 2);
    fflush(stdout);
    sleep(1); 
    
    init();

    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // create a socket
    if(server_socket<0)
        perror("socket() failed");
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // address 
    server_addr.sin_port = htons(8088); // port
    int one = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    // bind the socket to the address
    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        perror("bind() failed");
    }
    if(listen(server_socket, 1)<0){
        perror("listen() failed");
    }

    WINDOW *area = newwin(HEIGHT_MAP, WIDTH_MAP, 1, 1); // create a window
    box(stdscr, 0, 0);
    int menu_ = menu(area); // welcome menu

    switch(menu_){ // switch to manage the menu
        case 0:
            werase(area); 
            break;
        case 1:
            delwin(area);    
            endwin();
            return 0;                                               
            break;
    }
    
    box(stdscr, 0, 0);
    wrefresh(area); 

    // initialize the game status
    gs.lifes = INIT_LIFES; 
    gs.score = 0;
    for (int i = 0; i < 5; i++){
        gs.lair_status[i] = true;
    }
    int counter_gate = 0;

    while (true){
        // initialize the SDL library
        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
        }

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            fprintf(stderr, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
            return 1;
        }

        Mix_Music *bgm = Mix_LoadMUS("../music/song1.mp3");
        if (bgm == NULL) {
            fprintf(stderr, "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
            return 1;
        }

        Mix_VolumeMusic(64);
        Mix_PlayMusic(bgm, -1);

        pthread_t tid_frog, tid_plant, tid_river, tid_timer;

        pthread_mutex_init(&mtx, NULL);
        // create the timer thread
        pthread_create(&tid_timer, NULL, &timer_, NULL); 
        // create the frog thread
        pthread_create(&tid_frog, NULL, &frog, NULL);
        // create the river thread
        pthread_create(&tid_river, NULL, &river, NULL);
        // create the plants thread
        pthread_create(&tid_plant, NULL, &land_plants, NULL);

        client_socket = accept(server_socket, NULL, NULL);
        int flags=fcntl(client_socket, F_GETFL);
        fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);

        // manage the game status
        game_status render_value = render(area, gs, client_socket);
        
        pthread_cancel(tid_frog);
        pthread_cancel(tid_timer); 
        pthread_cancel(tid_river); 
        pthread_cancel(tid_plant); 

        pthread_mutex_destroy(&mtx); 

        if(render_value.status == -1){
            endwin(); 
        }

        else if(render_value.status == 5){
            wrefresh(area); 
            end_music(bgm);

            gs.score = render_value.score; 
            gs.lifes--;

            if(gs.lifes < 1){
                werase(area);
                
                mvprintw(HEIGHT_MAP / 2 - 5 , WIDTH_MAP / 2 - 5, "GAME OVER :(");
                mvprintw(HEIGHT_MAP/2 - 3, WIDTH_MAP / 2 - 8, "YOUR SCORE IS: %d :)", gs.score); 

                int result_reset_menu = reset_menu(area); 

                if(result_reset_menu == 2){
                    gs.lifes = INIT_LIFES;
                    for(int j = 0; j < 5; j++){
                        gs.lair_status[j] = true; 
                    } 
                    gs.score = 0; 
                    continue;
                }
                else{
                    break;
                }
            }

            continue; 
        }
        else if(render_value.status == 6){
            wrefresh(area);
            end_music(bgm);

            gs.score = render_value.score;

            for(int i = 0; i < 5; i++){
                gs.lair_status[i] = render_value.lair_status[i];
            }
            counter_gate++; 
            if (counter_gate > 4){

                gs.score += 50; 
                werase(area);

                mvprintw(HEIGHT_MAP / 2 - 2 , WIDTH_MAP / 2 - 4, "YOU WIN!"); 
                mvprintw(HEIGHT_MAP/2 - 1, WIDTH_MAP / 2 - 6, "YOUR SCORE IS: %d :)", gs.score);

                int render_value = reset_menu(area); 
                
                if(render_value == 2){
                    gs.lifes = INIT_LIFES;
                    for(int j = 0; j < 5; j++){
                        gs.lair_status[j] = true; 
                    } 
                    gs.score = 0;

                    counter_gate = 0; 
                    continue;
                }

                else{
                    break;
                }
            }
            
            continue;  
        }
       
        return 0; 
    }

    close(server_socket);
    delwin(area); 
    endwin();
    pthread_exit(NULL);
}

void init() {
    srand(time(NULL));       
    initscr();            
    start_color();        
    game_colors(); 
    cbreak();             
    nodelay(stdscr, true);
    noecho();             
    curs_set(0);          
}

int rand_(int min, int max){
    return rand() % (max - min + 1) + min;
}

void kill_all_threads(pthread_t tid[], int count_tid){
    for(int i = 0; i < count_tid; i++){
        pthread_cancel(tid[i]); 
    }    
}

void end_music(Mix_Music *bgm){
    Mix_FreeMusic(bgm);
    Mix_CloseAudio();
    SDL_Quit();
}