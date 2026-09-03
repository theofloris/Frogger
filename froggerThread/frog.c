#include "frog.h"

// ---------------- FROG ---------------- //

// sprite 
const char *sprite_frog[HEIGHT_FROG] = {
        "x_x",
        "<_>"
};

int client_socket_;
struct sockaddr_in server_addr_;

void connect_server(){ // connect to the server 
    client_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // create a socket
    if(client_socket_<0){
        perror("socket() failed");
    }

    server_addr_.sin_family = AF_INET; // set the family 
    server_addr_.sin_addr.s_addr = INADDR_ANY; // set the address
    server_addr_.sin_port = htons(8088); // set the port

    if(connect(client_socket_, (struct sockaddr*) &server_addr_, sizeof(server_addr_))<0){ 
        perror("connection() failed"); 
    }
}

// function to create the frog
void *frog(){
    connect_server();

    int ch;
    msg msg_f; 
    msg_f.id = ID_FROG; 
    msg_f.y = 0;
    msg_f.x = 0;

    keypad(stdscr, true);

    while (true){
        msg_f.y = 0; msg_f.x = 0; msg_f.id = ID_FROG;
        ch = getch(); 

        switch(ch){ // switch to move the frog
            case KEY_UP:
                msg_f.y = - 2; // up 
                send(client_socket_, &msg_f, sizeof(msg), 0);
                break;
            case KEY_DOWN:
                msg_f.y = 2; // down
                send(client_socket_, &msg_f, sizeof(msg), 0);
                break; 
            case KEY_LEFT:
                msg_f.x = - 1; // left
                send(client_socket_, &msg_f, sizeof(msg), 0);
                break; 
            case KEY_RIGHT:
                msg_f.x = 1; // right
                send(client_socket_, &msg_f, sizeof(msg), 0);
                break; 
            case 'q':
                msg_f.id = 999; // 'q' to quit
                send(client_socket_, &msg_f, sizeof(msg), 0);
                break;
            case 32:
                msg_f.id = 900; // 'space' to shoot
                send(client_socket_, &msg_f, sizeof(msg), 0);
                break; 
        }

        flushinp(); 
        usleep(10000);   
    }

    close(client_socket_);  
}

// function to color the frog
void coloring_frog(WINDOW *win, frog_s f){
    for(int i = 0; i < HEIGHT_FROG; i++){
        for(int j = 0; j < WIDTH_FROG; j++){
            if(sprite_frog[i][j] == 'x'){
                wattron(win, COLOR_PAIR(1));
                mvwaddch(win, f.y + i, f.x + j, sprite_frog[i][j]);
                wattroff(win, COLOR_PAIR(1)); 
            }
            else if(sprite_frog[i][j] == '_'){
                wattron(win, COLOR_PAIR(2));
                mvwaddch(win, f.y + i, f.x + j, sprite_frog[i][j]);
                wattroff(win, COLOR_PAIR(2));
            }
            else{
                wattron(win, COLOR_PAIR(7));
                mvwaddch(win, f.y + i, f.x + j, sprite_frog[i][j]);
                wattroff(win, COLOR_PAIR(7));
            }
        }
        
    }
    
}

// function to color the lifes
void coloring_lifes(WINDOW *win, int lifes){
    int life_y = HEIGHT_MAP - HEIGHT_FROG; 
    int start_x = WIDTH_MAP - (3 * WIDTH_FROG) - 2;
    for(int index_life = 0; index_life <= lifes; index_life++){
        int life_x = start_x + index_life * (WIDTH_FROG + 1);
        for (int i = 0; i < HEIGHT_FROG; i++){
            for (int j = 0; j < WIDTH_FROG; j++){
                if (sprite_frog[i][j] == 'x'){
                    wattron(win, COLOR_PAIR(1));
                    mvwaddch(win, life_y + i, life_x + j, sprite_frog[i][j]);
                    wattroff(win, COLOR_PAIR(1));
                }
                
                else{
                    wattron(win, COLOR_PAIR(2));
                    mvwaddch(win, life_y + i , life_x + j, sprite_frog[i][j]);
                    wattroff(win, COLOR_PAIR(2));
                }
            }
        }
    }

    for (int index_life = lifes; index_life < INIT_LIFES; index_life++) {
        int life_x = start_x + index_life * (WIDTH_FROG + 1);

        for (int i = 0; i < HEIGHT_FROG; i++) {
            for (int j = 0; j < WIDTH_FROG; j++) {
                mvwaddch(win, life_y + i, life_x + j, ' ');
            }
        }
    }

    wrefresh(win);
}