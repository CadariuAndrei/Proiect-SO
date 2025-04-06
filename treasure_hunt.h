#ifndef TREASURE_HUNT_HEATHER
#define TREASURE_HUNT_HEATHER

#define MAX_USERNAME_SIZE 100
#define MAX_CLUETEXT_SIZE 100

typedef struct 
{
    int treasure_id;
    char user_name[MAX_USERNAME_SIZE];
    double latitude;
    double longitude;
    char clue_text[MAX_CLUETEXT_SIZE];
    int value;
}Treasure;

void add_treasure(char *hunt_id);   
#endif