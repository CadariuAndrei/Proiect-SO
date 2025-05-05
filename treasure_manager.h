#ifndef TREASURE_MANAGER_H
#define TREASURE_MANAGER_H

#define MAX_ID_SIZE 64
#define MAX_USERNAME_SIZE 64
#define MAX_CLUETEXT_SIZE 128

typedef struct 
{
    char treasure_id[MAX_ID_SIZE];
    char user_name[MAX_USERNAME_SIZE];
    double latitude;
    double longitude;
    char clue_text[MAX_CLUETEXT_SIZE];
    int value;
}Treasure;

void add_treasure(const char *hunt_id);
void list(const char *hunt_id);
void view(const char *hunt_id,const char *treasure_id);
void remove_treasure(const char *hunt_id,const char * treasure_id);
void remove_treasure_hunt(const char *hunt_id);

#endif