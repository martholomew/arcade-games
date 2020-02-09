#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

#define windowWidth (800)
#define windowHeight (800)
#define BLOCK_SIZE (20)
#define MAX_BLOCKS (windowWidth * windowHeight) / BLOCK_SIZE

struct keys
{
    int left;
    int right;
    int up;
    int down;
};
struct snake_block
{
    int x_p;
    int y_p;
    int x_v;
    int y_v;
};

void renderBlock(SDL_Renderer *rend_ptr,
                 struct snake_block s_blocks_ptr[MAX_BLOCKS], SDL_Rect *block)
{
    //set a rect to the coords of the head block
    SDL_Rect s_rect;
    s_rect.x = s_blocks_ptr[0].x_p;
    s_rect.y = s_blocks_ptr[0].y_p;
    s_rect.h = BLOCK_SIZE;
    s_rect.w = BLOCK_SIZE;
    //if the head block and the block the snake eats intersect
    if (SDL_HasIntersection(block, &s_rect))
    {
        //change the position of the block that gets eaten
        block->x = (rand() % (windowWidth / BLOCK_SIZE)) * BLOCK_SIZE;
        block->y = (rand() % (windowHeight / BLOCK_SIZE)) * BLOCK_SIZE;
        //find a block in the snake that is out of bounds
        int i = 0;
        for (i = 0; i < MAX_BLOCKS; i++)
        {
            struct snake_block *block;
            block = &s_blocks_ptr[i];
            //set it to a dum number so it gets caught in renderBlocks and break
            if (block->x_p == -1 && block->y_p == -1)
            {
                block->x_p = -100;
                block->y_p = -100;
                break;
            }
        }
    }
    //set color to white
    SDL_SetRenderDrawColor(rend_ptr, 0xFF, 0xFF, 0xFF, 0xFF);
    //render the block that gets eaten
    SDL_RenderFillRect(rend_ptr, block);
}

int renderBlocks(SDL_Renderer *rend_ptr,
                 struct snake_block s_blocks_ptr[MAX_BLOCKS],
                 struct keys *keys_ptr)
{
    //make a rectangle to be used for each section upon rendering
    SDL_Rect rect;
    //loop over the blocks starting from the end of the array
    int i = 0;
    for (i = MAX_BLOCKS - 1; i >= 0; i--)
    {
        struct snake_block *block;
        block = &s_blocks_ptr[i];
        //if the block is in bounds
        if (block->x_p == -1 && block->y_p == -1) continue;
        //if the block is not the first and is no long out of bounds, put it at the same position as the next block
        if (i != 0)
        {
            block->x_p = s_blocks_ptr[i - 1].x_p;
            block->y_p = s_blocks_ptr[i - 1].y_p;
        }
        //these are the things the first block will do
        else
        {
            //if the block moves out of bounds, reinit
            if (i == 0 && (block->x_p < 0 || block->y_p < 0
                || block->x_p == windowWidth || block->y_p == windowHeight))
            {
                return 1;
            }
            //can only turn up and down if sideways
            if (block->x_v)
            {
                if (keys_ptr->up)
                {
                    block->y_v = -1;
                    block->x_v = 0;
                }
                else if (keys_ptr->down)
                {
                    block->y_v = 1;
                    block->x_v = 0;
                }
            }
            //can only turn sideways if up and down
            else if (block->y_v)
            {
                if (keys_ptr->right)
                {
                    block->x_v = 1;
                    block->y_v = 0;
                }
                else if (keys_ptr->left)
                {
                    block->x_v = -1;
                    block->y_v = 0;
                }
            }
            //move the head block
            if (block->x_v) block->x_p += BLOCK_SIZE * block->x_v;
            if (block->y_v) block->y_p += BLOCK_SIZE * block->y_v;
        }
        //set rect to bounds of whatever block the loop is on
        rect.x = block->x_p;
        rect.y = block->y_p;
        rect.w = BLOCK_SIZE;
        rect.h = BLOCK_SIZE;
        //change color to white
        SDL_SetRenderDrawColor(rend_ptr, 0xFF, 0xFF, 0xFF, 0xFF);
        //render the block
        SDL_RenderFillRect(rend_ptr, &rect);
    }
    return 0;
}

int loop(SDL_Renderer *rend_ptr, struct snake_block s_blocks_ptr[MAX_BLOCKS],
         SDL_Rect *block_ptr)
{
    // must define or it will pick some random junk
    struct keys keys;
    keys.left = 0;
    keys.right = 0;
    keys.up = 0;
    keys.down = 0;
    // returns: 1 quit, 2 re-initialize...maybe something else later
    while(1)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                return 1;
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
        SDL_RenderClear(rend_ptr);
        //render the snake blocks, if it returns anything but zero, reinit
        if (renderBlocks(rend_ptr, s_blocks_ptr, &keys))
        {
            SDL_SetRenderDrawColor(rend_ptr, 0x00, 0x00, 0x00, 0xFF);
            return 2;
        }
        //render the block the snake eats
        renderBlock(rend_ptr, s_blocks_ptr, block_ptr);
        // Set the color to black
        SDL_SetRenderDrawColor(rend_ptr, 0x00, 0x00, 0x00, 0xFF);
        // Throw the back to the front
        SDL_RenderPresent(rend_ptr);
        SDL_Delay(100);
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

    SDL_Window* win = SDL_CreateWindow("TopSnek: My BFF",
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

    // 1 is quit, 2 is init/reinit
    int event = 2;
    while (1)
    {
        int i = 0;
        srand(time(0));
        struct snake_block s_blocks[MAX_BLOCKS];
        switch (event)
        {
            case 1:
                // clean up resources before exiting
                SDL_DestroyRenderer(rend);
                SDL_DestroyWindow(win);
                SDL_Quit();
                return 0;
            case 2:
                // make the max number of snake blocks
                for (i = 0; i < MAX_BLOCKS; i++)
                {
                    //initialize them at zero
                    s_blocks[i].x_p = 0;
                    s_blocks[i].y_p = 0;
                    s_blocks[i].x_v = 0;
                    s_blocks[i].y_v = 0;
                    if (i == 0)
                    {
                        //first block starts at a random spot going a random direction
                        int dir;
                        s_blocks[i].x_p = (rand() % (windowWidth / BLOCK_SIZE)) * BLOCK_SIZE;
                        s_blocks[i].y_p = (rand() % (windowHeight / BLOCK_SIZE)) * BLOCK_SIZE;
                        dir = rand() % 4;
                        if (dir == 0) s_blocks[i].x_v = 1;
                        if (dir == 1) s_blocks[i].x_v = -1;
                        if (dir == 2) s_blocks[i].y_v = 1;
                        if (dir == 3) s_blocks[i].y_v = -1;
                    }
                    else
                    {
                        // otherwise the block is put out of bounds
                        s_blocks[i].x_p = -1;
                        s_blocks[i].y_p = -1;
                        s_blocks[i].x_v = 0;
                        s_blocks[i].y_v = 0;
                    }
                }
                //init the blocks that are eaten by the snake
                SDL_Rect block;
                block.x = (rand() % (windowWidth / BLOCK_SIZE)) * BLOCK_SIZE;
                block.y = (rand() % (windowHeight / BLOCK_SIZE)) * BLOCK_SIZE;
                block.w = BLOCK_SIZE;
                block.h = BLOCK_SIZE;
                //start the main loop
                event = loop(rend, s_blocks, &block);
                break;
        }
    }
}
