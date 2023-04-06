//
// Created by chelz on 5/13/20.
//

#ifndef MULTIPLAYER_MINESWEEPER_STATE_HPP
#define MULTIPLAYER_MINESWEEPER_STATE_HPP

#include <memory>
#include <map>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Network.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

/*
 * Important construct to handle switching between different States
 */
class State {
 private:

 protected:
  sf::RenderWindow *window;
  std::vector<std::unique_ptr<State>> *states;

  // textures
  std::map<std::string, sf::Texture> *textures;

  sf::Event *ev;

  bool quit;

 public:
  std::vector<std::unique_ptr<sf::TcpSocket>> *conn = nullptr;

  State(State &oldState);
  State();
  virtual ~State();

  const bool getQuit();
  void exit();

  virtual void update(float dt) = 0;
  virtual void render() = 0;
};;

#endif //MULTIPLAYER_MINESWEEPER_STATE_H
