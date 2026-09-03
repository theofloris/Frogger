#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "main.h"

void collision(frog_s *frog); 
int collision_bullet(frog_s *player, bullet_s *bullet, plant_s plant[], bullet_s bullet_f[]);  
int collision_plant_frog(frog_s *player, plant_s plant[]); 
int collision_bullet_plants(frog_s *player, bullet_s *bullet); 
int collision_croc_frog(crocodile croc[], frog_s *player);
int collisions_lair(frog_s *player, lair_s lair[]);
int bullet_v_bullet(bullet_s *bullet, bullet_s bullet_f[]);
int collision_bullet_croc(frog_s *player, bullet_s *bullet, crocodile croc[]);

#endif