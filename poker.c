
/*------------------------------------------------------------------------
ポーカー（ファイブカードドロー）でコンピューターと1対1勝負するプログラム

役：弱い順
0、ハイカード
1、ワンペア
2、ツーペア
3、スリーカード
4、ストレート
5、フラッシュ
6、フルハウス
7、フォーカード
8、ストレートフラッシュ
9、ロイヤルストレートフラッシュ

数字：弱い順
2,3,4,5,6,7,8,9,10,J,Q,K,A

図柄：優劣なしとする

ジョーカーなし
初期軍資金：100ドル
参加料：5ドル
賭け金：自由に設定


--------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define MAX 52

//playerとCPUの手札
typedef struct {
    int suit; //図柄 ♠,♥,♦,♣, 0,1,2,3
    int number; //数字 2,3,4,5,6,7,8,9,10,J,Q,K,A
    int hand_rank;
    int change;
}Hand_t;

//トランプの山札
typedef struct{
    int suit;
    int number;
}Stock_t;


//交換するカードを入力
void input_change(Hand_t*);
//カードを配る
void deal(Hand_t*, Stock_t*, int*, int*, int*, int*);
void shuffle(Stock_t*, int*, int*, int*, int*);
//山札のキュー
void enque(Stock_t, Stock_t*, int*, int*);
Hand_t deque(Stock_t*, int*, int*);
//ランク、番号、絵柄でソート
void sort_card(Hand_t*);
//ランクをチェック
void check_hand_rank(Hand_t*);
//結果
int result(Hand_t*, Hand_t*);
//カード表示
void display(Hand_t*, char**, char**);
//賭け金や終了の入力
int input_value(int, int);

int main(void){
    //ランク、番号、絵柄の文字列
    char *suit_str[4] = {"C", "D", "H", "S"};
    char *number_str[13] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
    char *hand_rank_str[10] ={"ハイカード",
                            "ワンペア",
                            "ツーペア",
                            "スリーカード",
                            "ストレート",
                            "フラッシュ",
                            "フルハウス",
                            "フォーカード",
                            "ストレートフラッシュ",
                            "ロイヤルストレートフラッシュ"};

    //キューの変数
    int size = 0;
    int head = 0;
    int tail = 0;
    int pre_head = 0;

    //for文の変数
    int i, j;

    //山札の初期化
    Stock_t stock[52];
    Stock_t input_stock;
    for(i = 0; i <= 3; i++){
        for(j = 0; j <= 12; j++){
            input_stock.suit = i;
            input_stock.number = j;
            enque(input_stock, stock, &size, &head);
        }
    }

    //初期金額
    int money = 100;
    //参加料
    int ante = 5;

    //開始と終了
    int option;
    printf("始める：0、終わる：1  ");
    option = input_value(0, 1);
    if(option == 1)return 0;

    

    while(1){
        //乱数の初期化
        srand(time(NULL));

        //参加料の支払い
        printf("\n参加料を支払う。残金：%dドル\n", money - ante);
        int pot = ante;
        
        //手札の初期化
        Hand_t player_hand[5];
        Hand_t CPU_hand[5];
        Hand_t default_hand = {0, 0, 0, -1};
        for(i = 0; i < 5; i++){
            player_hand[i] = default_hand;
            CPU_hand[i] = default_hand;
        }
        
        //カードの配布
        deal(CPU_hand, stock, &size, &head, &tail, &pre_head);
        deal(player_hand, stock, &size, &head, &tail, &pre_head);
        check_hand_rank(CPU_hand);
        check_hand_rank(player_hand);
        
        display(player_hand, suit_str, number_str);

        //賭け金の入力
        printf("\n賭ける場合は賭ける金額を、賭けない場合は0を入力してください  ");
        int bet_value = input_value(0, 10000000);
        pot += bet_value;
        printf("\n残金：%dドル\n", money - ante - bet_value);
        
        //CPUはrankが0のカードのみ交換
        for(i = 0; i < 5; i++){
            if((CPU_hand + i)->hand_rank == 0){
                (CPU_hand + i)->change = 1;
            }
        }

        //カードの交換
        deal(CPU_hand, stock, &size, &head, &tail, &pre_head);
        input_change(player_hand);
        deal(player_hand, stock, &size, &head, &tail, &pre_head);

        //ランクのチェックと結果表示
        check_hand_rank(CPU_hand);
        check_hand_rank(player_hand);
        display(CPU_hand, suit_str, number_str);
        printf("CPU:\t%s\n", hand_rank_str[CPU_hand->hand_rank]);
        display(player_hand, suit_str, number_str);
        printf("player:\t%s\n", hand_rank_str[player_hand->hand_rank]);
        int result_value = result(player_hand, CPU_hand);

        //結果の反映
        money += result_value * pot;
        if(result_value >= 0){
            printf("\n賭けの結果を反映 +%d。残金：%dドル\n", pot, money);
        }
        else{
            printf("\n賭けの結果を反映 -%d。残金：%dドル\n", pot, money);
        }

        //続行と終了
        printf("\n続ける：0、終わる：1  ");
        option = input_value(0, 1);
        if(option == 1)return 0;
    }
}

void input_change(Hand_t *p_hand){
    int i; 
    char str[6];
    char *draw_card;
    char *draw_check = "12345";

    
    printf("\n左から何枚目をドローするか選択（昇順） 交換しない場合は0を入力  ");
    
    //交換するカードを入力
    while(1){
        scanf("%s", str);
        draw_card = (char*)malloc(sizeof(char*) * (strlen(str) + 1));
        strcpy(draw_card, str);
        int asc_check = 0;
        if(strcmp(draw_card, "0") == 0){
            return;
        }
        else if(strlen(draw_card) > 1){
            for(i = 0; i < strlen(draw_card) - 1; i++){
                if(strncmp(draw_card + i, draw_card + i + 1, 1) >= 0)asc_check = -1;
            }
        }
        else{}

        if(asc_check == 0){
            break;
        }
        else {
            printf("error\n");
        }
    }

    //交換するカードのchangeを1に
    //0番目の配列で2なら1番目の配列は3から調べる
    int j = 0;
    for(i = 0; i < strlen(draw_card); i++){
        for(; j < 5; j++){
            if(strncmp(draw_card + i, draw_check + j, 1) == 0){
                (p_hand + j)->change = 1;
                break;
            }
        }
    }

    free(draw_card);

    return;
}

void deal(Hand_t *p_hand, Stock_t *p_stock, int *size, int *head, int *tail, int *pre_head){

    for(int i = 0; i < 5; i++){
        //１周したらシャッフル
        if(*tail == *pre_head){
            shuffle(p_stock, size, head, tail, pre_head);
        }

        //changeが1のときは交換
        if((p_hand + i)->change == 1){
            Stock_t input_stock;
            input_stock.suit = (p_hand + i)->suit;
            input_stock.number = (p_hand + i)->number;
            enque(input_stock, p_stock, size, head);
            *(p_hand + i) = deque(p_stock, size, tail);
            
        }
        //-1のときは配るだけ
        else if((p_hand + i)->change == -1){
            *(p_hand + i) = deque(p_stock, size, tail);
        }
        else{}
        
    }
    return;
}

void shuffle(Stock_t *p_stock, int *size, int *head, int *tail, int *pre_head){
    Stock_t temp;
    int temp_value[2];
    

    for(int i = 0; i < 100; i++){
        temp_value[0] = (rand() % *size + *tail) % MAX;
        temp_value[1] = (rand() % *size + *tail) % MAX;

        temp = *(p_stock + temp_value[0]);
        *(p_stock + temp_value[0]) = *(p_stock + temp_value[1]);
        *(p_stock + temp_value[1]) = temp;
    }
    *pre_head = *head;
    
    return;
}

void enque(Stock_t input_hand, Stock_t *p_stock, int *size, int *head){
    if(*size < MAX){
        (p_stock + *head)->suit = input_hand.suit;
        (p_stock + *head)->number = input_hand.number;
        (*head)++;

        if(*head == MAX){
            *head = 0;
        }
        
        (*size)++;
    }

    return;
}

Hand_t deque(Stock_t *p_stock, int *size, int *tail){
    
    Hand_t hand = {-1,-1,-1,-1};

    if(*size > 0){
        hand.suit = (p_stock + *tail)->suit;
        hand.number = (p_stock + *tail)->number;
        hand.hand_rank = 0;
        hand.change = 0;
        (*tail)++;

        if(*tail == MAX){
            *tail = 0;
        }
        
        (*size)--;
    }

    return hand;
}

void sort_card(Hand_t *p_hand){
    Hand_t temp;
    
    //挿入ソートで並びかえる
    for(int i = 1; i < 5; i++){
        for(int j = i; j >= 1; j--){
            if((p_hand + j)->hand_rank * 53 + (p_hand + j)->number * 4 + (p_hand + j)->suit
            > (p_hand + j - 1)->hand_rank * 53 + (p_hand + j - 1)->number * 4 + (p_hand + j - 1)->suit){
                temp = *(p_hand + j);
                *(p_hand + j) = *(p_hand + j - 1);
                *(p_hand + j - 1) = temp;
            }
        }
    }

    return;
}

void check_hand_rank(Hand_t *p_hand){
    int i;
    int hand_rank = 0;
    int number[2] = {-1};
    int count[13] = {0};

    //ワンペア、ツーペア、スリーカード、フルハウス、フォーカードを調べる
    //手札のそれぞれの番号の枚数を数える
    for(i = 0; i < 5; i++){
        count[(p_hand + i)->number]++;
    }
    //枚数で分類してランクづけする
    //number[0]に1つ目の揃った番号、number[1]に2つ目にそろった番号が入る
    //フルハウスの場合、3枚の方を先頭に持っていきたいため3枚の方のみランク付けされるようにする
    for(i = 0, number[i] = 0; number[i] < 13; number[i]++){
        if(count[number[i]] == 2){
            //ハイカードからワンペア、他の数がそろう可能性があるため続行
            if(hand_rank == 0){
                hand_rank = 1;
                number[1] = number[0];
                i = 1;
            }
            //ワンペアからツーペア、これ以上作れないためbreak
            else if(hand_rank == 1){
                hand_rank = 2;
                break;
            }
            //スリーカードからフルハウス、2枚の方は-1を代入して調べないようにして、これ以上作れないためbreak
            else{
                hand_rank = 6;
                number[1] = -1;
                break;
            }
        }
        else if(count[number[i]] == 3){
            //ハイカードからスリーカード、フルハウスの可能性があるため続行
            if(hand_rank == 0){
                hand_rank = 3;
                number[1] = number[0];
                i = 1;
            }
            //ツーペアからフルハウス、2枚の方は-1を代入して調べないようにして、これ以上作れないためbreak
            else{
                hand_rank = 6;
                number[0] = -1;
                break;
            }
        }
        //4枚のためフォーカード
        else if(count[number[i]] == 4){
            hand_rank = 7;
            break;
        }
        else{}
    }

    //ランクを役の要素となっている番号にのみ代入
    //ハイカードの場合はAにランクが代入されるが、どちらにしても0なので問題はない
    for(i = 0; i < 5; i++){
        if((p_hand + i)->number == number[0] || (p_hand + i)->number == number[1]){
            (p_hand + i)->hand_rank = hand_rank;
        }
    }
    
    //残りの役は全てのカードで構成されるため、ここでソートを行って問題ない
    //また、ここでソートを行ってストレートを調べやすいようにする
    sort_card(p_hand);

    //役が確定している場合は戻る
    if(hand_rank != 0)return;

    //残りの役を調べる

    //フラッシュを調べる
    //1枚目の絵柄と違う柄が出たところで-1を代入する
    int flush = 0;
    for(i = 1; i < 5; i++){
        if((p_hand + i)->suit != p_hand->suit){
            flush = -1;
        }
    }

    //ストレートとロイヤル(AKQJ10)を調べる
    //ソートしたとき左にAがくるようにしたため、A5432のストレートは個別に調べて5432Aに並び替える
    int royal = -1;
    int straight = 0;
    //1番目がAで2番目がKだとロイヤルストレートの可能性
    if(p_hand->number == 12 && (p_hand + 1)->number == 11){
        //3番目以降がQJ10でないときstraightに-1を代入
        if((p_hand + 2)->number != 10 || (p_hand + 3)->number != 9 || (p_hand + 4)->number != 8){
            straight = -1;
        }
        //AKQJ10だからroyalに0を代入
        else{
            royal = 0;
        }
    }
    //1番目がAで2番目が5だとA5432ストレートの可能性
    else if(p_hand->number == 12 && (p_hand + 1)->number == 3){
        //3番目以降が432でないときstraightに-1を代入
        if((p_hand + 2)->number != 2 || (p_hand + 3)->number != 1 || (p_hand + 4)->number != 0){
            straight = -1;
        }
        //A5432ストレートの場合5432Aに並び替える
        else{
            Hand_t temp = *p_hand;
            for(i = 0; i < 4; i++){
                *(p_hand + i) = *(p_hand + i + 1);
            }
            *(p_hand + 4) = temp;
        }
    }
    //他のストレートを調べる
    //右に行くにと1つずつ減っていればstraightは0のままで、そうでない場合-1を代入する
    else{
        for(i = 0; i < 4; i++){
            if((p_hand + i + 1)->number != (p_hand + i)->number - 1){
                straight = -1;
                break;
            }
        }
    }

    //ロイヤルストレートフラッシュならhand_rankに9を代入
    if(royal == 0 && straight == 0 && flush == 0){
        hand_rank = 9;
    }
    //ストレートフラッシュならhand_rankに8を代入
    else if(straight == 0 && flush == 0){
        hand_rank = 8;
    }
    //フラッシュならhand_rankに5を代入
    else if(flush == 0){
        hand_rank = 5;
    }
    //ストレートならhand_rankに4を代入
    else if(straight == 0){
        hand_rank = 4;
    }
    else{}

    //全てのカードにランク付け
    for(i = 0; i < 5; i++){
        (p_hand + i)->hand_rank = hand_rank;
    }

    return;
}

int result(Hand_t *p_player_hand, Hand_t *p_CPU_hand){
    int result_value = 0;

    
    for(int i = 0; i < 5; i++){
        //playerが勝利
        if((p_player_hand + i)->hand_rank * 13 + (p_player_hand + i)->number > (p_CPU_hand + i)->hand_rank * 13 + (p_CPU_hand + i)->number){
            printf("\nYOU WIN\n");
            result_value = 1;
            break;
        }
        //playerが敗北
        else if((p_player_hand + i)->hand_rank * 13 + (p_player_hand + i)->number < (p_CPU_hand + i)->hand_rank * 13 + (p_CPU_hand + i)->number){
            printf("\nYOU LOSE\n");
            result_value = -1;
            break;
        }
    }

    if(result_value == 0){
        printf("\nDRAW\n");
    }

    return result_value;
}

void display(Hand_t *p_hand, char **p_suit_str, char **p_number_str){
    int i;

    printf("\n");

    for(i = 0; i < 5; i++){
        printf("/-----\\ ");
    }
    printf("\n");

    //絵柄の表示
    for(i = 0; i < 5; i++){
        printf("|%s    | ", *(p_suit_str + (p_hand + i)->suit));
    }
    printf("\n");

    //番号の表示
    for(i = 0; i < 5; i++){
        if((p_hand + i)->number == 8){
            printf("|  10 | ");
        }
        else{
            printf("|   %s | ", *(p_number_str + (p_hand + i)->number));
        }
    }
    printf("\n");

    for(i = 0; i < 5; i++){
        printf("\\-----/ ");
    }
    printf("\n");
    
    return;
}

int input_value(int min, int max){
    int value;
    //値の入力とエラー処理
    while(1){
        scanf("%d", &value);
        if(value >= min && value <= max){
            break;
        }
        else{
            printf("error  ");
        }
    }
    return value;
}
