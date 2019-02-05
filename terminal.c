/*
  Sudoku Game Driver

  This is the driver program for the Sudoko ADT provided
  by sudoku.h.

  This file is provided to help you test your code.  There may be
  unintentional bugs here.  Buyer beware!

  You have the following options:
	command              : description
	load <filename>      : load a game from file
	save <filename>      : load a game to file

	set <A-I><1-9> <val> : set position to val
	reset <A-I><1-9>     : reset position

	reset                : reset to the beginning of play

	hint                 : provide a hint about next square to set
	solve                : find a solution if one exists
	print                : print game

	quit                 : exit

 */
#include "sudoku.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void Sudoku_print_solution(Board);
void usage()
{
  printf("You have the following options:\n");
  printf("\tcommand              : description\n");
  printf("\tload <filename>      : load a game from file\n");
  printf("\tsave <filename>      : load a game to file\n");
  printf("\n");
  printf("\tset <A-I><1-9> <val> : set position to val\n");
  printf("\tclear <A-I><1-9>      : reset position\n");
  printf("\n");
  printf("\treset                : reset to the beginning of play\n");
  printf("\n");
  printf("\thint                 : provide a hint about next square to set\n");
  printf("\tsolve                : find a solution if one exists\n");
  printf("\tprint                : print game\n");
  printf("\n");
  printf("\tquit                 : exit\n");
}

int main(int argc, char **argv)
{
  int num;
  char command[10];
  char filename[32];
  char move[5];
  int val;

  Sudoku_initialize();

  usage();
  while(1)
    {
      printf(">> ");
      fflush(stdout);

      num = scanf("%10s",command);

      if (num)
	{
	  if (strcmp(command,"load")==0)
	    {
	      num = scanf("%31s",filename);
	      if (num)
		{
		  printf("File = %s\n",filename);
		  int ret;
		  if( SUDOKU_SUCCESS != (ret=Sudoku_loadfile(filename)) )
		    printf("problem loading file! %d", ret);

		}
	    }
	  else if (strcmp(command,"reset")==0)
	    {
	      int col;
	      char row;
	      for(col=1; col<=9; col++)
      		for(row='A'; row<='A'+9; row++)
      		  Sudoku_set_square(row,col,'-');
	    }
    else if (strcmp(command,"solve")==0)
	    {
        Board* solution = malloc(81 * sizeof(Board));
        int i;
        for(i = 0; i< 81; i++){
          solution[i].blank = 0;
          solution[i].guess = 0;
          solution[i].obvious = 0;
        }
	      solver(solution);
        printf("\n   |--------+ SOLUTION +---------|\n\n");
        for(i = 0; i < 81; i++){
          Sudoku_print_solution(solution[i]);
        }
        free(solution);

	    }
	  else if (strcmp(command,"hint")==0)
	    {
	      char row;
	      int col;
	      char value;
	      if (Sudoku_give_hint(&row, &col, &value)==SUDOKU_SUCCESS){
		       printf("Try setting %c%d to %c\n", row,col, value);
		    }
	    }
	  else if (strcmp(command,"set")==0)
	    {
	      num = scanf("%4s %d",move,&val);
	      if (num==2){
		      Sudoku_set_square(move[0],move[1]-'0','0'+val);
		    }
	    }
	  else if (strcmp(command,"print")==0)
	    {
	      Sudoku_print();
	    }
	  else if (strcmp(command,"save")==0)
	    {
	      num = scanf("%31s",filename);
	      if (num)
		{
		  Sudoku_savefile(filename);
		}
	    }
	  else if (strcmp(command,"quit")==0 || strcmp(command,"q")==0)
	    {
	      break;
	    }
	  else
	    {
	      printf("I am not user friendly. Please use only these commands:\n");
	      usage();
	    }

	}

    }

  printf("Thanks for playing Sodoku!\n");

  return EXIT_SUCCESS;
}


void Sudoku_print_solution(Board board)
{
  static int step = 0;
  step++;
  if(board.blank == 0){
    step = 0;
    return;
  }
  printf(" Step %d: ", step);

  if(board.guess == 1){
    printf("A guess was made at row: %c, col: %c witha value of %c\n", board.row, board.col, board.fill);
  }
  else{
    printf("A logical deduction was made at row: %c, col: %c with a value of %c\n", board.row, board.col, board.fill);
  }

  printf("   | 1  2  3 | 4  5  6 | 7  8  9 |\n");
  printf("   |---------+---------+---------|\n");

  char row = 'A';
  printf(" %c |",row);
  row++;
  int i;
  for(i = 0; i< 81; i++){
    if((i+1) % 27 == 0){
      printf(" %c ", board.value[i]);
      printf("|");
      printf("\n");
      printf("   |---------+---------+---------|\n");
      if(i < 80){
        printf(" %c |",row);
      }
      row++;
    }
    else if( (i+1) % 9 == 0 ){

      printf(" %c ", board.value[i]);
      printf("|\n");
      if(i < 79){
        printf(" %c |",row);
      }
      row++;
    }
    else if ((i+1) % 3 == 0){
      printf(" %c ", board.value[i]);
      printf("|");
    }
    else{
      printf(" %c ", board.value[i]);
    }
  }
  printf("\n\n");

}
