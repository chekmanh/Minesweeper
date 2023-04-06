//
// Created by chelz on 5/28/20.
//

#ifndef MULTIPLAYER_MINESWEEPER_SETTINGSSTATE_HPP
#define MULTIPLAYER_MINESWEEPER_SETTINGSSTATE_HPP

#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>
#include "States/State.hpp"

/*
 * Render help text
 * Show necessary controls to user
 */
class SettingsState : public State {

 private:
  std::unique_ptr<tgui::Gui> gui;

  void initText();

  sf::Text text;
  sf::Font font;

 public:
  SettingsState(State &oldstate);
  ~SettingsState();
  void update(float dt);
  void render();

};

#endif //MULTIPLAYER_MINESWEEPER_SETTINGSSTATE_H
