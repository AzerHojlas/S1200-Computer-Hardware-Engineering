/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   For copyright and licensing, see file COPYING */

#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

uint8_t menu_navigation = 0; // this is a global variable used for
uint8_t mode_navigation = 0;
uint8_t mode = 0;
int highscore[2] = {0, 0};

uint8_t body0 = 11;
uint8_t body1 = 11;

int playing = 0;
int score = 0;

// ball stuct, a global object
typedef struct game_ball {
	float xPos; 	// coordinate
	float yPos; 	// coordinate
	float xSpeed;
	float ySpeed;
	float maxXSpeed;
} Game_Ball; 

Game_Ball def = {63, 15, 1, 0, 3};		// Default placement of ball in beginning of game
Game_Ball ball = {63, 15, 1, 0, 3};		// values change

static void num32asc( char * s, int );

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

// Skriver ut _length_ segment ur _data_ från och med _seg_ över blad _ss_
// Starts writing at segment
// writes out a specified amount of columns/segments with variable length out of data array
// writes out from s to s, i.e from starting page to stop page where both s represent respective page, written in hexadecimal
// one data is 8 bits
void display_image(int seg, const uint8_t *data, int length, int ss) {
	int i, j;

    int start = (ss >> 4) & 0xF; // we retrieve the start value
    int stop = ss & 0xF; // we retrieve the stop value
    int pages = stop - start + 1; // over how many pages to write
	
	for(i = 0; i < pages; i++) { 
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22); 
		spi_send_recv(i + start);

		spi_send_recv(0x00);	// Reset
		spi_send_recv(0x10);	// Reset
		
		spi_send_recv(seg & 0xF);
		spi_send_recv(0x10 | ((seg >> 4) & 0xF));
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < length; j++)
			spi_send_recv(data[i*length + j]);
	}
}

// Speglar bitarna för _length_ element ur _data_
// computer reads input in mirrored order, we reverse bits to get desired resullt
// we read length amount of elements from data array 
void reverse_bits(uint8_t *data, int length) {
	int k;
	for (k = 0; k < length; k++) {
		int i = 7; // i is 7 because it reepresents msb
		unsigned char j = 0; // 
		unsigned char temp = 0;

		while(i >= 0) {
			temp |= ((data[k] >> j) & 1) << i;  // index bit 0 is shifted 0 bits to the right then 7 bits to the msb; index bit 1 is shifted 1 bit right then 6 
			i--;								// bits to the left; this is repeated until all bits are in their desired positions
			j++;
		}
		data[k] = temp; // finally the correct value in temp is stored in data
	}
}

// Skriver ut _data_ över _seg_
// Writes out data over all pages in a column
// is only used in starting logo
void display_verticalSeg(int seg, int data) {
	int i;
	for (i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0x00);	// Reset
		spi_send_recv(0x10);	// Reset

		spi_send_recv(seg & 0xF);
		spi_send_recv(0x10 | ((seg >> 4) & 0xF));
			
		DISPLAY_CHANGE_TO_DATA_MODE;

		spi_send_recv((data >> i*8) & 0xFF);
	}
}

// Skriver ut pong
void print_pong (void) {
    reverse_bits(pong, 94);
	clear_board();
	display_image(0, board_data, 128, 0x03);
	int i = 40;
	for (; i < 169; i++) {
		display_image(i, pong, 47, 0x12);
		if (i == 40)
			delay(1000);
		display_verticalSeg(i - 1, 0xc00000);
		delay(25);
	}
	delay(1000);
}

// Nollställer skärmen
// resets the entire OLED screen
// The Oled is 128 pixels long and 32 pixels wide
void clear_board(void) {
	int row, column, i;
	for (row = 0; row < 32; row++)
		for (column = 0; column < 128; column++)
			board[row][column] = 0; // here every pixel is cleared
	
	for (i = 0; i < 512; i++)
		board_data[i] = 0; // all the data is set to 0

	int j;
	for (j = 0; j < 4; j++)
		display_string(j, ""); // fills each page with empty strings, each char is written over 64 pixels. With each new action the buffer needs to be cleared
}

// Skapar en kropp med början i punkten (x, y) och arean xLen * yLen
void create_body(int x, int y, int xLen, int yLen) {
  int row, column;
  for (row = 0; row < 32; row++)
    for (column = 0; column < 128; column++)
      if (row >= y && row < (y + yLen) && column >= x && column < (x + xLen)) // as long as we are in the desired intervals we fill in the pixels
        board[row][column] = 1;
}

// Översätter koordinatsystemet till data som kan skrivas ut med hjälp av display_image
// This is where the board_data coordinates are translated to one dimension to make pixelation possible
// The two dimensionall board is copied to the one dimensionall board_data
void make_image(void) {
	int page0, page1, column, row, c;
	uint8_t position = 1;
	uint8_t data = 0;

	for (page0 = 0, page1 = 3; page0 < 4; page0++, page1--) {
		for (column = 0; column < 128; column++) {
			position = 1;
			data = 0;
			for (row = 0; row < 8; row++) { 	// one page is 8 pixels wide, so we iterate through pages, each page is incremented by eight
				if (board[page0*8 + row][column]) // if this pixel is one then we copy it to data
					data |= position;			
				position <<= 1;					// then we iterate through all the 8 bits
			}

			board_data[page1*128 + column] = data; // board data gets assigned whats in data for the specific area of the board
		}
	}
	reverse_bits(board_data, 512);
	if (mode == 0) {	// Survival
		int c0 = textbuffer[0][0], c1 = textbuffer[0][1];
		int k;
		for (k = 0; k < 8; k++) {
			board_data[56 + k] = font[c0*8 + k];
			board_data[64 + k] = font[c1*8 + k];
		}
	}
}

// Skriver ut meny
void print_menu(void) {

	clear_board(); // first the board is cleared
	display_image(0, board_data, 128, 0x03); // we display a black screen on all segments from first to last page

	if (menu_navigation == 3) menu_navigation = 0;
	switch (menu_navigation) // menu_navigation tracks where our current position is on the menu, always in one of the three scenarios below
	{
	case 0:
		display_string(1, "> START"); //start is displayed on page 1
		display_string(2, "  MODE");
		display_string(3, "  HIGHSCORE");
		break;
	case 1:
		display_string(1, "  START");
		display_string(2, "> MODE");
		display_string(3, "  HIGHSCORE");
		break;
	case 2:
		display_string(1, "  START");
		display_string(2, "  MODE");
		display_string(3, "> HIGHSCORE");
		break;
	default:
		break;
	}
	display_update(4); 
}

// Hanterar meny
// handles our movement in the menu
void menu(void) {

	while (1) { // we are always in here
		print_menu(); // we start from start, i.e case 0
		int buttons;
		while (!(buttons = (getbtns() & 6))); // while non of the controling buttons is pressed. i.e btn 4 and 3
		delay(200); // Delay is necessary or glitch
		if (buttons & 4) {// if btn 4 is pressed, whos function it is to confirm/press on options depending on menu_navigation variable
			if (menu_navigation == 0) start_game();
			if (menu_navigation == 1) choose_mode();
			if (menu_navigation == 2) show_highscore();
		}

		if (buttons & 2) menu_navigation++; // button 2 iterates through menu options by incrementing menu_navigation variable
	}
}

// Startar spelet
// playing is a global variable that tracksif we are currently playing or not
void start_game(void) {
	playing = 1;
	if (mode == 0) survival_mode();
	if (mode == 1) two_players_mode();
	playing = 0; // once finished with game playing is set to 0
}

// Nollställer brädet och skriver ut det
// initiates the beginning position for the game
void default_board(void) {
	body0 = 11; 	// left player, starts on position 11 in y-direction
	body1 = 11; 	// right  player
	create_body(3, body0, 3, 10); // left player is created in 3 x-direction
	create_middle_line();			// middle line is made
	create_body(122, body1, 3, 10);		// right player is created in 122 x-direction with area of 3x10
	make_image();						// make image creates the board_data
	display_image(0, board_data, 128, 0x03);	// in display we write said board_data
}

// Skapar en linje i mitten av brädet
void create_middle_line(void) {
	int y;
	for (y = 3; y < 24; y += 3) 
		create_body(63, y, 2, 2); //the dots are two by two, on x = 63, starting from y = 3 with 3 pixel space inbetween
}

// Spelar survival mode
// 
void survival_mode(void) {
	default_board();
	throw_ball();
	int buttons;
	char *scoreString; // we have gloabl string variable, tracks the score in survival mode
	while(playing) { 
		scoreString = itoaconv(score, 0);
		display_string(0, scoreString);
		while (!(buttons = (getbtns() & 7)) && playing) // while btns 4,3,2 are not pressed and we are playing
			move_body();								// the game moves the ball and or the AI player
		if (buttons & 4 && body0 != 21)		// if btn 4 is pressed and our player is not at the ceiling (21 because we track the lowest point of the plank which adds 10)
			body0 += 1;						// then body goes up 1 pixel
		else if (buttons & 2 && body0 != 0)	// if btn 3 is pressed and we are not at thebottom
			body0 -= 1;						// then we move 1 pixel downwards
		else if (buttons & 1) return;		// if btn 2 is pressed, return to start game, we set playing to 0 then we return to menu
		move_body();						//  we update the moving parts
	}
}

void ai_movement(void) {

}

// Returnerar ett slumpmässigt nummer mellan 0.5 och 0.9
// used in throw ball
float get_random(void) {
	float rand = TMR2 % 5; // use the clock to get a random value between 0 and 4
	rand /= 10;				// this value is divided by 10 to get value between 0 to 0,4
	rand += 0.5;			// add 0,5 to get between 0,5 and 0,9
	rand *= get_random_sign(); //assign it a random sign
	return rand;
}

// Returnerar ett slumpmässigt tecken
// used in throw ball
int get_random_sign(void) {
	if (TMR2 % 10 < 5) 		// if modulo clock is under 5 give -1, otherwise give 1
		return -1;
	return 1;
}

// Skickar iväg bollen slumpmässigt
// The different values will give a random direction and speed
void throw_ball(void) {
	ball.ySpeed = get_random();
	ball.xSpeed = get_random_sign();
}

// Skriver ut brädet efter en rörelse
void move_body(void) {
	clear_board();

	ball_adjustment();
	move_ball();
	create_body(ball.xPos, ball.yPos, 3, 3);	// Skapar boll

	create_body(3, body0, 3, 10);	// Skapar body0
	create_middle_line();
	create_body(122, body1, 3, 10);	// Skapar body1

	make_image();
	display_image(0, board_data, 128, 0x03);
}

// Justerar bollens position
// ball is 3x3 pixels area, with the first pixel starting in the lowest part of the body, which explains the lower if statement
void move_ball(void) {
	ball.xPos += ball.xSpeed; // ball is an object with xpos property gets added speed
	ball.yPos += ball.ySpeed;
	if (ball.yPos > 28 || ball.yPos < 0) // if ball comes to the roof, direction is reversed in y-direction or if it hits bottom.
		ball.ySpeed *= -1;
}

// Justerar bollens färdriktning efter kollision och anropar goal
void ball_adjustment(void) {

	if (is_touching(0))
	{
		// Från negativ riktning till positiv
		ball.xSpeed *= -1;
	
		if (ball.xSpeed < ball.maxXSpeed)
			ball.xSpeed += 0.5;
	
		if (lower_half(0))
			if (ball.xSpeed > 2)
				ball.ySpeed -= 0.55;
			else
				ball.ySpeed -= 0.35;
		
		if (upper_half(0))
			if (ball.xSpeed > 2)
				ball.ySpeed += 0.55;
			else
				ball.ySpeed += 0.35;
	
		if (mode == 0) score++;
	}

	if (is_touching(1))
	{
		// Från positiv till negativ riktning
		ball.xSpeed *= -1;

		if (ball.xSpeed > -1 * ball.maxXSpeed)
			ball.xSpeed -= 0.5;

		if (lower_half(1))
			if (ball.xSpeed < -2)
				ball.ySpeed -= 0.55;
			else
				ball.ySpeed -= 0.35;

		if (upper_half(1))
			if (ball.xSpeed < -2)
				ball.ySpeed -= 0.55;
			else
				ball.ySpeed -= 0.35;
	}

	if ((ball.xPos + 3) < 0 || ball.xPos > 128)
		goal();
}

// Om bollen rör en kropp
int is_touching(int body) {

	if (body == 0)
		return (ball.xSpeed < 0 && (ball.xPos >= 3 && ball.xPos <= 5) && (ball.yPos - 2 >= body0 && ball.yPos <= (body0 + 9)));
	if (body == 1)
		return (ball.xSpeed > 0 && (ball.xPos >= 122 && ball.xPos <= 124) && (ball.yPos - 2 >= body1 && ball.yPos <= (body1 + 9)));
}

// Om bollen rör den undre delen av en kropp (Varför 1.5?)
int lower_half(int body) {

	if (body == 0)
		return ((ball.yPos + 0.5) < (body0 + 3.5) && ball.ySpeed > - 1.5);
	if (body == 1)
		return ((ball.yPos + 0.5) < (body1 + 3.5) && ball.ySpeed > - 1.5);
}

// Om bollen rör den övre delen av en kropp (Varför 1.5?)
int upper_half(int body) {

	if (body == 0)
		return ((ball.yPos + 0.5) > (body0 + 3.5) && ball.ySpeed < 1.5);
	if (body == 1)
		return ((ball.yPos + 0.5) > (body0 + 3.5) && ball.ySpeed < 1.5);
}
// if someone scores
void goal(void) { 
	if (mode == 0) { // if we are in survival and someone has scored, it is the ai and the game is over
		clear_board();
		display_string(1, "    GAME OVER");
		display_update(4);
		if (score > highscore[0]) {
			highscore[0] = score;
			display_string(2, "NEW #1 HIGHSCORE");
		}
		else if (score > highscore[1]) {
			highscore[1] = score;
			display_string(2, "NEW #2 HIGHSCORE");
		}
		playing = 0;
		ball = def;
		display_update(4);
		delay(3000); // makes the score remain a while for the player to read
		return;
	}
}

void two_players_mode(void) {}

// Skriver ut highscore
void show_highscore(void) {
	clear_board();
	display_image(0, board_data, 128, 0x03);
	display_string(0, "    HIGHSCORE");

	int i = 0;
	char *hs;
	for (; i < 2; i++) {
		hs = itoaconv(highscore[i], i + 1);
		display_string(i + 2, hs);
	}
	display_update(4);
	while (!(getbtns() & 4));
	delay(200);
}

// Skriver ut mode
// works like print menu but navigates in the mode menu, mode_navigation is used instead of menu_navigation
void print_mode(void) {

	clear_board();
	display_image(0, board_data, 128, 0x03); 

	if (mode_navigation == 2) mode_navigation = 0; 
	switch (mode_navigation)
	{
	case 0:
		display_string(1, "> SURVIVAL");
		display_string(2, "  TWO PLAYERS");
		display_string(3, "");
		break;
	case 1:
		display_string(1, "  SURVIVAL");
		display_string(2, "> TWO PLAYERS");
		display_string(3, "");
		break;
	default:
		break;
	}
	display_update(4);
}

// Hanterar mode
void choose_mode(void) {

	while (1) {
		print_mode();
		int buttons;
		while (!(buttons = (getbtns() & 6))); // while btns 3 and 4 are not pressed
		delay(200);
		if (buttons & 4) { // button 4 confirms option
			if (mode_navigation == 0) mode = 0;	// Survival
			if (mode_navigation == 1) mode = 1;	// Two players
			return;
		}

		if (buttons & 2) mode_navigation++; // scrolls downward
	}
}

// FÖLJANDE FUNKTIONER ÄR EJ EGNA

static void num32asc(char * s, int n) 
{
  int i;
  for( i = 28; i >= 0; i -= 4 )
    *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}

#define ITOA_BUFSIZ ( 24 )
char * itoaconv(int num, int place)
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";
  
  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */

  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }

	// FÖLJANDE KOD HAR LAGTS TILL
	int j = 0, k;
	if (playing) k = 0;
	else k = 6;

	for (; j < k; j++, i--)
		itoa_buffer[i] = ' ';
	
	if (place) {
		if (place == 1) itoa_buffer[i--] = '1';
		if (place == 2) itoa_buffer[i--] = '2';
		itoa_buffer[i] = '#';
	}
	else i++;
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return( &itoa_buffer[ i ] );
}

void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void display_init(void) {
  DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}

void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;
	
	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

void display_update(int pages) {
	int i, j, k;
	int c;
	for(i = 0; i < pages; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0x0);
		spi_send_recv(0x10);

		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;
			
			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}