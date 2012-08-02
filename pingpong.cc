#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

struct ball {
	float x, y, dx, dy;
	int speed, angle;
};

int hittest(int x, int y, int pY, int nY);

int main() {
	
	const int FPS = 20, screenW = 640, screenH = 480;
	int scorePlayer = 0, scoreNPC = 0, pY = 200, nY = 200, move_count = 0, hit_test;
	bool redraw = true, game_end = false, up = false, down = false, valid_move = false;
	ball ball = {320, 240, 0, 0, 12, 0};
	srand(time(NULL));
	ball.angle = (rand() % 50 + 200);
	
	if(!al_init()) {
		fprintf(stderr, "Failed to initialize allegro!\n");
		return -1;
	}

	if(!al_install_keyboard()) {
		fprintf(stderr, "Failed to initialize the keyboard!\n");
		return -1;
	}
	
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_DISPLAY *display = NULL;

	timer = al_create_timer(1.0 / FPS);
	if(!timer) {
		fprintf(stderr, "Failed to create timer!\n");
		return -1;
	}

	al_set_new_display_flags(ALLEGRO_WINDOWED);
	display = al_create_display(screenW, screenH);
	if(!display) {
		al_destroy_timer(timer);
		return -1;
	}

	event_queue = al_create_event_queue();
	if(!event_queue) {
		al_destroy_timer(timer);
		al_destroy_display(display);
		return -1;
	}
	
	if (!al_init_primitives_addon()) {
		al_destroy_timer(timer);
		al_destroy_display(display);
		return -1;
	}
	
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	
	al_init_font_addon();
	al_init_ttf_addon();
	ALLEGRO_FONT *ubuntu_16 = al_load_ttf_font("Ubuntu-R.ttf", 16, 0);
	
	al_clear_to_color(al_map_rgb(0,0,0));
	al_flip_display();
	al_start_timer(timer);
	
	while (!game_end) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;
			
			if (ball.x < 15 || ball.x > 620) {
				if (ball.x < 15) {
					scoreNPC++;
				} else {
					scorePlayer++;
				}
				srand(time(NULL));
				ball.angle = (rand() % 50 + 200);
				ball.x = 320;
				ball.y = 240;
				pY = 200;
				nY = 200;
			}
			
			// get angle back in 0-360 range
			while (ball.angle < 0) {
				ball.angle += 360;
			}
			while (ball.angle > 360) {
				ball.angle -= 360;
			}
			
			if (up) {
				if ((pY - 6) <= 42) {
					pY = 42;
				} else {
					pY -= 6;
				}
			}
			if (down) {
				if ((pY + 6) >= 358) {
					pY = 358;
				} else {
					pY += 6;
				}
			}
			
			if (((ball.angle < 90 && ball.angle >= 0) || (ball.angle > 270 && ball.angle <= 360)) && ball.x > 320) {
				if (ball.y < nY + 40) {
					if ((nY - 6) <= 42) {
						nY = 42;
					} else {
						nY -= 6;
					}
				}
				if (ball.y > nY + 40) {
					if ((nY + 6) >= 358) {
						nY = 358;
					} else {
						nY += 6;
					}
				}
			}
			
			valid_move = false;
			move_count = 0;
			do {
				move_count++;
				if (move_count > 2) {
					srand(time(NULL));
					ball.angle = (rand() % 360);
				}
				ball.dx = ball.speed*cos(ball.angle*3.14159/180.0f);
				ball.dy = ball.speed*sin(ball.angle*3.14159/180.0f);
				hit_test = hittest((ball.x + ball.dx), (ball.y + ball.dy), pY, nY);
				if (hit_test == 0) {
					valid_move = true;
				} else {
					if (hit_test == 1) {
						ball.angle = 180 + ball.angle - ball.angle * 2 + 180;
					} else {
						ball.angle = 180 + ball.angle - ball.angle * 2;
					}
				}
			} while (!valid_move);
			
			ball.x += ball.dx;
			ball.y += ball.dy;
			
		} else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (ev.keyboard.keycode == ALLEGRO_KEY_UP) {
				up = true;
			}
			if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) {
				down = true;
			}
		}	else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			if (ev.keyboard.keycode == ALLEGRO_KEY_UP) {
				up = false;
			}
			if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) {
				down = false;
			}
		} else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			game_end = true;
		}
		
		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;
			al_clear_to_color(al_map_rgb(0,0,0));
			
			// draw score
			al_draw_textf(ubuntu_16, al_map_rgb(255, 255, 255), 20, 18, ALLEGRO_ALIGN_LEFT, "%d", scorePlayer);
			al_draw_textf(ubuntu_16, al_map_rgb(255, 255, 255), 620, 18, ALLEGRO_ALIGN_RIGHT, "%d", scoreNPC);
			
			// draw borders
			al_draw_filled_rectangle(20, 37, 620, 42, al_map_rgb(255, 255, 255));
			al_draw_filled_rectangle(20, 438, 620, 443, al_map_rgb(255, 255, 255));
			for (int x = 1; x < 11; x += 2) {
				al_draw_filled_rectangle(318, (42 + (x * 36)), 322, (42 + ((x + 1) * 36)), al_map_rgb(207, 207, 207));
			}
			
			// draw rackets
			al_draw_filled_rectangle(20, pY, 25, (pY + 80), al_map_rgb(255, 255, 255));
			al_draw_filled_rectangle(620, nY, 615, (nY + 80), al_map_rgb(255, 255, 255));
			
			// draw ball
			al_draw_filled_circle(ball.x, ball.y, 5, al_map_rgb(224, 215, 58));
			
			al_flip_display();
		}
		
	}
	
	if (timer) {
		al_destroy_timer(timer);
	}
	if (display) {
		al_destroy_display(display);
	}
	if (event_queue) {
		al_destroy_event_queue(event_queue);
	}
	
	return 0;
}

int hittest(int x, int y, int pY, int nY) {
	if (y - 5 <= 42) {
		return 1;
	}
	if (y + 5 >= 438) {
		return 1;
	}
	if (x - 5 <= 25 && x - 5 > 15 && y + 5 >= pY && y - 5 <= pY + 80) {
		return 2;
	}
	if (x + 5 <= 620 && x + 5 > 610 && y + 5 >= nY && y - 5 <= nY + 80) {
		return 2;
	}
	return 0;
}
