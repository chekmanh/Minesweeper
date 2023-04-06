//
// Created by chelz on 5/13/20.
//

#ifndef MULTIPLAYER_MINESWEEPER_CHOOSELAYOUTSTATE_HPP
#define MULTIPLAYER_MINESWEEPER_CHOOSELAYOUTSTATE_HPP

#include "States/State.hpp"
#include <TGUI/TGUI.hpp>

enum Type : unsigned int;

/*
 * Most of the code is only handling GUI stuff
 */

class ChooseLayoutState : public State {
 private:
  // pass to GamingState
  int numPlayers;

  // BUILD, SERVER, CLIENT???
  Type type;
  // flexible parameters
  tgui::Slider::Ptr width;
  tgui::Slider::Ptr height;
  tgui::Slider::Ptr numBombs;

  tgui::Label::Ptr wLabel;
  tgui::Label::Ptr hLabel;
  tgui::Label::Ptr nLabel;

  // choose file
  tgui::ComboBox::Ptr files;

  std::unique_ptr<tgui::Gui> gui;
  // poll events
  void updateGui();

  void initGui();
  // click on random
  void initRandomField();
  // from file
  void initFileField();
  // from fixed parameters
  void initChooseField();

  // recursive, without points or normal?
  void gameType();

  // only choose one radio button
  tgui::RadioButtonGroup::Ptr group;

 public:
  ChooseLayoutState(State &oldState, Type type, int numPlayers = 0);
  ~ChooseLayoutState();
  void update(float dt);
  void render();
};
#endif //MULTIPLAYER_MINESWEEPER_CHOOSELAYOUTSTATE_H
