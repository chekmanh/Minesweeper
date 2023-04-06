//
// Created by chelz on 5/13/20.
//

#ifndef MULTIPLAYER_MINESWEEPER_MAINMENUSTATE_CPP_HPP
#define MULTIPLAYER_MINESWEEPER_MAINMENUSTATE_CPP_HPP

#include <SFML/Network.hpp>
#include <TGUI/TGUI.hpp>
#include "States/State.hpp"

class Connection;

/*
 * MainMenuState is the main menu.
 * let user choose what to play next
 */
class MainMenuState : public State {
 private:
  std::unique_ptr<tgui::Gui> gui;
  //    void updateInput(const float &dt);
  void initVariables();
  void initButtons();
  void pollEvents();

 public:
  MainMenuState(sf::RenderWindow *window, std::vector<std::unique_ptr<State>> *states,
                std::map<std::string, sf::Texture> *texture, sf::Event *eva);
  ~MainMenuState();
  void update(float dt);
  void render();

  // for other states to resize the window back to default
  sf::View stdView;
  sf::Vector2u stdWinSize;

  bool waiting = false;
};
#endif //MULTIPLAYER_MINESWEEPER_MAINMENUSTATE_CPP_H
