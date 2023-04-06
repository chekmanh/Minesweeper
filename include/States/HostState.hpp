//
// Created by chelz on 5/13/20.
//

#ifndef MULTIPLAYER_MINESWEEPER_HOSTSTATE_HPP
#define MULTIPLAYER_MINESWEEPER_HOSTSTATE_HPP

#include <SFML/Network/TcpSocket.hpp>
#include <TGUI/Gui.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Slider.hpp>
#include "States/State.hpp"

class HostState : public State {

 private:
  std::unique_ptr<tgui::Gui> gui;

  // confirm parameters
  tgui::Button::Ptr button;
  // choose port
  tgui::Slider::Ptr slider;
  // choose number of Player, theoretically infinite Players(game is written
  // to be able to deal with an arbitrary number of players)
  tgui::Slider::Ptr numPlayers;
  tgui::Label::Ptr numlabel;
  void initLabel();

  // listen for a certain number of players
  void listenToConnection();

  // show ip from clients
  std::string myip();

  bool waiting;
 public:
  HostState(State &oldstate);
  ~HostState();
  void update(float dt);
  void render();

};

#endif //MULTIPLAYER_MINESWEEPER_HOSTSTATE_H
