#ifndef GAME_H 
#define GAME_H 

typedef struct
{
	void (*draw_start)(void);
	void (*draw_stop)(void);
	void (*draw_pixel)(uint8_t, uint8_t, bool);
} GameConfig;

void game_run(GameConfig *cfg);

#endif /* GAME_H */