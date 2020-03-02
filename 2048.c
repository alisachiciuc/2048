#include <stdio.h> 
#include <curses.h> 
#include <time.h>
#include <stdlib.h> 
#include <string.h>
#define NEW_GAME 1
#define RESUME 2
#define EXIT 3
#define WIN 1
#define GAME_OVER 2

typedef struct _tile {
	int value;
	int changed;
	int tem_old;
	int old;
} TILE;

typedef struct _cell {
	int x;
	int y;
} CELL;

typedef struct _score {
	int value;
	int tem_old;	
	int old;
	int high;
} SCORE;

void print_menu(WINDOW *menu_win, int highlight, char** choices, int numberOfChoices) {  
	int i;
	wattron(menu_win, COLOR_PAIR(5));
	box(menu_win, 0, 0);
	wattroff(menu_win, COLOR_PAIR(5));
	for (i = 0; i < numberOfChoices; ++i) {
		if (highlight == i + 1) { 
			wattron(menu_win, COLOR_PAIR(3) | A_BOLD); 
			mvwprintw(menu_win, 2+i, 2, "%s", choices[i]); 
			wattroff(menu_win, COLOR_PAIR(3) | A_BOLD); 
		} else { 
			mvwprintw(menu_win, 2+i, 2, "%s", choices[i]); 
		}
	} 
	wattron(menu_win, COLOR_PAIR(2) | A_BOLD); 
	mvwprintw(menu_win, 0, 6, "2048"); 
	wattroff(menu_win, COLOR_PAIR(2) | A_BOLD);
	wattron(menu_win, COLOR_PAIR(5));
	mvwaddch(menu_win, 0, 5, ACS_RTEE);
	mvwaddch(menu_win, 0, 10, ACS_LTEE);
	wattroff(menu_win, COLOR_PAIR(5));
}

int menu(int terminalX, int terminalY, int _highlight) {
	char *choices[] = { 
		"NEW GAME", 
		"RESUME", 
		"EXIT", 
	}; 
	int numberOfChoices = sizeof(choices) / sizeof(char *); 
	WINDOW *menu_win; 
	int highlight = _highlight; 
	int choice = 0; 
	int c;
	int width = 16;
	int height = 7;
	int startx = (terminalX - width) / 2; 
	int starty = (terminalY - height) / 2;
	menu_win = newwin(height, width, starty, startx); 
	keypad(menu_win, TRUE); 
	print_menu(menu_win, highlight, choices, numberOfChoices); 
	wrefresh(menu_win); 
	while (1) { 
		c = wgetch(menu_win); 
		switch (c) { 
			case KEY_UP: 
				if (highlight == 1) 
					highlight = numberOfChoices; 
				else 
					--highlight; 
				break; 
			case KEY_DOWN: 
				if (highlight == numberOfChoices) 
					highlight = 1; 
				else 
					++highlight; 
				break; 
			case 10: 
				choice = highlight; 
				break; 
			default:
				break; 
		}
		print_menu(menu_win, highlight, choices, numberOfChoices); 
		if (choice != 0) 
			break; 
	} 
	return choice;
}

int handleKeyRight(TILE** matrix, SCORE* gameScore) {
	int row, column, moved = 0;

	for(row=0; row<4; row++)
		for(column=0; column<4; column++)
			matrix[row][column].tem_old = matrix[row][column].value;  
	gameScore->tem_old = gameScore->value;

	for (row=0; row<4; row++) {
		for (column=2; column>=0; column--) {
			if (matrix[row][column].value == 0){
				continue;
			}
			int next = 3;
			for (int t=column+1; t<4; t++){
				if (matrix[row][t].value != 0) {
					next = t;
					break;
				}
			}
			if (matrix[row][next].value == 0) {
				matrix[row][next].value = matrix[row][column].value;
				matrix[row][column].value = 0;
				moved = 1;
			} else if (matrix[row][next].value == matrix[row][column].value && matrix[row][next].changed == 0) {
				matrix[row][next].value *= 2;
				matrix[row][next].changed = 1;
				matrix[row][column].value = 0;
				gameScore->value += matrix[row][next].value;
				moved = 1;
			} else if (matrix[row][next].value != 0 && next-1 != column) {
				matrix[row][next-1].value = matrix[row][column].value;
				matrix[row][column].value = 0;
				moved = 1;
			}
		}
	}
	
	if (moved == 1) {
		for(row=0; row<4; row++)
		for(column=0; column<4; column++)
			matrix[row][column].old = matrix[row][column].tem_old;  
	    gameScore->old = gameScore->tem_old;
	}
		
	return moved;
}

int handleKeyLeft(TILE** matrix, SCORE* gameScore) {
	int row, column, moved = 0;

	for(row=0; row<4; row++)
		for(column=0; column<4; column++)
			matrix[row][column].tem_old = matrix[row][column].value; 
	
	gameScore->tem_old = gameScore->value;

	for (row=0; row<4; row++) {
		for (column=1; column<4; column++) {
			if (matrix[row][column].value == 0){
				continue;
			}
			int next = 0;
			for (int t=column-1; t>=0; t--){
				if (matrix[row][t].value != 0) {
					next = t;
					break;
				}
			}
			if (matrix[row][next].value == 0) {
				matrix[row][next].value = matrix[row][column].value;
				matrix[row][column].value = 0;
				moved = 1;
			} else if (matrix[row][next].value == matrix[row][column].value && matrix[row][next].changed == 0) {
				matrix[row][next].value *= 2;
				matrix[row][next].changed = 1;
				matrix[row][column].value = 0;
				gameScore->value += matrix[row][next].value;
				moved = 1;
			} else if (matrix[row][next].value != 0 && next+1 != column) {
				matrix[row][next+1].value = matrix[row][column].value;
				matrix[row][column].value = 0;
				moved = 1;
			}
		}
	}
	if (moved == 1) {
		for(row=0; row<4; row++)
		for(column=0; column<4; column++)
			matrix[row][column].old = matrix[row][column].tem_old;  
	gameScore->old = gameScore->tem_old;
	}
	return moved;
}

int handleKeyUp(TILE** matrix, SCORE* gameScore) {
	int row, column, moved = 0;

	for(row=0; row<4; row++)
		for(column=0; column<4; column++)
			matrix[row][column].tem_old = matrix[row][column].value;  

	gameScore->tem_old = gameScore->value;

	for (row=1; row<4; row++) {
		for (column=0; column<4; column++) {
			if (matrix[row][column].value == 0){
				continue;
			}
			int next = 0;
			for (int t=row-1; t>=0; t--){
				if (matrix[t][column].value != 0) {
					next = t;
					break;
				}
			}
			if (matrix[next][column].value == 0) {
				matrix[next][column].value = matrix[row][column].value;
				matrix[row][column].value = 0;
				moved = 1;
			} else if (matrix[next][column].value == matrix[row][column].value && matrix[next][column].changed == 0) {
				matrix[next][column].value *= 2;
				matrix[next][column].changed = 1;
				matrix[row][column].value = 0;
				gameScore->value += matrix[next][column].value;
				moved = 1;
			} else if (matrix[next][column].value != 0 && next+1 != row) {
				matrix[next+1][column].value = matrix[row][column].value;
				matrix[row][column].value = 0;
				moved = 1;
			}
		}
	}
	if (moved == 1) {
		for(row=0; row<4; row++)
		for(column=0; column<4; column++)
			matrix[row][column].old = matrix[row][column].tem_old;  
	    gameScore->old = gameScore->tem_old;
	}
	return moved;
}

int handleKeyDown(TILE** matrix, SCORE* gameScore) {
	int row, column, moved = 0;

	for(row=0; row<4; row++)
		for(column=0; column<4; column++)
			matrix[row][column].tem_old = matrix[row][column].value;  

	gameScore->tem_old = gameScore->value;

	for (row=2; row>=0; row--) {
		for (column=0; column<4; column++) {
			if (matrix[row][column].value == 0){
				continue;
			}
			int next = 3;
			for (int t=row+1; t<4; t++){
				if (matrix[t][column].value != 0) {
					next = t;
					break;
				}
			}
			if (matrix[next][column].value == 0) {
				matrix[next][column].value = matrix[row][column].value;
				matrix[row][column].value = 0;
				moved = 1;
			} else if (matrix[next][column].value == matrix[row][column].value && matrix[next][column].changed == 0) {
				matrix[next][column].value *= 2;
				matrix[next][column].changed = 1;
				matrix[row][column].value = 0;
				gameScore->value += matrix[next][column].value;
				moved = 1;
			} else if (matrix[next][column].value != 0 && next-1 != row) {
				matrix[next-1][column].value = matrix[row][column].value;
				matrix[row][column].value = 0;
				moved = 1;
			}
		}
	}
	if (moved == 1) {
		for(row=0; row<4; row++)
		for(column=0; column<4; column++)
			matrix[row][column].old = matrix[row][column].tem_old;  
	    gameScore->old = gameScore->tem_old;
	}
	return moved;
}

int hasFreeCells(TILE** matrix) {
	int i, j, hasFreeCells = 0;
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			if (matrix[i][j].value == 0) {
				hasFreeCells = 1;
				break;
			}
		}
		if (hasFreeCells) {
			break;
		}
	}
	return hasFreeCells;
}

CELL getRandomCell(TILE** matrix) {
	CELL availableCells[16];
	int i, j;
	int numberOfAvailableCells = 0;
	int randomCell;
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			if (matrix[i][j].value == 0) {
				availableCells[numberOfAvailableCells].x = i;
				availableCells[numberOfAvailableCells].y = j;
				numberOfAvailableCells++;
			}
		}
	}
	randomCell = rand();
	randomCell = randomCell % numberOfAvailableCells;
	return availableCells[randomCell]; 
}

void insertRandomCell(TILE** matrix) {
	if (hasFreeCells(matrix)) {
		CELL randomCell = getRandomCell(matrix);
		int random = rand();
		int value = random % 10 ;
		if (value < 8) {
			value =2;
		}
		else {
			value=4;
		}
		matrix[randomCell.x][randomCell.y].value = value;
	}
}

void clearMatrix(TILE** matrix) {
	int i, j;
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			matrix[i][j].value = 0;
			matrix[i][j].changed = 0;
		}
	}
}

void resetGame(TILE **matrix, SCORE* gameScore) {
	gameScore->value = 0;
	clearMatrix(matrix);
	insertRandomCell(matrix);
	insertRandomCell(matrix);
}

void gameWin(int terminalX, int terminalY) {
	WINDOW *win;
	int width = 20;
	int height = 5;
	int startx = (terminalX - width) / 2; 
	int starty = (terminalY - height) / 2; 
	char *helpMessage = "Press any key to return in main menu.";
	int helpMessageLength = strlen(helpMessage);
	int helpMessageX = terminalX / 2 - helpMessageLength / 2;
	int helpMessageY = terminalY - 1;
	win = newwin(height, width, starty, startx);
	keypad(win, TRUE);
	wattron(win, COLOR_PAIR(4));
	box(win, 0, 0);
	mvwprintw(win, 2, 2, "Congratulations!");
	wattroff(win, COLOR_PAIR(4));
	clear();
	attron(A_DIM);
	mvprintw(helpMessageY, helpMessageX, helpMessage);
	attroff(A_DIM);
	refresh();
	wgetch(win);
}

void gameOver(int terminalX, int terminalY) {
	WINDOW *over;
	int width = 13;
	int height = 5;
	int startx = (terminalX - width) / 2; 
	int starty = (terminalY - height) / 2; 
	char *helpMessage = "Press any key to return in main menu.";
	int helpMessageLength = strlen(helpMessage);
	int helpMessageX = terminalX / 2 - helpMessageLength / 2;
	int helpMessageY = terminalY - 1;
	over = newwin(height, width, starty, startx);
	keypad(over, TRUE);
	wattron(over, COLOR_PAIR(7));
	box(over, 0, 0);
	mvwprintw(over, 2, 2, "GAME OVER");
	wattroff(over, COLOR_PAIR(7));
	clear();
	attron(A_DIM);
	mvprintw(helpMessageY, helpMessageX, helpMessage);
	attroff(A_DIM);
	refresh();
	wgetch(over);
}

int _checkWin(TILE** matrix) {
	int i, j;
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			if (matrix[i][j].value == 2048) {
				return 1;
			}
		}
	}
	return 0;
}

int _checkGameOver(TILE** matrix) {
	int i, j;
	if (!hasFreeCells(matrix)) {
		for (i=0; i<3; i++) {
			for (j=0; j<3; j++) {
				if (matrix[i][j].value == matrix[i][j+1].value) {
					return 0;
				}
				if (matrix[i][j].value == matrix[i+1][j].value) {
					return 0;
				}
			}
		}
		return 1;
	}
	return 0;
}

int checkGame(TILE** matrix) {
	int win = _checkWin(matrix);
	if (win) {
		return WIN;
	}
	int gameOver = _checkGameOver(matrix);
	if (gameOver) {
		return GAME_OVER;
	}
	return 0;
}

void drawBoard(WINDOW *board, int width) {
	int i, row, k, ch;
	mvwaddch(board, 0, 0, ACS_ULCORNER);
	for (i=1; i < width - 1; i++) {
		ch = (i%7==0) ? ACS_TTEE : ACS_HLINE;
		mvwaddch(board, 0, i, ch);
	} 
	mvwaddch(board, 0, width - 1, ACS_URCORNER);
	for (row=0; row<4; row++) {
		for (k=1; k<4; k++) {
			for (i=0; i < 5; i++) {
				mvwaddch(board, 4*row+k, i*7, ACS_VLINE);
			}
		}
		for (i=0; i < width; i++) {
			if (i == 0) {
				ch = (row == 3) ? ACS_LLCORNER : ACS_LTEE;
			} else if (i == width - 1) {
				ch = (row == 3) ? ACS_LRCORNER : ACS_RTEE;
			} else if (i%7==0) {
				ch = (row == 3) ? ACS_BTEE : ACS_PLUS;
			} else {
				ch = ACS_HLINE;
			}
			mvwaddch(board, 4*row+4, i, ch);
		} 
	}
}

void fillBoard(WINDOW *board, TILE** matrix) {
	int i, j, color;
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			if (matrix[i][j].value != 0) {
				switch (matrix[i][j].value) {
					case 2:
						color = 1;
						break;
					case 4:
						color = 2;
						break;
					case 8:
						color = 3;
						break;
					case 16:
						color = 4;
						break;
					case 32:
						color = 5;
						break;
					case 64:
						color = 6;
						break;
					case 128:
						color = 7;
						break;
					case 256:
						color = 1;
						break;
					case 512:
						color = 2;
						break;
					case 1024:
						color = 3;
						break;
					case 2048:
						color = 4;
						break;
				}
				wattron(board, COLOR_PAIR(color));
				mvwprintw(board, 4*i+2, 7*j+2, "%d", matrix[i][j].value);
				wattroff(board, COLOR_PAIR(color));
			}
		}
	}
}

void _clearTilesStatus(TILE** matrix) {
	int i, j;
	for (i=0; i<4; i++) {
		for (j=0; j< 4; j++) {
			matrix[i][j].changed = 0;
		}
	}
}

int pauseGame(int terminalX, int terminalY) {
	int choice;
	clear();
	refresh();
	choice = menu(terminalX, terminalY, 2);
	return choice;
}

void drawHelp(int terminalX, int terminalY, SCORE gameScore) {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	mvprintw(1, terminalX - 17, "%04d-%02d-%02d %02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
	mvprintw(1, 1, "High Score: ");
	mvprintw(2, 1, "Score: ");
	attron(COLOR_PAIR(3));
	mvprintw(2, 8, "%d", gameScore.value);
	mvprintw(1, 13, "%d", gameScore.high);
	attroff(COLOR_PAIR(3));
	attron(A_DIM);
	mvprintw(terminalY - 1, 0, "Press Q for exit to menu.");
	mvprintw(terminalY - 2, 0, "Press U for UNDO.");
	mvprintw(terminalY -3 , 0, "For moving tiles use:");
	mvaddch(terminalY -3, 22, ACS_UARROW);
	mvaddch(terminalY -3, 24, ACS_DARROW);
	mvaddch(terminalY -3, 26, ACS_LARROW);
	mvaddch(terminalY -3, 28, ACS_RARROW);
	attroff(A_DIM);
	refresh();
}

void stepBack(TILE **matrix,SCORE *gameScore) {
	int row,column;
	for(row=0; row<4; row++)
		for(column=0; column<4; column++)
			matrix[row][column].value = matrix[row][column].old;  
	gameScore->value = gameScore->old;
	clear();
	refresh();
}

void highScore(SCORE *gameScore) {
	FILE *f;
	char tem[5];
	f=fopen("high.txt","r");
	fscanf(f, "%s", tem);
	fclose(f);
	gameScore->high = atoi(tem);
	if (gameScore->value > gameScore->high) {
		gameScore->high = gameScore->value;
		f=fopen("high.txt","w");
		fprintf(f, "%d", gameScore->high);
		fclose(f);
	}
}

	
int gameBoard(int terminalX, int terminalY) {
	WINDOW *board;
	TILE** matrix;
	SCORE gameScore;
	int width = 29;
	int height = 17;
	int startx = (terminalX - width) / 2; 
	int starty = (terminalY - height) / 2; 
	int c;
	int choice;
	int moved;
	int gameResult;
	int i;
	gameScore.value = 0;
	highScore(&gameScore);
	matrix = malloc(4 * sizeof(TILE*));
	for (i=0; i<4; i++) {
		matrix[i] = malloc(4 * sizeof(TILE));
	}
	board = newwin(height, width, starty, startx);
	keypad(board, TRUE);
	resetGame(matrix, &gameScore);
	drawBoard(board, width);
	fillBoard(board, matrix);
	drawHelp(terminalX, terminalY, gameScore);
	while (1) { 
		c = wgetch(board); 
		moved = 0;
		switch (c) { 
			case KEY_UP: 
				moved = handleKeyUp(matrix, &gameScore);
				break; 
			case KEY_DOWN: 
				moved = handleKeyDown(matrix, &gameScore);
				break; 
			case KEY_LEFT: 
				moved = handleKeyLeft(matrix, &gameScore);
				break; 
			case KEY_RIGHT: 
				moved = handleKeyRight(matrix, &gameScore);
				break;
			case 81:
			case 113:
				choice = pauseGame(terminalX, terminalY);
				if (choice == EXIT) {
					return EXIT;
				} else if(choice == NEW_GAME) {
					resetGame(matrix, &gameScore);
				}
				break;
			case 85:
			case 117:
				stepBack(matrix, &gameScore);
				
			default:
				break; 
		} 
		_clearTilesStatus(matrix);
		if (moved) {
			insertRandomCell(matrix);
		}
		highScore(&gameScore);
		wclear(board);
		drawBoard(board, width);
		fillBoard(board, matrix);
		drawHelp(terminalX, terminalY, gameScore);
		gameResult = checkGame(matrix);
		if (gameResult == WIN) {
			gameWin(terminalX, terminalY);
			return 0;
		} else if (gameResult == GAME_OVER) {
			gameOver(terminalX, terminalY);
			return 0;
		}
	} 
}

int main() { 
	int choice = 0; 
	int exit = 0; 
	int terminalX, terminalY;
	srand(time(NULL));
	// initializare ncurses
	initscr();
	noecho(); 
	cbreak(); 
	// creaza lungimea si latimea terminalului
	getmaxyx(stdscr, terminalY, terminalX);
	// defineste perechile de culori
	start_color();  
	init_pair(1, COLOR_WHITE, COLOR_BLACK); 
	init_pair(2, COLOR_YELLOW, COLOR_BLACK); 
	init_pair(3, COLOR_CYAN, COLOR_BLACK); 
	init_pair(4, COLOR_GREEN, COLOR_BLACK); 
	init_pair(5, COLOR_BLUE, COLOR_BLACK); 
	init_pair(6, COLOR_MAGENTA, COLOR_BLACK); 
	init_pair(7, COLOR_RED, COLOR_BLACK);
	// ascunde cursorul
	curs_set(0);
	while(1) { 
		// sterge tot de pe ecran
		clear(); 
		refresh();
		// deseneaz meniul, returneaza alegerea selectata
		choice = menu(terminalX, terminalY, 1);
		if (choice == NEW_GAME) {
			exit = gameBoard(terminalX, terminalY);
		} else if (choice == EXIT) {
			break;
		}
		if (exit) {
			break;
		}
	}
	// esire
	endwin();
	return 0; 
}
