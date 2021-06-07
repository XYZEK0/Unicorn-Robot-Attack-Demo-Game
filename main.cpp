#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>


extern "C" {
#include"../UnicornRobotAttack/SDL2-2.0.10/include/SDL.h"
#include"../UnicornRobotAttack/SDL2-2.0.10/include/SDL_main.h"
}

class Player;
class Game;
class Ground;
class Obstacle;
class GameObject;
class Platform;


#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define GRAVITYACC 4
#define DEFAULTGROUNDXPOS 550
#define GROUNDOFFSET 50
#define JUMPDIST 1
#define PLAYERJUMPS 2
#define DASHDIST 0.6
#define NUMBEROFPLATFORMS 20
#define JUMPDECREASE 0.002
#define KEYMOVEMENTVERTICALSPEED 13
#define KEYMOVEMENTHORIZONTALSPEED 13
#define NUMBEROFLIVES 3
#define PLAYERSPEED 0.4
#define TEXTLENGTH 128
#define PLAYERJUMPAMOUNT 500  
#define PLAYERDASHAMOUNT 200
#define ROUNDTIMETEXTOFFSET 150
#define ROUNDTIMETEXTPOSY 10
#define POINTSTEXTXOFFSET 70
#define EXITTEXTPOSY 26
#define ROUNDSPEEDMULTIPLIER 300
#define PLAYERLIVESPOSXOFFSET 290
#define PLAYERLIVESPOSY 23




void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y);
void DrawAnOutlineOfAnRectangle(SDL_Surface* screen, int x, int y, int l, int k, Uint32 fillColor);
void Gravity(Player* player, Game* game);
void RenderScreen(Game* game, Player* player);
void CheckTime(Game* game);
void GameLogics(Game* game, Player* player);
void ConstGameActions(Game* game, Player* player);
void GetPlayerInput(Game* game, Player* player);
void CheckColission(GameObject* gameObject, Game* game, Player* player);
void MoveObjectsLeft(Game* game, GameObject* gameObject,Player* player);
void CheckCollAll(Game* game, Player* player);
void MoveAllL(Game* game, Player* player);
void MoveAllV(Game* game, Player* player, int dir, float speed);
void MoveObjectsVertical(Game* game, GameObject* gameObject, int dir, float speed);
void DeathScreen(Game* game, Player* player);
void ResetGame(Game* game, Player* player);
void Menu(Game* game, Player* player);
int initializeGame(Game* game, Player* player);

class GameObject
{
	public:
	int objectType = 0;

	float posX = 0;
	float posY = 0;

	float originPosX = 0;
	float originPosY = 0;

	float centerPosX = 0;
	float centerPosY = 0;

	bool isPlayerColliding = false;
	SDL_Surface* surface;

};

class Game
{
	public:
	int t1 = 0;
	int	t2 = 0;
	int quit = 0;
	int rc = 0;
	int points = 0;
	int czarny, zielony, czerwony, niebieski, bialy;

	char text[TEXTLENGTH];

	bool isGameRunning = false;
	bool isGravityON = true;
	bool wasPlayerHit = false;
	bool wasJumpKeyPressed = false;
	bool alternativeControls = false;
	bool wasLifeLost = false;

	double delta = 0;
	double distance = 0;

	float roundTime = 0;
	float jumpTime = 0;
	float deltaJump = 0.002;
	float playerSpeed = PLAYERSPEED;
	float cameraOffset = 0;
	float levelLength = 0;
	float baseGameSpeed = 0.25;

	SDL_Event event;
	SDL_Surface* screen, * charset;
	SDL_Texture* scrtex;
	SDL_Window* window;
	SDL_Renderer* renderer;

	GameObject** playerHearts;

	Platform** platforms;
};

class Player :public GameObject
{
	public:

	bool canJump = false;
	bool isColliding = false;
	bool isDashing = false;
	bool isJumping = false;
	bool canDash = true;

	float dist = JUMPDIST;
	float dash = DASHDIST;

	int dashDistance = PLAYERDASHAMOUNT;
	int possibleJumps = PLAYERJUMPS;
	int jumpDistance = PLAYERJUMPAMOUNT;
	int gravityModifier = 1;
	int numberOFLives = NUMBEROFLIVES;
	
	GameObject* fallIndicator;

	void Jump(Game* game)
	{
		isJumping = true;
		for (int i = 0; i < jumpDistance; i++)
		{
			if (isDashing || game->wasPlayerHit) break;
			MoveAllV(game,this,1, dist);
			dist -= JUMPDECREASE;
			ConstGameActions(game, this);
			GetPlayerInput(game, this);
		}
		isJumping = false;
		dist = JUMPDIST;
	}

	void Dash(Game* game)
	{
		game->isGravityON = false;
		isDashing = true;
		canDash = false;
		possibleJumps = PLAYERJUMPS;
		for (int i = 0; i < dashDistance; i++)
		{
			posX += dash;
			ConstGameActions(game, this);
			GetPlayerInput(game, this);
		}
		isDashing = false;
		game->isGravityON = true;
		for (int i = 0; i < 2*dashDistance; i++)
		{
			posX -= dash/2;
			ConstGameActions(game, this);
			GetPlayerInput(game, this);
		}
		canDash = true;
	}
};

class Ground :public GameObject
{
	public:
	Ground()
	{
		objectType = 0;
	}

};

class Obstacle :public GameObject
{
	public:
	Obstacle()
	{
		objectType = 1;
	}
};

class Platform: public GameObject
{
	public:
	Obstacle* obstacle;
	Ground* stalaktyt;
	Ground* bump;

};


// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface* screen, int x, int y, const char* text,
	SDL_Surface* charset)
{
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text)
	{
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};


// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y)
{
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};


// draw a single pixel
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color)
{
	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32*)p = color;
};


// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color)
{
	for (int i = 0; i < l; i++)
	{
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};


// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
	Uint32 outlineColor, Uint32 fillColor)
{

	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);

	DrawAnOutlineOfAnRectangle(screen, x, y, l, k, fillColor);
};

void DrawAnOutlineOfAnRectangle(SDL_Surface* screen, int x, int y, int l, int k, Uint32 fillColor)
{
	int i;
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
}

void Gravity(Player* player, Game* game)
{
	MoveAllV(game, player, -1, (GRAVITYACC * pow(game->roundTime - game->jumpTime, 2)) / 2);
}

void RenderScreen(Game* game, Player* player)
{

	for (int i = 0; i < NUMBEROFPLATFORMS; i++)
	{
		DrawSurface(game->screen, game->platforms[i]->surface, game->platforms[i]->posX, game->platforms[i]->posY);
		DrawSurface(game->screen, game->platforms[i]->obstacle->surface, game->platforms[i]->obstacle->posX, game->platforms[i]->obstacle->posY);
		DrawSurface(game->screen, game->platforms[i]->stalaktyt->surface, game->platforms[i]->stalaktyt->posX, game->platforms[i]->stalaktyt->posY);
		DrawSurface(game->screen, game->platforms[i]->bump->surface, game->platforms[i]->bump->posX, game->platforms[i]->bump->posY);
	}
	
	DrawRectangle(game->screen, 4, 4, SCREEN_WIDTH - 8, 36, game->zielony, game->niebieski);

	sprintf(game->text, "Czas trwania etapu = %.1lf s ", game->roundTime);
	DrawString(game->screen, game->screen->w / 2 - ROUNDTIMETEXTOFFSET, ROUNDTIMETEXTPOSY, game->text, game->charset);

	sprintf(game->text, "   Punkty: %d", game->points);
	DrawString(game->screen, (game->screen->w / 2) + POINTSTEXTXOFFSET, ROUNDTIMETEXTPOSY, game->text, game->charset);

	sprintf(game->text, "Esc - wyjscie");
	DrawString(game->screen, game->screen->w / 2 - strlen(game->text) * 8 / 2, EXITTEXTPOSY, game->text, game->charset);

	for (int i = 0; i < player->numberOFLives; i++)
	{
		DrawSurface(game->screen, game->playerHearts[i]->surface, game->playerHearts[i]->posX, game->playerHearts[i]->posY);
	}

	DrawSurface(game->screen, player->surface, player->posX, player->posY);

	SDL_UpdateTexture(game->scrtex, NULL, game->screen->pixels, game->screen->pitch);
	SDL_RenderCopy(game->renderer, game->scrtex, NULL, NULL);
	SDL_RenderPresent(game->renderer);

	SDL_FillRect(game->screen, NULL, game->bialy);
}

void CheckTime(Game* game)
{
	game->t2 = SDL_GetTicks();
	game->delta = (game->t2 - game->t1) * 0.001;
	game->t1 = game->t2;
	game->roundTime += game->delta;
}

void GameLogics(Game* game, Player* player)
{
	while (!game->quit)
	{
		while (player->numberOFLives < 1)
		{
			game->alternativeControls = false;
			Menu(game, player);
			GetPlayerInput(game, player);
		}
		if (game->wasPlayerHit)
		{
			game->alternativeControls = false;
			player->numberOFLives--;
			DeathScreen(game, player);
		}

		
		ConstGameActions(game, player);

		// handling of events (if there were any)
		GetPlayerInput(game, player);
	};
}

void GetPlayerInput(Game* game, Player* player)
{
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	while (SDL_PollEvent(&game->event))
	{
		switch (game->event.type)
		{
		case SDL_KEYDOWN:
			if (game->event.key.keysym.sym == SDLK_ESCAPE) exit(0);
			else if (game->event.key.keysym.sym == SDLK_z && player->canJump && !player->isJumping && !game->wasLifeLost && game->alternativeControls)
			{
				game->wasJumpKeyPressed = true;
				player->possibleJumps--;
				game->jumpTime = game->roundTime;
				player->Jump(game);
			}
			else if (game->event.key.keysym.sym == SDLK_n && !game->wasLifeLost)
			{
				initializeGame(game, player);
				game->isGameRunning = true;
				player->numberOFLives = NUMBEROFLIVES;
			}
			else if (game->event.key.keysym.sym == SDLK_x && player->canDash && !game->wasLifeLost && game->alternativeControls)
			{
				player->Dash(game);
			}
			else if (game->event.key.keysym.sym == SDLK_DOWN && !game->alternativeControls)
			{
				MoveAllV(game, player, -1, KEYMOVEMENTVERTICALSPEED);
			}
			else if (game->event.key.keysym.sym == SDLK_UP && !game->alternativeControls)
			{
				MoveAllV(game, player, 1, KEYMOVEMENTVERTICALSPEED);
			}
			else if (game->event.key.keysym.sym == SDLK_RIGHT && !game->alternativeControls)
			{
				for(int i=0; i<3*KEYMOVEMENTHORIZONTALSPEED; i++)
				MoveAllL(game, player);
			}
			else if (game->event.key.keysym.sym == SDLK_d && !game->wasLifeLost)
			{
				game->alternativeControls = !game->alternativeControls;
				game->jumpTime = game->roundTime;
			}
			else if (game->wasLifeLost && game->event.key.keysym.sym == SDLK_c)
			{
				ResetGame(game, player);
				game->wasPlayerHit = false;
				game->wasLifeLost = false;
				game->alternativeControls = false;
			}
			break;
		case SDL_QUIT:
			game->quit = 1;
			break;
		};
	};
}

void ConstGameActions(Game* game, Player* player)
{

	if ((player->posY - player->fallIndicator->posY) >= SCREEN_HEIGHT*2) game->wasPlayerHit = true;

	player->centerPosX = player->posX;
	player->centerPosY = player->posY;
	
	if (player->possibleJumps > 0) player->canJump = true;
	else player->canJump = false;
	
	game->playerSpeed = game->roundTime * 1 / ROUNDSPEEDMULTIPLIER + game->baseGameSpeed;

	CheckTime(game);

	CheckCollAll(game, player);

	if (game->isGravityON)
		Gravity(player, game);

	if(game->alternativeControls)
	MoveAllL(game, player);
	

	RenderScreen(game, player);
}

int initializeGame(Game* game, Player* player)
{
	
	game->platforms = new Platform * [NUMBEROFPLATFORMS];
	game->playerHearts = new GameObject * [NUMBEROFLIVES];
	player->fallIndicator = new GameObject;

	for (int i = 0; i < NUMBEROFLIVES; i++)
	{
		game->playerHearts[i] = new GameObject;
	}

	for (int i = 0; i < NUMBEROFPLATFORMS; i++)
	{
		game->platforms[i] = new Platform();
		game->platforms[i]->obstacle = new Obstacle();
		game->platforms[i]->stalaktyt = new Ground();
		game->platforms[i]->bump = new Ground();
	}
	

	
	

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return -1;
	}
	
	game->rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &game->window, &game->renderer);
	if (game->rc != 0)
	{
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return -1;
	};

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(game->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
	SDL_SetWindowTitle(game->window, "Unicorn Robot Attack");


	game->screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	game->scrtex = SDL_CreateTexture(game->renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);

	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	game->charset = SDL_LoadBMP("../UnicornRobotAttack/cs8x8.bmp");
	if (game->charset == NULL)
	{
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(game->screen);
		SDL_DestroyTexture(game->scrtex);
		SDL_DestroyWindow(game->window);
		SDL_DestroyRenderer(game->renderer);
		SDL_Quit();
		return -1;
	};
	SDL_SetColorKey(game->charset, true, 0x000000);

	game->t1 = SDL_GetTicks();

	player->surface = SDL_LoadBMP("../UnicornRobotAttack/Sprites/unicorn.bmp");
	if (player->surface == NULL)
	{
		printf("SDL_LoadBMP(unicorn.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(game->charset);
		SDL_FreeSurface(game->screen);
		SDL_DestroyTexture(game->scrtex);
		SDL_DestroyWindow(game->window);
		SDL_DestroyRenderer(game->renderer);
		SDL_Quit();
		return -1;
	};

	

	
	for (int i = 0;i < NUMBEROFLIVES; i++)
	{
		game->playerHearts[i]->surface = SDL_LoadBMP("../UnicornRobotAttack/Sprites/heart.bmp");
		if (game->playerHearts[i]->surface == NULL)
		{
			SDL_FreeSurface(game->charset);
			SDL_FreeSurface(game->screen);
			SDL_DestroyTexture(game->scrtex);
			SDL_DestroyWindow(game->window);
			SDL_DestroyRenderer(game->renderer);
			SDL_Quit();
			return -1;
		};
	}
	
	for (int i = 0;i < NUMBEROFPLATFORMS;i++)
	{
		game->platforms[i]->surface = SDL_LoadBMP("../UnicornRobotAttack/Sprites/platform.bmp");
		game->platforms[i]->obstacle->surface = SDL_LoadBMP("../UnicornRobotAttack/Sprites/obstacle.bmp");
		game->platforms[i]->stalaktyt->surface = SDL_LoadBMP("../UnicornRobotAttack/Sprites/stalaktyt.bmp");
		game->platforms[i]->bump->surface = SDL_LoadBMP("../UnicornRobotAttack/Sprites/highground.bmp");
		if (game->platforms[i]->surface == NULL || game->platforms[i]->obstacle->surface == NULL || game->platforms[i]->stalaktyt->surface == NULL || game->platforms[i]->bump->surface == NULL)
		{
			SDL_FreeSurface(game->charset);
			SDL_FreeSurface(game->screen);
			SDL_DestroyTexture(game->scrtex);
			SDL_DestroyWindow(game->window);
			SDL_DestroyRenderer(game->renderer);
			SDL_Quit();
			return -1;
		};
	}
	

	char text[128];
	int czarny = SDL_MapRGB(game->screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(game->screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(game->screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(game->screen->format, 0x11, 0x11, 0xCC);
	int bialy = SDL_MapRGB(game->screen->format, 0xe6, 0xcc, 0xff);

	game->czarny = czarny;
	game->zielony = zielony;
	game->czerwony = czerwony;
	game->niebieski = niebieski;
	game->bialy = bialy;

	player->posX = player->surface->w;
	player->posY = 0;

	player->originPosX = player->surface->w;
	player->originPosY = 0;
	
	player->fallIndicator->posY = SCREEN_HEIGHT / 2;
	player->fallIndicator->originPosY = player->fallIndicator->posY;


	
	srand(time(NULL));
	for (int i = 0; i < NUMBEROFPLATFORMS; i++)
	{
		if (i != 0)
		{
			int x = 0;
			if (rand() % 4 >= 2) x = -1;
			else x = 1;
			int dist = rand() % (JUMPDIST * player->jumpDistance/3) + (2 * game->platforms[i]->surface->h);

			game->platforms[i]->posX = game->platforms[i-1]->posX    + game->platforms[i]->surface->w/1.2;
			game->platforms[i]->posY = game->platforms[i-1]->posY - x *dist;
			
			game->platforms[i]->obstacle->posX = game->platforms[i]->posX + rand() % game->platforms[i]->surface->w / 2;
			game->platforms[i]->obstacle->posY = game->platforms[i]->posY - game->platforms[i]->obstacle->surface->h / 2;
			game->platforms[i]->stalaktyt->posX = game->platforms[i]->posX + rand()% game->platforms[i]->surface->w/2 - (rand() % game->platforms[i]->stalaktyt->surface->w);
			game->platforms[i]->stalaktyt->posY = game->platforms[i]->posY + game->platforms[i]->surface->h;
			game->platforms[i]->bump->posX = game->platforms[i]->posX - game->platforms[i]->surface->w / 4;
			game->platforms[i]->bump->posY = game->platforms[i]->posY - game->platforms[i]->surface->h / 2 - game->platforms[i]->bump->surface->h / 2;

			game->platforms[i]->originPosX = game->platforms[i]->posX;
			game->platforms[i]->originPosY = game->platforms[i]->posY;
			game->platforms[i]->obstacle->originPosX = game->platforms[i]->obstacle->posX;
			game->platforms[i]->obstacle->originPosY = game->platforms[i]->obstacle->posY;
			game->platforms[i]->stalaktyt->originPosX = game->platforms[i]->stalaktyt->posX;
			game->platforms[i]->stalaktyt->originPosY = game->platforms[i]->stalaktyt->posY;
			game->platforms[i]->bump->originPosX = game->platforms[i]->bump->posX;
			game->platforms[i]->bump->originPosY = game->platforms[i]->bump->posY;
		}
		else
		{
			game->platforms[i]->posX = SCREEN_WIDTH/2  + i * game->platforms[i]->surface->w;
			game->platforms[i]->posY = SCREEN_HEIGHT/2 - i * 5 * game->platforms[i]->surface->h;
			game->platforms[i]->obstacle->posX = game->platforms[i]->posX + rand() % game->platforms[i]->surface->w / 2;
			game->platforms[i]->obstacle->posY = game->platforms[i]->posY - game->platforms[i]->obstacle->surface->h / 2;
			game->platforms[i]->stalaktyt->posX = game->platforms[i]->posX + game->platforms[i]->surface->w / 4;
			game->platforms[i]->stalaktyt->posY = game->platforms[i]->posY + game->platforms[i]->surface->h;
			game->platforms[i]->bump->posX = game->platforms[i]->posX - game->platforms[i]->surface->w / 4;
			game->platforms[i]->bump->posY = game->platforms[i]->posY - game->platforms[i]->surface->h / 2 - game->platforms[i]->bump->surface->h / 2;

			game->platforms[i]->originPosX = game->platforms[i]->posX;
			game->platforms[i]->originPosY = game->platforms[i]->posY;
			game->platforms[i]->obstacle->originPosX = game->platforms[i]->obstacle->posX;
			game->platforms[i]->obstacle->originPosY = game->platforms[i]->obstacle->posY;
			game->platforms[i]->stalaktyt->originPosX = game->platforms[i]->stalaktyt->posX;
			game->platforms[i]->stalaktyt->originPosY = game->platforms[i]->stalaktyt->posY;
			game->platforms[i]->bump->originPosX = game->platforms[i]->bump->posX;
			game->platforms[i]->bump->originPosY = game->platforms[i]->bump->posY;
		}
		
	}

	for (int i = 0;i < NUMBEROFLIVES; i++)
	{
		game->playerHearts[i]->posX = game->screen->w / 2 - PLAYERLIVESPOSXOFFSET + i*2* game->playerHearts[i]->surface->w;
		game->playerHearts[i]->posY = PLAYERLIVESPOSY;
	}
	return 1;
}

void MoveObjectsLeft(Game* game, GameObject* gameObject,Player*player)
{
	if (gameObject->posX <= gameObject->originPosX -  game->platforms[0]->surface->w/1.2 * NUMBEROFPLATFORMS) ResetGame(game,player);
	else
		gameObject->posX -= game->playerSpeed;
}

void MoveObjectsVertical(Game* game, GameObject* gameObject, int dir, float speed)
{
	gameObject->posY += dir*speed;
}

void CheckColission(GameObject* gameObject, Game* game, Player* player)
{
	
	for (int i = 0; i < NUMBEROFPLATFORMS; i++)
	{
		game->platforms[i]->isPlayerColliding = false;
		
		game->platforms[i]->stalaktyt->isPlayerColliding  = false;
		game->platforms[i]->obstacle->isPlayerColliding = false;
		game->platforms[i]->bump->isPlayerColliding  = false;
	}
	
	if (player->centerPosY >= gameObject->posY - gameObject->surface->h / 2 && player->centerPosY < gameObject->posY + gameObject->surface->h / 2)
	{
		if (player->centerPosX + player->surface->w / 2 <= gameObject->posX + gameObject->surface->w / 2 && player->centerPosX + player->surface->w / 2 > gameObject->posX - gameObject->surface->w / 2 )
		{
			
			gameObject->isPlayerColliding = true;
			
			if (gameObject->objectType == 1)
			{
				if (!player->isDashing)
				{
					game->wasPlayerHit = true;
					player->posX = gameObject->posX - gameObject->surface->w / 2 - player->surface->w / 2;
				}
			}
			else
			{
				game->wasPlayerHit = true;
				player->posX = gameObject->posX - gameObject->surface->w / 2 - player->surface->w / 2;
			}
		}
	}
	//-----------------------------------------------------
	else if (player->centerPosX - player->surface->w/2 < gameObject->posX + gameObject->surface->w / 2 && player->centerPosX + player->surface->w / 2 >= gameObject->posX - gameObject->surface->w / 2)
	{
		if (player->posY + player->surface->h >= gameObject->posY && !(player->posY + player->surface->h >= gameObject->posY + gameObject->surface->h ))
		{
			if (gameObject->objectType == 1 && !player->isDashing)
			{
				game->wasPlayerHit = true;
			}
			else 
			{
				if(!player->isDashing)
				{
					gameObject->isPlayerColliding = true;
					player->posY = gameObject->posY - player->surface->h;
					
				}
				else
				{
					gameObject->isPlayerColliding = true;
				}
			}
			
		}
		else if (player->posY <= gameObject->posY + gameObject->surface->h && player->posY > gameObject->posY)
		{
			if (gameObject->objectType == 1 && !player->isDashing) game->wasPlayerHit = true;
			else
			{
				game->wasPlayerHit = true;
				player->posY = gameObject->posY + gameObject->surface->h;
				
			}
			
		}
		else
		{
			gameObject->isPlayerColliding = false;
			
		}
	}
	
	
//----------------------------------------------------------------------------
	

	
	for (int i = 0; i < NUMBEROFPLATFORMS; i++)
	{
		if (game->platforms[i]->isPlayerColliding) player->isColliding = true;
		if(game->platforms[i]->stalaktyt->isPlayerColliding) player->isColliding = true;
		if (game->platforms[i]->obstacle->isPlayerColliding)player->isColliding = true;
		if (game->platforms[i]->bump->isPlayerColliding)player->isColliding = true;
	}
	

	if (player->isColliding)
	{
		player->fallIndicator->posY = player->fallIndicator->originPosY;
		game->jumpTime = game->roundTime;
		player->possibleJumps = PLAYERJUMPS;
		game->isGravityON = false;
	}
	else if(game->alternativeControls) game->isGravityON = true;
}

void MoveAllL(Game* game, Player* player)
{
	
	for (int i = 0; i < NUMBEROFPLATFORMS; i++)
	{
		MoveObjectsLeft(game, game->platforms[i],player);
		MoveObjectsLeft(game, game->platforms[i]->obstacle,player);
		MoveObjectsLeft(game, game->platforms[i]->stalaktyt,player);
		MoveObjectsLeft(game, game->platforms[i]->bump,player);

	}
}

void MoveAllV(Game* game, Player* player,int dir, float speed)
{
	if ((player->posY >= 2 * (SCREEN_HEIGHT / 4) && dir < 0 ) || player->posY <= SCREEN_HEIGHT / 4 && dir > 0)
	{
		MoveObjectsVertical(game, player->fallIndicator, dir, speed);
		
		for (int i = 0; i < NUMBEROFPLATFORMS; i++)
		{
			MoveObjectsVertical(game, game->platforms[i], dir, speed);
			MoveObjectsVertical(game, game->platforms[i]->obstacle, dir, speed);
			MoveObjectsVertical(game, game->platforms[i]->stalaktyt, dir, speed);
			MoveObjectsVertical(game, game->platforms[i]->bump, dir, speed);
		}
	}
	else player->posY -= dir * speed;
}

void CheckCollAll(Game* game, Player* player)
{
	player->isColliding = false;

	
	for (int i = 0; i < NUMBEROFPLATFORMS; i++)
	{
		CheckColission(game->platforms[i], game, player);
		CheckColission(game->platforms[i]->obstacle, game, player);
		CheckColission(game->platforms[i]->stalaktyt, game, player);
		CheckColission(game->platforms[i]->bump, game, player);

	}
}

void DeathScreen(Game* game, Player* player)
{
	while (game->wasPlayerHit)
	{
		game->wasLifeLost = true;
		DrawRectangle(game->screen, 4, SCREEN_WIDTH / 6, SCREEN_WIDTH - 8, SCREEN_HEIGHT / 2, game->czerwony, game->niebieski);
		sprintf(game->text, "YOU HAVE DIED! CONTINUE?");
		DrawString(game->screen, game->screen->w / 2 - strlen(game->text) * 8 / 2, SCREEN_WIDTH / 4, game->text, game->charset);

		sprintf(game->text, "Esc - wyjscie   C - continue");
		DrawString(game->screen, game->screen->w / 2 - strlen(game->text) * 8 / 2, SCREEN_WIDTH / 3, game->text, game->charset);
		
		SDL_UpdateTexture(game->scrtex, NULL, game->screen->pixels, game->screen->pitch);
		SDL_RenderCopy(game->renderer, game->scrtex, NULL, NULL);
		SDL_RenderPresent(game->renderer);

		SDL_FillRect(game->screen, NULL, game->czarny);

		GetPlayerInput(game, player);
	}
}

void Menu(Game* game, Player* player)
{
	
	DrawRectangle(game->screen, 4, SCREEN_WIDTH / 6, SCREEN_WIDTH - 8, SCREEN_HEIGHT / 2, game->czerwony, game->niebieski);

	sprintf(game->text, "UNICORN ROBOT ATTACK");
	DrawString(game->screen, game->screen->w / 2 - strlen(game->text) * 8 / 2, SCREEN_WIDTH / 4, game->text, game->charset);

	sprintf(game->text, "Esc - wyjscie   N - nowa gra");
	DrawString(game->screen, game->screen->w / 2 - strlen(game->text) * 8 / 2, SCREEN_WIDTH / 3, game->text, game->charset);


	SDL_UpdateTexture(game->scrtex, NULL, game->screen->pixels, game->screen->pitch);
	SDL_RenderCopy(game->renderer, game->scrtex, NULL, NULL);
	SDL_RenderPresent(game->renderer);

	SDL_FillRect(game->screen, NULL, game->czarny);
}

void ResetGame(Game* game, Player* player)
{
	game->roundTime = 0;
	game->playerSpeed = PLAYERSPEED;

	player->posX = player->originPosX;
	player->posY = player->originPosY;

	player->fallIndicator->posY = player->fallIndicator->originPosY;


	for (int i = 0; i < NUMBEROFPLATFORMS; i++)
	{
		srand(time(NULL));

		game->platforms[i]->posX = game->platforms[i]->originPosX;
		game->platforms[i]->posY = game->platforms[i]->originPosY;
		game->platforms[i]->obstacle->posX = game->platforms[i]->obstacle->originPosX;
		game->platforms[i]->obstacle->posY = game->platforms[i]->obstacle->originPosY;
		game->platforms[i]->stalaktyt->posX = game->platforms[i]->stalaktyt->originPosX;
		game->platforms[i]->stalaktyt->posY = game->platforms[i]->stalaktyt->originPosY;
		game->platforms[i]->bump->posX = game->platforms[i]->bump->originPosX;
		game->platforms[i]->bump->posY = game->platforms[i]->bump->originPosY;

	}
	
}

// main
#ifdef __cplusplus
extern "C"
#endif

int main(int argc, char** argv)
{
	Game* game = new Game;

	Player* player = new Player;


	if (initializeGame(game, player) < 0) exit(0);

	while (!game->isGameRunning)
	{
		Menu(game, player);
		GetPlayerInput(game, player);
	}

	GameLogics(game, player);

	// zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(game->charset);
	SDL_FreeSurface(game->screen);
	SDL_DestroyTexture(game->scrtex);
	SDL_DestroyRenderer(game->renderer);
	SDL_DestroyWindow(game->window);


	SDL_Quit();

	return 0;
};

