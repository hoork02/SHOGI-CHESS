#include<iostream>
#include<fstream>
#include<windows.h>
#include<iomanip>
#include<math.h>
#define BLACK 5
using namespace std;

enum colour{white, black };
const int MAX_MOVES = 1000; char prevBoard[10][100];
int gameStateIndex = 0;

struct Move {
	int start_row;
	int start_col;
	int end_row;
	int end_col;
	char captured_piece;
};
struct position {
	int ri; int ci;
};
void getRowColbyLeftClick(int &rpos, int &cpos)
{
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD Events;
	INPUT_RECORD InputRecord;
	SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
	do
	{
		ReadConsoleInput(hInput, &InputRecord, 1, &Events);
		if (InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			cpos = InputRecord.Event.MouseEvent.dwMousePosition.X;
			rpos = InputRecord.Event.MouseEvent.dwMousePosition.Y;
			break;
		}
	} while (true);
}


void gotoRowCol(int rpos, int cpos)
{
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = cpos;
	scrn.Y = rpos;
	SetConsoleCursorPosition(hOuput, scrn);
}
void SetClr(int tcl, int bcl)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (tcl + (bcl * 16)));
}

void drawcell(char **B,int rd,int cd,int dim,int x,int y,char sy=-37)
{
	for (int k = 0; k < dim; k++)
	{
		gotoRowCol(rd, cd++);
		cout << setw(1)<<sy;
	}
	for (int k = 0; k < dim; k++)
	{
		gotoRowCol(rd++, cd);
		cout << setw(1) << sy;
	}
	for (int k = 0; k < dim; k++)
	{
		gotoRowCol(rd, cd--);
		cout << setw(1) << sy;
	}
	for (int k = 0; k < dim; k++)
	{
		gotoRowCol(rd--, cd);
		cout << setw(1) << sy;
	}
	gotoRowCol(rd+3, cd+3);
	cout << B[x][y];

}

void initial(char **&B, int &dim, string pname[], int &turn)
{
	ifstream rdr("Text.txt");
	rdr >> dim;
	B = new char*[dim];
	for (int i = 0; i < dim; i++)
	{
		B[i] = new char[dim]; 
	}
	for (int i = 0; i < dim; i++)
	{
		for (int c = 0; c < dim; c++)
		{
			rdr >> B[i][c]; 
		}
	}

	turn = white ;

}
void init(string Pnames[])
{
	cout << "Enter 1st player's: ";
	cin >> Pnames[0];
	cout << "Enter 2nd Player's Name: ";
	cin >> Pnames[1];
}
void displayboard(char **&B, int dim)
{
	system("cls");
	
	for (int i = 0; i < dim; i++)
	{
		
		for (int c = 0; c < dim; c++)
		{
			
			drawcell(B, i * 5, c * 5, 5, i, c, -37);
			
					
		}
		cout << endl;
	}
	cout << endl;


}
void selectposition(position &p, int dim)
{
	cout << "give coordinates : (A1:I9) ";
	char c;
	int d;
	cin >> c >> d;
	c = toupper(c);
	p.ri = d - 1;
	p.ci = c - 'A';

}
void selectposition2(position &p,int dim)
{
	int r, c;
	getRowColbyLeftClick(r, c); 
	p.ri = (r / 50.0) * 10;
	p.ci = (c / 50.0) * 10;
	gotoRowCol((p.ri), (p.ci));

}

bool isblack(char sym)
{
	return (sym >= 'A' && sym <= 'Z');
}
bool isWHITE(char sym)
{
	return (sym >= 'a' && sym <= 'z');
}
bool mypiece(char sym, int &turn)
{
	if (turn == white)
	{
		return isWHITE(sym);
	}
	else
		return isblack(sym);

}
bool isvalidsrc(char **&B, int dim,position sc,int &turn)
{
	if (sc.ri < 0 || sc.ri >= dim || sc.ci < 0 || sc.ci >= dim)
		return false;
	return mypiece(B[sc.ri][sc.ci], turn);

}
bool isvaliddes(char **&B, int dim, position dc, int &turn)
{
	if (dc.ri < 0 || dc.ri >= dim || dc.ci < 0 || dc.ci >= dim)
		return false;
	return mypiece(B[dc.ri][dc.ci], turn);

}

void turnmsg(string Pnames[], int turn)
{
	gotoRowCol(50, 100);
	cout << "\n\n" << Pnames[turn] << "'s turn....." << '\n';
}
void turnchange(int &turn)
{
	turn =1-turn;
}
void updateboard(char ** B, position sc,position dc,int dim)
{
	
	B[dc.ri][dc.ci] = B[sc.ri][sc.ci];
	B[sc.ri][sc.ci] = '-';

}
bool ishorizontal(position sc,position dc)
{
	return (sc.ri == dc.ri) ;
}
bool isvertical(position sc, position dc)
{
	return (sc.ci == dc.ci);
}
bool isdiagnol(position sc, position dc)
{
	int dr = abs(sc.ri - dc.ri);
	int dco = abs(sc.ci - dc.ci);
	
	
	return (dr == dco);
}
bool ishorizontalpathclear(char **&B,position sc, position dc)
{
	if (sc.ci < dc.ci)
	{
		for (int c = sc.ci+1; c < dc.ci; c++)
		{
			if (B[sc.ri][c] != '-')
				return false;
		}
		return true;
	}
	else
	{
		for (int c = dc.ci + 1; c < sc.ci; c--)
		{
			if (B[sc.ri][c] != '-')
				return false;
		}
		return true;

	}
	
	
}
bool isverticalpathclear(char **&B, position sc, position dc)
{
	int cs, ce;
	if (sc.ri < dc.ri)
	{
		cs = sc.ri + 1, ce = dc.ri - 1;
	}
	else
	{
		cs = dc.ri + 1, ce = sc.ri - 1;
	}
	for (int c = cs; c < ce; c++)
	{
		if (B[sc.ci][c] != '-')
			return false;
	}
	return true;


}
bool isdiagpathLRclear(char **&B, position sc, position dc)
{
	
	if (sc.ri < dc.ri)
	{
		int d = dc.ri - (sc.ri - 1);
		for (int c = 1; c < d; c++)
		{
			if (B[sc.ri+c][sc.ci+c] != '-')
				return false;
		}
		return true;
	}
	else
	{
		int d = sc.ri - (dc.ri - 1);
		for (int c = 1; c < d; c++)
		{
			if (B[sc.ri - c][sc.ci - c] != '-')
				return false;
		}
		return true;
	}
	


}
bool isdiagpathRLclear(char **&B, position sc, position dc)
{
	
	if (sc.ri < dc.ri)
	{
		int d = dc.ri - sc.ri - 1;
		for (int c = 1; c < d; c++)
		{
			if (B[sc.ri + c][sc.ci - c] != '-')
				return false;
		}
		return true;
	}
	else
	{
		int d = sc.ri - dc.ri - 1;
		for (int c = 1; c < d; c++)
		{
			if (B[sc.ri - c][sc.ci + c] != '-')
				return false;
		}
		return true;
	}
}
bool rooklegality(char **&B, position sc, position dc)
{
	return (ishorizontal(sc, dc) && ishorizontalpathclear(B, sc, dc)) || (isvertical(sc, dc) && isverticalpathclear(B, sc, dc));
}
bool bishoplegality(char **&B, position sc, position dc)
{
	return (isdiagnol(sc, dc) && isdiagpathLRclear(B, sc, dc)) || (isdiagnol(sc, dc) && isdiagpathRLclear(B, sc, dc));
}
bool knightlegality(char **&B, position sc, position dc)
{
	if (abs(sc.ri - dc.ri) > 1 || abs(sc.ci - dc.ci) > 1) {
		return false;
	}

    if (abs(sc.ri - dc.ri) == 1 && abs(sc.ci - dc.ci) == 1) {
    return false;
    }
	return true;
}
bool pawnlegality(char **&B, position sc, position dc,int &turn)
{
	if (sc.ci == dc.ci && sc.ri == dc.ri - 1 && turn == white) {
		return true;
	}
	if (sc.ci == dc.ci && sc.ri == dc.ri + 1 && turn == black) {
		return true;
	}

	
	if (abs(sc.ci - dc.ci) == 1 && sc.ri == dc.ci - 1 && turn == white) {
		return true;
	}
	if (abs(sc.ci - dc.ci) == 1 && sc.ri == dc.ri + 1 && turn == black) {
		return true;
	}

	// The move is not legal
	return false;
}
bool silverlegality(char**& B, position sc, position dc,int &turn)
{
	if (abs(sc.ci - dc.ci) == 1 && abs(sc.ri - dc.ri) == 1) {
		return true;
	}

	// Check if the silver is moving one square straight forward and then one square diagonally forward
	if (sc.ci == dc.ci && abs(sc.ri - dc.ri) == 2 && turn == white) {
		return (B[sc.ci][(sc.ri + dc.ri) / 2] == '-');
	}
	if (sc.ci == dc.ci && abs(sc.ri - dc.ri) == 2 && turn == black) {
		return (B[sc.ci][(sc.ri + dc.ri) / 2] == '-');
	}
	return false;
}
bool goldlegality(char **&B, position sc, position dc)
{
	if (abs(sc.ci - dc.ci) == 1 && abs(sc.ri - dc.ri) == 1) {
		return true;
	}

	// Check if the gold is moving one square straight forward
	if (sc.ci == dc.ci && abs(sc.ri - dc.ri) == 1) {
		return true;
	}

	// The move is not legal
	return false;
}
bool kinglegality(char **&B, position sc, position dc)
{
	if (abs(sc.ci - dc.ci) <= 1 && abs(sc.ri - dc.ri) <= 1) {
		return true;
	}

	// The move is not legal
	return false;
}
bool lancelegality(char **&B, position sc, position dc)
{
	if (sc.ri == dc.ri && sc.ci < dc.ci) {
		// Check if there are no pieces in the way
		for (int i = sc.ci + 1; i < dc.ci; i++) {
			if (B[i][sc.ri] != '-') {
				return false;
			}
		}
		return true;
	}
	if (sc.ri == dc.ri && sc.ci > dc.ci) {
		// Check if there are no pieces in the way
		for (int i = dc.ci + 1; i < sc.ci; i++) {
			if (B[i][sc.ri] != '-') {
				return false;
			}
		}
		return true;
	}

	// The move is not legal
	return false;
}
bool islegalmove(char **&B, position sc, position dc, int dim, int &turn)
{
	switch (B[sc.ri][sc.ci])
	{
	case('r'):
	{
		return rooklegality(B, sc, dc);
		break;
	}
	case('R'):
	{
		return rooklegality(B, sc, dc);
		break;
	}
	case('b'):
	{
		return bishoplegality(B, sc, dc);
		break;
	}
	case('B'):
	{
		return bishoplegality(B, sc, dc);
		break;
	}
	case('k'):
	{
		return kinglegality(B, sc, dc);
		break;
	}
	case('K'):
	{
		return kinglegality(B, sc, dc);
		break;
	}
	case('l'):
	{
		return lancelegality(B, sc, dc);
		break;
	}
	case('L'):
	{
		return lancelegality(B, sc, dc);
		break;
	}
	case('n'):
	{
		return knightlegality(B, sc, dc);
		break;
	}
	case('N'):
	{
		return knightlegality(B, sc, dc);
		break;
	}
	case('p'):
	{
		return pawnlegality(B, sc, dc,turn);
		break;
	}
	case('P'):
	{
		return pawnlegality(B, sc, dc,turn);
		break;
	}
	case('g'):
	{
		return goldlegality(B, sc, dc);
		break;
	}
	case('G'):
	{
		return goldlegality(B, sc, dc);
		break;
	}
	case('s'):
	{
		return silverlegality(B, sc, dc,turn);
		break;
	}
	case('S'):
	{
		return silverlegality(B, sc, dc,turn);
		break;
	}
	case('+s'):
	{
		return goldlegality(B,sc,dc); break;
	}
	case('+S'):
	{
		return goldlegality(B, sc, dc); break;
	}
	case('+n'):
	{
		return goldlegality(B, sc, dc); break;
	}
	case('+N'):
	{
		return goldlegality(B, sc, dc); break;
	}
	case('+l'):
	{
		return goldlegality(B, sc, dc); break;
	}
	case('+L'):
	{
		return goldlegality(B, sc, dc); break;
	}
	case('+b'):
	{
		return kinglegality(B, sc, dc); break;
	}
	case('+r'):
	{
		return kinglegality(B, sc, dc); break;
	}
	case('+R'):
	{
		return kinglegality(B, sc, dc); break;
	}
	case('+p'):
	{
		return goldlegality(B, sc, dc); break;
	}
	case('+P'):
	{
		return goldlegality(B, sc, dc); break;
	}
	}
	

}
void TurnMsgColor(int turn)
{
	gotoRowCol(20,100);
	if (turn == 1)
		cout << "black's Turn:";
	else
		cout << "white's Turn:";
}

position findKing(char** board, int board_size, int turn) {
	char king_symbol;
	if (turn == white) {
		king_symbol = 'K';
	}
	else {
		king_symbol = 'k';
	}

	for (int i = 0; i < board_size; i++) {
		for (int j = 0; j < board_size; j++) {
			if (board[i][j] == king_symbol) {
				position pos;
				pos.ri = i;
				pos.ci = j;
				return pos;
			}
		}
	}
	position pos;
	pos.ri = -1;
	pos.ci = -1;
	return pos;
}
bool check(char** B, int dim, int turn)
{
	position king_pos = findKing(B, dim, turn);
	if (king_pos.ri == -1 || king_pos.ci == -1) {
		
		return false;
	}

	
	for (int i = 0; i < dim;i++)
	{
		for (int j = 0; j < dim; j++)
		{
			if (islegalmove(B, king_pos, king_pos ,dim,turn))
			{
				return true;
			}
		}
	}
	return false;



}
bool selfcheck(char **B,int dim,int turn)
{
	turnchange(turn);
	return check(B, dim, turn);

}
void Chighlight(char **I, char** B, int turn, position sc, char L[][11], char M[][11], char A[], int D[], int a, int b)
{
	I = new char* [9];
	for (int i = 0; i < 9; i++)
	{
		I[i] = new char[9];
	}
	position dc{};
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			dc.ri = i;
			dc.ci = j;
			if (isvaliddes(B,9,dc,turn) && islegalmove(B, sc, dc, 9, turn))
			{
				I[i][j] = '*';
			}
			else
			{
				I[i][j] = '-';
			}

		}
	}
}
void highlight(bool **&map, char **Board, int dim, int rdim, int cdim)
{

	for (int ri = 0, sr = 10; ri < dim, sr <= 6 * dim; ri++, sr += 5)
	{
		for (int ci = 0, sc = 10; ci < dim, sc <= 6 * dim; ci++, sc += 5)
		{
			if (map[ri][ci] == true)
			{
				gotoRowCol(sr + 1, sc + 1);
				SetClr(12,5);
				displayboard(Board, dim);


			}
		}
	}
}



void SaveGame(char** board, int board_size, int turn, string player_names[2]) {
	ofstream writer("savegame.txt");

	// Write the board size and current turn to the file
	writer << board_size << " " << turn << endl;

	// Write the player names to the file
	writer << player_names[0] << " " << player_names[1] << endl;

	// Write the board to the file
	for (int i = 0; i < board_size; i++) {
		for (int j = 0; j < board_size; j++) {
			writer << board[i][j] << " ";
		}
		writer << endl;
	}

	writer.close();
}
Move moves[MAX_MOVES];
int num_moves = 0;

void ReplayGame(char** B,int dim,string pname[2],int &turn)
{
	// Reset the board to the initial state
	initial(B, dim, pname, turn);

	// Reapply all the moves in the array
	for (int i = 0; i < num_moves; i++) {
		Move move = moves[i];

		// Capture the piece at the end position, if any
		char captured_piece = B[move.end_row][move.end_col];

		// Make the move
		B[move.end_row][move.end_col] = B[move.start_row][move.start_col];
		B[move.start_row][move.start_col] = '-';
		displayboard(B, dim);

		// Pause for a short time to allow the user to see the move
		Sleep(500);
	}
}
bool IsWin(char** board, int board_size, int turn) {
	// Find the position of the king
	position king_pos = findKing(board, board_size, turn);

	// Check if the king has been captured
	return board[king_pos.ri][king_pos.ci] == ' ';
}
void undo(char** B, char prevBoard[][100],int dim)
{
	//restore board to previous state stored in prevBoard
	for (int i = 0; i < dim; i++)
	{
		for (int j = 0; j < dim; j++)
		{
			B[i][j] = prevBoard[i][j];
		}
	}
}
bool canPromote(char piece, int row, int dim) {
	// Check if the piece is in the promotion zone
	if ((piece == 'P' || piece == 'L' || piece == 'N' || piece == 'S' || piece == 'G' || piece == 'B'|| piece == 'p' || piece == 'l' || piece == 'n' || piece == 's' || piece == 'g' || piece == 'b') &&
		((piece == 'P'||piece=='p' && row <= 2) || (piece == 'B'||piece=='b' && row >= dim - 3))) {
		return true;
	}
	return false;
}

void promotePiece(char &piece) {
	// Prompt the player to choose whether to promote the piece or not
	cout << "Do you want to promote the piece? (Y/N) ";
	char choice;
	cin >> choice;
	if (choice == 'Y' || choice == 'y') {
		// Replace the original piece with the promoted version
		switch (piece) {
		case 'P':
			piece = '+P';
			break;
		case 'L':
			piece = '+L';
			break;
		case 'N':
			piece = '+N';
			break;
		case 'S':
			piece = '+S';
			break;
		case 'R':
			piece = '+R';
			break;
		case 'G':
			piece = '+G';
			break;
		case 'B':
			piece = '+B';
			break;
		case 'p':
			piece = '+p';
			break;
		case 'l':
			piece = '+l';
			break;
		case 'n':
			piece = '+n';
			break;
		case 's':
			piece = '+s';
			break;
		case 'g':
			piece = '+g';
			break;
		case 'b':
			piece = '+b';
			break;
		case 'r':
			piece = '+r';
			break;
		}
	}
}
bool isEnemyPiece(char piece, int &turn) {
	if (turn == white) {
		return piece >= 'a' && piece <= 'z';
	}
	else {
		return piece >= 'A' && piece <= 'Z';
	}
}
bool capturePiece(char** board, int dim, position src, position dest, int& turn)
{
	// Check if destination cell is occupied by an enemy piece
	if (board[dest.ri][dest.ci] != ' ' && isEnemyPiece(board[dest.ri][dest.ci], turn))
	{
		// Capture the piece by replacing it with the moving piece
		board[dest.ri][dest.ci] = board[src.ri][src.ci];
		board[src.ri][src.ci] = ' ';
		return true;
	}
	return false;
}

bool checkmate(char **&B, int dim,position sc,position dc, int turn)
{
	// Check if the current player's king is in check
	if (check(B, dim, turn))
	{
		// Iterate over all the pieces on the board
		for (int i = 0; i < dim; i++)
		{
			for (int j = 0; j < dim; j++)
			{
				// If the current piece belongs to the current player
				if (B[i][j] != ' ' && (isupper(B[i][j]) == turn))
				{
					// Iterate over all the cells on the board
					for (int x = 0; x < dim; x++)
					{
						for (int y = 0; y < dim; y++)
						{
							// If the current cell is a legal move for the current piece
							if (islegalmove(B, sc, dc, dim, turn))
							{
								// Make a temporary copy of the board
								char** tempBoard = {};
								for (int k = 0; k < dim; k++)
								{
									
									for (int l = 0; l < dim; l++)
									{
										tempBoard[k][l] = B[k][l];
									}
								}
								// Make the move on the temporary board
								updateboard(B, sc, dc, dim);
								// If the king is not in check on the temporary board, return false (not checkmate)
								if (!check(tempBoard, dim, turn))
								{
									return false;
								}
								// Otherwise, delete the temporary board and continue checking
								for (int k = 0; k < dim; k++)
								{
									delete[] tempBoard[k];
								}
								delete[] tempBoard;
							}
						}
					}
				}
			}
		}
	}

}
void displayCapturedBoard(char capturedBoardWhite[][10], char capturedBoardBlack[][10])
{
	cout << "Captured Pieces (White): ";
	for (int i = 0; i < 10; i++)
	{
		cout << capturedBoardWhite[i] << " ";
	}
	cout << endl;
	cout << "Captured Pieces (Black): ";
	for (int i = 0; i < 10; i++)
	{
		cout << capturedBoardBlack[i] << " ";
	}
	cout << endl;
}






int main()
{	
	char** B = {}; int dim; string pname[2]; int turn = 1; position sc; position dc = { 0,0 }; char** I = {};
	char L[10][11]; char M[10][11]; char A[10]; int D[10]; int a = 0; int b = 0; char op = 0;
	char capturedBoardWhite[10][10]={};
	char capturedBoardBlack[10][10]={};
	init(pname);
	initial(B, dim, pname, turn);
	displayboard(B, dim);
	gotoRowCol(55, 100);
	displayCapturedBoard(capturedBoardWhite, capturedBoardBlack);
	while (true)
	{
		do{
			TurnMsgColor(turn);
				do {
				gotoRowCol(11, 60);
					turnmsg(pname, turn);
					cout << " select source......";
					selectposition2(sc, dim);
					Chighlight(I, B, turn, sc, L, M, A, D, a, b);
				} while (isvalidsrc(B, dim, sc, turn));
				
				
				do {
					gotoRowCol(12, 60);
					cout << " select dest......";
				selectposition2(dc, dim);
				
			} while (isvaliddes(B, dim, dc, turn));
			
			if (isEnemyPiece(B[a][b], turn))
			{
				

				// Initialize the captured boards to empty
				for (int i = 0; i < 100; i++) {
					for (int j = 0; j < 100; j++) {
						capturedBoardWhite[i][j] = ' ';
						capturedBoardBlack[i][j] = ' ';
					}
				}
				// Capture the enemy piece and update the captured piece count
				capturePiece(B, dim, sc, dc, turn); int nextWhiteRow=0, nextWhiteCol=0; int nextBlackRow=0, nextBlackCol = 0;
				if (turn == white) {
					// White player captures a black piece
					// Add the captured piece to the white player's captured board
					capturedBoardWhite[nextWhiteRow][nextWhiteCol] = B[a][b];
					nextWhiteCol++;
					if (nextWhiteCol >= 100) {
						nextWhiteCol = 0;
						nextWhiteRow++;
					}
					else {
						// Black player captures a white piece
						// Add the captured piece to the black player's captured board
						capturedBoardBlack[nextBlackRow][nextBlackCol] = B[a][b];
						nextBlackCol++;
						if (nextBlackCol >= 100) {
							nextBlackCol = 0;
							nextBlackRow++;
						}
					}

				}
				displayCapturedBoard(capturedBoardWhite, capturedBoardBlack);
			}
			/*if (checkmate(B, dim, sc, dc, turn))
			{

				cout << "this is checkmate...." << endl;
				IsWin(B, dim, turn);
			}*/
			if (check(B, dim, turn))
			{
				cout << "Opponent's king is in check!" << endl;
			}
			else if (selfcheck(B, dim, turn))
			{

				cout << "Your own king is in selfcheck!" << endl;
			}
			
			if (canPromote(B[a][b], sc.ri, dim) == true)
			{
				promotePiece(B[a][b]);
			}
		} while (!islegalmove(B,sc,dc,dim,turn));
		
		
		updateboard(B, sc, dc, dim);
		displayboard(B, dim);
		cout << " choose option : " << endl
			<< "1.undo" << endl
			<< "2.replay" << endl
			<< "3.save" << endl;
		cin >> op;
		if (op == 1)
		{
			undo(B, prevBoard, dim);
		}
		else if (op == 2)
		{
			ReplayGame(B, dim, pname, turn);

		}
		else if (op == 3)
		{
			SaveGame(B, dim, turn, pname);

		}
		gotoRowCol(55, 100);
		displayCapturedBoard(capturedBoardWhite, capturedBoardBlack);
		cout << endl;
		cout << endl;
		
		turnchange(turn);
	}

	 


	return 0;
}
