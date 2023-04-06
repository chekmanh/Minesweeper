#include "States/State.hpp"

State::State(State &oldState)
    : states(oldState.states),
      window(oldState.window),
      textures(oldState.textures),
      ev(oldState.ev),
      conn(oldState.conn),
      quit(false) {
  // annoying if you have 2 windows
  // this->window->setPosition(sf::Vector2(300, 300));
}

const bool State::getQuit() {
  return this->quit;
}

void State::exit() {
  this->quit = true;
}

State::State() {
}

State::~State() {
}

