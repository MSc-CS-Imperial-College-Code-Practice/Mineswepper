#include <iostream>
#include <fstream>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <cassert>
#include "minesweeper.h"

using namespace std;

/* You are pre-supplied with the functions below. Add your own 
   function definitions to the end of this file. */

/* pre-supplied function to load mine positions from a file */
void load_board(const char *filename, char board[9][9]) {

  cout << "Loading board from file '" << filename << "'... ";

  ifstream in(filename);
  if (!in)
    cout << "Failed!" << endl;
  assert(in);

  char buffer[512];

  int row = 0;
  in.getline(buffer,512);
  while (in && row < 9) {
    for (int n=0; n<9; n++) {
      assert(buffer[n] == '.' || buffer[n] == '*' || buffer[n] == ' ' || buffer[n] == '?' || isdigit(buffer[n]));
      board[row][n] = buffer[n];
    }
    row++;
    in.getline(buffer,512);
  }

  cout << ((row == 9) ? "Success!" : "Failed!") << endl;
  assert(row == 9);
}

/* internal helper function */
void print_row(const char *data, int row) {
  cout << (char) ('A' + row) << "|";
  for (int i=0; i<9; i++) 
    cout << ( (data[i]=='.') ? ' ' : data[i]);
  cout << "|" << endl;
}

/* pre-supplied function to display a minesweeper board */
void display_board(const char board[9][9]) {
  cout << "  ";
  for (int r=0; r<9; r++) 
    cout << (char) ('1'+r);
  cout <<  endl;
  cout << " +---------+" << endl;
  for (int r=0; r<9; r++) 
    print_row(board[r],r);
  cout << " +---------+" << endl;
}

/* pre-supplied function to initialise playing board */ 
void initialise_board(char board[9][9]) {
  for (int r=0; r<9; r++)
    for (int c=0; c<9; c++)
      board[r][c] = '?';
}

/* add your functions here */

bool is_complete(const char mines[9][9], const char unrevealed[9][9]){
  for(int r = 0; r < 9; r++){
    for(int c = 0; c < 9; c++){
      if(mines[r][c] == '*' && unrevealed[r][c] == '?')
        return false; 
    }
  }
  return true;
}

int count_mines(const char* position, const char mines[9][9]){
  
  int mines_number = 0; // mines number around square
  int row, col; // row and column position (zero indexed)

  get_position(position, row, col);

  for (int i = -1; i <= 1; i++){
    // Boundary cases for rows -> skip
    if((row == 0 && i == -1) || (row == 8 && i == 1))
      continue;
    for (int j = -1; j <= 1; j++){
      // Not reading self position
      if(i==0 && j==0)
        continue;
      // Boundary cases for columns -> skip
      if((col == 0 && j == -1) || (col == 8 && j == 1))
        continue;
      // Calculate mines
      if(mines[row+i][col+j] == '*')
        mines_number++;
      
    }
  }
  return mines_number;
}

int count_mines(int row, int col, const char mines[9][9]){
  
  int mines_number = 0; // mines number around square

  for (int i = -1; i <= 1; i++){
    // Boundary cases for rows -> skip
    if((row == 0 && i == -1) || (row == 8 && i == 1))
      continue;
    for (int j = -1; j <= 1; j++){
      // Not reading self position
      if(i==0 && j==0)
        continue;
      // Boundary cases for columns -> skip
      if((col == 0 && j == -1) || (col == 8 && j == 1))
        continue;
      // Calculate mines
      if(mines[row+i][col+j] == '*')
        mines_number++;
      
    }
  }
  return mines_number;
}

int count_unrevealed(int row, int col, const char revelead[9][9]){
  
  int unrevealed_squares = 0; // mines number around square

  for (int i = -1; i <= 1; i++){
    // Boundary cases for rows -> skip
    if((row == 0 && i == -1) || (row == 8 && i == 1))
      continue;
    for (int j = -1; j <= 1; j++){
      // Not reading self position
      if(i==0 && j==0)
        continue;
      // Boundary cases for columns -> skip
      if((col == 0 && j == -1) || (col == 8 && j == 1))
        continue;
      // Calculate unrevelaed squares
      if(revelead[row+i][col+j] == '?')
        unrevealed_squares++;
      
    }
  }
  return unrevealed_squares;
}

MoveResult make_move(const char* position, const char mines[9][9], 
  char unrevealed[9][9]){

  int row, col; // row and column position (zero indexed)
  get_position(position, row, col);

  // Check invalid move
  if(row == -1 || col == -1)
    return INVALID_MOVE;
  // Check redundant move
  if(unrevealed[row][col] == ' ' || unrevealed[row][col] == '*' || 
    isdigit(unrevealed[row][col]))
    return REDUNDANT_MOVE;
  // Check blown up
  if(mines[row][col] == '*' && !is_flagging(position))
    return BLOWN_UP;
  // Check solved board
  if(is_complete(mines, unrevealed))
    return SOLVED_BOARD;
  
  // Flooding or Flagging squares
  if(is_flagging(position))
    unrevealed[row][col] = '*';
  else
    flooding(row, col, mines, unrevealed);

  return VALID_MOVE;
}

void get_position(const char* position, int &row, int &col){

  if (strlen(position) < 2){
    row = -1;
    col = -1;
    return;
  }

  if (strlen(position) == 3 && position[2] != '*'){
    row = -1;
    col = -1;
    return;
  }

  char row_chr = position[0];
  char col_chr = position[1];

  if(row_chr < 'A' || row_chr > 'I' || col_chr <'1' || col_chr > '9'){
    row = -1;
    col = -1;
    return;
  }
  
  // Using 0 indexing positions
  row = static_cast<int>(row_chr)-65; // 65 -> 'A' in ASCII code
  col = static_cast<int>(col_chr)-49; // 49 -> '1' in ASCII code
}

bool find_safe_move(char revealed[9][9], char* move){
  
  int number_of_mines;
  int row_unrel, col_unrel;

  for(int row = 0; row < 9; row++){
    for (int col =0; col < 9; col++){
      if(isdigit(revealed[row][col]) && count_unrevealed(row,col,revealed)==1){
        
        number_of_mines = static_cast<int>(revealed[row][col]) - '0';
        if(number_of_mines==count_mines(row,col,revealed)+1){
          
          get_position_unrevealed_neighbour(row, col, revealed,
          row_unrel, col_unrel);

          move[0] = static_cast<char>(row_unrel) + 'A';
          move[1] = static_cast<char>(col_unrel+1) + '0';
          move[2] = '*';
          move[3] ='\0';
          return true;
        }
        else if(number_of_mines==count_mines(row,col,revealed)){
          
          get_position_unrevealed_neighbour(row, col, revealed,
          row_unrel, col_unrel);
          move[0] = static_cast<char>(row_unrel) + 'A';
          move[1] = static_cast<char>(col_unrel+1) + '0';
          move[2] ='\0';
          return true;
        }
      }
    }
  }

  return false;
}

void flooding(int row, int col, const char mines[9][9], 
  char unrevealed[9][9]){
  
  int number_of_mines;
  char number_of_mines_chr;

  // Base Case 1 -> out of bounds
  if(is_out_of_bound(row,col))  
    return;
  // Base Case 2 -> space already revealed
  if(unrevealed[row][col] != '?')  
    return;

  // Recursive Case -> Flodding

  // Update unrevealed board
  number_of_mines = count_mines(row,col,mines);
  if(number_of_mines == 0)
    number_of_mines_chr = ' ';
  else
    number_of_mines_chr = static_cast<char>(number_of_mines)+'0';
  unrevealed[row][col] = number_of_mines_chr;
  
  // If it is found neighboard mines
  if(number_of_mines != 0){
    return;
  }

  // Flooding
  for (int i = -1; i <=1; i++ ){
      for (int j = -1; j <=1; j++){
      flooding(row+i, col+j, mines,unrevealed);
      }
  }
  return;
}

void get_position_unrevealed_neighbour(int row, int col,char revealed[9][9],
    int &row_unrel, int &col_unrel){

  for (int i = -1; i <= 1; i++){
    // Boundary cases for rows -> skip
    if((row == 0 && i == -1) || (row == 8 && i == 1))
      continue;
    for (int j = -1; j <= 1; j++){
      // Not reading self position
      if(i==0 && j==0)
        continue;
      // Boundary cases for columns -> skip
      if((col == 0 && j == -1) || (col == 8 && j == 1))
        continue;
      // Get position 1st unrevealed square
      if(revealed[row+i][col+j] == '?'){
        row_unrel = row+i;
        col_unrel = col+j;
        return;
      }

      
    }
  }
 
}

bool is_flagging(const char* position){

  if (strlen(position) == 3 && position[2] == '*'){
    return true;
  }
  return false;
}

bool is_out_of_bound(int row, int col){

  if (row < 0 || row > 8 || col < 0 || col > 8)
    return true;
  return false;
}
