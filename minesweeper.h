enum MoveResult { 
    INVALID_MOVE=-3, 
    REDUNDANT_MOVE=-2, 
    BLOWN_UP=-1, 
    SOLVED_BOARD=1, 
    VALID_MOVE=0 
};

void load_board(const char *filename, char board[9][9]);
void display_board(const char board[9][9]);
void initialise_board(char board[9][9]);

bool is_complete(const char mines[9][9], const char unrevealed[9][9]);
int count_mines(const char* position, const char mines[9][9]);
int count_mines(int row, int col, const char mines[9][9]);
int count_unrevealed(int row, int col, const char revelead[9][9]);


void get_position(const char* position, int &row, int &col);
MoveResult make_move(const char* position, const char mines[9][9], 
  char unrevealed[9][9]);
bool find_safe_move(char revealed[9][9], char* move);

void flooding(int row, int col, const char mines[9][9], char unrevealed[9][9]);
void get_position_unrevealed_neighbour(int row, int col,char revealed[9][9],
    int &row_unrel, int &col_unrel);
bool is_flagging(const char* position);
bool is_out_of_bound(int row, int col);

