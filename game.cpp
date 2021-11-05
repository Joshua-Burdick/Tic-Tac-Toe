#include <iostream>
#include <stdio.h>
#include <thread>
#include <future>
#include <vector>

#include <random>
#include <time.h>

using namespace std;

//Game mechanics
void drawBoard(char board[3][3]);
bool GetMove(char board[3][3], int& row, int& col);
bool gameWon(char board[3][3]);
bool boardFull(char board[3][3]);
void botMove(char board[3][3], char side, int& row, int& col);
void botDec(char board[3][3], char side, int& row, int& col, bool& win);

//Misc. functions
bool Validate(char board[3][3], int row, int col);
void duo();
void bot();

int main() {

    srand(time(NULL));

    int a;

    printf("1. 2 players\n");
    printf("2. Play the computer\n");
    printf("3. Exit\n"); //Exits other than (1) or (2)

    printf("  Enter your choice: ");
    cin >> a;
    cout << endl;

    if(a == 1) duo();
    else if(a == 2) bot();

    return 0;
}

//Draw the board to the screen
void drawBoard(char board[3][3]) {
    printf("    1   2   3\n");
    for(int i = 0; i < 3; i++) {
        printf("  +---+---+---+\n%d ", i + 1);
        for (int j = 0; j < 3; j++) {
            printf("| %c ", board[i][j]);
        }
        printf("|\n");
    }
    printf("  +---+---+---+\n\n");
}

bool GetMove(char board[3][3], int& row, int& col) {
    printf("Enter row (1-3): ");
    cin >> row;

    printf("Enter col (1-3): ");
    cin >> col;

    if(Validate(board, row, col)) {
        //Shift for to 0-indexed array
        row -= 1;
        col -= 1;
        return true;
    }
    return false;
}

bool Validate(char board[3][3], int row, int col) {
    if((row >= 1 && row <=3) && (col >= 1 && col <= 3)) {
        if(board[row - 1][col - 1] != ' ') {
            printf("* (%d, %d) is already in use\n", row, col);
            return false;
        }
        return true;
    }
    else {
        printf("* Row or column must be 1 to 3\n");
        return false;
    }
}

bool gameWon(char board[3][3]) {
    //Check rows and columns for wins (so long as they aren't spaces)
    for (int i = 0; i < 3; i++) {
        if((board[i][0] != ' ') && (board[i][0] == board[i][1]) && (board[i][0] == board[i][2])) return true; // Rows
        else if((board[0][i] != ' ') && (board[0][i] == board[1][i]) && (board[0][i] == board[2][i])) return true; // Cols
    }
    //Check diagonals for wins (so long as they aren't spaces)
    if((board[0][0] != ' ') && (board[0][0] == board[1][1]) && (board[0][0] == board[2][2])) return true; //Main diagonal
    else if((board[0][2] != ' ') && (board[0][2] == board[1][1]) && (board[0][2] == board[2][0])) return true; //Opposite diagonal

    return false; //False if game is not won
}

bool boardFull(char board[3][3]) {
    int count = 0;
    for(int i = 0; i < 3; i++) {
        for(char j : board[i]) {
            if(j != ' ') count++;
        }
    }

    if(count == 9) return true;
    return false;
}

void duo() {
    bool playing = true;
    char player = 'X';
    int row, col;

    char board[3][3] = {
        {' ', ' ', ' '},
        {' ', ' ', ' '},
        {' ', ' ', ' '}
    };

    drawBoard(board);

    //Game loop
    do {
        do {
            printf("\n%c's turn\n", player);
        } while(!GetMove(board, row, col));

        //Draw and validate the new board
        printf("\n");
        board[row][col] = player;
        drawBoard(board);

        if(gameWon(board)) {
            printf("Game over! Player %c wins!", player);
            playing = false;
        }
        else if(boardFull(board)) {
            printf("The game is a draw!");
            playing = false;
        }

        //Switch player
        (player == 'X') ? (player = 'O') : (player = 'X');
    } while(playing);
}

void bot() {
    bool playing = true, checking = true;
    char player, robo, main = 'X';
    int row, col;

    printf("Chose side (X|O): ");
    cin >> player;
    cout << endl;

    player = toupper(player); //In case user inputs lower case letter

    (player == 'X') ? (robo = 'O') : (robo = 'X');

    printf("Player:   %c\n", player);
    printf("Computer: %c\n\n", robo);

    char board[3][3] = {
        {' ', ' ', ' '},
        {' ', ' ', ' '},
        {' ', ' ', ' '}
    };

    drawBoard(board);

    //Game loop
    do {
        if(main == player) {
            do {
                printf("\n%c's turn\n", player);
            } while(!GetMove(board, row, col));
        }
        else { //Bot moves
            printf("\n%c's turn\n", robo);
            do {
                botMove(board, robo, row, col);

                if(board[row][col] == ' ') {
                    checking = false;
                }
                else checking = true;
            } while(checking);
        }

        //Draw and validate the new board
        printf("\n");
        board[row][col] = main;
        drawBoard(board);

        if(gameWon(board)) {
            printf("Game over! Player %c wins!", main);
            playing = false;
        }
        else if(boardFull(board)) {
            printf("The game is a draw!");
            playing = false;
        }

        //Switch player
        (main == 'X') ? (main = 'O') : (main = 'X');
    } while(playing);
}

void botMove(char board[3][3], char side, int& row, int& col) {

    /*row = rand() % 3;
    col = rand() % 3;*/
    
    vector<vector<int>> pairs;
    vector<thread> threads;
    vector<bool> choices;
    
    for(int i = 0; i < 3; i++) { //Check for available moves
        for(int j = 0; j < 3; j++) {
            if(board[i][j] == ' ') {
                pairs.push_back({i, j});
            }
        }
    }

    for(int i = 0; i < pairs.size(); i++) { //Compue the moves on a separate thread
        bool t;
        threads.push_back(thread(&botDec, board, side, ref(pairs[i][0]), ref(pairs[i][1]), ref(t)));
        choices.push_back(t);
    }

    for(int i = 0; i < threads.size(); i++) { //Wait for the threads to complete
        threads[i].join();
    }

    for(int i = 0; i < choices.size(); i++) { //Pick a winning move, if one exists. Otherwise, the bot makes a random move.
        if(choices[i]) {
            row = pairs[i][0];
            col = pairs[i][1];
        }
        else {
            row = rand() % 3;
            col = rand() % 3;
        }
    }
}

void botDec(char board[3][3], char side, int& row, int& col, bool& win) {
    board[row][col]= side;
    if(gameWon(board)) {
        board[row][col] = ' ';
        win = true;
    }
    else {
        board[row][col] = ' ';
        win = false;
    }
}
