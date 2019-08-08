#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 256

typedef struct board {
    int player;  //0or1
    int pitID;   //store:-1
    int stone;
    struct board *next;
} BOARD, *PBOARD;

PBOARD Binfo;

void init_board(int pit_num, int stone_num);
void print_board();

int main(int argc, char const *argv[]) {
    init_board(3, 3);
    print_board();

    return 0;
}

void init_board(int pit_num, int stone_num) {
    if(pit_num <= 0 || stone_num <= 0) exit(EXIT_FAILURE);
    free(Binfo);
    PBOARD pit= malloc(sizeof(BOARD));
    PBOARD ptr= pit, prev= NULL;
    int i= 0;
    for(i= 0; i < (pit_num + 1) * 2; i++) {
        ptr->player= i / (pit_num + 1);
        ptr->pitID= (i % (pit_num + 1) == pit_num) ? -1 : i % (pit_num + 1);
        ptr->stone= (ptr->pitID < 0) ? 0 : stone_num;
        ptr->next= malloc(sizeof(BOARD));
        prev= ptr;
        ptr= ptr->next;
    }
    free(ptr);
    prev->next= pit;
    Binfo= pit;
}

void print_board() {
    PBOARD ptr= Binfo;
    char p0info[BUFSIZE]= "", p1info[BUFSIZE]= "", buf[BUFSIZE];
    if(ptr == NULL) {
        printf("The board is not initialized.\n");
        return;
    }
    do {
        if(ptr->player == 0) {
            snprintf(buf, BUFSIZE, "%s[%2d] %2d ", p0info, ptr->pitID, ptr->stone);
            snprintf(p0info, BUFSIZE, "%s", buf);
        } else if(ptr->player == 1) {
            snprintf(buf, BUFSIZE, "[%2d] %2d %s", ptr->pitID, ptr->stone, p1info);
            snprintf(p1info, BUFSIZE, "%s", buf);
        }
        ptr= ptr->next;
    } while(ptr != Binfo);
    printf("{player:1} %s\n", p1info);
    printf("{player:0}         %s\n", p0info);
}