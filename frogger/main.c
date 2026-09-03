#include "main.h"

// ---------------- MAIN ---------------- //

msg mex;
game_status gs; 

void init();

struct sockaddr_in server_addr;
int server_socket;
int client_socket;

int main() {
    // adapt the terminal size to the game 
    printf("\e[8;%d;%dt", HEIGHT_MAP + 2, WIDTH_MAP + 2);
    fflush(stdout);
    sleep(1); 
    
    init(); // initialize the terminal for the game

    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // create a socket
    if(server_socket<0)
        perror("socket() failed");
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // address family and IP address
    server_addr.sin_port = htons(8088); // port number

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        perror("bind() failed");
    }
    if(listen(server_socket, 1)<0){
        perror("listen() failed");
    }

    WINDOW *area = newwin(HEIGHT_MAP, WIDTH_MAP, 1, 1); // create a window for the game
    box(stdscr, 0, 0);
    int menu_ = menu(area); // welcome menu

    switch(menu_){ // switch case to choose between starting the game or exiting
        case 0:
            werase(area); 
            break;
        case 1:
            delwin(area);
            endwin();
            return 0;                                               
            break;
    }

    // initialize the game status
    gs.lifes = INIT_LIFES; 
    gs.score = 0; 
    for (int i = 0; i < 5; i++){
        gs.lair_status[i] = true;
    }

    int counter_gate = 0; 

    while(true){
        // initialize the SDL library
        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
        }

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            fprintf(stderr, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
            return 1;
        }

        // load the background music
        Mix_Music *bgm = Mix_LoadMUS("../music/song1.mp3");
        if (bgm == NULL) {
            fprintf(stderr, "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
            return 1;
        }

        // play the background music
        Mix_VolumeMusic(64);
        Mix_PlayMusic(bgm, -1);
        
        // create a pipe
        int pipe_fd[2];

        if (pipe(pipe_fd) == -1){
            perror("Pipe call"); 
            _exit(1); 
        }

        pid_t pid_frog, pid_river, pid_plant, pid_timer;

        // create the processes
        pid_timer = fork();
        if (pid_timer < 0){
            perror("timer error");
            _exit(1);
        }
        else if (pid_timer == 0){ // create a process that will manage the timer
            timer_(pipe_fd);
            _exit(0);
        }
        else {
            mex = (msg){200, 0, 0, pid_timer}; 
            write(pipe_fd[1], &mex, sizeof(msg));

            pid_frog = fork(); // create a process that will manage the frog
            if(pid_frog < 0){
                perror("frog not created"); 
                _exit(1); 
            }
            else if(pid_frog == 0){
                frog(pipe_fd);
                _exit(0);
            }
            else{
                mex = (msg){200, 0, 0, pid_frog}; 
                write(pipe_fd[1], &mex, sizeof(msg));

                pid_river = fork(); // create a process that will manage the river
                if(pid_river < 0){ 
                    perror("river not created"); 
                    _exit(1); 
                }
                else if(pid_river == 0){
                    river(pipe_fd);
                    _exit(0);
                }

                else{
                    mex = (msg){200, 0, 0, pid_river}; 
                    write(pipe_fd[1], &mex, sizeof(msg));

                    pid_plant = fork(); // create a process that will manage the plants
                    if(pid_plant < 0){
                        perror("land not created"); 
                        _exit(1); 
                    }     
                    else if(pid_plant == 0){
                        land_plants(pipe_fd, area); 
                        _exit(0);
                    }
                    else{   
                        mex = (msg){200, 0, 0, pid_plant};
                        write(pipe_fd[1], &mex, sizeof(msg));

                        client_socket = accept(server_socket, NULL, NULL);
                        int flags=fcntl(client_socket, F_GETFL);
                        fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);

                        game_status result_render = render(pipe_fd, area, gs, client_socket); // initialize the render function
  
                        // based on the result of the render function, we can decide what to do
                        if (result_render.status == - 1) endwin(); // quit the game
                        
                        // frogger died
                        else if(result_render.status == 5){ 
                            wrefresh(area); 
                            end_music(bgm);

                            gs.score = result_render.score;
                            gs.lifes--; 

                            if (gs.lifes < 1){
                                werase(area);
                                
                                mvprintw(HEIGHT_MAP / 2 - 2 , WIDTH_MAP / 2 - 4, "GAME OVER :(");
                                mvprintw(HEIGHT_MAP/2 - 1, WIDTH_MAP / 2 - 6, "YOUR SCORE IS: %d", gs.score); 
                                
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
                        
                        // close the lair
                        else if(result_render.status == 6){
                            wrefresh(area);
                            end_music(bgm);

                            gs.score = result_render.score;
                            
                            for(int i = 0; i < 5; i++){
                                gs.lair_status[i] = result_render.lair_status[i];
                            }

                            counter_gate++; 
                            if (counter_gate > 4){

                                gs.score += 50; // se vengono chiuse tutte le tane, si assegna un bonus 
                                werase(area);

                                mvprintw(HEIGHT_MAP / 2 - 2 , WIDTH_MAP / 2 - 4, "YOU WIN!"); 
                                mvprintw(HEIGHT_MAP/2 - 1, WIDTH_MAP / 2 - 6, "YOUR SCORE IS: %d :)", gs.score);

                                int result_reset_menu = reset_menu(area); 
                                
                                if(result_reset_menu == 2){
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
                }
            }
        } 
    }

    // close the sockets and the window 
    close(server_socket);
    delwin(area);
    endwin(); 
}


// initialize the terminal for the game
void init() {
    srand(time(NULL));       // Seme per i numeri randomici
    initscr();               // Inizializza la scherma per ncurses
    start_color();           // Attiva i colori
    game_colors(); 

    cbreak();                // Line buffering disabled
    nodelay(stdscr, true);
    noecho();                // Don't echo while we do getch
    curs_set(0);             // Hide the cursor
}

// generator of random numbers
int rand_(int min, int max){
    return rand() % (max - min + 1) + min;
}

// kill a process
void kill_process(pid_t pid){
    int status;
    kill(pid, SIGKILL);
    waitpid(pid, &status, 0);
}

// kill all processes
void kill_all(int pid[], int len_pid){
    for(int i = 0; i < len_pid; i++){
        kill_process(pid[i]); 
    }
}

// end the music
void end_music(Mix_Music *bgm){
    Mix_FreeMusic(bgm);
    Mix_CloseAudio();
    SDL_Quit();
}