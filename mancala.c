#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 256

typedef struct board {
    int player;  // 0or1
    int pitID;   // store:-1
    int stone;
    struct board *next;
} BOARD, *PBOARD;

PBOARD Binfo;

void init_board(int pit_num, int stone_num);
bool sowing(int player, int pitID);
int sum_player_stones(int player);
void print_board();

int main(int argc, char const *argv[]) {
    int player = 0, pitID;
    init_board(3, 3);
    while(sum_player_stones(0) * sum_player_stones(1) != 0) {
        do {
            print_board();
            printf("Player %d's turn. Choose your pit: ", player);
            scanf("%6d", &pitID);
            while(getchar() != '\n') {
            };
        } while(sowing(player, pitID) && sum_player_stones(0) * sum_player_stones(1) != 0);
        player = 1 - player;
    }
    player = 1 - player;
    print_board();
    printf("The winner is player %d!\n", player);
    return 0;
}

void init_board(int pit_num, int stone_num) {
    if(pit_num <= 0 || stone_num <= 0) exit(EXIT_FAILURE);
    free(Binfo);
    PBOARD pit = malloc(sizeof(BOARD));
    PBOARD ptr = pit, prev = NULL;
    int i = 0;
    for(i = 0; i < (pit_num + 1) * 2; i++) {
        ptr->player = i / (pit_num + 1);
        ptr->pitID = (i % (pit_num + 1) == pit_num) ? -1 : i % (pit_num + 1);
        ptr->stone = (ptr->pitID < 0) ? 0 : stone_num;
        ptr->next = malloc(sizeof(BOARD));
        prev = ptr;
        ptr = ptr->next;
    }
    free(ptr);
    prev->next = pit;
    Binfo = pit;
}

bool sowing(int player, int pitID) {
    int pick = 0;
    PBOARD ptr = Binfo;
    do {
        if(ptr->player == player && ptr->pitID == pitID) {
            pick = ptr->stone;
            break;
        }
        ptr = ptr->next;
    } while(ptr != Binfo);

    if(pick == 0 || pitID < 0) {
        printf("Sowing failed. Please try again.\n");
        return true;
    }
    ptr->stone = 0;
    while(pick > 0) {
        ptr = ptr->next;
        ptr->stone++;
        pick--;
    }
    if(ptr->pitID < 0) {
        printf("Sowing was finished at the store. You can sowing once again!\n");
        return true;
    }
    return false;
}

int sum_player_stones(int player) {
    PBOARD ptr = Binfo;
    int sum = 0;
    do {
        if(ptr->player == player && ptr->pitID >= 0) {
            sum += ptr->stone;
        }
        ptr = ptr->next;
    } while(ptr != Binfo);
    return sum;
}

void print_board() {
    PBOARD ptr = Binfo;
    char p0info[BUFSIZE] = "", p1info[BUFSIZE] = "", buf[BUFSIZE];
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
        ptr = ptr->next;
    } while(ptr != Binfo);
    printf("{player:1} %s        (stones in pit: %2d)\n", p1info, sum_player_stones(1));
    printf("{player:0}         %s(stones in pit: %2d)\n", p0info, sum_player_stones(0));
}