#include "collision.h"

// ---------------- COLLISIONS ---------------- //

// function to check frog collisions
void collision(frog_s *player){
   if (player->x < 0){ // left border
     player->x = 0; 
   }
   else if (player-> x + WIDTH_FROG > WIDTH_MAP ){ // right border
      player-> x = WIDTH_MAP - WIDTH_FROG; 
   }
   else if (player-> y + HEIGHT_FROG > HEIGHT_MAP - 3){ // bottom of the map
      player-> y = HEIGHT_MAP - HEIGHT_FROG - 3; 
   }
}

// function to check collisions with lairs
int collisions_lair(frog_s *player, lair_s lair[]) {
   if (player->y < 5) {
      for (int i = 0; i < 5; i++){
         if (player->x == lair[i].x && lair[i].open == true) return i; 
         
      }
      return -2;
   }
   
   return -1;   
}

// function to check collisions between frog and crocodiles
int collision_croc_frog(crocodile croc[], frog_s *player){
   if(player->y > 9 && player->y < 28){ // if the frog is in the river
      for (int i = 0; i < 18; i++){
         if(player->y == croc[i].y){ 
            if(player->x >= croc[i].x && player->x + WIDTH_FROG <= croc[i].x + WIDTH_CROC && croc[i].status != UNDERWATER){ 
               player->on_croc = 1; 
               return i; 
            }
         }   
      }
   }
   
   player->on_croc = 0; // if the crocodile is underwater or the frog is in the river 
    
   return -1;
} 

// function to check collisions bullet's frog
int collision_bullet(frog_s *player, bullet_s *bullet, plant_s plant[], bullet_s bullet_f[]){
   if (bullet->y < 6){ 
      player->has_shot = false;
      bullet->is_alive = false;  
      pthread_cancel(bullet->tid);
      
      return 3;
   }

   // if the bullet is in the grass bank
   else if (bullet->y > 5 && bullet->y < 9){
      for (int i = 0; i < 3; i++){ 
         // if the bullet hits a plant 
         if (bullet->y == plant[i].y && plant[i].is_alive == true){ 
            if (bullet->x >= plant[i].x && bullet->x < plant[i].x + WIDTH_PLANT){
               plant[i].is_alive = false;
               player->has_shot = false;
               bullet->is_alive = false;
               return i;
            }
         }
      }
   }
}

// function to check collisions of frog bullets with crocodiles
int collision_bullet_croc(frog_s *player, bullet_s *bullet, crocodile croc[]){
   if(bullet->y > 9 && bullet->y < 28){ // check if the bullet is in the river
      for (int i = 1; i <= 18; i++){ 
         if(bullet->y == croc[i].y && croc[i].status != UNDERWATER){

            if(croc[i].type == BAD && (bullet->x >= croc[i].x && bullet->x < croc[i].x + WIDTH_CROC)){ 
               player->has_shot = false; 
               bullet->is_alive = false; 
               pthread_cancel(bullet->tid);
               return i; 
            }
            // if the bullet hits a good crocodile
            else if(croc[i].status != UNDERWATER && croc[i].type == GOOD && (bullet->x >= croc[i].x && bullet->x < croc[i].x + WIDTH_CROC)){
               player->has_shot = false; 
               bullet->is_alive = false; 
               pthread_cancel(bullet->tid);
            }
         }
      }
   }
}

// function to check collisions of plants bullets with frog
int collision_bullet_plants(frog_s *player, bullet_s *bullet){
   if(bullet->y > 31){ 
      bullet->is_alive = false;  
      return 1;

   }

   // if the bullet hit the frog
   else if (bullet->y == player->y){
      if (bullet->x >= player->x && bullet->x < player->x + WIDTH_FROG){
         return -2; //restituisco -2
      }
   } 
}

// function to check collisions between bullets
int bullet_v_bullet(bullet_s *bullet, bullet_s bullet_f[]){
   for (int i = 0; i < 3; i++){ 
      if (bullet->x == bullet_f[i].x){

         if (bullet->y <= bullet_f[i].y){
            return i;
         }
      }
   }

   return -1; 
}

// function to check collisions between frog and plants
int collision_plant_frog(frog_s *player, plant_s plant[]){
   if(player->y > 5 && player->y < 10){ // if the frog is in the grass bank
      for(int i = 0; i < NUM_PLANTS; i++){ 
                
         if(player->y == plant[i].y || player->y == plant[i].y + 1){
            
            if ((player->x + WIDTH_FROG > plant[i].x && player->x < plant[i].x + WIDTH_PLANT) && plant[i].is_alive == true){
               return -1; 
            } 
         }
      }
   }
   return 0; 
}