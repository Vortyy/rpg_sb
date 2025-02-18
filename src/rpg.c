// Use SQLite --> as storage to pick random team
// Operational rpg turn based (as Fera_Rising)
// Do graphic with open_gl or Raylib then
// pthread ???

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

sqlite3 *db; /* database ptr */

int test_callback(void *ptr, int nb_col, char **col_values, char **col_names){
  for(int i = 0; i < nb_col; i++){
    printf("| %s : %s |", *(col_names + i), *(col_values + i));  
  }
  printf("\n");
  return 0;
}

void generate_account(int n){
  while(n-- > 0){
    char *str;
    int id = rand() % 1000;
    sprintf(str, "insert into account(account_id, account_name) values ('%d', 'vorty_%d');", id, id);
    sqlite3_exec(db, str, NULL, NULL, NULL);
  }
}

int main(){

  srand(time(NULL));

  if(sqlite3_open("./src/rpg.db", &db) == SQLITE_OK){
    printf("db opened ready to be queried\n");

    generate_account(5);
    //do query
    sqlite3_exec(
        db,
        "select * from account;",
        test_callback,
        NULL,
        NULL);

    sqlite3_close(db);
    exit(0);
  }

  return 1;
}
