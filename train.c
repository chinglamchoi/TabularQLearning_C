#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define EMPTY '-'
#define CIRCLE '1'
#define CROSS '2'
#define LEN(arr1) ((int) (sizeof (arr1) / sizeof (arr1)[0]))


int returnRandom(int lower, int upper) {
    return (rand() % (upper-lower)) + lower; // end exclusive
}

struct TicTacToe {
    char state[10];
    int marker;
};

void init_reset(struct TicTacToe *ttt, int turn_n) {
    for (int i=0; i<9; i++) {
        ttt->state[i] = EMPTY;
    }
    ttt->marker = (turn_n) ? 1 : 0;
}

int gameEnds(struct TicTacToe ttt) {
    char mm = (ttt.marker) ? CIRCLE : CROSS;
    // 0: continue; 1: won; 2: draw; 
    if ((ttt.state[0] == ttt.state[4] && ttt.state[4] == ttt.state[8] && ttt.state[8] == mm) || (ttt.state[2] == ttt.state[4] && ttt.state[4] == ttt.state[6] && ttt.state[6] == mm)) {
        return 1;
    }
    for (int i=0; i<3; i++) {
        if ((ttt.state[3*i] == ttt.state[3*i+1] && ttt.state[3*i+1] == ttt.state[3*i+2] && ttt.state[3*i+2] == mm) || (ttt.state[i] == ttt.state[i+3] && ttt.state[i+3] == ttt.state[i+6] && ttt.state[i+6] == mm)) {
            return 1;
        }
    }
    for (int i=0; i<9; i++) {
        if (ttt.state[i] == EMPTY) {
            return 0;
        }
    }
    return 2;
}

int playMoveRandom(struct TicTacToe ttt) {
    int cnt = 0;
    int idxs[9];
    for (int i=0; i<9; i++) {
        if (ttt.state[i] == EMPTY) {
            idxs[cnt] = i;
            cnt += 1;
        }
    }
    return idxs[returnRandom(0, cnt)];
}

struct dict {
    //19683 = number of possible ttt states
    char (*keys)[19690][19690];
    double (*res)[19690][19690];
};

struct rstates {
    char state[10];
    int move;
};

struct TabularQLearningAgent {
    double alpha;
    double d;
    double init_;
    struct rstates recordedStates[9];
    struct dict states;
    double q_values[9];
    double reward;
};

void getReward(struct TabularQLearningAgent *ag1, int status) {
    if (status > 0) {
        ag1->reward = (status == 1) ? 1 : 0.5; //0.5
    } else {
        ag1->reward = 0;
    }
}

void calculate(struct TabularQLearningAgent *ag1) {
    int first = 1;
    double maximum = 0;
    int cnt = 8;
    while (((int) ag1->recordedStates[cnt].state[0]) == 0) {
        cnt -= 1;
    }
    for (int i=cnt; i>-1; i--) {
        int cnt1 = 2;
        char temp[9];
        while (((int) (*ag1->states.keys)[cnt1][0]) != 0) {
            if (strcmp(ag1->recordedStates[i].state, (*ag1->states.keys)[cnt1]) == 0) {
                break;
            }
            cnt1 += 1;
        }
        if (first) {
            (*ag1->states.res)[cnt1][ag1->recordedStates[i].move] = ag1->reward;
            first = 0;
        } else {
            double t1 = (1- (double)ag1->alpha);
            double t2 = (double) (*ag1->states.res)[cnt1][ag1->recordedStates[i].move];
            double t3 = (double) ag1->alpha;
            double t4 = (double) ag1->d;
            double t5 = (double) maximum;
            double t6 = t1 * t2 + t3 * t4 * t5;
            (*ag1->states.res)[cnt1][ag1->recordedStates[i].move] = t6;
        }
        maximum = (*ag1->states.res)[cnt1][0];
        for (int i=1; i<9; i++) {
            maximum = ((*ag1->states.res)[cnt1][i] > maximum) ? (*ag1->states.res)[cnt1][i] : maximum;
        }
    }
}

void get_q(struct TicTacToe ttt, struct TabularQLearningAgent *ag1) {
    int cnt = 2;
    int val = 0;
    while (((int) (*ag1->states.keys)[cnt][0]) != 0) {
        if (strcmp((*ag1->states.keys)[cnt], ttt.state) == 0) {
            for (int i=0; i<9; i++) {
                ag1->q_values[i] = (*ag1->states.res)[cnt][i];
            }
            val = 1;
        }
        cnt += 1;
    } 
    if (val == 0) {
        for (int i=0; i<9; i++) {
            (*ag1->states.keys)[cnt][i] = ttt.state[i];
            (*ag1->states.res)[cnt][i] = (ttt.state[i] == EMPTY) ? ag1->init_ : -INFINITY;
            ag1->q_values[i] = (*ag1->states.res)[cnt][i];
        }
    }
}

int get_move(struct TicTacToe ttt, struct TabularQLearningAgent *ag1, int move_num) {
    int max_q[9];
    double max = ag1->q_values[0];
    for (int i=1; i<9; i++) {
        max = (ag1->q_values[i] > max) ? ag1->q_values[i] : max;
    }
    int cnt = 0;
    for (int i=0; i<9; i++) {
        if (ag1->q_values[i] == max) {
            max_q[cnt] = i;
            cnt += 1;
        }
    }
    int move = returnRandom(0, cnt); // select random max move
    for (int i=0; i<9; i++) {
        ag1->recordedStates[move_num].state[i] = ttt.state[i];
    }
    ag1->recordedStates[move_num].move = max_q[move];
    return max_q[move];
}


int gameEnds(struct TicTacToe ttt);
int playMoveRandom(struct TicTacToe ttt);


void resetRecordedStates(double recordedS[9][9]) {
    for (int i=0; i<9; i++) {
        for (int o=0; o<9; o++) {
            recordedS[i][o] = EMPTY;
        }
    }
}

void printGameBoard(char gameBoard[9]) {
    printf("=========\n");
    for (int i=0; i<9; i++) {
        if (i % 3 == 0 && i != 0) {
            printf("\n");
        }
        printf("|");
        if (gameBoard[i] == EMPTY) {
            printf("%d", i);
        } else if (gameBoard[i] == CIRCLE) {
            printf("O");
        } else {
            printf("X");
        }
        printf("|");
    }
    printf("\n=========\n\n");
}


void test(struct TicTacToe ttt, struct TabularQLearningAgent agent1, struct TabularQLearningAgent agent2) {
    int turn = 1;
    for (int k=0; k<2; k++) {
        init_reset(&ttt, turn);
        int currentPlayer = turn;
        int cnt_1 = 0;
        int cnt_2 = 0;

        while (gameEnds(ttt) < 1) {
            int move;
            if (currentPlayer) {
                if (k) {
                    get_q(ttt, &agent1);
                    move = get_move(ttt, &agent1, cnt_1);
                } else {
                    get_q(ttt, &agent2);
                    move = get_move(ttt, &agent2, cnt_1);
                }
                cnt_1 += 1;
            } else {
                printGameBoard(ttt.state);
                printf("Your move: ");
                scanf("%d", &move);
                cnt_2 += 1;
            }
            ttt.state[move] = (currentPlayer) ? CIRCLE : CROSS;
            ttt.marker = (currentPlayer) ? 1 : 0;
            int end = gameEnds(ttt);
            if (end == 1) {
                printGameBoard(ttt.state);
                if (currentPlayer) {
                    printf("AI wins!\n");
                } else {
                    printf("You win!\n");
                }
            } else {
                if (end == 2) {
                    printGameBoard(ttt.state);
                    printf("You draw!\n");
                }
            }
            currentPlayer = !currentPlayer;
        }
        turn = !turn;
    }
}

void train_random(struct TicTacToe ttt, struct TabularQLearningAgent agentn, int turn) {
    init_reset(&ttt, turn);

    for (int j=0; j<9; j++) {
        memset(agentn.recordedStates[j].state, ((char) 0), 9);
        agentn.recordedStates[j].move = 0;
    }
    int currentPlayer = turn;
    int cnt_1 = 0;
    while (gameEnds(ttt) < 1) {
        int move;
        if (currentPlayer) {
            get_q(ttt, &agentn);
            move = get_move(ttt, &agentn, cnt_1);
            cnt_1 += 1;
        } else {
            move = playMoveRandom(ttt);
        }
        ttt.state[move] = (currentPlayer) ? CIRCLE : CROSS;
        ttt.marker = (currentPlayer) ? 1 : 0;
        int end = gameEnds(ttt);
        if (end == 1) {
            if (currentPlayer) {
                getReward(&agentn, end);
            } else {
                getReward(&agentn, 0);
            }
        } else {
            if (end == 2) {
                getReward(&agentn, end);
            }
        }
        if (end != 0) {
            calculate(&agentn);
        }
        currentPlayer = !currentPlayer;
    }
}

void train_human(struct TicTacToe ttt, struct TabularQLearningAgent agentn, int turn) {
    init_reset(&ttt, turn);
    for (int j=0; j<9; j++) {
        memset(agentn.recordedStates[j].state, ((char) 0), 9);
        agentn.recordedStates[j].move = 0;
    }

    int currentPlayer = turn;
    int cnt_1 = 0;
    while (gameEnds(ttt) < 1) {
        int move;
        if (currentPlayer) {
            get_q(ttt, &agentn);
            move = get_move(ttt, &agentn, cnt_1);
            cnt_1 += 1;
        } else {
            printGameBoard(ttt.state);
            printf("Your move: ");
            scanf("%d", &move);
        }
        ttt.state[move] = (currentPlayer) ? CIRCLE : CROSS;
        ttt.marker = (currentPlayer) ? 1 : 0;
        int end = gameEnds(ttt);
        if (end == 1) {
            printGameBoard(ttt.state);
            if (currentPlayer) {
                getReward(&agentn, end);
                printf("AI wins!\n");
            } else {
                getReward(&agentn, 0);
                printf("You win!\n");
            }
        } else {
            if (end == 2) {
                printGameBoard(ttt.state);
                getReward(&agentn, end);
                printf("You draw!\n");
            }
        }
        if (end != 0) {
            calculate(&agentn);
        }
        currentPlayer = !currentPlayer;
    }
}

void printWeights(struct TabularQLearningAgent *agent1, struct TabularQLearningAgent *agent2) {
    FILE *ptr1 = fopen("C:/Users/CCL/Desktop/CUHKCUHK/sem1/ENGG1110D/q_learning/weights1.txt", "w");
    FILE *ptr2 = fopen("C:/Users/CCL/Desktop/CUHKCUHK/sem1/ENGG1110D/q_learning/weights2.txt", "w");
    if (ptr1 == NULL || ptr2 == NULL) {
        printf("Error\n");
        exit(1);
    }

    fprintf(ptr1, "{");
    for (int j=0; j<19690; j++) {
        fprintf(ptr1, "\"%s\", ", (*agent1->states.keys)[j]);
    }
    fprintf(ptr1, "}\n\n{");
    for (int j=0; j<19690; j++) {
        fprintf(ptr1, "{");
        for (int k=0; k<9; k++) {
            fprintf(ptr1, "%lf,", (*agent1->states.res)[j][k]);
        }
        fprintf(ptr1, "}, ");
    }
    fprintf(ptr1, "}");

    fprintf(ptr2, "{");
    for (int j=0; j<19690; j++) {
        fprintf(ptr2, "\"%s\", ", (*agent2->states.keys)[j]);
    }
    fprintf(ptr2, "}\n\n{");
    for (int j=0; j<19690; j++) {
        fprintf(ptr2, "{");
        for (int k=0; k<9; k++) {
            fprintf(ptr2, "%lf,", (*agent2->states.res)[j][k]);
        }
        fprintf(ptr2, "}, ");
    }
    fprintf(ptr2, "}");
    fflush(ptr1);
    fflush(ptr2);

    fclose(ptr1);
    fclose(ptr2);
}


int main(void) {
    srand(time(0));
    int epochs = 400;
    int batch = 10000;
    double dbatch = (double) batch;

    double one_wins = 0.0;
    double two_wins = 0.0;
    double draws = 0.0;

    struct TicTacToe grid;
    struct TabularQLearningAgent agent1;
    struct TabularQLearningAgent agent2;

    agent1.states.keys = malloc(sizeof *agent1.states.keys);
    agent2.states.keys = malloc(sizeof *agent2.states.keys);
    agent1.states.res = malloc(sizeof *agent1.states.res);
    agent2.states.res = malloc(sizeof *agent2.states.res);

    agent1.alpha = 0.9;
    agent2.alpha = 0.9;
    agent1.d = 0.96;
    agent2.d = 0.96;
    agent1.init_ = 0.6;
    agent2.init_ = 0.6;

    for (int i=0; i<epochs; i++) {
        int turn = 0;
        for (int o=0; o<batch; o++) {
            init_reset(&grid, turn);
            int currentPlayer = turn;
            int cnt_1 = 0;
            int cnt_2 = 0;

            train_random(grid, agent1, turn);
            train_random(grid, agent2, turn);

            // Training with human (optional):

            if (i >= 350) {
                printf("Trianing AI & 2 for 50 epochs (each) to enhance performane!\n");
                train_human(grid, agent1, turn);
                train_human(grid, agent2, turn);
            }

            for (int j=0; j<9; j++) {
                memset(agent1.recordedStates[j].state, ((char) 0), 9);
                memset(agent2.recordedStates[j].state, ((char) 0), 9);
                agent1.recordedStates[j].move = 0;
                agent2.recordedStates[j].move = 0;
            }

            while (gameEnds(grid) < 1) {
                int move;
                if (currentPlayer) {
                    get_q(grid, &agent1);
                    move = get_move(grid, &agent1, cnt_1);
                    cnt_1 += 1;
                } else {
                    get_q(grid, &agent2);
                    move = get_move(grid, &agent2, cnt_2);
                    cnt_2 += 1;
                }
                grid.state[move] = (currentPlayer) ? CIRCLE : CROSS;
                grid.marker = (currentPlayer) ? 1 : 0;
                int end = gameEnds(grid);
                if (end == 1) {
                    if (currentPlayer) {
                        getReward(&agent1, end);
                        getReward(&agent2, 0);
                        one_wins += 1;
                    } else {
                        getReward(&agent2, end);
                        getReward(&agent1, 0);
                        two_wins += 1;
                    }
                } else {
                    if (end == 2) {
                        getReward(&agent1, end);
                        getReward(&agent2, end);
                        draws += 1;
                    }
                }
                if (end != 0) {
                    calculate(&agent1);
                    calculate(&agent2);
                }
                currentPlayer = !currentPlayer;
            }
            turn = !turn;
        }
        printf("[%d] 1 win %: %.3f | 2 win %: %.3f | Draw %: %.3f\n\n", (i+1), (one_wins/ dbatch), (two_wins/ dbatch), (draws/ dbatch));
        draws = 0.0;
        one_wins = 0.0;
        two_wins = 0.0;
    }
    printf("Testing begins: AI versus human!\n");
    test(grid, agent1, agent2);

    printf("%lf %lf %lf\n", agent1.alpha, agent1.d, agent1.init_);
    printf("%lf %lf %lf\n", agent2.alpha, agent2.d, agent2.init_);
    printWeights(&agent1, &agent2);
    return 0;
}
