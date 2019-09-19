
#include <stdlib.h> // for exit
#include <stdio.h>	// for sfont
#include <string.h>	// for sfont
#include <time.h>	// for srand

#include "gui.h"
#include "gfx.h"

#include "SDL.h"	// for sfont
#include "SFont.h"	// for sfont
#include "SDL/SDL_image.h"	// for sfont

#define XOFF 40
#define YOFF 40

#define BORDER_X 585
#define BORDER_Y 237
#define STEP_Y 49
#define STEP_X1 55
#define STEP_X2 100
#define DELTA_X 10
#define DELTA_Y 7

#define ATOM_X 576
#define ATOM_Y 20

#define MAX_BUTTON_GAME 5
#define MAX_BUTTON_MAIN 6

SDL_Surface *bauer[2], *laeufer[2], *dame[2], *koenig[2], *turm[2], *springer[2];
SDL_Surface *black, *white, *select_pic, *bomb, *board, *button, *button_light, *wood;
SDL_Surface *anim[15], *atom, *atom_rot;

//SDL_Surface *Font, *FontLarge;
SDL_Surface *Font;
SFont_FontInfo FontLargeData;
SFont_FontInfo *FontLarge=&FontLargeData;

SDL_Event event;
int animCounter = 0;
int selected = 0, select_x, select_y, select_old_x=0, select_old_y=0;
int fertig = 0;
long Now, delta_t;
zug_t *zug_p, tip_zug;
bool tip = FALSE;
bool random_position = FALSE;
bool rules = FALSE;
int mensch_farbe = 1;
bool tip_calculated = FALSE;
bool end_game;
int active_button = -1;
int fullscreen = 0;
int sound = 0;	// not implemented

int merke_computer;
int merke_mensch;
int merke_mensch_farbe;
int merke_tiefe;
bool merke_random_position;
zug_lesbar_t show_tip; 
zug_lesbar_t message;
bool allow_timer = TRUE;
bool letzter_zug_mensch = FALSE;
 
Uint32 GetPixel(SDL_Surface *Surface, Sint32 X, Sint32 Y);

void init_selected (void) {
    int x,y;
    
    select_x = 0;		
    select_y = 0;		
    if (spiel) {
	selected = 1;
    } else {
	selected = 0;
    }
    for (x=0; x<8; x++) {
	for (y=0; y<8; y++) {
	    if (brett[x+8*y] * (int) mensch == 6) {
		select_x = x;		
		select_y = y;		
		return;
	    }
	}
    }
}

void mensch_zug(brett_t *brett_p, zug_t *zug_p_new )
{

    init_selected();
    fertig = 0;
    tip_calculated = FALSE;
    SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
    SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_ENABLE);
    
    zug_p = zug_p_new;

    brett_anzeigen(TRUE, TRUE);

    while (!fertig) {
	SDL_WaitEvent(&event);
    }

    strcpy(message,"");
    SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    sprintf(show_tip, "%s", "");

    tip = FALSE;
}

SDL_Surface *figur_pic(int figur) {

    int farbe = 1;
	    
    if (figur < 0) {
	figur = -figur;
	farbe = 0;
    }

    switch ( figur ) {
	case 1:
	    return bauer[farbe];
	break;
	case 2:
	    return laeufer[farbe];
	break;
	case 3:
	    return springer[farbe];
	break;
	case 4:
	    return turm[farbe];
	break;
	case 5:
	    return dame[farbe];
	break;
	case 6:
	    return koenig[farbe];
	break;
	default:
	    return NULL;
	break;
    }
}

void show_border (bool spiel, bool update) {
    int j;
    bool merke_allow_timer = allow_timer;
    
    allow_timer = FALSE;
    if (spiel) { // waehrend des Spiels
	int i;
	zug_lesbar_t punkte_string;
	
	for (i=0; history[i].von_x != -1; i++) {}; i--;
	
	/* Anzeige */ 
	// Punkte anzeigen
	PutString(Screen,BORDER_X,BORDER_Y,"points");
	if (!fertig) {
	    if (i >= 0) {
		sprintf(punkte_string, "%.0f", bewertung_float(history[i].punkte, mensch_farbe, tiefe));
		PutString(Screen,BORDER_X+STEP_X2,BORDER_Y, punkte_string);
	    }
	}
	
	// letzten Zug anzeigen
	PutString(Screen,BORDER_X,BORDER_Y+1*STEP_Y,"last");
	if (i >= 0) {
	    PutString(Screen,BORDER_X+STEP_X1,BORDER_Y+1*STEP_Y,history[i].zuglesbar);
	}

	/* buttons */
	for (j=2; j<=6; j++) {
	    if (j == active_button) {
		Blit(BORDER_X-DELTA_X, BORDER_Y+j*STEP_Y-DELTA_Y, button);
	    } else {
		Blit(BORDER_X-DELTA_X, BORDER_Y+j*STEP_Y-DELTA_Y, button_light);
	    }
	}

	// Tipp anzeigen
        PutString(Screen,BORDER_X,BORDER_Y+2*STEP_Y,"hint");
	if (tip) {
	    PutString(Screen,BORDER_X+STEP_X1,BORDER_Y+2*STEP_Y,show_tip);
	}
	
	// back
	PutString(Screen,BORDER_X,BORDER_Y+3*STEP_Y,"back");

	// change color
	PutString(Screen,BORDER_X,BORDER_Y+4*STEP_Y,"change color");

	// restart game
	PutString(Screen,BORDER_X,BORDER_Y+5*STEP_Y,"restart game");

	// main menu
	PutString(Screen,BORDER_X,BORDER_Y+6*STEP_Y,"main menu");

    } else { // vor dem Spiel
	zug_lesbar_t text;
		
	/* buttons */
	for (j=1; j<=6; j++) {
	    if (j == active_button) {
		Blit(BORDER_X-DELTA_X, BORDER_Y+j*STEP_Y-DELTA_Y, button);
	    } else {
		Blit(BORDER_X-DELTA_X, BORDER_Y+j*STEP_Y-DELTA_Y, button_light);
	    }
	}

	// color
	sprintf(text, "%s %s", "color", string_farbname(mensch));
	PutString(Screen,BORDER_X,BORDER_Y+1*STEP_Y,text);

	// difficulty
	sprintf(text, "%s %d", "difficulty", tiefe);
	PutString(Screen,BORDER_X,BORDER_Y+2*STEP_Y, text);

	// random position
	if (random_position) {
	    PutString(Screen,BORDER_X,BORDER_Y+3*STEP_Y,"type random");
	} else {
	    PutString(Screen,BORDER_X,BORDER_Y+3*STEP_Y,"type standard");
	}
	
	// rules
	PutString(Screen,BORDER_X,BORDER_Y+4*STEP_Y,"rules");

	// start
	PutString(Screen,BORDER_X,BORDER_Y+5*STEP_Y,"start");

	// exit
	PutString(Screen,BORDER_X,BORDER_Y+6*STEP_Y,"exit");
    }
    
    if (update)
	Update();    

    allow_timer = merke_allow_timer;
}


void game_over (zug_lesbar_t end_text) {
    int i;

    SDL_Delay(1000);
    strcpy(message, end_text);
    brett_anzeigen(TRUE, TRUE);

    end_game = TRUE;
    SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
    SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_ENABLE);

    printf("\n");
    if (mensch_farbe == 1) {
	printf(" human      : %s\n", "white");
    } else {
	printf(" human      : %s\n", "black");
    }
    printf(" difficulty : %d\n", tiefe);
    if (random_position) {
	printf(" type       : %s\n", "random");
    } else {
	printf(" type       : %s\n", "standard");
    }
    printf("    | white    | black   \n");
    printf(" ---|----------+----------\n");
    for (i=0; history[i].von_x != -1; i++) {
	if (i%2) {
	    printf(" %s\n", history[i].zuglesbar);
	} else {
	    printf(" %2d | %s |",(i/2)+1 , history[i].zuglesbar);
	}
	// PutString(Screen,XOFF,YOFF+i*STEP_Y,history[i].zuglesbar);
    }
    //printf(" %d half turns\n", i);
    if (i%2) {
	printf("\n");
    }
    printf(" %s\n",end_text);
    printf("\n");

    while (1) {
	SDL_WaitEvent(&event);
    }

}

void brett_anzeigen (bool update, bool border)
{
    int x,y;

    allow_timer = FALSE;
    if (rules) {
	Blit(0, 0, wood);
	PutString(Screen,2*XOFF, 2*YOFF,"1. The known chess rules are valid.");
	PutString(Screen,2*XOFF, 3*YOFF,"2. Whenever a piece is captured ");
	PutString(Screen,3*XOFF, 4*YOFF,"both pieces and all pieces on");
	PutString(Screen,3*XOFF, 5*YOFF,"neighbour fields die.");
	PutString(Screen,2*XOFF, 6*YOFF,"3. There is no en passent capturing.");
	PutString(Screen,2*XOFF, 7*YOFF,"4. When a pawn reaches the last rank");
	PutString(Screen,3*XOFF, 8*YOFF,"he becomes a queen.");
	PutString(Screen,2*XOFF, 9*YOFF,"5. Castling is allowed if the king");
	PutString(Screen,3*XOFF,10*YOFF,"and the rook are on their start");
	PutString(Screen,3*XOFF,11*YOFF,"position and there are no pieces");
	PutString(Screen,3*XOFF,12*YOFF,"between them.");
    } else {

	zug_t dummy_zug;
    
	// male Untergrund    
	Blit(0, 0, board);
	if (!fertig && (im_schach(mensch_farbe, &brett, &dummy_zug) == 1)) {
	    Blit(ATOM_X, ATOM_Y, atom_rot);
	} else {
	    Blit(ATOM_X, ATOM_Y, atom);
	}

	
	// male Figuren		
	for (x=0; x<8; x++) {
	    for (y=0; y<8; y++) {
		int figur = brett[x + y*8];

    		if (selected && select_x == x && select_y == y) {
		    Blit(x * black->w + XOFF, y * black->h + YOFF, select_pic);
		}

		if (figur != 0) {
		    Blit(x * black->w + XOFF, y * black->h+YOFF, figur_pic(figur) );
		}
	    }
	}

    }

    if (border) {
	show_border(spiel, FALSE);
    }

    if (update && strcmp(message,"") != 0) {
	PutString2(Screen, FontLarge, 4*black->w+XOFF-(int)(TextWidth(message)/2*1.4), 4*black->h+(int)(YOFF-FontLarge->h/2), message);
    }
    
    if (update) {
	Update();    
    }

    allow_timer = TRUE;
}

void show_selected_piece (int x, int y, int x_old, int y_old) {
    // male Figuren		
    int figur;

    SDL_Rect blitrects[2];

    if (x >= 0 && x < 8 && y >= 0 && y < 8 && x_old >= 0 && x_old < 8 && y_old >= 0 && y_old < 8) {
	allow_timer = FALSE;
	// new selected
	blitrects[0].x = x * black->w + XOFF;
	blitrects[0].y = y * black->h + YOFF;
	blitrects[0].w = black->w;
	blitrects[0].h = black->h;

	// old selected
	blitrects[1].x = x_old * black->w + XOFF;
	blitrects[1].y = y_old * black->h + YOFF;
	blitrects[1].w = black->w;
	blitrects[1].h = black->h;

	SDL_BlitSurface(board, &blitrects[0], Screen, &blitrects[0]);
	SDL_BlitSurface(board, &blitrects[1], Screen, &blitrects[1]);

	SDL_BlitSurface(select_pic, NULL, Screen, &blitrects[0]);

	figur = brett[x + y*8];
	if (figur != 0) {
	    SDL_BlitSurface(figur_pic(figur), NULL, Screen, &blitrects[0]);
	}
	figur = brett[x_old + y_old*8];
	if (figur != 0) {
	    SDL_BlitSurface(figur_pic(figur), NULL, Screen, &blitrects[1]);
	}

	SDL_UpdateRects(Screen, 2, blitrects);

	allow_timer = TRUE;
    } else {
	printf("ERROR: Falsche Koordinaten: neu %d %d alt %d %d\n", x, y, x_old, y_old);
	exit(1);
    }
}

void Putpix(SDL_Surface *surf, Sint32 X, Sint32 Y, Uint32 pixel) {
    Uint8   *bits, bpp;

    /* Calculate the framebuffer offset of the center of the surf */
    if ( SDL_MUSTLOCK(surf) ) {
        if ( SDL_LockSurface(surf) < 0 )
            return;
    }
    bpp = surf->format->BytesPerPixel;
    bits = ((Uint8 *)surf->pixels)+Y*surf->pitch+X*bpp;

    /* Set the pixel */
    switch(bpp) {
        case 1:
            *((Uint8 *)(bits)) = (Uint8)pixel;
            break;
        case 2:
            *((Uint16 *)(bits)) = (Uint16)pixel;
            break;
        case 3: { /* Format/endian independent */
            Uint8 r, g, b;

            r = (pixel>>surf->format->Rshift)&0xFF;
            g = (pixel>>surf->format->Gshift)&0xFF;
            b = (pixel>>surf->format->Bshift)&0xFF;
            *((bits)+surf->format->Rshift/8) = r; 
            *((bits)+surf->format->Gshift/8) = g;
            *((bits)+surf->format->Bshift/8) = b;
            }
            break;
        case 4:
            *((Uint32 *)(bits)) = (Uint32)pixel;
            break;
    }

    /* Update the display */
    if ( SDL_MUSTLOCK(surf) ) {
        SDL_UnlockSurface(surf);
    }
    SDL_UpdateRect(surf, X, Y, 1, 1);

    return;
}

SDL_Surface *White( SDL_Surface *surf)
{
    Uint8 red,green,blue,alpha;
    Uint32 x,y,pixel;
    float value;

    surf = SDL_DisplayFormatAlpha(surf);
    lock(surf);
    for ( x=0; x<surf->w ; x++) {
	for ( y=0; y<surf->h ; y++) {
	    pixel = GetPixel( surf, x, y);
	    SDL_GetRGBA( pixel, surf->format, &red, &green, &blue, &alpha);
	    value = (255*3 - red - green - blue) / 3;
	    pixel = SDL_MapRGBA( surf->format, value, value, value, alpha);
	    Putpix( surf, x, y, pixel);
	}
    }
    unlock(surf);
    return surf;
}

void load_images()
{
    int i;

    black 	= LoadImage("black.png");
    white 	= LoadImage("white.png");
    select_pic 	= LoadImage("select.png");
    board 	= LoadImage("board.png");
    wood 	= LoadImage("wood.png");
    button 	= LoadImage("button.png");
    button_light= LoadImage("button-light.png");

    bauer[0] 	= LoadImage("bauer.png");
    bauer[1] 	= White(bauer[0]);
    laeufer[0] 	= LoadImage("laeufer.png");
    laeufer[1] 	= White(laeufer[0]);
    koenig[0] 	= LoadImage("koenig.png");
    koenig[1]	= White(koenig[0]);
    dame[0] 	= LoadImage("dame.png");
    dame[1] 	= White(dame[0]);
    turm[0] 	= LoadImage("turm.png");
    turm[1] 	= White(turm[0]);
    springer[0] = LoadImage("springer.png");
    springer[1] = White(springer[0]);

    atom 	= LoadImage("atom.png");
    atom_rot 	= LoadImage("atom-rot.png");
    
    for (i=0; i<15; i++) {
	char string[100];
    
	sprintf(string, "anim%d.png", i+1);
	anim[i] = LoadImage(string);
    }
}

void mouse_click(int x, int y)
{
    if (brett[x + 8*y] * (int) mensch > 0) {
	selected = 1;
	select_x = x;
	select_y = y;
    } else {	// Nicht auf unsere Figur geclickt
	if (selected) {
	    if (legal(select_x, select_y, x, y, &brett)) {
		selected = 0;

		zug_p->von_x = select_x;
		zug_p->von_y = select_y;
		zug_p->nach_x = x;
		zug_p->nach_y = y;		
		
		fertig = 1;
	    }
	}
    }
}

float symrandom (void) {
    // returns random float between -1 and +1
    float random;
    
    random = 2* ((float) rand()) / ((float) RAND_MAX) - 1; 
    return random;
}

int rowrandom (void) {
    // returns random int between 0 and 7
    int random;
    
    random = 8* ((float) rand()) / ((float) RAND_MAX); 
    return random;
}

/***********************/
/* waehrend des Spiels */
/***********************/

void button_tip (void) {
    int i;

    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
    
    if (end_game) {
	return;
    }
    
    tip = TRUE;

    if (!tip_calculated) {
	int punkte_dummy;
	int start_tiefe;
	zug_t zug; 
			
	tip_calculated = TRUE;

	start_tiefe = tiefe+2;
	if (start_tiefe < MAX_TIEFE-2) {
	    start_tiefe = MAX_TIEFE-2;
	}
	if (start_tiefe > MAX_TIEFE) {
	    start_tiefe = MAX_TIEFE+1;
	}
	computer_zug(mensch_farbe, start_tiefe, &brett, &zug, &punkte_dummy, FALSE);

	zuglesbar_func(&zug, -mensch_farbe, &brett, show_tip); // to do: hier farbe richtig (scheint so!)
	allow_timer = FALSE;
	PutString(Screen,BORDER_X+STEP_X1,BORDER_Y+2*STEP_Y,show_tip);
	allow_timer = TRUE;

	tip_zug.von_x 	= zug.von_x;
	tip_zug.von_y 	= zug.von_y;
	tip_zug.nach_x 	= zug.nach_x;
	tip_zug.nach_y	= zug.nach_y;
    }

    selected = 1;

    for (i=0; i<2; i++) {
    select_x = tip_zug.von_x;
    select_y = tip_zug.von_y;
    brett_anzeigen(TRUE, TRUE);
    SDL_Delay(350);

    select_x = tip_zug.nach_x;
    select_y = tip_zug.nach_y;
    brett_anzeigen(TRUE, TRUE);
    SDL_Delay(350);
    }
    
    select_x = tip_zug.von_x;
    select_y = tip_zug.von_y;
    brett_anzeigen(TRUE, TRUE);

    SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
    SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_ENABLE);
}

void button_back (void) {
    int i;
    zug_t zug;
    bool letzter_zug_mensch_und_ende = FALSE;

    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);

    if (end_game && letzter_zug_mensch) {
	letzter_zug_mensch_und_ende = TRUE;
    }

    for (i=0; history[i].von_x != -1; i++) {};

    if ((!letzter_zug_mensch_und_ende && i > 1) || (letzter_zug_mensch_und_ende && i > 2)) {
	sprintf(message, "%s", "");
	end_game = FALSE;
	tip = FALSE;
	tip_calculated = FALSE;

	history[i-1].von_x = -1;
	history[i-2].von_x = -1;
	if (letzter_zug_mensch_und_ende) {
	    history[i-3].von_x = -1;
	}

        // Fade start
	allow_timer = FALSE;
	brett_anzeigen(TRUE, TRUE);
	SDL_BlitSurface(Screen, NULL, FadeBuffer, NULL);
	Blit(0, 0, board);

	for (i=0; i<64; i++) {
    	    brett[i] = merke_brett[i];
	}
    
	for (i=0; history[i].von_x != -1; i++) {
	    zug.von_x = history[i].von_x;
	    zug.von_y = history[i].von_y;
	    zug.nach_x = history[i].nach_x;
	    zug.nach_y = history[i].nach_y;
    	    anwenden(&brett, &zug);
	}
	init_selected();
	brett_anzeigen(FALSE, TRUE);

	BlitToBB(0,0,Screen);
	SDL_BlitSurface(FadeBuffer, NULL, Screen, NULL);
	FadeScreen(2);
	allow_timer = TRUE;
	// Fade end

	// to do: kommt das hier rein oder nicht ???
	sub_main(mensch_farbe, tiefe, &brett); // to do: ist dies wirklich richtig ???
    }
    
    SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
    SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_ENABLE);
}

void button_change_color (void) {

    if (end_game) {
	return;
    }
    
    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);

    computer *= -1;
    mensch *= -1;
    mensch_farbe *= -1;
    fertig = 1;

    sprintf(message, "you are %s now", string_farbname(mensch));
    brett_anzeigen(TRUE, TRUE);
    SDL_Delay(1000);

    sprintf(message, "%s", "");
    brett_anzeigen(TRUE, TRUE);

    SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
    SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_ENABLE);
}

void button_main_menu (void) {
    int i;

    tip = FALSE;
    spiel = FALSE;
    tip_calculated = FALSE;
    rules = FALSE;

    strcpy(show_tip,"");                                                                                                                       
    sprintf(message, "%s", "");

    for (i=0; i<64; i++) {
	brett[i] = merke_brett[i];
    }

    computer 		= merke_computer;
    mensch 		= merke_mensch;
    mensch_farbe 	= merke_mensch_farbe;
    tiefe		= merke_tiefe;
    random_position 	= merke_random_position;

    init_selected();
    brett_anzeigen(TRUE, TRUE);
}

void button_restart_game (void) {
    int i;

    end_game = FALSE;
    tip_calculated = FALSE;
    for (i=0; i<64; i++) {
    }

    for (i=0; i<64; i++) {
	brett[i] = merke_brett[i];
    }

    spiel 	= TRUE;
    tip 	= FALSE;
    rules 	= FALSE;
    history[0].von_x = -1;
    allow_timer = TRUE;
    
    computer 		= merke_computer;
    mensch 		= merke_mensch;
    mensch_farbe 	= merke_mensch_farbe;
    tiefe		= merke_tiefe;
    random_position 	= merke_random_position;

    strcpy(show_tip,"");
    sprintf(message, "%s", "");

    init_selected();
    brett_anzeigen(TRUE, TRUE);

    sub_main(weiss, tiefe, &brett); // weiss faengt an
}

/*****************/
/* vor dem Spiel */
/*****************/

void button_color (void) {
    computer *= -1;
    mensch *= -1;
    mensch_farbe *= -1;

    rules = FALSE;

    brett_anzeigen(TRUE, TRUE);
}

void button_difficulty (bool left) {

    if (left) {
	tiefe++;

	if (tiefe > MAX_TIEFE) {
	    tiefe = 0;
	}
    } else {
	tiefe--;

	if (tiefe < 0) {
	    tiefe = MAX_TIEFE;
	}
    }

    rules = FALSE;

    brett_anzeigen(TRUE, TRUE);
}

void button_random_position (void) {
    int pos1, pos2, tmp, i;	

    rules = FALSE;

    if (!random_position) {
	random_position = TRUE;
	for (i=0; i < 100; i++) {
	    pos1 = rowrandom();
	    pos2 = rowrandom();
	    tmp = brett[pos1+8*0];
	    brett[pos1+8*0] = brett[pos2+8*0];
	    brett[pos2+8*0] = tmp;
	}    

	for (i=0; i < 100; i++) {
	    pos1 = rowrandom();
	    pos2 = rowrandom();
	    tmp = brett[pos1+8*7];
	    brett[pos1+8*7] = brett[pos2+8*7];
	    brett[pos2+8*7] = tmp;
	}    
    } else {
	random_position = FALSE;

	brett[0+8*0] = -4;
	brett[1+8*0] = -3;
	brett[2+8*0] = -2;
	brett[3+8*0] = -5;
	brett[4+8*0] = -6;
	brett[5+8*0] = -2;
	brett[6+8*0] = -3;
	brett[7+8*0] = -4;

	brett[0+8*7] = 4;
	brett[1+8*7] = 3;
	brett[2+8*7] = 2;
	brett[3+8*7] = 5;
	brett[4+8*7] = 6;
	brett[5+8*7] = 2;
	brett[6+8*7] = 3;
	brett[7+8*7] = 4;
    }

    brett_anzeigen(TRUE, TRUE);
}

void button_rules (void) {
    if (!rules) {
	rules = TRUE;
    } else {
	rules = FALSE;
    }    

    brett_anzeigen(TRUE, TRUE);
}

void button_start_game (void) {
    int i;

    end_game = FALSE;
    tip = FALSE;
    spiel = TRUE;
    rules = FALSE;
    allow_timer = TRUE;

    for (i=0; i<64; i++) {
	merke_brett[i] = brett[i];
    }

    merke_computer 	= computer;
    merke_mensch 	= mensch;
    merke_mensch_farbe 	= mensch_farbe;
    merke_tiefe		= tiefe;
    merke_random_position = random_position;

    history[0].von_x = -1;

    sprintf(message, "%s", "");
    init_selected();
    brett_anzeigen(TRUE, TRUE);
    sub_main(weiss, tiefe, &brett);	// weiss faengt an
}

void button_exit (void) {

    tip = FALSE;
    spiel = FALSE;
    rules = FALSE;

    // Fade start
    allow_timer = FALSE;
    brett_anzeigen(TRUE, TRUE);
    SDL_BlitSurface(Screen, NULL, FadeBuffer, NULL);
    Blit(0, 0, board);
    BlitToBB(0,0,Screen);
    SDL_BlitSurface(FadeBuffer, NULL, Screen, NULL);
    FadeScreen(2);
    allow_timer = TRUE;
    // Fade end
    
    exit(0);
}

int get_button_nr (int x, int y) {
    int button_nr = -1;
    int max_button = 0;
    
    if (spiel) {
	max_button = MAX_BUTTON_GAME;
    } else {
	max_button = MAX_BUTTON_MAIN;
    }
    
    if (x > BORDER_X-DELTA_X && x < BORDER_X-DELTA_X+button->w) {
	for (button_nr = 0; button_nr < max_button; button_nr++) {	 
	    if (y > BORDER_Y+(7-max_button+button_nr)*STEP_Y-DELTA_Y && y < BORDER_Y+(7-max_button+button_nr)*STEP_Y-DELTA_Y + button->h) {
		break;
	    }
	}
    }

    return (button_nr);
}

void menu(int x, int y, bool left) {
    int button_nr;

    button_nr = get_button_nr(x, y);

    if (spiel) {
	switch (button_nr) {
	    case 0:
		if (left)
		    button_tip();
	    break;
	    case 1:
		if (left)
	    	    button_back();
	    break;
	    case 2:
		if (left)
		    button_change_color();
	    break;
	    case 3:
		if (left)
		    button_restart_game();
	    break;
	    case 4:
		if (left)
		    button_main_menu();
	    break;
	}
    } else {
	switch (button_nr) {
	    case 0:
		if (left)
		    button_color();
	    break;
	    case 1:
		    button_difficulty(left);
	    break;
	    case 2:
		if (left)
		    button_random_position();
	    break;
	    case 3:
		if (left)
		    button_rules();
	    break;
	    case 4:
		if (left)
		    button_start_game();
	    break;
	    case 5:
		if (left)
		    button_exit();
	    break;
	}
    
    }
}

int HandleEvents(const SDL_Event *event) {
    int x, y, active_button_old;

    switch (event->type) {
	case SDL_QUIT:
	    exit(0);
	break;
	case SDL_MOUSEBUTTONDOWN:
	    switch((event->button).button) {
		case SDL_BUTTON_LEFT:
		    SDL_GetMouseState(&x, &y);
		    if (x < 8*black->w + XOFF && y < 8*black->h +YOFF) {
			if (spiel) {
			    mouse_click((x-XOFF) / black->w, (y-YOFF) / black->h);
			    brett_anzeigen(TRUE, TRUE);
			}
		    } else {
			menu(x, y, TRUE);
		    }
		break;
		case SDL_BUTTON_RIGHT:
		    SDL_GetMouseState(&x, &y);
		    if (x < 8*black->w + XOFF && y < 8*black->h +YOFF) {
			if (spiel) {
			    // do nothing
			} else {
			    // do nothing
			}
		    } else {
			menu(x, y, FALSE);
		    }
		break;
	    }
	break;
	case SDL_MOUSEMOTION:
	    active_button_old = active_button;
	    SDL_GetMouseState(&x, &y);

	    active_button = get_button_nr(x, y);
	    if (spiel) {
		active_button += 2;
	    } else {
		active_button += 1;
	    }
	
	    if (active_button != active_button_old) {
		SDL_Rect rect;
	    
		SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
		
		rect.x = BORDER_X - DELTA_X;
		rect.y = BORDER_Y;
		rect.w = Screen->w - rect.x;
		rect.h = Screen->h - rect.y;

		allow_timer = FALSE;
		SDL_BlitSurface(board,& rect, Screen, &rect);
		allow_timer = TRUE;

		show_border(spiel, TRUE);

		SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
	    }    

	break;
    }

    return 1;
}

void get_delta_t() {
    delta_t = SDL_GetTicks() - Now;
    while (delta_t < 10) {
	SDL_Delay(1);
	delta_t = SDL_GetTicks() - Now;
    }
    Now = SDL_GetTicks();
}

void zeige_zug(zug_t *zug) {
    float x = zug->von_x * black->w, y = zug->von_y * black->h;
    brett_t old_brett;
    int figur = brett[zug->von_x + 8*zug->von_y];
    SDL_Rect rect;
    int pos_x, pos_y;
    int schlagende_figur = abs(brett[zug->von_x + 8*zug->von_y]);    
    int i;

    // altes Brett sichern
    for (i=0; i<64; i++) {
	old_brett[i] = brett[i];
    }
    
    // figur loeschen
    rect.w = rect.h = figur_pic(figur)->w;
    brett[zug->von_x + 8*zug->von_y] = 0;
    brett_anzeigen(FALSE, TRUE);

    allow_timer = FALSE;
    BlitToBB(0,0,Screen);
    Now = SDL_GetTicks();

    // bewegen
    while ( abs((int)x - zug->von_x * black->w) <= abs(zug->nach_x - zug->von_x) * black->w &&
	    abs((int)y - zug->von_y * black->h) <= abs(zug->nach_y - zug->von_y) * black->h )
    {	
	Blit( (int)x+XOFF, (int)y+YOFF, figur_pic(figur) );
	Update();
	rect.x = x + XOFF;
	rect.y = y + YOFF;
	SDL_BlitSurface(BackBuffer, &rect, Screen, &rect);
	AddThisRect(rect);
	get_delta_t();
	x += (zug->nach_x - zug->von_x) * 0.1 * delta_t;
	y += (zug->nach_y - zug->von_y) * 0.1 * delta_t;
    }

    if (brett[zug->nach_x + 8*zug->nach_y] != 0) {
	// Explosion?

	// Beben
	int quake, quake_max, index;
	umgebung_t *uref_p;	

	quake_max = schlagende_figur+3; // zwischen 1 und 16
        uref_p = &(umgebung_liste[zug->nach_x + 8*zug->nach_y]);
        for (index=0 ; (*uref_p)[index] != -1 ; index++) {
	    quake_max += abs(brett[(*uref_p)[index]]);
	}
	if (quake_max > black->w) {
	    quake_max = black->w;
	}
	
	if (quake_max == 0) {
	    printf("quake_max == 0\n");
	}

	rect.x = 600;
	rect.y = 600;

	for (quake = 0 ; quake < quake_max; quake++) {
	    Blit(0, 0, board);
	    Blit(ATOM_X, ATOM_Y, atom);

	    show_border(spiel, FALSE);

	    for (pos_x=0; pos_x<8; pos_x++) {
		for (pos_y=0; pos_y<8; pos_y++) {
		    int figur = brett[pos_x + pos_y*8];
		    int vol = quake_max-abs(pos_x - zug->nach_x)-abs(pos_y - zug->nach_y); 
		    
		    if (vol < 0) vol = 0;
		    // male Untergrund    

		    // male Figuren		
		    if (figur != 0) {
			int vol_x, vol_y;

			float zeitfaktor = (1-((float) quake)/((float) quake_max));
			zeitfaktor = zeitfaktor*zeitfaktor;
		        vol_x = vol * symrandom() * zeitfaktor;
		        vol_y = vol * symrandom() * zeitfaktor;
			if (abs(pos_x - zug->nach_x) > 1 || abs(pos_y - zug->nach_y) > 1) {
			    vol_x = 0;
			    vol_y = 0;
			}

			if (pos_x == 7 && vol_x > 0) {
			    vol_x = 0;
			} else if (pos_x == 0 && vol_x < 0) {
			    vol_x = 0;
			}

			if (pos_y == 7 && vol_y > 0) {
			    vol_y = 0;
			} else if (pos_y == 0 && vol_y < 0) {
			    vol_y = 0;
			}
			Blit(pos_x * black->w + XOFF + vol_x, pos_y * black->h + YOFF + vol_y, figur_pic(figur) );
		    }
		}
	    }
	    get_delta_t();
	    Update();    
	}
	
	for (x=-1; x<=1; x++) {
	    for (y=-1; y<=1; y++) {
		int feld_x = zug->nach_x + x;
		int feld_y = zug->nach_y + y;
		
		if ( (feld_x >= 0 && feld_x < 8) && (feld_y >= 0 && feld_y < 8) )
		    brett[feld_x + 8*feld_y] = 0;
	    }
	}

	// Fade start
	SDL_BlitSurface(Screen, &rect, FadeBuffer, &rect);
	BlitToBB(0,0,Screen);
	SDL_BlitSurface(FadeBuffer, &rect, Screen, &rect);
	FadeScreen(3);
	// Fade end
    }
    
    for (i=0; i<64; i++)
	brett[i] = old_brett[i];

    allow_timer = TRUE;
}

Uint32 think (Uint32 interval, void *dummy) {

    SDL_Rect rect;
    
    if (allow_timer) {
	rect.x = ATOM_X;
	rect.y = ATOM_Y;
	rect.w = anim[animCounter]->w;
	rect.h = anim[animCounter]->h;
    
	SDL_BlitSurface(board, &rect, Screen, &rect);
	Blit(ATOM_X, ATOM_Y, anim[animCounter++]);

	Update();	

	animCounter %= 15;
    
	SDL_PollEvent(&event);
    }
    
    return 50;
}

void ReadCommandLine(char *argv[]) {
    int i;
    for (i=1; argv[i]; i++) {
	if 	((strcmp(argv[i],"--windowed")  ==0)||(strcmp(argv[i],"-w")==0)) fullscreen=0;
	else if	((strcmp(argv[i],"--fullscreen")==0)||(strcmp(argv[i],"-f")==0)) fullscreen=1;
	else if ((strcmp(argv[i],"--nosound")   ==0)||(strcmp(argv[i],"-s")==0)) sound=0;
	else printf("Unknown parameter: \"%s\" \n", argv[i]);
    }
}

void init_gui() {
    init_SDL(fullscreen); // fullscreen?
    load_images();

    sprintf(message, "%s", "");
    srand( (unsigned) time(NULL) );

    // Load the font - You don't have to use the IMGlib for this
    Font 		= LoadImage("SmallStone.png");
    FontLarge->Surface 	= LoadImage("StoneFont.png");

    // Prepare the font for use
    InitFont(Font);
    InitFont2(FontLarge);

    // ganz am Schluss !!!
    SDL_SetEventFilter(HandleEvents);
}
