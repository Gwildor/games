#include <stdio.h>
#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>

/* GLOBALS */
const int FPS = 20, screenW = 640, screenH = 480;
bool game_over, game_pause, game_main = true, game_end = false, up, left, right, down;
int segments = 3, counter;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_DISPLAY *display = NULL;

/* PROTOTYPES */
void end(int stage);
void key_down(int key);
void toggle(bool& var);

struct snake_segment {
	int x, y;
} snake[100];

/* IMPLEMENTATIONS */
int main() {
	
	if(!al_init()) {
		fprintf(stderr, "Failed to initialize allegro!\n");
		return -1;
	}

	if(!al_install_keyboard()) {
		fprintf(stderr, "Failed to initialize the keyboard!\n");
		return -1;
	}

	timer = al_create_timer(1.0 / FPS);
	if(!timer) {
		fprintf(stderr, "Failed to create timer!\n");
		return -1;
	}

	al_set_new_display_flags(ALLEGRO_WINDOWED);
	display = al_create_display(screenW, screenH);
	if(!display) {
		end(1);
		return -1;
	}

	event_queue = al_create_event_queue();
	if(!event_queue) {
		end(2);
		return -1;
	}
	
	if (!al_init_primitives_addon()) {
		end(3);
		return -1;
	}
	
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	
	al_clear_to_color(al_map_rgb(0,0,0));
	al_flip_display();
	al_start_timer(timer);
	
	al_init_font_addon();
	al_init_ttf_addon();
	ALLEGRO_FONT *ubuntu_16 = al_load_ttf_font("Ubuntu-R.ttf", 16, 0);
	
	/* COLORS */
	//ALLEGRO_COLOR red_color = al_map_rgb(184, 22, 22);
	//ALLEGRO_COLOR orange_color = al_map_rgb(255, 162, 0);
	ALLEGRO_COLOR green_color = al_map_rgb(109, 255, 0);
	ALLEGRO_COLOR dark_green_color = al_map_rgb(75, 175, 0);
	
	while (!game_end) {
		game_over   = false;
		game_pause  = false;
		up    = false;
		down  = false;
		right = false;
		left  = true;
		bool redraw = true;
		counter = 0;
		
		// start position for the snake
		for (int i = 0; i < segments; ++i) {
			if (i < 3) {
				snake[i].x = (screenW / 2) + (i * 16);
				snake[i].y = (screenH / 2);
				//std::cout << "(" << snake[i].x << ", " << snake[i].y << ")\n";
			} else { // reset old snake
				snake[i].x = 0;
				snake[i].y = 0;
			}
		}
		segments = 3;
		
		while (!game_over) {
			ALLEGRO_EVENT ev;
			al_wait_for_event(event_queue, &ev);
			
			if (ev.type == ALLEGRO_EVENT_TIMER) {
				redraw = true;
				if (!game_pause && !game_main) {
					++counter;
					// update pos
					if ((counter % 3) == 0) { // move snake one segment further
						counter = 0;
						for (int i = (segments - 1); i > 0; --i) {
							snake[i] = snake[i-1];
						}
						
						if (up) {
							snake[0].y = snake[0].y - 16;
						} else if (down) {
							snake[0].y = snake[0].y + 16;
						} else if (left) {
							snake[0].x = snake[0].x - 16;
						} else if (right) {
							snake[0].x = snake[0].x + 16;
						}
						
						if (snake[0].x <= -16 || snake[0].x >= screenW || snake[0].y <= -16 || snake[0].y >= screenH) {
							game_over = true;
							break;
						}
						
					}
				}
					
			} else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
				key_down(ev.keyboard.keycode);
			}	else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				game_over = true;
				game_end  = true;
			}
			
			if (redraw && al_is_event_queue_empty(event_queue)) {
				redraw = false;
				al_clear_to_color(al_map_rgb(0,0,0));
				
				if (game_main) {
					al_draw_text(ubuntu_16, al_map_rgb(255,255,255), (screenW / 2), (screenH / 2 - 48), ALLEGRO_ALIGN_CENTRE, "Snake");
					al_draw_text(ubuntu_16, al_map_rgb(255,255,255), (screenW / 2), (screenH / 2), ALLEGRO_ALIGN_CENTRE, "Press space to start");
					al_draw_text(ubuntu_16, al_map_rgb(255,255,255), (screenW / 2), (screenH / 2 + 20), ALLEGRO_ALIGN_CENTRE, "Press escape or enter/return to pause");
				} else { // playing
					// draw snake
					for (int i = 0; i < segments; ++i) {
						if (i == 0) {
							al_draw_filled_rectangle((snake[i].x + 1), (snake[i].y + 1), (snake[i].x + 15), (snake[i].y + 15), dark_green_color);
						} else {
							al_draw_filled_rectangle((snake[i].x + 1), (snake[i].y + 1), (snake[i].x + 15), (snake[i].y + 15), green_color);
						}
					}
					
					if (game_pause) {
						//al_draw_filled_rectangle((screenW / 2 - 50), (screenW / 2 + 50), (screenH / 2 - 30), (screenH / 2 + 30), orange_color);
						//al_draw_rectangle((screenW / 2 - 50), (screenW / 2 + 50), (screenH / 2 - 30), (screenH / 2 + 30), red_color, 2);
						al_draw_text(ubuntu_16, al_map_rgb(255,255,255), (screenW / 2), (screenH / 2 - 48), ALLEGRO_ALIGN_CENTRE, "Game paused");
						al_draw_text(ubuntu_16, al_map_rgb(255,255,255), (screenW / 2), (screenH / 2), ALLEGRO_ALIGN_CENTRE, "Press escape or enter/return to continue");
						al_draw_text(ubuntu_16, al_map_rgb(255,255,255), (screenW / 2), (screenH / 2 + 20), ALLEGRO_ALIGN_CENTRE, "Press space to restart");
					}
				}
				
				al_flip_display();
			}
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

void end(int stage) {
	switch (stage) {
		case 3:
		case 2:
			al_destroy_display(display);
		case 1:
			std::cout << "Failed at stage " << stage << "!\n";
			al_destroy_timer(timer);
	}
}

void key_down(int key) {
	if (key == ALLEGRO_KEY_UP && !down && !game_main && !game_pause) {
		up    = true;
		down  = false;
		left  = false;
		right = false;
	}
	if (key == ALLEGRO_KEY_DOWN && !up && !game_main && !game_pause) {
		up    = false;
		down  = true;
		left  = false;
		right = false;
	}
	if (key == ALLEGRO_KEY_LEFT && !right && !game_main && !game_pause) {
		up    = false;
		down  = false;
		left  = true;
		right = false;
	}
	if (key == ALLEGRO_KEY_RIGHT && !left && !game_main && !game_pause) {
		up    = false;
		down  = false;
		left  = false;
		right = true;
	}
	if ((key == ALLEGRO_KEY_ESCAPE || key == ALLEGRO_KEY_ENTER) && !game_main) {
		toggle(game_pause);
	}
	if (key == ALLEGRO_KEY_SPACE && game_main) {
		game_main  = false;
	}
	if (key == ALLEGRO_KEY_SPACE && game_pause) {
		game_over = true;
	}
}

void toggle(bool& var) {
	if (var) {
		var = false;
	} else {
		var = true;
	}
}
