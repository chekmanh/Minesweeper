#ifndef MULTIPLAYER_MINESWEEPER_GAME_H
#define MULTIPLAYER_MINESWEEPER_GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <thread>
#include <TGUI/Gui.hpp>

class State;

class Game {
 private:
   // states to handle different windows
  std::vector<std::unique_ptr<State>> states;
  // renderwindow to draw things
  std::unique_ptr<sf::RenderWindow> window;
  // global event to save resources
  sf::Event ev;
  // global texture map to save resources
  std::map<std::string, sf::Texture> textures;

  // track dt
  sf::Clock dtClock;
  sf::Time elapsed;
  // works fine for me
  sf::Time updateTime = sf::microseconds(100);
  // only render if true;
  bool changed;
  float dt;

  // push_back a main state
  void initMainState();
  // malloc window
  void initWindow();
  //    delta time for consistent movement speed
  void updateDt();

  // poll events
  void update();
  // draw sprites
  void render();

  // return to MainMenuState
  void backToMainMenu();

  // background
  sf::Texture tex;
  sf::Sprite background;

  // nice music on loop
  sf::Music music;
 public:
  // main method that checks states
  // push and pop when necessary 
  void run();
  Game();
  ~Game();
};

#endif
