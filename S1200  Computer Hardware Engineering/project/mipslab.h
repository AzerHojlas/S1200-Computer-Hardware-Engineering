/* mipslab.h
   Header file for all labs.
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Latest update 2015-08-28 by F Lundevall

   For copyright and licensing, see file COPYING */

void display_init(void);
void display_string(int line, char *s);
void display_update(int pages);  // Ändrade argument
uint8_t spi_send_recv(uint8_t data);
void display_debug( volatile int * const addr );

/* Functions by Nina Shamaya and Azer Hojlas*/
void display_image(int seg, const uint8_t *data, int length, int ss); 
void reverse_bits(uint8_t *data, int length);
void display_verticalSeg(int seg, int data);
void print_pong (void);
void clear_board(void);
// Fyller i en kropp med början i punkten (x, y) och arean xLen * yLen
// Hela koordinatsystemet är 128 * 32, där 128 motsvarar x-led och 32 motsvarar y-led
void create_body(int x, int y, int xLen, int yLen);
void make_image(void);
void print_menu(void);

void menu(void);
void start_game(void); //nina
void default_board(void); 
void create_middle_line(void);
void survival_mode(void);
float get_random(void);
int get_random_sign(void);
void throw_ball(void);
void move_body(void);
void move_ball(void);
void ball_adjustment(void);
int is_touching(int body);
int lower_half(int body);
int upper_half(int body);
void goal(void);


void two_players_mode(void);
void show_highscore(void); // LÄS SJÄLV
void print_mode(void);  // LÄS SJÄLV
void choose_mode(void); // LÄS SJÄLV

/* Variables by Nina Shamaya */
extern uint8_t pong[94];
extern uint8_t board[32][128];
extern uint8_t board_data[512];

char * itoaconv( int num, int place );
void labwork(void);
void quicksleep(int cyc);

extern const uint8_t const font[128*8];
extern char textbuffer[4][16];


void delay(int);
int getbtns(void);
int getsw(void);
void enable_interrupt(void);