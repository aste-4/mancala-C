#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 256

/**
 * 1つのpit(ポケット)・store(ゴール)を表す構造体
 */
typedef struct board {
    int player;          // 0 or 1
    int pitID;           // store: -1, pit: それ以外
    int stone;           // 入っている石の数
    struct board *next;  // 隣のpit(store)
} BOARD, *PBOARD;

// 場(ボード全体)を表す構造体線形リストの先頭を指すポインタ
PBOARD Binfo;

void init_board(int pit_num, int stone_num);
bool sowing(int player, int pitID);
int sum_player_stones(int player);
void print_board();

int main(int argc, char const *argv[]) {
    int player = 0, pitID;

    // 場の生成: 1プレイヤーあたりのpitは3、1pitあたりの石の数は3
    init_board(3, 3);

    // player0のpit内にある石の合計とplayer1のそれがどちらも0でない間
    while(sum_player_stones(0) * sum_player_stones(1) != 0) {
        // 1度pit選択を行い、続けて選択可能でどちらも石がなくなってない間それを繰り返す
        do {
            print_board();
            printf("Player %d's turn. Choose your pit: ", player);
            scanf("%6d", &pitID);
            // 改行文字まで読み飛ばす
            while(getchar() != '\n') {
            };
        } while(sowing(player, pitID) && sum_player_stones(0) * sum_player_stones(1) != 0);

        // 次の番のプレイヤー番号に(0なら1に、1なら0に)
        player = 1 - player;
    }

    // ループ抜けたタイミングでは敗者のプレイヤー番号になっているので、勝者の番号に更新する
    player = 1 - player;
    print_board();
    printf("The winner is player %d!\n", player);
    return 0;
}

/**
 * 場の生成・初期化。
 * @param (pit_num) 1プレイヤーあたりのpit数
 * @param (stone_num) 1pitあたりの石の数
 */
void init_board(int pit_num, int stone_num) {
    // 引数のいずれかが負の数なら異常終了
    if(pit_num <= 0 || stone_num <= 0) exit(EXIT_FAILURE);

    free(Binfo);
    PBOARD pit = malloc(sizeof(BOARD));
    PBOARD ptr = pit, prev = NULL;

    for(int i = 0; i < (pit_num + 1) * 2; i++) {
        // ループの前半はプレイヤー0、後半はプレイヤー1のpit/store
        ptr->player = i / (pit_num + 1);
        // pitIDは0からpit_num-1まで、storeは-1とする
        ptr->pitID = (i % (pit_num + 1) == pit_num) ? -1 : i % (pit_num + 1);
        // storeの初期石は0個、それ以外は引数の設定値
        ptr->stone = (ptr->pitID < 0) ? 0 : stone_num;
        // 隣のpit情報用メモリ確保
        ptr->next = malloc(sizeof(BOARD));
        prev = ptr;
        ptr = ptr->next;
    }
    // 確保したが使用しないメモリ解放
    free(ptr);
    // 最後のstoreの隣は最初のpit
    prev->next = pit;
    // 先頭アドレスをグローバル変数に格納
    Binfo = pit;
}

/**
 * 指定されたpitから石を移動する。
 * @param (player) プレイヤー番号
 * @param (pitID) 移動元のpitID
 * @return true: 同じプレイヤーが続けて石を動かせる
 */
bool sowing(int player, int pitID) {
    // 指定されたpitにある動かす石の数
    int pick = 0;
    PBOARD ptr = Binfo;

    // 場を一周するまで指定されたpitを探索
    do {
        // 引数で指定されたpitなら動かすべき石を得てbreak
        if(ptr->player == player && ptr->pitID == pitID) {
            pick = ptr->stone;
            break;
        }
        ptr = ptr->next;
    } while(ptr != Binfo);

    // 動かすべき石が0(存在しないpitID指定時含む)か、storeが指定されていれば失敗
    if(pick == 0 || pitID < 0) {
        printf("Sowing failed. Please try again.\n");
        // もう一度同じプレイヤーにpitを選ばせる
        return true;
    }

    // この時点でptrは指定されたpitを指している
    // 指定されたpitの石の数を0とする
    ptr->stone = 0;

    // 石がなくなるまで1つずつ順に隣のpitに石を動かす
    while(pick > 0) {
        ptr = ptr->next;
        ptr->stone++;
        pick--;
    }

    // 最後に石を入れたのがstoreの場合
    if(ptr->pitID < 0) {
        printf("Sowing was finished at the store. You can sowing once again!\n");
        // もう一度同じプレイヤーがpitを選ぶことができる
        return true;
    }

    // 次のプレイヤーに手番を交代する
    return false;
}

/**
 * 指定されたプレイヤーのpitにある石の総数を返す。
 * @param (player) プレイヤー番号
 * @return 指定されたプレイヤーのpitにある石の総数
 */
int sum_player_stones(int player) {
    PBOARD ptr = Binfo;
    int sum = 0;
    do {
        // pitのプレイヤー番号が一致し、storeでないときだけカウントする
        if(ptr->player == player && ptr->pitID >= 0) {
            sum += ptr->stone;
        }
        ptr = ptr->next;
    } while(ptr != Binfo);
    return sum;
}

/**
 * 現在の場の状態を表示する。
 */
void print_board() {
    PBOARD ptr = Binfo;
    char p0info[BUFSIZE] = "", p1info[BUFSIZE] = "", buf[BUFSIZE];

    // 場の情報がNULLのとき
    if(ptr == NULL) {
        printf("The board is not initialized.\n");
        return;
    }

    // 場を一周するまで各pitを探索
    do {
        // プレイヤー0のpitならp0infoに、プレイヤー1のpitならp1infoにそのpitの情報を文字列結合する
        if(ptr->player == 0) {
            // 一旦bufに格納してから、p0infoに格納する
            snprintf(buf, BUFSIZE, "%s[%2d] %2d ", p0info, ptr->pitID, ptr->stone);
            snprintf(p0info, BUFSIZE, "%s", buf);
        } else if(ptr->player == 1) {
            // プレイヤー0と同様だが、左右反転させるため前に結合する
            snprintf(buf, BUFSIZE, "[%2d] %2d %s", ptr->pitID, ptr->stone, p1info);
            snprintf(p1info, BUFSIZE, "%s", buf);
        }
        ptr = ptr->next;
    } while(ptr != Binfo);

    // プレイヤーによってstoreの位置が左右異なるので揃えて表示させる
    printf("{player:1} %s        (stones in pit: %2d)\n", p1info, sum_player_stones(1));
    printf("{player:0}         %s(stones in pit: %2d)\n", p0info, sum_player_stones(0));
}