#ifndef GUI_H
#define GUI_H

#include <SDL.h> 
#include "atomschach.h"

extern void ReadCommandLine(char *argv[]);
extern void init_gui();
extern void brett_anzeigen(bool update, bool border);
extern void mensch_zug(brett_t *brett_p, zug_t *zug_p );
extern void zeige_zug(zug_t *zug_p);
extern void game_over (zug_lesbar_t end_text);
extern void brett_anzeigen (bool update, bool border);
extern Uint32 think(Uint32 interval, void *dummy);
extern void show_selected_piece (int x, int y, int x_old, int y_old);

extern zug_lesbar_t message;
extern bool random_position;
extern bool rules;
extern zug_lesbar_t show_tip;
extern int selected, select_x, select_y, select_old_x, select_old_y;
extern SDL_Event event;
extern bool letzter_zug_mensch;

#endif
