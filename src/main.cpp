#include "Game.hpp"

int main() {
  std::srand(time(nullptr));

  // personally designed Tgui theme
  tgui::Theme theme{"../resources/themes/Mine.txt"};
  tgui::Theme::setDefault(&theme);

  Game game;
  game.run();
}
