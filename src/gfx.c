#include "SDL_image/SDL_image.h"
#include <stdlib.h>

#define PATHNUM 5
#define BPP 0
#define RECTS_NUM 300

#ifndef DATADIR
#define DATADIR "."
#endif
char DATAPATH[200]=DATADIR;
const char PATH[PATHNUM][200]={DATADIR,".","../","Nuclear Chess.app/Contents/Resources",DATADIR};SDL_Surface *Screen,*BackBuffer,*FadeBuffer;
SDL_Rect blitrect,blitrects[RECTS_NUM];
int blitrects_num = 0;
int NoAlpha = 0;

void Update()
{
  SDL_UpdateRects(Screen, blitrects_num, blitrects);
  blitrects_num=0;
}

void AddThisRect(SDL_Rect blitrect)
{
    blitrects[blitrects_num]=blitrect;
    if (++blitrects_num>=RECTS_NUM-2)
        {printf("Too many blits!\n");blitrects_num--;Update();}
}

void ComplainAndExit(void)
{
        fprintf(stderr, "Problem: %s\n", SDL_GetError());
        exit(1);
}

int (*_PutPixel)(SDL_Surface *Surface, Sint32 X, Sint32 Y, Uint32 Color);

int fast_putpixel1(SDL_Surface *Surface, Sint32 X, Sint32 Y, Uint32 Color)
{
  if (X < 0 || X > Surface->w || Y < 0 || Y > Surface->h) 
    return -1;

  *((Uint8 *)Surface->pixels + Y * Surface->pitch + X) = Color;

  return 0;
}

int fast_putpixel2(SDL_Surface *Surface, Sint32 X, Sint32 Y, Uint32 Color)
{
  if (X < 0 || X > Surface->w || Y < 0 || Y > Surface->h) 
    return -1;

 *((Uint16 *)Surface->pixels + Y * Surface->pitch/2 + X) = Color;

  return 0;
}

int fast_putpixel3(SDL_Surface *Surface, Sint32 X, Sint32 Y, Uint32 Color)
{
  Uint8 *pix;
  int shift;

  if (X < 0 || X > Surface->w || Y < 0 || Y > Surface->h) 
    return -1;

  /* Gack - slow, but endian correct */
  pix = (Uint8 *)Surface->pixels + Y * Surface->pitch + X*3;
  shift = Surface->format->Rshift;
  *(pix+shift/8) = Color>>shift;
  shift = Surface->format->Gshift;
  *(pix+shift/8) = Color>>shift;
  shift = Surface->format->Bshift;
  *(pix+shift/8) = Color>>shift;

  return 0;
}

int fast_putpixel4(SDL_Surface *Surface, Sint32 X, Sint32 Y, Uint32 Color)
{
  if (X < 0 || X > Surface->w || Y < 0 || Y > Surface->h) 
    return -1;

  *((Uint32 *)Surface->pixels + Y * Surface->pitch/4 + X) = Color;

  return 0;
}

void init_SDL(int fullscreen)  // sets the video mode
{
  int bpp=BPP,flags=0;

  //  if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0 ) {ComplainAndExit();}

  // puts("** Init video **");
  if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) ComplainAndExit();

  // puts("** Init timer **");
  if ( SDL_Init(SDL_INIT_TIMER) < 0 ) ComplainAndExit();

  atexit(SDL_Quit);

  // Set the video mode (800x600 at 16-bit depth)  
  if (fullscreen)  
    Screen = SDL_SetVideoMode(800, 600, bpp, SDL_FULLSCREEN | flags);
  else 
    { Screen = SDL_SetVideoMode(800, 600, bpp, flags); }
  if ( Screen == NULL ) {ComplainAndExit();}

  // create BackBuffer
  BackBuffer = SDL_AllocSurface(Screen->flags,
                               800,
                               600,
                               Screen->format->BitsPerPixel,
                               Screen->format->Rmask,
                               Screen->format->Gmask,
                               Screen->format->Bmask, 0);
  if (BackBuffer == NULL)
  printf("ERROR: Couldn't create BackBuffer: %s\n", SDL_GetError());
  FadeBuffer = SDL_AllocSurface(Screen->flags,
                               800,
                               600,
                               Screen->format->BitsPerPixel,
                               Screen->format->Rmask,
                               Screen->format->Gmask,
                               Screen->format->Bmask, 0);
  if (FadeBuffer == NULL)
  printf("ERROR: Couldn't create FadeBuffer: %s\n", SDL_GetError());

  // Figure out what putpixel routine to use
  switch (Screen->format->BytesPerPixel)
   {
    case 1:
      _PutPixel = fast_putpixel1;
      break;
    case 2:
      _PutPixel = fast_putpixel2;
      break;
    case 3:
      _PutPixel = fast_putpixel3;
      break;
    case 4:
      _PutPixel = fast_putpixel4;
      break;
   }
}

void lock()
{
        if ( SDL_MUSTLOCK(Screen) ) {
                if ( SDL_LockSurface(Screen) < 0 )
		return;        }                                                                       
}

void unlock()
{
        if ( SDL_MUSTLOCK(Screen) ) {                                           
                SDL_UnlockSurface(Screen); }                                                                       
}

void FullUpdate()
{
  blitrects_num=0;
  SDL_UpdateRect(Screen,0,0,0,0);
}

void AddRect(int x1, int y1, int x2, int y2)
{
  int temp;
   /* Make sure X1 is before X2 */
   if (x2 < x1){
      temp = x2;
      x2 = x1;
      x1 = temp;
   }
   /* Make sure Y1 is before Y2 */
   if (y2 < y1){
      temp = y2;
      y2 = y1;
      y1 = temp;
   }
  blitrect.x = x1;
  blitrect.y = y1;
  blitrect.w = x2-x1+1;
  blitrect.h = y2-y1+1;
  if (x1<0) printf("x is too small in function AddRect!\n");else
  if (y1<0) printf("y is too small in function AddRect!\n");else
  if (x2>=800) printf("x is too big in function AddRect!\n");else
  if (y2>=600) printf("y is too big in function AddRect!\n");else {
      AddThisRect(blitrect);
  }
}

void Blit(int Xpos,int Ypos,SDL_Surface *image)  //blits one GIF or BMP from the memory to the screen
{
  blitrect.x = Xpos;
  blitrect.y = Ypos;
  blitrect.w = image->w;
  blitrect.h = image->h;

  if (image==NULL) printf("WRONG BLIT: surface pointer is NULL!\n");
  if (Xpos<-image->w) printf("WRONG BLIT: Xpos is too small! - %d\n",Xpos); else
  if (Xpos>=800) printf("WRONG BLIT: Xpos is too big! - %d\n",Xpos); else
  if (Ypos<-image->h) printf("WRONG BLIT: Ypos is too small!\n - %d",Ypos); else
  if (Ypos>=600) printf("WRONG BLIT: Ypos is too big! - %d\n",Ypos); else
  if ( SDL_BlitSurface(image, NULL, Screen, &blitrect) < 0 ) 
  {
    SDL_FreeSurface(image);
    ComplainAndExit();
  }
  AddThisRect(blitrect);
}

SDL_Surface *LoadImage(char *datafile)   // reads one png into the memory
{
  SDL_Surface *pic=NULL,*pic2=NULL;
  char filename[200];
  int i=0;

  sprintf(filename,"%s/gfx/%s",DATAPATH,datafile);
  pic=IMG_Load(filename);
  while ( pic == NULL ) {
    strcpy(DATAPATH,PATH[i]);
    sprintf(filename,"%s/gfx/%s",DATAPATH,datafile);
    pic=IMG_Load(filename);
    i++;
    
    if (i>=PATHNUM)
    {
      fprintf(stderr,"Couldn't load %s: %s\n", filename, SDL_GetError());
      exit(2);
    }
  }
  
  pic2 = SDL_DisplayFormatAlpha(pic);
  SDL_FreeSurface(pic);
  return (pic2);
}

void BlitToBB(int Xpos,int Ypos,SDL_Surface *image)
{
  blitrect.x = Xpos;
  blitrect.y = Ypos;
  blitrect.w = image->w;
  blitrect.h = image->h;
  if ( SDL_BlitSurface(image, NULL, BackBuffer, &blitrect) < 0 )
  {
    SDL_FreeSurface(image);
    ComplainAndExit();
  }
}

void BlitPart(int Xpos,int Ypos,SDL_Surface *image, SDL_Rect srcrect)
{
  blitrect.x = srcrect.x;											
  blitrect.y = srcrect.y;											
  blitrect.w = srcrect.w;											
  blitrect.h = srcrect.h;
  if ( SDL_BlitSurface(image, &srcrect , Screen, &blitrect) < 0 )
  {
    SDL_FreeSurface(image);
    ComplainAndExit();
  }
  AddThisRect(blitrect);
}

void FadeScreen(float speed)
{
  Sint32 now,i;

  SDL_BlitSurface(Screen,NULL,FadeBuffer,NULL);
  now=SDL_GetTicks();
  for (i=255*speed;i>=0;i-=SDL_GetTicks()-now)
  {
    now=SDL_GetTicks();
    SDL_BlitSurface(FadeBuffer,&blitrect,Screen,&blitrect);
    SDL_SetAlpha(BackBuffer,SDL_SRCALPHA,255-(int)(i/speed));
    Blit(0,0,BackBuffer);
    SDL_UpdateRects(Screen,1,&blitrect);
  }
  SDL_SetAlpha(BackBuffer,0,0);
  Blit(0,0,BackBuffer);
  SDL_UpdateRects(Screen,1,&blitrect);
}

int PutPixel(SDL_Surface *Surface, Sint32 X, Sint32 Y, Uint32 Color)
{
    if (X<0) printf("X < 0 in function PutPixel! - %d\n",X); else
    if (X>=800) printf("X >= 800 in function PutPixel! - %d\n",X); else
    if (Y<0) printf("Y < 0 in function PutPixel! - %d\n",Y); else
    if (Y>=600) printf("Y >= 600 in function PutPixel! - %d\n",Y); else
    {
        _PutPixel(Surface,X,Y,Color);
        AddRect(X,Y,X,Y);
    }
    return 0;
}
