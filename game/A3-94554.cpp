#include "../src/rsdl.hpp"
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <ctime> // Needed for the true randomization
using namespace std;

#define WINDOW_WIDTH 1317
#define WINDOW_HEIGHT 714
#define MAX_STRING_LENGTH 20
#define WINDOW_TITLE "Geometry Game"
#define BACK_SPACE 8
#define RETURN 13
#define ESC 27
#define LASERKEY 115
#define NORMALKEY 110
#define UP_ARROW 119
#define DOWN_ARROW 115
#define BOMB_KEY 98


#define TICK_DURATION 3
#define CURSOR_AGE 50
#define PI 3.14159265
#define NORMAL 0
#define LASER 1

#define EASY_ENEMY 1
#define NORMAL_ENEMY 2
#define HARD_ENEMY 3
#define NUM_SECONDS 0.25

#define ENEMY_WIDTH 38
#define ENEMY_HEIGHT 50
#define ENEMY_EASY_CHANCE 1
#define ENEMY_EASY_MAX 10
#define ENEMY_EASY_SCORE 1250
#define ENEMY_EASY_SCORE_GAINED 20
#define ENEMY_NORMAL_CHANCE 3
#define ENEMY_NORMAL_MAX 20
#define ENEMY_NORMAL_SCORE 750
#define ENEMY_NORMAL_SCORE_GAINED 40
#define ENEMY_HARD_CHANCE 9
#define ENEMY_HARD_MAX 30
#define ENEMY_HARD_SCORE 500
#define ENEMY_HARD_SCORE_GAINED 80

#define PARTICLE_WIDTH 12
#define PARTICLE_HEIGHT 20
#define PARTICLE_MAX 5
#define PARTICLE_TIME_MAX 200
#define PARTICLE_EASY_SCORE_GAINED 5
#define PARTICLE_NORMAL_SCORE_GAINED 10
#define PARTICLE_HARD_SCORE_GAINED 15

#define MISILE_NORMAL_WIDTH 16
#define MISILE_NORMAL_HEIGHT 19
#define MISILE_LASER_WIDTH 4
#define MISILE_LASER_HEIGHT 25

#define IMG_SQUARE "examples/assets/square.png"
#define IMG_BACKGROUND "examples/assets/background.jpg"
#define FONT_FREESANS "examples/assets/FreeSans.ttf"
#define FONT_CAPTURE "examples/assets/Capture_it.ttf"
#define IMG_DRONE "examples/assets/drone.png"
#define IMG_LASER_DRONE "examples/assets/laser_drone.png"
#define IMG_SHOOT "examples/assets/shoot.png"
#define IMG_PARTICLE "examples/assets/particle.png"
#define IMG_LASER "examples/assets/laser.png"
#define IMG_ENEMY "examples/assets/diamond.png"

typedef double Angle;

//mouse position 
struct Mouse_Position {
	int x, y;
};

struct Misile {
	double x, y;
	int mode;
	Angle angle;
};
typedef vector<Misile> Amo;

//one self defined struct which represents the square on screen.
struct Drone {
	double x,y;
	int width,height;
	int life;
	int mode;
	int score, lives, energy;
	Angle angle;
	Amo misile;
};

struct Enemy {
	double x, y;
	int type;
	Angle angle;
};
typedef vector<Enemy> Enemies;

struct Particle {
	double x, y;
	Angle angle;
	int time;
};
typedef vector<Particle> Particles;

struct Menu {
	string value;
	int padding_left;
};
typedef vector<Menu> Menu_vec;

struct Menus {
	int active_menu_item;
	bool game_start;
	Menu_vec menu_vec;
};

struct LeaderBoard {
	string name;
	string score;
};
typedef vector<LeaderBoard> LeadTable;

//Initializing the square.
Drone create_drone(int x, int y, int width, int height, int mode) {
	Drone drone;
	drone.x = x;
	drone.y = y;
	drone.width = width;
	drone.height = height;
	drone.mode = mode;
	drone.angle = 0;
	drone.score = 0;
	drone.energy = 0;
	drone.lives = 3;
	return drone;
}

string int_to_str(int num)
{
    stringstream ss;
    ss << num;
    return ss.str();
}

//This segment reflects the square when it reaches the borders.
void collide_with_horizontal_border(Amo& amo) {
    for (int i = 0; i < amo.size(); i++) {
		if (amo[i].y <= 0 || amo[i].y > (WINDOW_HEIGHT) ) {
			amo.erase (amo.begin()+ i);
		}
	}
}

void collide_with_vertical_border(Amo& amo) {
	for (int i = 0; i < amo.size(); i++) {
		if (amo[i].x <= 0 || amo[i].x > (WINDOW_WIDTH) ) {
			amo.erase (amo.begin()+ i);
		}
	}
}

void collide_with_borders(Particles& particles) {
    for (int i = 0; i < particles.size(); i++) {
		if (particles[i].y < 0 || particles[i].y > (WINDOW_HEIGHT) ) {
				particles[i].angle = fmod(180 - particles[i].angle, 360);
		}
		if (particles[i].x < 0 || particles[i].x > (WINDOW_WIDTH) ) {
				particles[i].angle = fmod(360 - particles[i].angle, 360);
		}
	}
}

double where_is_my_drone_head_x(Drone drone) {
	return drone.x + (drone.width / 2) - cos((drone.angle + 90) * PI / 180) * (drone.width / 2);
}

double where_is_my_drone_head_y(Drone drone) {
	return drone.y + (drone.height / 2) - sin((drone.angle + 90) * PI / 180) * (drone.height / 2);
}

double drone_area_restricted_x(Drone drone) {
	return drone.x + (drone.width / 2) + cos((drone.angle + 90) * PI / 180) * (drone.width / 2);
}
double drone_area_restricted_y(Drone drone) {
	return drone.y + (drone.height / 2) + sin((drone.angle + 90) * PI / 180) * (drone.height / 2);
}

//Move the drone for one frame.
void move_drone(Drone& drone, Mouse_Position mouse_position) {
	if (drone.mode == NORMAL) {
		if ((abs(mouse_position.x - where_is_my_drone_head_x(drone)) > 3  ||
					abs(mouse_position.y - where_is_my_drone_head_y(drone)) > 3) &&
				(abs(mouse_position.x - drone_area_restricted_x(drone)) > drone.width - 10  ||
					abs(mouse_position.y - drone_area_restricted_y(drone)) > drone.height - 10)) {
			drone.x -= cos((drone.angle + 90) * PI / 180) * 2; 
			drone.y -= sin((drone.angle + 90) * PI / 180) * 2;
		}
	}
}

Angle calculate_angle(int x1, int y1, int x2, int y2, Angle angle) {
	double x, y;
	x = (x2 - x1);
	y = (y2 - y1);

	if (abs(x) < .1 && abs(y) < .1) {
		return angle;
	}

	angle =  (atan2(y, x) * 180.00000) / PI + 90.00000;
	return angle;
}

void make_particle(Particles& particles, int x, int y) {
	srand (time(NULL));
	int number_of_particles = rand() % PARTICLE_MAX + 1;
	Particle p;
	for(int i = 0; i < number_of_particles; i++) {
		p.x = x;
		p.y = y;
		p.time = PARTICLE_TIME_MAX;
		p.angle = rand() % 360;
		particles.push_back(p);
		p = Particle();
	}
}

void draw_drone(window& win, Drone drone) {
	if (drone.mode == NORMAL) {
	  	win.draw_png(IMG_DRONE, drone.x, drone.y, drone.width, drone.height, drone.angle);
	} else if (drone.mode == LASER) {
		win.draw_png(IMG_LASER_DRONE, drone.x, drone.y, drone.width, drone.height, drone.angle);
	}
}

void draw_misile(window& win, Drone drone) {
	if (drone.misile.size() != 0) {
	  	for(int j = 0; j < drone.misile.size(); j++) {
	  		if (drone.misile[j].mode == NORMAL) {
	  			win.draw_png(IMG_SHOOT, drone.misile[j].x - MISILE_NORMAL_WIDTH / 2, drone.misile[j].y - MISILE_NORMAL_HEIGHT / 2, MISILE_NORMAL_WIDTH, MISILE_NORMAL_HEIGHT, drone.misile[j].angle);
	  		} else if (drone.misile[j].mode == LASER) {
	  			win.draw_png(IMG_LASER, drone.misile[j].x - MISILE_LASER_WIDTH / 2, drone.misile[j].y - MISILE_LASER_HEIGHT / 2, MISILE_LASER_WIDTH, MISILE_LASER_HEIGHT, drone.misile[j].angle);
	  		}
	  	}
	}
}

void draw_enemy(window& win, Enemies enemies) {
	for(int i = 0; i < enemies.size(); i++) {
		win.draw_png(IMG_ENEMY, enemies[i].x - ENEMY_WIDTH / 2, enemies[i].y - ENEMY_HEIGHT / 2, ENEMY_WIDTH, ENEMY_HEIGHT, enemies[i].angle);
	}
}

void draw_particles(window& win, Particles particles) {
	for(int i = 0; i < particles.size(); i++) {
		win.draw_png(IMG_PARTICLE, particles[i].x - PARTICLE_WIDTH / 2, particles[i].y - PARTICLE_HEIGHT / 2, PARTICLE_WIDTH, PARTICLE_HEIGHT, particles[i].angle);
	}
}

void draw_string(window& win,int x, int y, string text) {
	win.draw_rect(x - 5, y, (90), (22), WHITE);
	win.show_text(text, x, y, WHITE, FONT_FREESANS, 15);
}

void draw_text_field(window& win,int x, int y, string label, string text) {
	win.fill_rect(x, y, (80), (50), RED);
	win.show_text(label, x + 12, y + 12, WHITE, FONT_CAPTURE, 20);
	win.fill_rect(x + 80, y, (180), (50), WHITE);
	win.show_text(text, x + 85, y + 12, RED, FONT_CAPTURE, 20);
}

void draw_string_menu(window& win,int x, int y, string text,int active_one, int padding_left) {
	if (active_one == 1) {
		win.fill_rect(x, y, (220), (50), WHITE);
		win.show_text(text, x + padding_left, y + 8, RED, FONT_CAPTURE, 30);
	} else {
		win.draw_rect(x, y, (220), (50), WHITE);
		win.show_text(text, x + padding_left, y + 8, WHITE, FONT_CAPTURE, 30);
	}
}

void draw_lead_table(window& win, int x, int y, LeadTable lead_table) {
	win.fill_rect(x, y, 300, 50, WHITE);
	win.show_text("name", x + 75, y + 8, RED, FONT_CAPTURE, 30);
	win.fill_rect(x + 300, y, 300, 50, WHITE);
	win.show_text("score", x + 300 + 75, y + 8, RED, FONT_CAPTURE, 30);
	for(int i = 0; i < lead_table.size(); i++) {
		if (i == 5)
			break;
		win.draw_rect(x, y + (i + 1) * 50, 300, 50, WHITE);
		win.show_text(lead_table[i].name, x + 15, y + (i + 1) * 50 + 8, WHITE, FONT_CAPTURE, 30);
		win.draw_rect(x + 300, y + (i + 1) * 50, 300, 50, WHITE);
		win.show_text(lead_table[i].score, x + 300 + 15, y + (i + 1) * 50 + 8, WHITE, FONT_CAPTURE, 30);
	}
}

void draw_big_string(window& win,int x, int y, string text) {
	win.show_text(text, x, y, WHITE, FONT_CAPTURE, 150);
}

string prepare_output_text(string input_string, string lable) {
	return lable + input_string;
}

void draw_menu(window& win, Menus menu) {
	if(menu.menu_vec.size() > 1) {
		for (int i = 0; i < menu.menu_vec.size(); i++) {
			if(menu.active_menu_item == i + 1) {
				draw_string_menu(win, WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 100 + i * 100, menu.menu_vec[i].value, 1,menu.menu_vec[i].padding_left);
			} else {
				draw_string_menu(win, WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 100 + i * 100, menu.menu_vec[i].value, 0,menu.menu_vec[i].padding_left);
			}
		}
	} else if(menu.menu_vec.size() == 1) {
		draw_string_menu(win, WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT - 150, menu.menu_vec[0].value, 1,menu.menu_vec[0].padding_left);
	}
	
}

Menus main_menu_constructor() {
	Menu menu_main_menu;
	Menus menus_main_menu;

	menu_main_menu.value = "New Game";
	menu_main_menu.padding_left = 35;
	menus_main_menu.menu_vec.push_back(menu_main_menu);
	menu_main_menu.value = "LeaderBoard";
	menu_main_menu.padding_left = 10;
	menus_main_menu.menu_vec.push_back(menu_main_menu);
	menu_main_menu.value = "Exit";
	menu_main_menu.padding_left = 80;
	menus_main_menu.menu_vec.push_back(menu_main_menu);
	menus_main_menu.active_menu_item = 1;
	menus_main_menu.game_start = false;

	return menus_main_menu;
}

Menus newgame_menu_constructor() {
	Menu menu_new_game;
	Menus menus_new_game;

	menu_new_game.value = "Easy";
	menu_new_game.padding_left = 75;
	menus_new_game.menu_vec.push_back(menu_new_game);
	menu_new_game.value = "Normal";
	menu_new_game.padding_left = 55;
	menus_new_game.menu_vec.push_back(menu_new_game);
	menu_new_game.value = "Hard";
	menu_new_game.padding_left = 72;
	menus_new_game.menu_vec.push_back(menu_new_game);
	menu_new_game.value = "Back";
	menu_new_game.padding_left = 72;
	menus_new_game.menu_vec.push_back(menu_new_game);
	menus_new_game.active_menu_item = 1;
	menus_new_game.game_start = false;

	return menus_new_game;
}

Menus leaderboard_menu_constructor() {
	Menu menu_new_game;
	Menus menus_new_game;

	menu_new_game.value = "Back";
	menu_new_game.padding_left = 72;
	menus_new_game.menu_vec.push_back(menu_new_game);
	menus_new_game.active_menu_item = 1;
	menus_new_game.game_start = false;

	return menus_new_game;
}

bool compare_by_score(const LeaderBoard &a, const LeaderBoard &b) {
    return atoi(a.score.c_str()) > atoi(b.score.c_str());
}

LeadTable create_leadtable() {
	LeadTable lead_table;
	LeaderBoard leader_board;

	string STRING;
	ifstream infile;
	infile.open ("output.txt");
        while(getline(infile,STRING)) {
	        leader_board.name = STRING.substr(0, STRING.find(' '));
	        leader_board.score = STRING.substr(STRING.find(' ') + 1, STRING.length() - STRING.find(' '));
	        lead_table.push_back(leader_board);
        }
	infile.close();
	sort(lead_table.begin(), lead_table.end(), compare_by_score);
	return lead_table;
}

void menu_handler(Menus& menu, LeadTable& lead_table, bool& quit_flag, int& enemy_type) {
	if(menu.menu_vec[menu.active_menu_item - 1].value == "New Game") {
		menu = newgame_menu_constructor();
	} else if(menu.menu_vec[menu.active_menu_item - 1].value == "LeaderBoard") {
		lead_table = create_leadtable();
		menu = leaderboard_menu_constructor();
	} else if(menu.menu_vec[menu.active_menu_item - 1].value == "Exit") {
		quit_flag = true;
	} else if(menu.menu_vec[menu.active_menu_item - 1].value == "Easy") {
		menu.game_start = true;
		enemy_type = EASY_ENEMY;
	} else if(menu.menu_vec[menu.active_menu_item - 1].value == "Normal") {
		menu.game_start = true;
		enemy_type = NORMAL_ENEMY;
	} else if(menu.menu_vec[menu.active_menu_item - 1].value == "Hard") {
		menu.game_start = true;
		enemy_type = HARD_ENEMY;
	} else if(menu.menu_vec[menu.active_menu_item - 1].value == "Back") {
		menu = main_menu_constructor();
		lead_table.clear();
	} 	
}

void draw_dynamic_background(window& win, Drone drone, Enemies enemies, Particles particles, Menus menu, LeadTable& lead_table, string name, bool ready_to_play) {
	win.draw_bg(IMG_BACKGROUND, 0, 0);
	if(!menu.game_start) {
		draw_menu(win, menu);
		if(lead_table.size() > 0)
			draw_lead_table(win, 380, 100, lead_table);
	} else if (!ready_to_play && menu.game_start && drone.lives != 0) {
		draw_big_string(win, WINDOW_WIDTH / 2 - 300, WINDOW_HEIGHT / 2 - 100, "READY ?");
	} else {
		if (drone.lives == 0) {
			draw_string(win, 20, 5, prepare_output_text(int_to_str(drone.score), "Score : "));
			draw_text_field(win, 500, 300, "name", prepare_output_text(name, " "));
		} else {
			draw_drone(win, drone);
			draw_misile(win, drone);
			draw_enemy(win, enemies);
			draw_particles(win, particles);
			draw_string(win, 20, 5, prepare_output_text(int_to_str(drone.score), "Score : "));
			draw_string(win, 220, 5, prepare_output_text(int_to_str(drone.energy), "Energy : "));
			draw_string(win, 420, 5, prepare_output_text(int_to_str(drone.lives), "Lives : "));
		}
	}
}

void erase_last_char(string& input_string)
{
  if(input_string.size() > 0)
      input_string.erase(input_string.size() - 1);
}

void add_char(string& input_string, char c)
{
  if(input_string.size() < MAX_STRING_LENGTH)
    input_string += c;
}

void increase_score(Drone& drone, int type, int whos_score) {
	if (type == EASY_ENEMY) {
		if(whos_score == 1)
			drone.score += ENEMY_EASY_SCORE_GAINED;
		else
			drone.score += PARTICLE_EASY_SCORE_GAINED;
	} else if (type == NORMAL_ENEMY) {
		if(whos_score == 1)
			drone.score += ENEMY_NORMAL_SCORE_GAINED;
		else
			drone.score += PARTICLE_NORMAL_SCORE_GAINED;
	} else if (type == HARD_ENEMY) {
		if(whos_score == 1)
			drone.score += ENEMY_HARD_SCORE_GAINED;
		else
			drone.score += PARTICLE_HARD_SCORE_GAINED;
	}
}

void collision_misile_enemy(Drone& drone, Enemies& enemies, Particles& particles) {
	for (int i = 0; i < drone.misile.size(); i++) {
		for (int j = 0; j < enemies.size(); j++) {
			if (drone.misile[i].x > enemies[j].x - ENEMY_WIDTH / 2 && drone.misile[i].x < enemies[j].x + ENEMY_WIDTH / 2) {
				if (drone.misile[i].y > enemies[j].y - ENEMY_HEIGHT / 2 && drone.misile[i].y < enemies[j].y + ENEMY_HEIGHT / 2) {
					if (drone.misile[i].mode == NORMAL) {
						make_particle(particles, enemies[j].x, enemies[j].y);
						drone.misile.erase (drone.misile.begin() + i);
						enemies.erase (enemies.begin() + j);
						increase_score(drone, enemies[j].type, 1);
						drone.energy++;
						break;
					}
					make_particle(particles, enemies[j].x, enemies[j].y);
					enemies.erase (enemies.begin() + j);
					increase_score(drone, enemies[j].type, 1);
					drone.energy++;
				}
			}
		}
	}
}

void collision_particle_drone(Particles& particles, Drone& drone, int enemy_type) {
	for (int i = 0; i < particles.size(); i++) {
		if(particles[i].x > drone.x && particles[i].x < drone.x + drone.width) {
			if(particles[i].y > drone.y && particles[i].y < drone.y + drone.height){
				particles.erase (particles.begin() + i);
				increase_score(drone, enemy_type, 2);
				drone.energy++;
				break;
			}
		}
		if(particles[i].x > drone.x - 100 && particles[i].x < drone.x + drone.width + 100) {
			if(particles[i].y > drone.y - 100 && particles[i].y < drone.y + drone.height + 100)
				particles[i].angle = calculate_angle(particles[i].x, particles[i].y, drone.x + drone.width / 2, drone.y + drone.height / 2, particles[i].angle);
		}
	}
}

void collision_drone_enemy(Drone& drone, Enemies& enemies, Particles& particles, bool& ready_to_play) {
	for (int i = 0; i < enemies.size(); i++) {
		if (enemies[i].x + 10 > drone.x && enemies[i].x + 10 < drone.x + drone.width) {
			if (enemies[i].y + 6 > drone.y && enemies[i].y + 6 < drone.y + drone.height) {
				enemies.clear();
				particles.clear();
				drone.lives--;
				ready_to_play = false;
			}
		}
	}
}

void save_in_file(Drone drone, string name) {
    ofstream out("output.txt", ios_base::app);
    out << name << " " << drone.score << endl;
    out.close();
}

void collision(Drone& drone, Enemies& enemies, Particles& particles, int enemy_type, bool& quit_flag, bool& ready_to_play) {
	collision_misile_enemy(drone, enemies, particles);
	collision_drone_enemy(drone, enemies, particles, ready_to_play);
	collision_particle_drone(particles, drone, enemy_type);
}

Misile shoot_misile(Drone drone) {
	Misile misile;
	misile.x = drone.x + (drone.width / 2);
	misile.y = drone.y + (drone.height / 2);
	misile.angle = drone.angle;
	misile.mode = drone.mode;
	return misile;
}

void misile_move(Amo& amo) {
	for (int i = 0; i < amo.size(); i++) {
		if (amo[i].mode == NORMAL) {
			amo[i].x -= cos((amo[i].angle + 90) * PI / 180) * 6;
			amo[i].y -= sin((amo[i].angle + 90) * PI / 180) * 6;
			collide_with_horizontal_border(amo);
  			collide_with_vertical_border(amo);
		} else if (amo[i].mode == LASER) {
			amo[i].x -= cos((amo[i].angle + 90) * PI / 180) * 4;
			amo[i].y -= sin((amo[i].angle + 90) * PI / 180) * 4;
			collide_with_horizontal_border(amo);
  			collide_with_vertical_border(amo);
		}
		
	}
}

void move_particles(Particles& particles) {
	for (int i = 0; i < particles.size(); i++) {
		particles[i].x -= cos((particles[i].angle + 90) * PI / 180) * .4;
		particles[i].y -= sin((particles[i].angle + 90) * PI / 180) * .4;	
	}
	collide_with_borders(particles);
}

void enemy_creator (Enemies& enemies, Drone drone, int type, int max, int enemy_score, int chance, int drone_score) {
	if(enemies.size() < max + drone_score / enemy_score) {
		Enemy enemy;
		srand (time(NULL));
		enemy.x = rand() % WINDOW_WIDTH;
		enemy.y = rand() % WINDOW_HEIGHT;
		enemy.type = type;
		enemy.angle = calculate_angle(enemy.x, enemy.y, drone.x + drone.width / 2, drone.y + drone.height / 2, enemy.angle);
		if(!(enemy.x > drone.x - drone.width * 2 && enemy.x < drone.x + drone.width * 3) && 
			!(enemy.y > drone.y - drone.height * 2 && enemy.y < drone.y + drone.height * 3)){
			enemies.push_back(enemy);
		}
	}
}

void handle_particles_time(Particles& particles) {
	for(int i = 0; i < particles.size(); i++) {
		particles[i].time--;
		if(particles[i].time <= 0)
			particles.erase(particles.begin() + i);
	}
}

void create_enemy(Enemies& enemies, clock_t& last_time, double& time_counter, Drone drone, Particles& particles, int enemy_type) {
	clock_t this_time = clock();
	time_counter += (double)(this_time - last_time);
	last_time = this_time;
	if(time_counter > (double)(NUM_SECONDS * CLOCKS_PER_SEC) / 2) {
		handle_particles_time(particles);
		int probibility;
		srand (time(NULL));
		probibility = rand() % (ENEMY_EASY_CHANCE+ENEMY_NORMAL_CHANCE+ENEMY_HARD_CHANCE);
        time_counter -= (double)(NUM_SECONDS * CLOCKS_PER_SEC) / 2;
        if (enemy_type == EASY_ENEMY && probibility <= ENEMY_EASY_CHANCE) {
        	enemy_creator(enemies, drone, enemy_type, ENEMY_EASY_MAX, ENEMY_EASY_SCORE, ENEMY_EASY_CHANCE, drone.score);
        } else if (enemy_type == NORMAL_ENEMY && probibility <= ENEMY_NORMAL_CHANCE) {
        	enemy_creator(enemies, drone, enemy_type, ENEMY_NORMAL_MAX, ENEMY_NORMAL_SCORE, ENEMY_NORMAL_CHANCE, drone.score);
        } else if (enemy_type == HARD_ENEMY && probibility <= ENEMY_HARD_CHANCE){
        	enemy_creator(enemies, drone, enemy_type, ENEMY_HARD_MAX, ENEMY_HARD_SCORE, ENEMY_HARD_CHANCE, drone.score);
        }
    }
}

void move_enemy(Enemies& enemies, Drone drone) {
	for (int i = 0; i < enemies.size(); i++) {
		enemies[i].x -= cos((enemies[i].angle + 90) * PI / 180) * .2;
		enemies[i].y -= sin((enemies[i].angle + 90) * PI / 180) * .2;
		enemies[i].angle = calculate_angle(enemies[i].x, enemies[i].y, drone.x + drone.width / 2, drone.y + drone.height / 2, enemies[i].angle);
	}
}

void bomb_finisher(Drone& drone, Enemies& enemies, Particles& particles) {
	for (int j = 0; j < enemies.size(); j++) {
		make_particle(particles, enemies[j].x, enemies[j].y);
		increase_score(drone, enemies[j].type, 1);
		drone.energy++;
	}
	drone.energy += enemies.size();
	enemies.clear();
}

void process_rsdl_input(window& win, bool& quit_flag, string& input_string,
						 Mouse_Position& mouse_position, Drone& drone, Menus& menu, 
						 Enemies& enemies, LeadTable& lead_table, Particles& particles,
						  bool& ready_to_play, bool& first_init, int& enemy_type) {
	Event event = win.pollForEvent();

	if (event.type() == KEY_PRESS) {
		if (drone.lives == 0 && menu.game_start) {
			if(event.pressedKey() == BACK_SPACE)
		      erase_last_char(input_string);
		    else {
		      if(event.pressedKey() == RETURN){
		      	save_in_file (drone, input_string);
		      	drone = create_drone(300, 240, 65, 60, NORMAL);
		      	menu = main_menu_constructor();
		      } else
		        add_char(input_string, event.pressedKey());
			}
		} else {
			if (event.pressedKey() == LASERKEY && menu.game_start && ready_to_play) {
	    		drone.mode = LASER;
	    	} else if (event.pressedKey() == NORMALKEY && menu.game_start && ready_to_play) {
	    		drone.mode = NORMAL;
	    	} else if (event.pressedKey() == BOMB_KEY && menu.game_start && ready_to_play && drone.energy >= 200) {
	    		bomb_finisher(drone, enemies, particles);
	    		drone.energy -= 200;
	    	} else if (event.pressedKey() == UP_ARROW) {
	    		menu.active_menu_item = (menu.active_menu_item - 1) % menu.menu_vec.size();
	    		if (menu.active_menu_item == 0)
	    			menu.active_menu_item = menu.menu_vec.size();
	    	} else if (event.pressedKey() == DOWN_ARROW && !menu.game_start) {
	    		menu.active_menu_item = (menu.active_menu_item + 1) % menu.menu_vec.size();
	    		if (menu.active_menu_item == 0)
	    			menu.active_menu_item = menu.menu_vec.size();
	    	} else if (event.pressedKey() == RETURN && !menu.game_start) {
	    		menu_handler(menu, lead_table, quit_flag, enemy_type);
	    	} else {
	      		if(event.pressedKey() == ESC)
	      			quit_flag = true;
		      	if(menu.game_start) {
					ready_to_play = true;
					first_init = true;
				}
	    	}
		}
	} else if (event.type() == MMOTION) {
    	mouse_position.x = event.mouseX();
    	mouse_position.y = event.mouseY();
  	} else if (event.type() == LCLICK) {	
  		if(drone.mode == LASER && drone.energy >= 5) {
  			drone.misile.push_back(shoot_misile(drone));
  			drone.energy -= 5;
  		} else if(drone.mode == NORMAL){
  			drone.misile.push_back(shoot_misile(drone));
  		}
  	}
}

void run_input_capture_window(window& win, string& input_string, Menus menu) {	
	//Input Section
	Mouse_Position mouse_position;
	bool quit_flag = false, ready_to_play = false, first_init = false;
	Drone drone = create_drone(300, 240, 65, 60, NORMAL);
	Enemies enemies;
	Particles particles;
	double time_counter = 0;
	clock_t this_time;
    clock_t last_time;
    LeadTable lead_table;
    int enemy_type;

	while(!quit_flag) {
	    //Input Section
	    process_rsdl_input(win, quit_flag, input_string, mouse_position, drone, menu, enemies, lead_table, particles, ready_to_play, first_init, enemy_type);
	    //Simple Logic
	    if(ready_to_play) {
	    	if(first_init) {
	    		this_time = clock();
    			last_time = this_time;
    			first_init = false;
	    	}
	    	create_enemy(enemies, last_time, time_counter, drone, particles, enemy_type);
		   	drone.angle = calculate_angle(drone.x + (drone.width / 2), drone.y + (drone.height / 2), mouse_position.x, mouse_position.y, drone.angle);
		    move_drone(drone, mouse_position);
		    misile_move(drone.misile);
		    move_enemy(enemies, drone);
		    move_particles(particles);
		    collision(drone, enemies, particles, enemy_type, quit_flag, ready_to_play);
	    }
	    //Draw Section
	    win.clear();
	    draw_dynamic_background(win, drone, enemies, particles, menu, lead_table, input_string, ready_to_play);
	    //Update the current window.
	    win.update_screen();
	    //Delay your program for specific amount time.
	    Delay(TICK_DURATION);    
	}
}

int main() {
	window win(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
	string input_string;
	Menus menus;
	menus = main_menu_constructor();
	run_input_capture_window(win, input_string, menus);
}