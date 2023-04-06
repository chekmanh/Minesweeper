#include "States/SettingsState.hpp"
#include <iostream>

SettingsState::SettingsState(State &oldstate)
    : State(oldstate) {
  std::cout << "welcome to SettingsState" << std::endl;
  this->initText();
}

SettingsState::~SettingsState() {
  std::cout << "See you again in SettingsState" << std::endl;
}

void SettingsState::render() {
  this->window->draw(this->text);
}

void SettingsState::update(float dt) {
}

void SettingsState::initText() {
  this->font.loadFromFile("../resources/DejaVuSansMono.ttf");
  this->text.setFont(this->font);
  this->text.setCharacterSize(25);
  std::string str =
    "      This Game works just like the\n"
    "      classic Minesweeper you know.\n"
    "          standard keybindings:\n"
    "     W-A-S-D to move your character,\n"
    "     V to flag ans Space to open the\n"
    "                 field.\n"
    "             Vim bindings:\n"
    "     H-J-K-L to move, M to mark and\n"
    "              Enter to open.\n"
    "      Press Escape to go back to the\n"
    "                main-menu\n"
    "\n"
    "          for more information\n"
    "           please consult the\n"
    "             documentation\n"
    "";
  this->text.setString(str);
  this->text.setFillColor(sf::Color::Black);
  this->text.setPosition(0, 300);
}
