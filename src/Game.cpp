#include "Game.hpp"
#include <iostream>
#include "States/MainMenuState.hpp"

Game::Game() {
  this->initWindow();
  this->initMainState();
  this->tex.loadFromFile("../resources/textures/background.png");
  this->background.setTexture(this->tex);

  this->music.openFromFile("../resources/sounds/background.ogg");
  this->music.play();
  this->music.setVolume(6);
  this->music.setLoop(true);

  this->elapsed = sf::seconds(0);
}
void Game::initWindow() {
  this->window = std::make_unique<sf::RenderWindow>(sf::VideoMode(600, 800),
                                                    "Minesweeper", sf::Style::Close | sf::Style::Titlebar);
  // only to debug
  this->window->setFramerateLimit(60);

  this->window->setPosition(sf::Vector2(300, 300));
}

void Game::initMainState() {
  // usually you will use 4 states
  this->states.reserve(4);
  this->states.push_back(std::make_unique<MainMenuState>(this->window.get(), &this->states,
                                                         &this->textures, &this->ev));
}

void Game::updateDt() {
  this->dt = this->dtClock.restart().asMicroseconds();
  this->elapsed += this->dtClock.restart();
}

void Game::update() {
  this->updateDt();

  // while (this->elapsed > this->updateTime) {
  if (1) {
    this->changed = true;
    // this->elapsed -= this->updateTime;
    if (!this->states.empty()) {
      // if state wants to exit pop it
      if (this->states.back()->getQuit()) {
        this->states.pop_back();
      } else {
        this->states.back()->update(this->dt);
      }
    }
    // Application ends here
    else {
      this->window->close();
    }
  }

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && this->window->hasFocus()) {
    this->backToMainMenu();
    // don't register escape multiple times
    std::chrono::milliseconds timespan(140);
    std::this_thread::sleep_for(timespan);
  }
}

void Game::backToMainMenu() {
  for (int i = 1; i < this->states.size(); i++) {
    this->states.at(i)->exit();
  }
}

void Game::render() {
  if (this->changed) {
    this->changed = false;
    this->window->clear(sf::Color(135, 185, 41));
    this->window->draw(this->background);

    if (!this->states.empty()) {
      // render most recent state
      this->states.back()->render();
    }

    this->window->display();
  }
}


void Game::run() {
  // Game loop
  while (this->window->isOpen()) {
    this->update();
    this->render();
  }
}

Game::~Game() {
  std::cout << "End Application" << std::endl;
}
