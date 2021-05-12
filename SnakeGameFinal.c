#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

// main authors are given for each function/code block
// George was also in charge of the github and gameplay videos

#define DELAY 100000
#define TIMEOUT 10 

// No specific author
// A lot of variable declaration, pretty self explanatory.
int trophyGet, x, y, nextX, nextY, score = 0;
int length;
int congratsCondition = 10;
bool gameOver = false;

// Michael
typedef struct point 
{
		int x, y;
} point;

// Michael
// Create the type direction-type, with the possible values, LEFT, RIGHT, UP, DOWN
typedef enum 
{
	LEFT, RIGHT, UP, DOWN
} direction_type;

// Michael
// currDir = durrent direction
direction_type currDir;
// snake is an array of 'point's size 255
point snake[255] = {};
point trophy;

// George (mainly), Brian (trophy.x.y == snake.x.y check)
void createtrophy() 
{
    // trophy x between 1 to 98
    // trophy y between 1 to 30
	trophy.x = (rand() % (97)) + 1;
	trophy.y = (rand() % (29)) + 1;

    // If the trophy happens to overlap with any of the snake parts, createtrophy again
    for (int i = 0; i <= 255; i++)
        if (trophy.x == snake[i].x && trophy.y == snake[i].y)
            createtrophy();
}

// Michael
void drawPart(point drawPoint) 
{
	mvprintw(drawPoint.y, drawPoint.x, "o");
}

// Josh (mainly), Brian (some fixes)
// Creates a border
// We spent a while trying to get a working border, this was the only way we could get it to work
void drawBorder()
{
    // Top border, just a huge line of hashtags
    move(0,0);
    addstr("####################################################################################################");
    
    // Between the top and bottom border, add hashtags to the first and last hashtags, creating columns
    for(int i = 1; i <= 30; i++)
    {
        move(i,0);
        addstr("#                                                                                                  #");
    }

    // Bottom border, also a huge line of hashtags
    move(31,0);
    addstr("####################################################################################################");
}

// George
// Draw everything that needs to be on screen, on screen
void drawScreen() 
{
    drawBorder();

	// Print game over if gameOver is true
	if(gameOver)
		mvprintw(15, 50, "Game Over!");

	// Creates the snake
	for(int i = 0; i < length; i++) 
		drawPart(snake[i]);
	
    // Draw the score
	mvprintw(33, 3, "Score: %i", score);

    // Draw the current trophy
	drawPart(trophy);
        
    refresh();
	// Delay between movements
	usleep(DELAY); 
}

// George
// Contains curses settings
void cursesSettings() 
{
        // Initialize window
		initscr();
        // Stops echoing of keypresses
		noecho();
        // different form of 'raw'
		cbreak();
		timeout(TIMEOUT);
        // Hide cursor
		curs_set(FALSE);
        // Enables use of arrow keys
        keypad(stdscr,TRUE);
}

// George
// Contains settings, called at the beginning of each game
void settings() 
{
	currDir = RIGHT;
	length = 5;
	gameOver = false;
    score = 0;
	clear();
		
	// Initial snake coordinates
	int j = 0;
	for(int i = length + 2; i >= 0; i--) {
		point currPoint;
			
        // about the middle of the border, can be changed to start anywhere
		currPoint.x = 50;
		currPoint.y = 15;

		snake[j] = currPoint;
		j++;
	}

    createtrophy();
	refresh();
}

// Michael (includes everything related to this function)
// Displays a congrats every 10 trophies collected
// Not really necessary, but just something extra
void congrats()
{
    mvprintw(35, 30, "Congrats, you've collected %i trophies!", trophyGet);
}

// Brian
// Actually makes the snake move
void shiftSnake() 
{
	point temp = snake[length - 1];

	for(int i = length - 1; i > 0; i--) 
		snake[i] = snake[i-1];
		
	snake[0] = temp;
}

// Josh (input detection), Brian (the rest)
int main (int argc, char *argv[])
{
    // Get settings
    cursesSettings();
	settings();
    clear();
    
    // Since this while loop never ends, everything in this happens constantly
    while (1)
    {	
        // When you lose, wait two seconds, then restart the game
		if(gameOver) 
        {
			sleep(2);
			settings();
		}
        
        if (trophyGet == congratsCondition)
        {
            congrats();
            congratsCondition += 10;
        }
            
        // keypress equals whatever key is pressed next
        // Since it's in a while loop, you can press keys all the time
        // Although, pressing too many keys too fast will buffer them,
        // your inputs will still be executed, but not always when you hit the key
        int keypress=getch();
 
        // Keeps track of the snakes direction
        // If you were looking through the snakes eyes, you can only move to your left, right, and front
        // so, you cannot go right, then go through yourself to go left.
        switch(keypress) 
        {
            case KEY_UP:
                if (currDir != UP && currDir != DOWN)
                {
                    currDir = UP;
                    break;
                }
                break;
    
            case KEY_DOWN:
                if (currDir != DOWN && currDir != UP)
                {
                    currDir = DOWN;
                    break;
                }
                break;

            case KEY_LEFT:
                if (currDir != LEFT && currDir != RIGHT)
                {
                    currDir = LEFT;
                    break;
                }
                break;
            case KEY_RIGHT:
                if (currDir != RIGHT && currDir != LEFT)
                {
                    currDir = RIGHT;
                    break;
                }
                break;

            // 'x' or 'X' will close the program, not really necessary since interrupts exist, but it's nice to have
            case 'x':       
            case 'X':
                endwin();   
                return 0;
      
        }

        // Set the next X and Y to the heads X and Y
        // Without this, the snake would 'teleport' so to speak at the beginning of the game
        // from the initial position set earlier, to (0,0), or if played consecutively,
        // wherever you last died
        nextX = snake[0].x;
		nextY = snake[0].y;

        // Based on what your direction is, move in said direction by increasing/decreasing the next X and Y
        if(currDir == RIGHT) 
            nextX++;
		else if(currDir == LEFT) 
            nextX--;
		else if(currDir == UP) 
            nextY--;
		else if(currDir == DOWN) 
            nextY++;

        if(nextX == trophy.x && nextY == trophy.y) 
        {
			point tail;
			tail.x = nextX;
			tail.y = nextY;

			snake[length] = tail;

			if(length < 255)
				length++;
			else
            // If the array exceeds 255, reset it to length = 5 but keep accruing score
				length = 5; 

			score += 5;
            trophyGet++;
			createtrophy();
		}

		// Game over if the next x and y is the same x and y of any part of the snake
        else
        {
            for(int i = 0; i < length; i++) 
            {
                if(nextX == snake[i].x && nextY == snake[i].y) 
                {
                    gameOver = true;
                    break;
                }
            }
        }
		
        // Set tail as new head
		snake[length - 1].x = nextX;
		snake[length - 1].y = nextY;        

        // Since this is in the while loop, it makes the snake move continuously
		shiftSnake();

		// Game over if the player hits the border
        // Values have to be adjusted with the border
        // You will need to actually hit the wall to lose
        // which means you can move right next to it
		if((nextX >= 99 || nextX <= 0) || (nextY >= 31 || nextY <= 0)) 
			gameOver = true;
		
        // Continually draw the screen
		drawScreen();

	}

    // If anything were to break the while loop somehow, set everything to normal and end the program
    endwin();
	nocbreak();
	return 0;
               
}
