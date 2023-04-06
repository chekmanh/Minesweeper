//
// Created by chelz on 5/28/20.
//

#ifndef MULTIPLAYER_MINESWEEPER_LOADINGSTATE_HPP
#define MULTIPLAYER_MINESWEEPER_LOADINGSTATE_HPP

#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>
#include "States/State.hpp"

class Player;
class LoadingState : public State {

 private:
  // slow down background movement
  int cnt = 0;
  std::unique_ptr<tgui::Gui> gui;

  // condition to wait for
  bool &waiting;
  // rotating circle
  sf::Sprite loadSprite;

  // just for fun
  std::array<std::unique_ptr<Player>, 2> players;

  void initTexture(const std::string &text);
  void updateSprite(float dt);

 public:
  LoadingState(State &oldstate, bool &waiting, std::string text);
  ~LoadingState();
  void update(float dt);
  void render();

};

#endif //MULTIPLAYER_MINESWEEPER_LOADINGSTATE_H
