#include "../src/rsdl.hpp"
#include <cstdlib>
#include <vector>
#include <cmath>
#include <string>

using namespace std;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_TITLE "Spring96"

#define PEA_IMAGE "examples/assets/pvsz/pea.png"
#define PEA_SHOOTER_IMAGE "examples/assets/pvsz/Peashooter.png"
#define ZOMBIE_IMAGE "examples/assets/pvsz/Zombie.png"
#define FRONT_YARD_IMAGE "examples/assets/pvsz/Frontyard.jpg"

struct GameDetails
{
  int money;
  Tile focusedTile;
  vector<Zombie> zombies;
  vector<PeaShooter> peaShooters;
  vector<Pea> peas;
}

int main()
{
  window win(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
  while(true)
  {
    handle_user_input(win);

    handle_game_logic();

    draw_game_objects();

    delay();
  }
}
