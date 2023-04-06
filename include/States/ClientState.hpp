//
// Created by chelz on 5/14/20.
//

#ifndef MULTIPLAYER_MINESWEEPER_CLIENTSTATE_HPP
#define MULTIPLAYER_MINESWEEPER_CLIENTSTATE_HPP

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <TGUI/TGUI.hpp>
#include "State.hpp"

class ClientState : public State {

 private:
  std::unique_ptr<tgui::Gui> gui;
  // enter ip from host
  tgui::EditBox::Ptr editBox;
  // choose right port
  tgui::Slider::Ptr portSlider;

  // connect to server
  bool initConnection();
  // update GUI
  void pollEvent();

  void initEditBox();

 public:
  ClientState(State &oldstate);
  ~ClientState();
  void update(float dt);
  void render();
};

#endif //MULTIPLAYER_MINESWEEPER_CLIENTSTATE_H
