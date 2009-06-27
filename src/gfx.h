#ifndef GFX_H
#define GFX_H

#include <SDL.h>
extern SDL_Surface *Screen,*BackBuffer,*FadeBuffer;
extern SDL_Rect blitrect;
extern char DATAPATH[200];

// inits some SDL stuff
void init_SDL(int fullscreen);

// Load an image
SDL_Surface *LoadImage(char *datafile);

// Blit an image
void Blit(int Xpos,int Ypos,SDL_Surface *image);
void BlitToBB(int Xpos,int Ypos,SDL_Surface *image);   //blits to BackBuffer

// Updates all blitted rects
void Update();
void FullUpdate();

// Puts a pixel (lock and unlock needed!)
extern int PutPixel(SDL_Surface *Surface, Sint32 X, Sint32 Y, Uint32 Color);
void lock();
void unlock();

// Add rect to update queue
void AddRect(int x1, int y1, int x2, int y2);
void AddThisRect(SDL_Rect blitrect);

// returns the flipped surface
SDL_Surface *FlippedSurface(SDL_Surface *Surface);

// fades screen to BackBuffer
void FadeScreen(float speed);

#endif
