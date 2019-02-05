#include "sudoku.h"
#include "stdlib.h"
#include "stdio.h"
#include "sudoku.h"
#include <assert.h>

/*
  This file describes the interface for a Sudoku Abstract
  Data Type.

  You may not change the protoype for any function in this file, but
  you should provide a definition for all functions.  Also, you
  may define any additional functions you need within this file. But, they
  can only be used within this file (i.e. you cannot modify main).

  Your implementation will be tested with a different main function than the
  one provided to fully evaluate your entire interface.  Make sure that your
  function definitions work properly even if the main I provided you doesn't test
  them.
 */


const int base_row[9] = {1,2,3,4,5,6,7,8,9};
const int base_col[9] = {1,10,19,28,37,46,55,64,73};
const int base_square[9] = {1,2,3,10,11,12,19,20,21};

typedef struct sudoku_square{
  char value;
  int original_value;
  int row;
  int col;
  int big_square;
  int possible[10];
} Cell;

struct sudoku {
  Cell cells[81];
};


struct sudoku game;

void Sudoku_set_position(Cell*, int);
void Sudoku_set_possible(Cell*);
int Sudoku_check_contradiction(int, int, int, char);
int solver();
int Get_best_guess(Cell*);
int Check_win();

/*======================================================================*/
/*======================================================================*/


/* DEFINE EXTRA FUNCTIONS IN HERE */

// Convert row and col into a linear index between 0 and 81
static int index(char row, int col)
{
  return (row -'A')*9 + (col-1);
}


/*======================================================================*/
/*======================================================================*/

/* IMPLEMENT SUDOKU INTERFACE */

/*
   Sudoku_initialize: Initialize any global variables or other
      data that you need.  This function should be called first,
      before any other function.

   Return values:
      SUDOKU_SUCCESS: initialize succeeded.
      SUDOKU_UNKNOWN_ERROR: there was a problem.
 */
int Sudoku_initialize (void)
{
  /* PUT YOUR IMPLMENTATION HERE */
  int i;
  for(i=0; i<81; i++){
    game.cells[i].value = '-';
    game.cells[i].original_value = '-';
    Sudoku_set_position(&game.cells[i], i);
    int j;
    for( j = 0; j <10; j++){
      game.cells[i].possible[j] = 1;
    }
  }
  return SUDOKU_SUCCESS;
}

/*
  Sudoku_loadfile: Initialize the game to the board held in filename. The
  current game, if any, is discarded.

  The structure of the file must be as follows:
    - A digit represents a value from 1-9 and a - represents an empty game position.
    - There will be 9 lines in the file. Each line corresponds to one
      row of the Sudoku puzzle
    - Each line contains 9 digits/dashes. Each digit/dash falls into one of the 9 columns.
    - Read in the file and populate your structure.

  Return the following error codes:
     SUDOKU_SUCCESS : file read successfully
     SUDOKU_NO_FILE_ERROR : no file found or cannot open file
     SUDOKU_BAD_FORMAT : file contents in invalid format:
           - fewer than 9 digits/dashes on a line
           - fewer than 9 lines in the file
	   - unknown character
     SUDOKU_UNKNOWN_ERROR : any other error you think of
 */
int Sudoku_loadfile(const char *filename)
{
  Sudoku_initialize();
  FILE *file = fopen(filename,"r");
  if (file==NULL){
    return SUDOKU_NO_FILE_ERROR;
  }
  int row;
  int col;
  for(row=0; row<9; row++){
    for(col=1; col<=9; col++){
  	  char val;
  	  if(fscanf(file,"%c", &val)!= EOF){
        int error = Sudoku_set_square((char)(row +'A'),col,val);
        if(val == '\n'){
          return SUDOKU_BAD_FORMAT;
        }
        else if(error != SUDOKU_SUCCESS){
          return SUDOKU_BAD_FORMAT;
        }
        game.cells[index((char)(row +'A'),col)].original_value = val;
      }
      else{
        return SUDOKU_BAD_FORMAT;
      }
	  }
  fscanf(file,"%*c");
  }
  fclose(file);
  return SUDOKU_SUCCESS;
}

/*
  Sudoku_savefile: Save the current game to filename.

  The structure of the file must be compatible with Sudoku_loadfile.

  Return the following error codes:
     SUDOKU_SUCCESS : file read successfully
     SUDOKU_NO_FILE_ERROR : no file found or cannot open file
     SUDOKU_UNKNOWN_ERROR : any other error
 */

int Sudoku_savefile(const char *filename)
{
  FILE* file = fopen(filename,"w");
  if (file==NULL){
    return SUDOKU_NO_FILE_ERROR;
  }
  int i;
  for(i = 0; i < 81; i++){
    if(i % 9 == 0 && i != 0){
      fprintf(file,"\n");
    }
    fprintf(file, "%c", game.cells[i].value);
  }
  fclose(file);
  return SUDOKU_SUCCESS;
}

/*
  Sudoku_set_square: set the square at position (col, row) to value
  digit.

  Return values:
     SUDOKU_SUCCESS : square set successfully
     SUDOKU_CONTRADICTION : square set but it leads to a contradiction.
        - You do not need to solve the puzzle, just look at current values
	  and see if a contradiction already exists.
     SUDOKU_ILLEGAL_VALUE : digit is not legal.
     SUDOKU_ILLEGAL_INDEX : col and row do not form a legal index
 */
int Sudoku_set_square(char row, int col, char digit){
  int cell_index = index(row,col);
  if(cell_index < 0 || cell_index > 80 || row< 'A' || row > 'I' || col < 1 || col > 9){
    return SUDOKU_ILLEGAL_INDEX;
  }
  if((digit < '1' || digit > '9') && digit != '-'){
    return SUDOKU_ILLEGAL_VALUE;
  }
  else{
    int error = Sudoku_check_contradiction(game.cells[cell_index].row,game.cells[cell_index].col,game.cells[cell_index].big_square, digit);
    if(error != SUDOKU_SUCCESS){
      return error;
    }
    game.cells[cell_index].value = digit;
  }
  if(digit != '-'){
    Sudoku_set_possible(&(game.cells[cell_index]));
  }
  return SUDOKU_SUCCESS;
}
int Check_win(){
  int i = 0;
  for(i = 0;  i < 81; i++){
    if(game.cells[i].value == '-'){
      return SUDOKU_NO_SOLUTION;
    }
  }
  return SUDOKU_SOLVED;
}

void Sudoku_set_position(Cell* cell, int index){
  int i = 0;
  int j = 0;
  int mult = 0;
  int shift = 0;
  for(i = 0; i < 9; i++){
    for(j = 0; j < 9; j++){
      if(index + 1 == (base_row[j] + (9*i))){
        cell->row = i;
      }
      if(index + 1 == base_col[j] + i){
        if(i == 9){
          cell->col = 0;
        }
        else{
          cell->col = i;
        }
      }
      if(index + 1 == base_square[j] + shift + 3*mult){
        cell ->big_square = i;
      }
    }
    mult++;
    if((i+1)%3 == 0 ){
      mult = 0;
      shift = 27;
    }
    if((i+1)%6 == 0){
      mult = 0;
      shift = 27 * 2;
    }
  }
}

int Sudoku_check_contradiction(int row, int col, int big_square, char value){
  int i;
  for(i = 0; i < 81; i++){
    if(row == game.cells[i].row ){
      if(value == game.cells[i].value && value != '-'){
        return SUDOKU_CONTRADICTION;
      }
    }
    if(col == game.cells[i].col ){
      if(value == game.cells[i].value && value != '-'){
        return SUDOKU_CONTRADICTION;
      }
    }
    if(big_square == game.cells[i].big_square ){
      if(value == game.cells[i].value && value != '-'){
        return SUDOKU_CONTRADICTION;
      }
    }
  }
  return SUDOKU_SUCCESS;
}

void Sudoku_set_possible(Cell* cell){
  int i = 0;
  for( i = 0; i < 81; i++){
    int row = game.cells[i].row;
    int col = game.cells[i].col;
    int square = game.cells[i].big_square;
    if(cell->row == row && cell->col == col && cell->big_square == square){
      if(cell->value != '-'){
        int j = 0;
        for(j = 0; j < 10; j++){
          cell->possible[j] = 0;
        }
      }
    }
    else if(cell->row == row){
      game.cells[i].possible[cell->value - '0'] = 0;
    }
    else if(cell->col == col){
      game.cells[i].possible[cell->value-'0'] = 0;
    }
    else if (cell->big_square == square){
      game.cells[i].possible[(cell->value - '0')] = 0;
    }
  }
}

/*
  Sudoku_get_square: return the square at position (col, row) by reference in *value.

  Return values:
     SUDOKU_SUCCESS : value is returned successfully in *value
     SUDOKU_ILLEGAL_INDEX : col and row do not form a legal index
 */
int Sudoku_get_square (char row, int col, char *value)
{
  if(row < 'A' || row > 'I' || col < 1 || col > 9){
    return SUDOKU_ILLEGAL_INDEX;
  }
  else{
    int cell_index = index(row,col);
    *value = game.cells[cell_index].value;
    return SUDOKU_SUCCESS;
  }
}

/*
  Sudoku_give_hint: return a hint by value.  The col, row, and value are returned
     by reference in *col, *row, and *value.

  Return value:
     SUDOKU_SUCCESS : hint supplied
     SUDOKU_NO_HINT : no hint because solver could not find one.
*/
int Sudoku_give_hint (char *r, int *c, char *value )
{
  int i;
  for(i = 0; i < 81; i++ ){
    if(game.cells[i].possible[0] != 0){
      int j;
      int count = 0;
      for(j = 1; j < 10; j++){
        if(game.cells[i].possible[j] == 1){
          count++;
        }
        if(count>1){
          break;
        }
      }
      if(count == 1){
        for(j = 1; j < 10; j++){
          if(game.cells[i].possible[j] == 1){
            *r = (char)(game.cells[i].row + 'A');
            *c = game.cells[i].col+1;
            *value = (char) (j + '0');
            return SUDOKU_SUCCESS;
          }
        }
      }
    }
  }
  return SUDOKU_NO_HINT;
}

//returns 1 when puzzle is solved
int solver(Board* solution){
  int i;
  static int obvious = 0;
  static int guesses = 0;
  int guesses_state = guesses;
  int obvious_state = obvious;
  for(i = 0; i < 81; i++){
    char r ='O';
    char value = '0';
    int c = 0;
    //check to see if a logical deduction can be made.
    int error = Sudoku_give_hint(&r, &c, &value);
    //no logical deduction was found, break from the deduction loop.
    if(error == SUDOKU_NO_HINT){
      break;
    }
    //a logical deduction was found fill in the square.
    else{
      Sudoku_set_square(r,c,value);
      int j;
      for(j = 0; j < 81; j++){
        solution[obvious+guesses].value[j] = game.cells[j].value;
      }
      solution[obvious+guesses].row = r;
      solution[obvious+guesses].col = c + '0';
      solution[obvious+guesses].fill = value;
      solution[obvious+guesses].obvious = 1;
      solution[obvious+guesses].guess= 0;
      solution[obvious+guesses].blank = 1;
      obvious++;
      obvious_state++;
    }
  }

  //check to see if logical deductions have solved the board.
  int Chicken_dinner = Check_win();
  if(Chicken_dinner == SUDOKU_SOLVED){
    printf("It took %d guesses and %d deductions for a total of %d moves to solve the board.\n", guesses, obvious, guesses+obvious);
    guesses = 0;
    obvious = 0;
    return Chicken_dinner;
  }

  //try to get the best cell to make a guess.
  Cell guess_location;
  int board_status = Get_best_guess(&guess_location);
  if(board_status == SUDOKU_SOLVED){
    return board_status;
  }

  //save the game state
  struct sudoku temp;
  for(i = 0; i < 81; i++){
    temp.cells[i].value = game.cells[i].value;
    temp.cells[i].row = game.cells[i].row;
    temp.cells[i].col = game.cells[i].col;
    temp.cells[i].big_square = game.cells[i].big_square;
    int j;
    for(j = 0; j < 10; j++){
      temp.cells[i].possible[j] = game.cells[i].possible[j];
    }
  }
  //begin making guesses
  for(i = 1; i < 10; i++){
    //if value of possible array is 1, then i is the guess number.
    if(guess_location.possible[i] == 1){
      Sudoku_set_square((guess_location.row) + 'A',(guess_location.col)+1, i + '0');
      int j;
      for(j = 0; j < 81; j++){
        solution[obvious+guesses].value[j] = game.cells[j].value;
      }
      solution[obvious+guesses].row = guess_location.row + 'A';
      solution[obvious+guesses].col = (guess_location.col+1) + '0';
      solution[obvious+guesses].fill = i + '0';
      solution[obvious+guesses].guess = 1;
      solution[obvious+guesses].obvious = 0;
      solution[obvious+guesses].blank = 1;
      guesses++;
      int done = solver(solution);
      if(done == SUDOKU_SOLVED){
        return done;
      }
      //guess was incorrect load in game state before guess
      else{
        guesses = guesses_state;
        obvious = obvious_state;
        int y;
        for(y = 0; y < 81; y++){
          game.cells[y].value = temp.cells[y].value;
          game.cells[y].row = temp.cells[y].row;
          game.cells[y].col = temp.cells[y].col;
          game.cells[y].big_square = temp.cells[y].big_square;
          int j;
          for(j = 0; j < 10; j++){
            game.cells[y].possible[j] = temp.cells[y].possible[j];
          }
        }
      }
    }
  }
  return SUDOKU_NO_SOLUTION;
}

int Get_best_guess(Cell* best_guess){
  int i;
  int lowest_count = 10;
  int board_status = SUDOKU_SOLVED;
  for(i = 0; i < 81; i++){
    int j;
    int count = 0;
    for(j = 0; j < 10; j++){
      if(game.cells[i].possible[j] == 1){
        count++;
        board_status = SUDOKU_NO_SOLUTION;
      }
    }
    if(count < lowest_count && count != 0){
      *best_guess = game.cells[i];
      lowest_count = count;
    }
  }
  return board_status;
}



/*
  Print the Sodoku game to the screen.  Your code must work with this
  function as implemented.

  DO NOT MODIFY
 */
void Sudoku_print(void)
{
  char row = 'A';
  int col;
  char value;

  // Print the board: DO NOT MODIFY

  printf("   | 1  2  3 | 4  5  6 | 7  8  9 |\n");
  for(row='A'; row <= 'I'; row++)
    {
      if ((row-'A') % 3 == 0)
      printf("   |---------+---------+---------|\n");

      printf(" %c |",row);
      col = 1;

      for(; col<=3; col++)
	{
	  Sudoku_get_square(row,col,&value);
	  printf(" %c ",value);
	}
      printf("|");
      for(; col<=6; col++)
	{
	  Sudoku_get_square(row,col,&value);
	  printf(" %c ",value);
	}
      printf("|");
      for(; col<=9; col++)
	{
	  Sudoku_get_square(row,col,&value);
	  printf(" %c ",value);
	}
      printf("|\n");
    }
    printf("   |---------+---------+---------|\n");
}
