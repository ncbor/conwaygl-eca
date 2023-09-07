#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define COEF 10
#define W (WINDOW_HEIGHT/COEF)
#define H (WINDOW_WIDTH/COEF)
#define DELAY 25

int to_binary(int*, int []);

int check_grid(int(*)[W],int(*)[W]);

int ca_check_cell(int, int [], int []);

int gl_check_cell(int(*)[W],int ,int );

int print_grid(int(*)[W],SDL_Texture*,SDL_Renderer*);

int main()
{
  ////////////////////////////SDL/////////////////////////////////////////////
  ////////////////////////////SDL/////////////////////////////////////////////
  ////////////////////////////SDL/////////////////////////////////////////////
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS)!=0)
    {
      printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
      return 1;
    }

    //Create Window
    SDL_Window* wmain = SDL_CreateWindow
    (
      "Celular Automata",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      WINDOW_WIDTH,WINDOW_HEIGHT,0
    );//SDL_WINDOW_FULLSCREEN_DESKTOP

    if(!wmain)
    {
      printf("Erro ao gerar janela: %s\n",SDL_GetError());
      SDL_DestroyWindow(wmain);
      SDL_Quit();
      return 1;
    }

    //Create Renderer
    Uint32 renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer *r = SDL_CreateRenderer(wmain,-1,renderer_flags);
    if(!r)
    {
      printf("Erro ao gerar renderizador: %s\n",SDL_GetError());
      SDL_DestroyRenderer(r);
      SDL_DestroyWindow(wmain);
      SDL_Quit();
      return 1;
    }

    SDL_Texture *t = SDL_CreateTexture(r,0,SDL_TEXTUREACCESS_TARGET,WINDOW_WIDTH,WINDOW_HEIGHT);
    if(!t)
    {
      printf("Erro ao gerar renderizador: %s\n",SDL_GetError());
      SDL_DestroyTexture(t);
      SDL_DestroyRenderer(r);
      SDL_DestroyWindow(wmain);
      SDL_Quit();
      return 1;
    }
    // SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  ////////////////////////////SDL/////////////////////////////////////////////
  ////////////////////////////SDL/////////////////////////////////////////////
  ////////////////////////////SDL/////////////////////////////////////////////
  int grid[H][W]={0}, newgrid[H][W], wr=30, rules[8];
  grid [0][W/2]=1;
  int (*g)[W] = grid,(*newg)[W] = newgrid;
  SDL_RenderClear(r);SDL_RenderPresent(r);
  SDL_SetRenderTarget(r, t);
  SDL_RenderSetScale(r,COEF,COEF);
  to_binary(&wr,rules);

  int close=0;
  while(!close)
  {
      SDL_Event e;
      while(SDL_PollEvent(&e))
      {
        switch(e.type)
        {
          case SDL_QUIT:
            close=1; exit(1);
            break;
        }
      }
      if(close){break;}


      check_grid(g,newg);
      for(int i = H/2;i>=0;--i)
      {
        for(int j = 0;j<W;++j)
        {
          if(!i){*(newg[i]+j)=ca_check_cell(j,g[i],rules);}
          else{*(newg[i]+j)=*(g[i-1]+j);}
        }
      }
      SDL_SetRenderTarget(r,t);
      print_grid(newg,t,r);
      SDL_SetRenderTarget(r,NULL);
      SDL_RenderCopy(r,t,0,0);
      SDL_RenderPresent(r);
      int (*swap)[W] = g;
      g = newg;
      newg = swap;
      SDL_Delay(DELAY);
  }

  SDL_DestroyTexture(t);
  SDL_DestroyRenderer(r);
  SDL_DestroyWindow(wmain);
  SDL_Quit();

  return 0;
}

int check_grid(int grid[H][W],int newgrid[H][W])
{
  for(int i=H/2;i<H;++i)
  {
    for(int j=0;j<W;++j)
    {
      newgrid[i][j]=gl_check_cell(grid,i,j);
    }
  }
  return 0;
}

int gl_check_cell(int grid[H][W], int i, int j)
{
  int aux = 0, life = 0;
  int y0 = i-1, y1 = i+1;
  int x0 = j-1, x1 = j+1;
  if(y0<0){y0=0;};
  if(y1>=H){y1=H-1;};
  if(x0<0){x0=0;};
  if(x1>=W){x1=W-1;};

  if(grid[i][j]==1){life=1;}

  for (i=y0;i<=y1;++i)
  {
    for (j=x0;j<=x1;++j)
    {
      if(grid[i][j]){++aux;}
    }
  }

  aux -= life;
  switch(aux)
  {
    case 3:
      return 1;
      break;
    case 2:
      if(life){return 1;}
      break;
  }
  return 0;
}

int print_grid(int grid[H][W],SDL_Texture*t, SDL_Renderer*r)
{
  SDL_SetRenderTarget(r,t);
  SDL_SetRenderDrawColor(r,10,10,10,255);
  SDL_RenderClear(r);
  SDL_SetRenderDrawColor(r,04,215,15,255);
  SDL_RenderSetScale(r,COEF,COEF);
  for(int i = 0; i < H; ++i)
  {
    for(int j = 0; j < W; ++j)
    {
      if(grid[i][j]==1)
      {
      SDL_RenderDrawPoint(r,i,j);
      }
    }
  }
  SDL_SetRenderTarget(r,NULL);
  return 0;
}

//Transform number between 0-255 into an array of its binaries
int to_binary(int*n, int bn[])
{
  for(int i=0;i<8;++i)
  {
    bn[i]=(*n)%2;
    (*n)/=2;
  }
  return 0;
}

int ca_check_cell(int i, int old[], int rules[])
{
  int c1 = i-1, c2 = i+1;
  if(c1<0){c1+=W;};
  if(c2>=W){c2-=W;};
  int n1=old[c1], n2=old[c2];
  switch(old[i])
  {
    case 0:
      if(!n1&&!n2&&rules[0])
      {
        return 1;
      }
      if(!n1&&n2&&rules[1])
      {
        return 1;
      }
      if(n1&&!n2&& rules[4])
      {
        return 1;
      }
      if(n1&&n2&&rules[5])
      {
        return 1;
      }
      break;
    case 1:
      if(!n1&&!n2&&rules[2])
      {
        return 1;
      }
      if(!n1&&n2&&rules[3])
      {
        return 1;
      }
      if(n1&&!n2&&rules[6])
      {
        return 1;
      }
      if(n1&&n2&&rules[7])
      {
        return 1;
      }
      break;
  }
  return 0;
}
