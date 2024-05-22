#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <iostream>
#include <vector>

using namespace std;
// SDL Color Definitions (Use correct definitions next time lol)
SDL_Color Gold{255, 215, 0, 255};
SDL_Color Black = {0, 0, 0, 255};
SDL_Color White = {255, 255, 255, 255};
SDL_Color Lava = {230, 46, 0, 255};
// ClearScreen Method
void clearScreen(SDL_Renderer *renderer, int r = 255, int g = 255, int b = 255, int a = 255)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderClear(renderer);
}
// DrawRect Method
void drawRect(SDL_Renderer *renderer, SDL_Rect *rect, SDL_Color color = Black)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, rect);
}
// DrawText
void drawText(SDL_Renderer *renderer, TTF_Font *font, char *text, int x, int y, SDL_Color color)
{
    // SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font, text, color);
    SDL_Surface *surfaceMessage = TTF_RenderText_Shaded(font, text, color, White);
    // SDL_Surface *surfaceMessage = TTF_RenderText_Blended(font, text, color);
    if (surfaceMessage == NULL)
    {
        cout << "Text not working \n"
             << TTF_GetError() << endl;
    }
    SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect messageRect;
    messageRect.x = x;
    messageRect.y = y;
    messageRect.w = surfaceMessage->w;
    messageRect.h = surfaceMessage->h;
    SDL_RenderCopy(renderer, Message, NULL, &messageRect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

/// @brief Checks if b is greater than a but smaller than c
bool IIB(int a, int b, int c, bool includeExtremes = false)
{
    if (includeExtremes)
    {
        if ((a <= b) & (b <= c))
        {
            return true;
        }
        return false;
    }
    if ((a < b) & (b < c))
    {
        return true;
    }
    return false;
}
bool PTSC(SDL_Rect rect1, SDL_Rect rect2)
{
    if (IIB(rect2.x, rect1.x, rect2.x + rect2.w) || IIB(rect2.x, rect1.x + rect1.w, rect2.x + rect2.w))
    {
        // Check if rect1 is within rect2's y range
        if (IIB(rect2.y, rect1.y, rect2.y + rect2.h) || IIB(rect2.y, rect1.y + rect1.h, rect2.y + rect2.h))
        {
            return true;
        }
    }
    return false;
}
/// @brief Checks if colliding, returns true if so
/// @param rect1 First Rectangle
/// @param rect2 Second Rectangle
bool checkRectCollisions(SDL_Rect rect1, SDL_Rect rect2)
{
    if (PTSC(rect1, rect2))
    {
        return true;
    }
    if (PTSC(rect2, rect1))
    {
        return true;
    }
    // Return false if no collision occured
    return false;
}

// Camera Class
class Camera
{
public:
    int x = 0;
    int y = 0;
    int w;
    int h;
};

// Coin class
class Coin
{
    int animationFrame = 0;
    int floatHeight = 20;
    float floatSpeed = 0.33f;
    float coinX;
    float coinY;
    float originX;
    float originY;
    bool floatingUp = true;

public:
    /// @brief The Coin's sprite, stores position and size
    SDL_Rect sprite;
    SDL_Color color;

    Coin(int x, int y, int w, int h, SDL_Color clr)
    {
        sprite.x = x;
        sprite.y = y;
        sprite.w = w;
        sprite.h = h;
        coinX = sprite.x;
        coinY = sprite.y;
        originX = sprite.x;
        originY = sprite.y;
        color = clr;
    }

    // This is a hardcoded animation for the coin to float up and down
    void Animate(SDL_Renderer *renderer, Camera Cam)
    {
        if (IIB(Cam.x, sprite.x + sprite.w, Cam.x + Cam.w, true) || IIB(Cam.x, sprite.x, Cam.x + Cam.w, true))
        {
            if ((coinY > (originY - floatHeight)) & floatingUp)
            {
                coinY -= floatSpeed;
                sprite.y = (int)coinY;
                animationFrame++;
            }
            else if (coinY < originY)
            {
                floatingUp = false;
                coinY += floatSpeed;
                sprite.y = (int)coinY;
                animationFrame++;
            }
            else
            {
                floatingUp = true;
                animationFrame = 0;
            }
            SDL_Rect temp = sprite;
            temp.x -= Cam.x;
            temp.y -= Cam.y;
            drawRect(renderer, &temp, color);
        }
    }
};
// Magma Class
class Magma
{
public:
    SDL_Rect sprite;
    SDL_Color color = Lava;
    // Constructor
    Magma(int x, int y, int w, int h)
    {
        sprite.x = x;
        sprite.y = y;
        sprite.w = w;
        sprite.h = h;
    }
    //
};
// Level Class
class Level
{
    int coinSizeX = 30;
    int coinSizeY = 30;

public:
    vector<SDL_Rect> platforms;
    vector<Coin> coins;
    vector<Magma> magma;

    void addPlatform(int x, int y, int width, int height)
    {
        SDL_Rect temp;
        temp.x = x;
        temp.y = y;
        temp.w = width;
        temp.h = height;
        platforms.push_back(temp);
        ;
    }

    /// @brief Draws A Level's Platforms to the temp screen before it is presented
    void drawPlatforms(SDL_Renderer *renderer, Camera Cam)
    {
        for (int i = 0; i < platforms.size(); i++)
        {
            int originX = platforms[i].x;
            platforms[i].x -= Cam.x;
            // Draw the platform
            drawRect(renderer, &platforms[i]);
            platforms[i].x = originX;
        }
    }

    void addCoin(int x, int y)
    {
        Coin temp(x, y, coinSizeX, coinSizeY, Gold);
        coins.push_back(temp);
    }
    /// @brief Draws a level's coins to the temp screen before it is presented
    void drawCoins(SDL_Renderer *renderer, Camera Cam)
    {
        for (int i = 0; i < coins.size(); i++)
        {
            coins[i].Animate(renderer, Cam);
        }
    }

    void addMagma(int x, int y, int w, int h)
    {
    }
    /// @brief Draws a level's magma to the temp screen before it is presented
    void drawMagma(SDL_Renderer *renderer, Camera Cam)
    {
        for (int i = 0; i < magma.size(); i++)
        {
            drawRect(renderer, &magma[i].sprite, magma[i].color);
        }
    }
};

// Key Tracker Class
class Keys
{
public:
    bool w = false;
    bool s = false;
    bool a = false;
    bool d = false;
};

// Player Class
class Player
{
public:
    int lives = 3;
    int speed = 13;
    int score = 0;
    // Color
    SDL_Color color = {70, 70, 70, 255};
    // Jumping
    int jumpForce = 40;
    int gravity = 2;
    bool canJump = false;
    // Positions and velocities
    int dx = 0;
    int dy = 0;
    SDL_Rect sprite;
    // Keys
    Keys keyDown;
    Player(int tx, int ty, int tw, int th)
    {
        sprite.x = tx;
        sprite.y = ty;
        sprite.w = tw;
        sprite.h = th;
    }
};

int main(int argc, char *argv[])
{
    // Initialize SDL Components
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "SDL INIT ERROR \n%s" << SDL_GetError();
    }
    // Initialize TTF
    if (TTF_Init() < 0)
    {
        cout << "TTF INIT ERROR \n%s" << TTF_GetError();
    }
    // Fonts
    TTF_Font *Antonio = TTF_OpenFont("Fonts/antonio/Antonio-Bold.ttf", 30);
    TTF_Font *OpenSans = TTF_OpenFont("Fonts/open-sans/OpenSans-Bold.ttf", 50);
    // Score Text
    char scoreText[9] = "Score:  ";
    // Declare Window and Renderer
    SDL_Window *window;
    SDL_Renderer *renderer;
    // Event Handler
    SDL_Event e;
    // Camera Setup
    Camera Cam;
    Cam.x = 0;
    Cam.y = 0;
    // Window Size
    SDL_DisplayMode DM;
    if (SDL_GetCurrentDisplayMode(0, &DM) != 0)
    {
        SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
        return 1;
    }
    Cam.w = DM.w;
    Cam.h = DM.h;
    // Set Left and Right borders (to prevent player just walking far away from the map)
    int leftBorder = -100;
    int rightBorder = 3000;
    // Make mouse invisible
    SDL_ShowCursor(SDL_DISABLE);
    // Frame Delays
    const int FPS = 60;
    const float DELAY_TIME = 1000.0f / FPS;
    Uint32 frameStart, frameTime;
    // Setup the Levels
    Level Level1;
    // Add Platforms
    Level1.addPlatform(300, 750, 300, 20);
    Level1.addPlatform(800, 400, 300, 20);
    Level1.addPlatform(1300, 200, 300, 20);
    Level1.addPlatform(1700, 570, 300, 20);
    Level1.addPlatform(2000, 150, 300, 20);
    Level1.addPlatform(2450, 150, 300, 20);
    // Add Coins
    Level1.addCoin(435, 670);
    Level1.addCoin(935, 320);
    Level1.addCoin(1435, 120);
    Level1.addCoin(2135, 70);
    Level1.addCoin(2660, 300);
    // Initialize Player Values
    Player player(100, 100, 40, 80);
    // Create the window
    window = SDL_CreateWindow("SDL Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Cam.w, Cam.h, SDL_RENDERER_ACCELERATED || SDL_RENDERER_PRESENTVSYNC);
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    // Make sure the window has loaded Correctly return an error if not
    if (window == NULL)
    {
        cout << "Could not create window: %s\n"
             << SDL_GetError();
        return 1;
    }

    // Initialize the Renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    // Make sure renderer was created
    if (renderer == NULL)
    {
        cout << "Could not create window: %s\n"
             << SDL_GetError();
        return 1;
    }

    // Gameloop
    bool gameloop = true;
    while (gameloop)
    {
        // Reset Screen
        clearScreen(renderer);
        // Key Inputs
        if (SDL_PollEvent(&e))
        {
            int key = e.key.keysym.sym;
            // Handling Keydown
            if (e.type == SDL_KEYDOWN)
            {
                // W Key
                if (key == SDLK_w)
                {
                    player.keyDown.w = true;
                }
                // S Key
                if (key == SDLK_s)
                {
                    player.keyDown.s = true;
                }
                // A Key
                if (key == SDLK_a)
                {
                    player.keyDown.a = true;
                }
                // D Key
                if (key == SDLK_d)
                {
                    player.keyDown.d = true;
                }
                // Quit
                if (key == SDLK_q)
                {
                    gameloop = false;
                }
            }
            // Keyup
            if (e.type == SDL_KEYUP)
            {
                if (key == SDLK_w)
                {
                    player.keyDown.w = false;
                }
                if (key == SDLK_s)
                {
                    player.keyDown.s = false;
                }
                if (key == SDLK_a)
                {
                    player.keyDown.a = false;
                }
                if (key == SDLK_d)
                {
                    player.keyDown.d = false;
                }
            }
        }

        // Apply forces to player's position
        // Reset player's x velocity before applying it (this is to keep the speed leeled as there is no ramping to the player's speed right now)
        player.dx = 0;

        // Horizontal movement
        if (player.keyDown.a)
        {
            player.dx -= player.speed;
        }
        if (player.keyDown.d)
        {
            player.dx += player.speed;
        }
        // Vertical movement (Jumping)
        // Gravity
        if (!player.canJump)
        {
            if (player.keyDown.s)
            {
                player.dy += 3 * player.gravity;
            }
            else
            {
                player.dy += player.gravity;
            }
        }
        // Jump if touching ground
        if (player.keyDown.w && player.canJump)
        {
            player.dy = -player.jumpForce;
            player.canJump = false;
        }

        // Check if Player Collected a coin
        for (int i = 0; i < Level1.coins.size(); i++)
        {
            if (PTSC(player.sprite, Level1.coins[i].sprite))
            {
                player.score++;
                Level1.coins.erase(Level1.coins.begin() + i);
                i--;
            }
        }

        // Platform Collisions
        // Loop through all the platforms and check collisions for future player x and stop player if it will collide
        bool onPlatform = false;
        for (int i = 0; i < Level1.platforms.size(); i++)
        {
            if (IIB(Cam.x, Level1.platforms[i].x + Level1.platforms[i].w, Cam.x + Cam.w, true) || IIB(Cam.x, Level1.platforms[i].x, Cam.x + Cam.w, true))
            {
                int originX = player.sprite.x;
                int originY = player.sprite.y;
                bool hCol = false;
                bool vCol = false;
                // Check if X is colliding with the platforms
                player.sprite.x += player.dx;
                if (checkRectCollisions(player.sprite, Level1.platforms[i]))
                {
                    hCol = true;
                }
                player.sprite.x = originX;
                // Check if the Y is colliding with the platforms
                player.sprite.y += player.dy;
                if (checkRectCollisions(player.sprite, Level1.platforms[i]))
                {
                    vCol = true;
                }
                player.sprite.y = originY;
                // Check if player clipped through the platform
                if (IIB(Level1.platforms[i].x, player.sprite.x, Level1.platforms[i].x + Level1.platforms[i].w) || IIB(Level1.platforms[i].x, player.sprite.x + player.sprite.w, Level1.platforms[i].x + Level1.platforms[i].w))
                {
                    if (player.sprite.y + player.sprite.h <= Level1.platforms[i].y)
                    {
                        player.sprite.y += player.dy;
                        if (player.sprite.y + player.sprite.h > Level1.platforms[i].y)
                        {
                            vCol = true;
                        }
                        player.sprite.y = originY;
                    }
                }
                // Do the same thing but Up instead of Down
                if (IIB(Level1.platforms[i].x, player.sprite.x, Level1.platforms[i].x + Level1.platforms[i].w) || IIB(Level1.platforms[i].x, player.sprite.x + player.sprite.w, Level1.platforms[i].x + Level1.platforms[i].w))
                {
                    if (player.sprite.y + player.sprite.h >= Level1.platforms[i].y + Level1.platforms[i].h)
                    {
                        player.sprite.y += player.dy;
                        if (player.sprite.y < Level1.platforms[i].y)
                        {
                            vCol = true;
                        }
                        player.sprite.y = originY;
                    }
                }
                // Apply Player collisions on the x;
                if (hCol & (player.dx > 0))
                {
                    player.sprite.x = Level1.platforms[i].x - player.sprite.w;
                    player.dx = 0;
                }
                else if (hCol)
                {
                    player.sprite.x = Level1.platforms[i].x + Level1.platforms[i].w;
                    player.dx = 0;
                }
                // Apply player y collisions
                if (vCol & (player.dy > 0))
                {
                    player.sprite.y = Level1.platforms[i].y - player.sprite.h;
                    player.dy = 0;
                    onPlatform = true;
                }
                else if (vCol)
                {
                    player.sprite.y = Level1.platforms[i].y + Level1.platforms[i].h;
                    player.dy = 0;
                }
                if (!onPlatform)
                {
                    player.canJump = false;
                }
                else
                {
                    player.canJump = true;
                }
            }
        }

        // Apply the velocity to the player
        player.sprite.x += player.dx;
        player.sprite.y += player.dy;
        // Apply the changes to the camera
        Cam.x = (player.sprite.x - Cam.w / 2) + player.sprite.w / 2;
        // Cam.y += player.dy;

        // Horizontal Border(Floor and Ceiling)
        if (player.sprite.y < 0)
        {
            player.sprite.y = 0;
            player.dy = 0;
        }

        // Check if player touching ground
        if (player.sprite.y + player.sprite.h > Cam.h)
        {
            player.sprite.y = Cam.h - player.sprite.h;
            player.canJump = true;
            player.dy = 0;
        }
        else if (player.sprite.y + player.sprite.h == Cam.h)
        {
            player.canJump = true;
            player.dy = 0;
        }
        // Make sure Player isn't too far to the left or right
        if (player.sprite.x < leftBorder)
        {
            player.sprite.x = leftBorder;
            Cam.x = (player.sprite.x - Cam.w / 2) + player.sprite.w / 2;
        }
        else if (player.sprite.x + player.sprite.w > rightBorder)
        {
            player.sprite.x = rightBorder - player.sprite.w;
            Cam.x = (player.sprite.x - Cam.w / 2) + player.sprite.w / 2;
        }
        // Draw Coins
        Level1.drawCoins(renderer, Cam);
        // Draw Player
        SDL_Rect pShift = player.sprite;
        pShift.x -= Cam.x;
        pShift.y -= Cam.y;
        drawRect(renderer, &pShift, player.color);
        // Draw Platforms
        Level1.drawPlatforms(renderer, Cam);
        // Score Text
        scoreText[7] = player.score + '0';
        drawText(renderer, OpenSans, scoreText, 10, 10, Black);
        // Render
        SDL_RenderPresent(renderer);
        // Check if Player won
        if (player.score >= 5)
        {
            cout << "You Win!"
                 << "\nYour Score Was: " << player.score << endl;
            gameloop = false;
        }

        // Frame Delay (what makes the gameloop go)
        frameStart = SDL_GetTicks();
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < DELAY_TIME)
        {
            SDL_Delay((int)(DELAY_TIME - frameTime));
        }
        else
        {
            SDL_Delay((int)DELAY_TIME);
        }
    }
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}