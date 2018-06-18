/*
Author: Andy Cox V
Date: 9/11/2017
Program: aigame.exe
Source Code File Name: aigame.c
Programming Language: C (C99)
Description:
        This game requires "levels.txt" to be in the same directory as this executable to load 80x24 ASCII map files.
        The purpose of the program was to demonstraight basic game AI for an ecomomics project.
        One of the biggest issues was with refreshing the screen. Since C does not have a concept for a screen it was
        up to the operating system to create a terminal for this application. Using the system clear or cursor move position
        caused much lag distorting the picture and making playing almost unbearable. The fix was to use 80x25 standard console
        and write a dump of the current game grid on one sitting. The automatic wrapping used by the console created the illusion
        of north, west, south, and east. This game is not cross platform due to the use of conio.h in particular kbhit() and getch().
        This game was compiled with tcc and ran on Windows 10. Code could have been written much more effeciently and the use of
        global structs could have been eliminated, but time was a major constraint that prevented effecientcy.
*/

#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>

#define HORIZONTAL      1       /* Used for horizontal movement subtract for left and add for right. */
#define VERTICAL        80      /* Used for vertical movement subtract for up and add for down. */
#define GRID_SIZE       1920    /* Used for the size of the GRID in memory 80 * 24 = 1920. */
#define PHYSICAL_GRID   1921    /* Used for display purposes. */
#define TRUE            1       /* Used for boolean statements in ANSI C99 any non-zero value is true. */
#define FALSE           0       /* Used for boolean statements false. */
#define LEGAL_CHARACTERS_SIZE   6       /* Used for the size of the legal characters array. */
#define TILE_UNIT       ' '     /* The tile used to represent a blank space. */
#define TILE_PLAYER     '@'     /* Used to represent the player. */
#define TILE_TOKEN      '$'     /* Used as the token for the player to capture. */
#define TILE_CHASER     'X'     /* Symbol used for the chasers. */
#define TILE_WALL       '#'     /* Symbol used for walls. */
#define TILE_EXIT       '*'     /* Symbol used to exit the level. */

/* Used to define and maintain the chasers. */
struct Chaser
{
        int counter;           /* Keep track of number of chasers per level and used to index from positon array. */
        int speed;             /* Delay in milliseconds before each move. */
        int total;             /* Used for statistic purposes. */
        int *array;            /* Pointer used to store the positions of the chasers. */
        char player_move;
};

/* Used to define and maintain level status. */
struct Level
{
        int level;
        int token;
        char legal_characters[LEGAL_CHARACTERS_SIZE];
        char *array;    /* Used as a pointer for the grid. */
};

struct Chaser Chaser;
struct Level Level;

/* Use at the start of each new level. */
void initilizeLevel(int speed)
{
        /* Initilize miscellenous data. */
        free(Chaser.array);
        free(Level.array);
        Level.level++;
        
        /* Initilize legal characters. */
        Level.legal_characters[0] = TILE_CHASER;
        Level.legal_characters[1] = TILE_EXIT;
        Level.legal_characters[2] = TILE_PLAYER;
        Level.legal_characters[3] = TILE_TOKEN;
        Level.legal_characters[4] = TILE_UNIT;
        Level.legal_characters[5] = TILE_WALL;
        
        /* Initilize a new grid. 
        Errors may occur on malloc if there is not enough memory. */
        Level.array = (char*)malloc(PHYSICAL_GRID);    /* Set the size of the grid. */
        memset(Level.array, TILE_UNIT, GRID_SIZE);     /* Set all of the bytes in the grid to blank tiles. */
        Level.array[PHYSICAL_GRID] = 0;                /* Set last byte of grid to zero to determine when the grid stops in memory. */
        
        /* Initilize a new irritation of chasers. */
        Chaser.player_move = 0;
        Chaser.counter = 0;
        Chaser.speed = speed;
        Chaser.array = (int*)malloc(GRID_SIZE * sizeof(int));    /* Set the pointer to a location in memory to store chaser positions. */
}

/* Used to check if the characters used in the loaded file are legal. */
unsigned char isLegal(char file_character)
{
        char boolean = FALSE;
        
        for(unsigned char i = 0 ; i < LEGAL_CHARACTERS_SIZE ; i++)
                if(file_character == Level.legal_characters[i])
                        boolean = TRUE;
        
        return boolean;                
}

/* Loads the level data from the file into memory to be played. */
void loadLevel(FILE *file)
{
        int irritations = 0;
        char file_character = 0;

        while(irritations < GRID_SIZE)
        {
                file_character = getc(file);
                
                if(file_character == EOF) /* End of file then go back to the first level. */
                        rewind(file);
                
                if(isLegal(file_character))
                {
                        Level.array[irritations] = file_character;
                        irritations++;
                }
        }
}

/* Use to display the playing feild and other character attributes. 
IMPORTANT: Due to time constraints DO NOT allow levels to be of different sizes than 25 x 80!
The main issue is refreshing by clearing the screen using Windows command prompt it is too slow and refresh is glitchy.*/
void gridDisplay(void)
{
        printf("\n%sLevel: %d\tTokens: %d\tTokens/Level: %lf", Level.array, Level.level, Level.token, ((double)Level.token / (double)Level.level));
}

void gameOver(void)
{
        
        char input = 0;
printf(
"\n  **    **                                     **\n"                              
" //**  **                                     /**\n"                              
"  //****    ******  **   ** **    **  *****   /**       *****   *****  *******\n" 
"   //**    **////**/**  /**/**   /** **///**  /******  **///** **///**//**///**\n"
"    /**   /**   /**/**  /**//** /** /*******  /**///**/*******/******* /**  /**\n"
"    /**   /**   /**/**  /** //****  /**////   /**  /**/**//// /**////  /**  /**\n"
"    /**   //****** //******  //**   //******  /****** //******//****** ***  /**\n"
"    //     //////   //////    //     //////   /////    //////  ////// ///   //\n"
"      ********** ******** *******   ****     **** ** ****     **     **\n"    
"     /////**/// /**///// /**////** /**/**   **/**/**/**/**   /**    ****\n"   
"         /**    /**      /**   /** /**//** ** /**/**/**//**  /**   **//**\n"  
"         /**    /******* /*******  /** //***  /**/**/** //** /**  **  //**\n" 
"         /**    /**////  /**///**  /**  //*   /**/**/**  //**/** **********\n"
"         /**    /**      /**  //** /**   /    /**/**/**   //****/**//////**\n"
"         /**    /********/**   //**/**        /**/**/**    //***/**     /**\n"
"         //     //////// //     // //         // // //      /// //      //\n" 
"                          ********** ******** *******\n"  
"                         /////**/// /**///// /**////**\n" 
"                             /**    /**      /**    /**\n"
"                             /**    /******* /**    /**\n"
"                             /**    /**////  /**    /**\n"
"                             /**    /**      /**    **\n" 
"                             /**    /********/*******\n"  
"                             //     //////// ///////\n"   
"                        --==Press any key to continue==--");

        getch();

        printf("\nLevel: %d | Tokens: %d | Tokens/Level: %lf | Total Chasers: %d | Speed: %d\n", Level.level, Level.token, ((double)Level.token / (double)Level.level), Chaser.total, Chaser.speed);

        printf("Replay (y/n)?");
        while((input != 'y') && (input != 'n'))
                input = getch();
        
        if(input == 'y')
        {
                Level.token = 0;
                Level.level = 0;
                main();
        }
        
        /* Player pressed 'n'. */
        exit(0); /* File pointer and other pointers will be freed at this point. */
}

/* Returns the ASCII tile value at the position. */
char callGrid(int location)
{
        return Level.array[location];
}

/* Write an ASCII character to a place on the grid. */
void writeGrid(int location, char unit)
{
        Level.array[location] = unit;
}

/* Spawns a unit at random positions between 0 and Leve.size. <==== NEEDS WORK!!! */ 
int spawn(char unit)
{
        int location = 0;
        srand(time(0));
        
        do
        {
                location = (int)rand() % GRID_SIZE;
        }while((callGrid(location) != TILE_UNIT));
        
        writeGrid(location, unit); /* Write the unit to the grid. */

        return location;
}

/* Adds new chasers the first chaser will be stored at location zero. */
void newChaser()
{
        Chaser.array[Chaser.counter] = spawn(TILE_CHASER);
        Chaser.counter++;
        Chaser.total++;
}

/* Moves all of the chasers closer to the player. */
void chaserMove(int player_location)
{
        int chaser_location = 0;
        int original_chaser_location = 0;
        int chaser_y = 0;
        int chaser_x = 0;
        int player_y = player_location / VERTICAL;
        int player_x = player_location  - (player_y * VERTICAL);
        
        /* Cycle through all of the chaser positions. */
        for(int i = 0 ; i < Chaser.counter ; i++)
        {
                chaser_location = Chaser.array[i];
                original_chaser_location = chaser_location;
                chaser_y = chaser_location / VERTICAL;
                chaser_x = chaser_location - (chaser_y * VERTICAL);
                
                /* Clear the original Chaser from the grid. */
                writeGrid(chaser_location, TILE_UNIT);
                
                        /* Find best path of attack. */
                        if(Chaser.player_move) /* Player is moving along X-Axis. */
                                if(chaser_x < player_x)
                                       chaser_location += HORIZONTAL;
                                else if(chaser_x > player_x)
                                        chaser_location -= HORIZONTAL;
                                else
                                        if(chaser_y < player_y)
                                                chaser_location += VERTICAL;
                                        else
                                                chaser_location -= VERTICAL;
                        else if(!Chaser.player_move) /* Player is moving along Y-Axis. */
                                if(chaser_y < player_y)
                                       chaser_location += VERTICAL;
                                else if(chaser_y > player_y)
                                        chaser_location -= VERTICAL;
                                else
                                        if(chaser_x < player_x)
                                                chaser_location += HORIZONTAL;
                                        else
                                                chaser_location -= HORIZONTAL;
                                        
                        /* Check for tile or player. */
                        if(callGrid(chaser_location) == TILE_UNIT)
                        {                
                                writeGrid(chaser_location, TILE_CHASER);
                                Chaser.array[i] = chaser_location;
                        }
                        else if (callGrid(chaser_location) == TILE_PLAYER)
                                gameOver();
                        else
                                writeGrid(original_chaser_location, TILE_CHASER);
        }
}

/* Manual player movement. */
int playerMove(void)
{
        int player_location = 0;
        char player_key = getch();
        
        switch(player_key)
        {
                        
                case 's': /* Move down. */
                        player_location += VERTICAL;
                        Chaser.player_move = FALSE; /* Player moves vertically. */
                        break;
                        
                case 'a': /* Move to left one unit. */
                        player_location -= HORIZONTAL;
                        Chaser.player_move = TRUE; /* Player moves horizontally. */
                        break;
                        
                case 'd': /* Move right one unit. */
                        player_location += HORIZONTAL;
                        Chaser.player_move = TRUE; /* Player moves horizontally. */
                        break;
                        
                case 'w': /* Move up. */
                        player_location -= VERTICAL;
                        Chaser.player_move = FALSE; /* Player moves vertically. */
                        break;
                        
                case 'q': /* Quit. */
                        gameOver();
                        break;
                        
                case 'p': /* Pause. */
                        printf("\tPAUSED");
                        getch();
                        break;
        }
        
        return player_location;
}

int main()
{  
        int speed = 0;
        int player_temp = 0;
        Chaser.total = 0;
        char input = 0;
        char num[4] = {0, 0, 0, 0};
        
        
        /* Welcome screen. */
        printf(
"\n\n                                ___\n"     
"                               /  /\\           ___\n"
"                              /  /::\\         /__/\\\n"
"                             /  /:/\\:\\        \\__\\:\\\n"
"                            /  /::\\ \\:\\       /  /::\\\n"
"                           /__/:/\\:\\_\\:\\   __/  /:/\\/\n"
"                           \\__\\/  \\:\\/:/  /__/\\/:/~~ \n"
"                                \\__\\::/   \\  \\::/\n"
"                                /  /:/     \\  \\:\\\n"
"                               /__/:/       \\__\\/\n"
"                               \\__\\/\n"
"       ______  __    __       ___           _______. _______ .______\n"      
"      /      ||  |  |  |     /   \\         /       ||   ____||   _  \\\n"     
"     |  ,----'|  |__|  |    /  ^  \\       |   (----`|  |__   |  |_)  |\n"    
"     |  |     |   __   |   /  /_\\  \\       \\   \\    |   __|  |      /\n"     
"     |  `----.|  |  |  |  /  _____  \\  .----)   |   |  |____ |  |\\  \\----.\n"
"      \\______||__|  |__| /__/     \\__\\ |_______/    |_______|| _| `._____|\n"
"      _              _          ____           __     __  ____   ___  _ _____\n"
"     / \\   _ __   __| |_   _   / ___|_____  __ \\ \\   / / |___ \\ / _ \\/ |___  |\n"
"    / _ \\ | '_ \\ / _` | | | | | |   / _ \\ \\/ /  \\ \\ / /    __) | | | | |  / /\n" 
"   / ___ \\| | | | (_| | |_| | | |__| (_) >  <    \\ V /    / __/| |_| | | / /\n"  
"  /_/   \\_\\_| |_|\\__,_|\\__, |  \\____\\___/_/\\_\\    \\_/    |_____|\\___/|_|/_/\n"   
"                       |___/\n"
"\n    --==Press any key to continue designed for 80x25 character terminals==--");
        getch();
        
        printf(
"\n   ______  ______      ____     __  __  ______  ____    ____    ____\n"
 "  /\\  _  \\/\\__  _\\    /\\  _`\\  /\\ \\/\\ \\/\\  _  \\/\\  _`\\ /\\  _`\\ /\\  _`\\\n"
 "  \\ \\ \\_\\ \\/_/\\ \\/    \\ \\ \\/\\_\\\\ \\ \\_\\ \\ \\ \\_\\ \\ \\,\\_\\_\\ \\ \\_\\_\\ \\ \\_\\ \\\n"   
 "   \\ \\  __ \\ \\ \\ \\     \\ \\ \\/_/_\\ \\  _  \\ \\  __ \\/_\\__ \\\\ \\  _\\_\\ \\ ,  /\n"   
 "    \\ \\ \\/\\ \\ \\_\\ \\__   \\ \\ \\_\\ \\\\ \\ \\ \\ \\ \\ \\/\\ \\/\\ \\_\\ \\ \\ \\_\\ \\ \\ \\\\ \\\n"  
 "     \\ \\_\\ \\_\\/\\_____\\   \\ \\____/ \\ \\_\\ \\_\\ \\_\\ \\_\\ `\\____\\ \\____/\\ \\_\\ \\_\\\n"
 "      \\/_/\\/_/\\/_____/    \\/___/   \\/_/\\/_/\\/_/\\/_/\\/_____/\\/___/  \\/_/\\/ /\n\n\n"
"\nInstructions:\n"
"The goal of AI CHASER is to collect and advance as many tokens and levels as\n"
"possible in one run. The score is given as a ratio between token/level.\n"
"The player cannot run into walls if the player does the player is bounced\n"
"back one tile. Chasers will spawn at every token collected. The game is\n"
"over when a Chaser catches the player. To advance levels the player must\n"
"reach the exit tile once reached all Chasers on the current feild are\n"
"removed. At every increasing level the Chasers become faster by a factor\n"
"of two. This executable program MUST be accompanied by \"levels.txt\"!\n"
"This file contains level information in 80x24 ASCII character maps.\n\n"
"                        --==Press any key to continue==--\n");
        getch();

printf(
"\nPlayer Controls:\n"
"\tW - Move North\n"
"\tA - Move West\n"
"\tS - Move South\n"
"\tD - Move East\n"
"\tQ - Quit Current Game\n"
"\tP - Pause Current Game\n\n"
"Characters:\n"
"\t@ - Player\n"
"\tX - Chaser\n"
"\t$ - Token\n"
"\t* - Exit Tile\n"
"\t# - Wall\n"
"\nInput difficulty (Chaser delay in units of time range 0-9999)\n"
"Press <ENTER> once done: ");
        
        /* Since speed is unused here to save space use speed as a temparary variable. 
        This do-while loop aquires user positive integer input from 0-9999. 
        Exit on <ENTER>, only accept ASCII characters '0'-'9' and backspace.*/
        do
        {
                input = getch();
                
                if((input == '\b') && (speed > 0)) /* Backspace pressed. */
                {
                        printf("\b \b");
                        speed--;
                }
                
                if(((input ^ 0x30) <= 9) && (speed < 4)) /* ASCII character '0'-'9' pressed. */
                {
                        num[speed] = input;
                        speed++;
                        putchar(input);
                }
        }while(input != 0x0D);
        
        speed = atoi(num);
        
        /* Process the level file must always be named levels.txt! */
        FILE *file = fopen("levels.txt", "r");

        if(!file)
        {
                printf("\nFATAL ERROR: levels.txt is not found!\n"
                        "Place the file levels.txt in the same directory as this executable!\n"
                        "Press any key to exit...\n");
                        getch();
                        exit(0);
        }
                        
        initilizeLevel(speed);
        loadLevel(file);
        
        /* --- BEGIN THE GAME --- */
        
        int player_location = spawn(TILE_PLAYER);
        
        spawn(TILE_TOKEN);

        while(1)
        {
                gridDisplay();
                for(int i = Chaser.speed ; i > 0 ; i--)
                {
                        if(kbhit())
                        {
                                writeGrid(player_location, TILE_UNIT);
                                
                                player_temp = playerMove();
                                
                                /* Cannot run through walls infact bouce back if running into a wall. */
                                if((callGrid(player_location + player_temp) == TILE_WALL) && (callGrid(player_location - player_temp) == TILE_UNIT))
                                                player_location -= player_temp;
                                else if(callGrid(player_location + player_temp) != TILE_WALL)
                                        player_location += player_temp;
                                /* Anything else just keep the old position. */
                                
                                /* Token captured by player! Spawn another token and chaser. */
                                if(callGrid(player_location) == TILE_TOKEN)
                                {
                                        spawn(TILE_TOKEN);
                                        newChaser();
                                        Level.token++;
                                }
                                
                                if(callGrid(player_location) == TILE_EXIT)
                                {
                                        speed /= 2;
                                        initilizeLevel(speed);
                                        loadLevel(file);
                                        spawn(TILE_TOKEN);
                                        player_location = spawn(TILE_PLAYER);
                                }
                               
                                if(callGrid(player_location) == TILE_CHASER)
                                        gameOver();
                               
                                writeGrid(player_location, TILE_PLAYER);
                                gridDisplay();
                        }
                        sleep(1);
                }
                chaserMove(player_location);
        }
        return 0;
}