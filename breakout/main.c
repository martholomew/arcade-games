#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

#define windowHeight (720)
#define windowWidth (1080)
#define BALLS (1)
#define BALL_SIZE (10)
#define SPEED (10)
#define VELOCITY (30)

struct pos_vel {
    float x_pos;
    float y_pos;

    float x_vel;
    float y_vel;
    SDL_Rect rect;
    int dead;
};

struct keys
{
    int left;
    int right;
    int up;
    int down;
};

void AnimateBalls(SDL_Renderer *rend_ptr, struct pos_vel balls_ptr[BALLS],  struct pos_vel *bar_ptr)
{
    int i = 0;
    int ball_middle;
    for(i = 0; i < BALLS; i++)
    {
        struct pos_vel *ball;
        ball = &balls_ptr[i];

        if (SDL_HasIntersection(&ball->rect, &bar_ptr->rect))
        {
            ball_middle = ball->rect.x + (ball->rect.w / 2);
            if (ball_middle < bar_ptr->rect.x + (bar_ptr->rect.w / 2))
            {
                ball->y_vel = -(ball_middle - (bar_ptr->rect.x - (BALL_SIZE / 2)));
                ball->x_vel = -((bar_ptr->rect.x + (bar_ptr->rect.w / 2) + (BALL_SIZE / 2)) - ball_middle);
                if (ball->x_vel < -30)
                {
                    if (bar_ptr->x_vel == -SPEED)
                    {
                        ball->x_vel = -30 * 1.5;
                        ball->y_vel = -20 * 1.5;
                    }
                    else
                    {
                        ball->x_vel = -30;
                        ball->y_vel = -20;
                    }
                }
            }
            else if (ball_middle > bar_ptr->rect.x + (bar_ptr->rect.w / 2))
            {
                ball->y_vel = -((bar_ptr->rect.x + bar_ptr->rect.w + (BALL_SIZE / 2)) - ball_middle);
                ball->x_vel = ball_middle - (bar_ptr->rect.x + (bar_ptr->rect.w / 2) - (BALL_SIZE / 2));
                if (ball->x_vel > 30)
                {
                    if (bar_ptr->x_vel == SPEED)
                    {
                        ball->x_vel = 30 * 1.5;
                        ball->y_vel = -20 * 1.5;
                    }
                    else
                    {
                        ball->x_vel = 30;
                        ball->y_vel = -20;
                    }
                }
            }
        }
        if (ball->x_pos <= 0)
        {
            ball->x_pos = 0;
            ball->x_vel = -ball->x_vel;
        }
        if (ball->y_pos <= 0)
        {
            ball->y_pos = 0;
            ball->y_vel = -ball->y_vel;
        }
        if (ball->x_pos + ball->rect.w >= windowWidth)
        {
            ball->x_pos = windowWidth - ball->rect.w;
            ball->x_vel = -ball->x_vel;
        }
        if (ball->y_pos + ball->rect.h >= windowHeight)
        {
            ball->y_pos = windowHeight + 50;
            ball->y_vel = 0;
        }
        ball->x_pos = ball->x_pos + (ball->x_vel * .15);
        ball->y_pos = ball->y_pos + (ball->y_vel * .15);

        ball->rect.x = ball->x_pos;
        ball->rect.y = ball->y_pos;

        SDL_SetRenderDrawColor(rend_ptr, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderFillRect(rend_ptr, &ball->rect);
    }

    return;
}

void AnimateBar(SDL_Renderer *rend_ptr,  struct pos_vel *bar_ptr, struct keys *keys_ptr)
{
    if(keys_ptr->left && !keys_ptr->right)
    {
        bar_ptr->x_vel = -SPEED;
    }
    else if(keys_ptr->right && !keys_ptr->left)
    {
        bar_ptr->x_vel = SPEED;
    }
    else
    {
        bar_ptr->x_vel = 0;
    }
    bar_ptr->x_pos += bar_ptr->x_vel;

    if (bar_ptr->x_pos <= 0)
    {
        bar_ptr->x_pos = 0;
    }
    if (bar_ptr->y_pos <= 0)
    {
        bar_ptr->y_pos = 0;
    }
    if (bar_ptr->x_pos + bar_ptr->rect.w >= windowWidth)
    {
        bar_ptr->x_pos = windowWidth - bar_ptr->rect.w;
    }
    if (bar_ptr->y_pos + bar_ptr->rect.h >= windowHeight)
    {
        bar_ptr->y_pos = windowWidth - bar_ptr->rect.h;
    }
    bar_ptr->rect.x = (int) bar_ptr->x_pos;
    bar_ptr->rect.y = (int) bar_ptr->y_pos;
    SDL_SetRenderDrawColor(rend_ptr, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(rend_ptr, &bar_ptr->rect);
    return;
}

void RenderBrkBars(SDL_Renderer *rend_ptr, struct pos_vel brk_bars_ptr[], struct pos_vel balls_ptr[BALLS], int total)
{
    int i = 0;
    int j = 0;
    int dead = 0;
    for(i = 0; i < total; i++)
    {
        struct pos_vel *brk_bar;
        brk_bar = &brk_bars_ptr[i];

        for(j = 0; j < BALLS; j++)
        {
            struct pos_vel *ball;
            ball = &balls_ptr[j];

            if (SDL_HasIntersection(&ball->rect, &brk_bar->rect))
            {
                brk_bar->rect.x = -100;
                brk_bar->rect.y = -100;
                brk_bar->dead = 1;
                ball->y_vel = -ball->y_vel;
            }
        }
        if (brk_bar->dead)
        {
            dead += 1;
            if (dead == total)
            {
                int nothing;
            }
        }
        SDL_SetRenderDrawColor(rend_ptr, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderFillRect(rend_ptr, &brk_bar->rect);
    }
}

int main(void)
{
    // attempt to initialize graphics and timer system
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* win = SDL_CreateWindow("Breakout: Not the Prison Kind",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       windowWidth, windowHeight, 0);
    if (!win)
    {
        printf("error creating window: %s\n", SDL_GetError());
        SDL_Quit();
	    return 1;
    }

    // create a renderer, which sets up the graphics hardware
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
    if (!rend)
    {
      printf("error creating renderer: %s\n", SDL_GetError());
      SDL_DestroyWindow(win);
      SDL_Quit();
      return 1;
    }
    int i = 0;
    int quit = 0;

    int brk_bars_x = windowWidth / 60;
    int brk_bars_y = (windowHeight * .40) / 20;
    int brk_bars_total = brk_bars_x * brk_bars_y;
    int x = 0;
    int y = 0;
    struct pos_vel brk_bars[brk_bars_x * brk_bars_y];
    for (i = 0; i < brk_bars_x * brk_bars_y; i++)
    {
        if (i % brk_bars_x == 0)
        {
            x = (windowWidth % (brk_bars_x * 60)) / 2;
            if (i != 0)
            {
                y += 20;
            }
        }
        else
        {
            x += 60;
        }
        brk_bars[i].rect.x = x;
        brk_bars[i].rect.y = y;
        brk_bars[i].rect.w = 60;
        brk_bars[i].rect.h = 20;
    }

    struct pos_vel bar;
    bar.rect.x = 0;
    bar.rect.y = 0;
    bar.rect.w = 80;
    bar.rect.h = 20;
    bar.x_pos = (windowWidth / 2) - (bar.rect.w / 2);
    bar.y_pos = windowHeight * .85;
    bar.x_vel = 0;
    bar.y_vel = 0;

    srand(time(0));
    struct pos_vel balls[BALLS];
    for(i = 0; i < BALLS; i++)
    {
        balls[i].rect.x = 0;
        balls[i].rect.y = 0;
        balls[i].rect.w = BALL_SIZE;
        balls[i].rect.h = BALL_SIZE;
        balls[i].x_pos = (windowWidth / 2) - (bar.rect.w / 2);
        balls[i].y_pos = windowHeight * .85 - 20;
        balls[i].x_vel = rand() % VELOCITY;
        balls[i].y_vel = 50 - balls[i].x_vel;
    }

    struct keys keys;
    keys.up = 0;
    keys.down = 0;
    keys.right = 0;
    keys.left = 0;
    while(!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = 1;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    keys.up = 1;
                    break;
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    keys.left = 1;
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    keys.down = 1;
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    keys.right = 1;
                    break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    keys.up = 0;
                    break;
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    keys.left = 0;
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    keys.down = 0;
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    keys.right = 0;
                    break;
                }
                break;
            }
        }
        // clear the window
        SDL_RenderClear(rend);
        // NOW TO DRAW OUR BARRRR
        AnimateBar(rend, &bar, &keys);
        // Now we can draw our BALLS
        AnimateBalls(rend, balls, &bar);
        // Now to the breaker brk_bars
        RenderBrkBars(rend, brk_bars, balls, brk_bars_total);
        // Set the color to what was before
        SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0x00, 0xFF);
        // Throw the back to the front
        SDL_RenderPresent(rend);
    }

    // clean up resources before exiting
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
