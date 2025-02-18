// Use SQLite --> as storage to pick random team
// Operational rpg turn based (as Fera_Rising)
// Do graphic with open_gl or Raylib then
// pthread ???

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define NAMES_SIZE 8
#define MAX_LVL 8 

#define TEAM_SIZE 3
#define STATS_NUM 6 /* 0:HP, 1:MP, 2:ATK, 3:MATK, 4:DEF, 5:MDEF */ 

#define print_entity(e) printf(" ------------------- \n - name : %s \n - lvl : %d (.xp:%d/%d) \n - stats: [%d,%d,%d,%d,%d,%d] \n", \
    e.name, e.level, e.xp, xp_required[e.level], \
    e.base_stats[0], e.base_stats[1], e.base_stats[2], e.base_stats[3], e.base_stats[4], e.base_stats[5])

int xp_required[MAX_LVL] = {0, 100, 200, 400, 800, 1600, 3200, 6400}; 

char *entity_names[NAMES_SIZE] = {
  "Rohan",
  "Vorty",
  "Mickeal",
  "Trevor",
  "Anastazia",
  "Elizabeth",
  "Rotomator",
  "Cumcumbre"
};

typedef struct _entity {
  char *name;
  int xp;
  int level;
  int base_stats[STATS_NUM];
} entity;

/* generate_entity : generate random entity base on seed from srand */
entity generate_entity(){
  entity e = {
    .name = entity_names[rand() % NAMES_SIZE],
    .xp = 0,
    .level = 1,
    .base_stats = {rand() % 20, rand() % 20, rand() % 20, rand() % 20, rand() % 20, rand() % 20}
  };

  return e;
}

int main(){

  srand(time(NULL)); /* seed fix but can be randomised by time(NULL) */
  entity team[TEAM_SIZE];
  
  for (int i = 0; i < TEAM_SIZE; i++){
    team[i] = generate_entity();
    print_entity(team[i]);
  }

  return 0;
}
