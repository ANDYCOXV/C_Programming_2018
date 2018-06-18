/*
Author: Andy Cox V
Date: 5/13/2016
Programming Language: C (C99)
Program: Coleccionista.c --> Coleccionista.exe
Compiler: tcc.exe
This program plays a simple 80x25 console based game.
This program was written for a game for spanish class.
*/

#include <stdio.h>
#include <time.h> /* Used onyl for time(), for srand() */
#include <windows.h> /* Used only for Beep()  */

const unsigned char blank = '_';
const unsigned char ply = '@'; /* Also used for the map clearer bonus. */
const unsigned char tile = '#';
const unsigned char token = '$';

const unsigned char hello[876] =       "Coleccionista.exe - Andy Cox V - 5/13/2016 - 80x25 - Microsoft Windows Xp Home+\n"
                                        "                _____     _             _         _     _\n"       
                                        "               |     |___| |___ ___ ___|_|___ ___|_|___| |_ ___ \n"
                                        "               |   --| . | | -_|  _|  _| | . |   | |_ -|  _| .'|\n"
                                        "               |_____|___|_|___|___|___|_|___|_|_|_|___|_| |__,|\n\n"
                                        "REGLAS:\n"
                                        "\tEL jugador empezara alazar en una parrilla\n"
                                        "y coleccionara fichas que aparecen uno por uno.\n"
                                        "No cruzara su previa comino sin pagar una ficha.\n"
                                        "Para ficha coleccionada un enemigo se generara y\n"
                                        "cazara al jugador. Cada paso valdra un dolar y cada\n"
                                        "ficha multiplicara el total. Si cruza su camino original\n"
                                        "una ficha. El juego terminara si un enemigo captura al\n"
                                        "Su movera w,a,s,d\n"
                                        "La terminara \'q\'\n"
                                        "jugador o cruza su camino original sin ficha.\n\n"
                                        "Ficha: $\n"
                                        "Tesela: #\n"
                                        "Jugador y presente: @\n"
                                        "Vacio: _\n"
                                        "Enemigo: ???\n\n"
                                        "Empuja algo de tecla a continua...";

unsigned int moves = 0;
unsigned int total_token = 0;
unsigned int score = 0;
unsigned short badguyloc = 0;

int tokens = 0;
short plyloc = 0;
                        
unsigned char badguytype[1840];                        
unsigned char grid[1841]; /* 80x23 grid */

short badguystore[1840];

void collecttoken(void)
{
        Beep(1000, 100);
        Beep(500, 50);
        Beep(1500, 80);
}

void spendtoken(void)
{
        Beep(1100, 100);
        Beep(1400, 50);
}

void clearcollect(void)
{
        Beep(1100, 120);
        Beep(800, 210);
        Beep(900, 200);
        Beep(1000, 100);
}

void badguycapture(void)
{
        Beep(500, 150);
        Beep(600, 250);
        Beep(400, 300);
        Beep(150, 400);
}

void clear_badguystore(void)
{
        for(unsigned short i = 0 ; i < 1840 ; i++)
                badguystore[i] = 0;
        
        badguyloc = 0;
        
}

void clear_grid(void)
{
        
        score = 0;
        total_token = 0;
        tokens = 0;
        moves = 0;
        badguyloc = 0;
        plyloc = 0;
        
        for(unsigned short i = 0 ; i < 1840 ; i++)
                grid[i] = blank;
        
        for(unsigned short i = 0 ; i < 1840 ; i++)
                badguytype[i] = 0;
        
        grid[1841] = 0;
}

void clear_screen(void)
{
        system("cls");
}

void clear_map(void)
{

        unsigned char clearer = 0;
        
        clear_screen();
        printf("Limpia la mapa tesela o enemigos o los dos?\n"
        "Empuja \'1\' limpia la mapa enemigos.\n"
        "Empuja \'2\' limpia la mapa tesela.\n"
        "Empuja \'3\' limpia la mapa todo (costa todos fichas)!\n"
        "Entrada: ");
        
        clearer = getch();

        if(clearer == '1')
        {
                for(unsigned short i = 0 ; i < 1840 ; i++)
                        if((grid[i] != tile) && (grid[i] != ply) && (grid[i] != token) && (grid[i] != blank))
                                grid[i] = blank;

                for(unsigned short i = 0 ; i < 1840 ; i++)
			badguystore[i] = 0;

		badguyloc = 0;
                        
                return;
        }
        
        if(clearer == '2')
        {
                for(unsigned short i = 0 ; i < 1840 ; i++)
                        if(grid[i] == tile)
                                grid[i] = blank;
                        
                return;
                        
        }
        
        if(clearer == '3')
        {
                tokens = 0;
                
                for(unsigned short i = 0 ; i < 1840 ; i++)
                        if((grid[i] != ply) && (grid[i] != token) && (grid[i] != blank))
                                grid[i] = blank;

                for(unsigned short i = 0 ; i < 1840 ; i++)
			badguystore[i] = 0;

		badguyloc = 0;

                        
                return;
        }
        
        else
                clear_map();
}

void endgame(char killer)
{
        badguycapture();
	clear_screen();
	printf("                                   ~~~FIN~~~\n%s", grid);
	getch();
        clear_screen();
        printf("Total: %d\n"
                "Total fichas: %d\n"
                "Actual Fichas: %d\n"
                "Pasos: %d\n"
                "Cazador de: %c\n\n"
                "Empuja algo de tecla a continua o \'q\' a termina...\n", score, total_token, tokens, moves, killer);
                
        clear_grid();
                
        killer = getch();
        killer |= 0x20;
        
        if(killer != 'q')
                main();
        else
                clear_screen();
                printf("\nColeccionista.exe - Andy Cox V - 5/12/2016\nEmpuja algo de tecla a continua...");
                getch();
                exit(0);
}

void badguymove(void)
{

        unsigned char nocol = 0; /* 0 = same col, 1 = need go left, 2 = need go right. */
        unsigned char norow = 0; /* 0 = same row, 1 = need to go down, 2 = need to go up. */
        unsigned short oldbadguystore = 0;
        
        for(unsigned short i = 0 ; badguystore[i] != 0 ; i++)
        {
                
                oldbadguystore = badguystore[i];

                if((badguystore[i] / 80 != plyloc / 80) && (badguystore[i] < plyloc)) /* Need to go up. */
                        norow = 2;
                
                if((badguystore[i] / 80 != plyloc / 80) && (badguystore[i] > plyloc)) /* Need to go down. */
                        norow = 1;
                
                if(badguystore[i] / 80 == plyloc / 80)
                        norow = 0;
                        
                if((badguystore[i] % 80) > (plyloc % 80)) /* Need to go left. */
                        nocol = 1;
                
                if((badguystore[i] % 80) < (plyloc % 80)) /* Need to go right. */
                        nocol = 2;
                        
                if((badguystore[i] % 80) == (plyloc % 80))
                        nocol = 0;
                
                if((nocol == 0) && (norow == 1))
                        if((grid[badguystore[i] - 80] == ply) || (grid[badguystore[i] - 80] == blank))
                                badguystore[i] -= 80;
                
                if((nocol == 0) && (norow == 2))
                        if((grid[badguystore[i] + 80] == ply) || (grid[badguystore[i] + 80] == blank))
                                badguystore[i] += 80;
                
                if((nocol == 1) && (norow == 0))
                        if((grid[badguystore[i] - 1] == ply) || (grid[badguystore[i] - 1] == blank))
                                badguystore[i]--;
                
                if((nocol == 2) && (norow == 0))
                        if((grid[badguystore[i] + 1] == ply) || (grid[badguystore[i] + 1] == blank))
                                badguystore[i]++;
                
                if((nocol == 1) && (norow == 1))
                        if((grid[badguystore[i] - 80] == ply) || (grid[badguystore[i] - 80] == blank))
                                badguystore[i] -= 80;
                        else
                                if((grid[badguystore[i] - 1] == ply) || (grid[badguystore[i] - 1] == blank))
                                        badguystore[i]--;
                                
                if((nocol == 2) && (norow == 2))
                        if((grid[badguystore[i] + 80] == ply) || (grid[badguystore[i] + 80] == blank))
                                badguystore[i] += 80;
                        else
                                if((grid[badguystore[i] + 1] == ply) || (grid[badguystore[i] + 1] == blank))
                                        badguystore[i]++;
                   
                if(badguytype[i] == 0)
                {
                 if((nocol == 2) && (norow == 1))
                        if((grid[badguystore[i] - 80] == ply) || (grid[badguystore[i] - 80] == blank))
                                badguystore[i] -= 80;
                        else
                                if((grid[badguystore[i] + 1] == ply) || (grid[badguystore[i] + 1] == blank))
                                        badguystore[i]++;

                  if((nocol == 1) && (norow == 2))
                        if((grid[badguystore[i] + 80] == ply) || (grid[badguystore[i] + 80] == blank))
                                badguystore[i] += 80;
                        else
                                if((grid[badguystore[i] - 1] == ply) || (grid[badguystore[i] - 1] == blank))
                                        badguystore[i]--;                 
                }
                
                if(badguytype[i] == 1)
                {
                 if((nocol == 2) && (norow == 1))
                                if((grid[badguystore[i] + 1] == ply) || (grid[badguystore[i] + 1] == blank))
                                        badguystore[i]++;
                        else
                                if((grid[badguystore[i] - 80] == ply) || (grid[badguystore[i] - 80] == blank))
                                        badguystore[i] -= 80;

                  if((nocol == 1) && (norow == 2))
                                if((grid[badguystore[i] - 1] == ply) || (grid[badguystore[i] - 1] == blank))
                                        badguystore[i]--;        
                        else
                                if((grid[badguystore[i] + 80] == ply) || (grid[badguystore[i] + 80] == blank))
                                        badguystore[i] += 80;
                }
                
                if(badguystore[i] != oldbadguystore)
                {
                        grid[badguystore[i]] = grid[oldbadguystore];
                        grid[oldbadguystore] = blank;       
                }
                
                if(((nocol == 0) && (norow == 0)) || (badguystore[i] == plyloc))
                        endgame(grid[badguystore[i]]);
                
        }
}

void moveply(short i)
{
        
        unsigned char badguy = 0;

	grid[plyloc] = tile;

	if((i == 1) && (plyloc == 1839))
		plyloc = -1;
        else
	{
		if(plyloc + i < 0)
		{
			plyloc = (plyloc % 80) + 1760;
			plyloc += 80;
		}

		if(plyloc + i > 1840)
		{
			plyloc %= 80;
			plyloc -= 80;
		}
	}
        
        plyloc += i;
        
        if(grid[plyloc] == token)
        {
                collecttoken();
                
                tokens++;
                total_token++;
                spawn(token);
                
                do
                {
                        badguy = (0x21 + (rand() % 0x94));
                }while((badguy == tile) || (badguy > 0x7E));
                
                if((badguy == ply) || (badguy == blank) || (badguy == token)) /* Spawn bonus instead of a bad guy. */
                        spawn(badguy);
                        
                else
                {
                        badguytype[badguyloc] = (rand() % 2);
                        badguystore[badguyloc] = spawn(badguy); /* Spawns a bad guy ASCII ! - ~, can also spawn tokens, blanks, and player character (map clearer)! */
                        badguyloc++;       
                }
        }
        
        if(grid[plyloc] == ply)
        {
                clearcollect();
                clear_map();
        }
        
        if(grid[plyloc] == tile)
        {
         
               tokens--;
               spendtoken();

               if(tokens < 0)
               {
                      tokens = 0;
                      endgame(tile);        
               }
        }
        
        if((grid[plyloc] != blank) && (grid[plyloc] != token) && (grid[plyloc] != tile) && (grid[plyloc] != ply))
                endgame(grid[plyloc]);   
                
        grid[plyloc] = ply;
        moves++;
        badguymove();
}

int spawn(unsigned char character)
{
        unsigned short i = 0;
        
        do
        {
                i = (rand() % 1841);
        }while(grid[i] != blank);
        
        grid[i] = character;
        
        return i;
}

int main(void)
{
        
        unsigned char input = 0;
        
        clear_screen();
        printf("%s", hello);
        getch();
        clear_screen();
        srand(time(NULL)); /* Set the rand() seed generation. */
        clear_badguystore();
        clear_grid();
        plyloc = spawn(ply);
        spawn(token);
        
        do
        {
                
                if(tokens > 0)
                        score = tokens * moves;
                else
                        score = moves;
                
                printf("Pasos: %d\tFichas: %d\tTotal: %d\n%s", moves, tokens, score, grid);
                
                input = getch();
                
                switch(input)
                {
                        case 'a':
                                moveply(-1);
                                break;
                                
                        case 'w':
                                moveply(-80);
                                break;
                                
                        case 'd':
                                moveply(1);
                                break;
                        
                        case 's':
                                moveply(80);
                                break;
                }
                
        }while(input != 'q');
        
        endgame(' ');
        
return 0;
}