#include "States/LoadingState.hpp"
#include "States/GamingState.hpp"
#include "Minesweeper/Player.hpp"

#include <iostream>

LoadingState::LoadingState(State &oldstate, bool &waiting, std::string text)
    : State(oldstate), waiting(waiting) {
  Player::loading = true;
  std::cout << "welcome to LoadingState" << std::endl;
  this->gui = std::make_unique<tgui::Gui>(*this->window);
  this->initTexture(text);
}

LoadingState::~LoadingState() {
  std::cout << "See you again in LoadingState" << std::endl;
}

void LoadingState::render() {
  // (*(this->states->end() - 2))->render(); //render background
  this->window->draw(this->loadSprite);
  this->players.at(0)->render(*this->window);
  this->players.at(1)->render(*this->window);
  this->gui->draw();
}

void LoadingState::updateSprite(float dt) {
  this->loadSprite.rotate(0.4);
  // std::this_thread::sleep_for(std::chrono::seconds(1));
  if (!(this->cnt++ % 20)) {
    this->players.at(0)->move(1, 0, 0);
    this->players.at(1)->move(-1, 0, 0);
  }
}

void LoadingState::update(float dt) {
  while (this->window->pollEvent(*this->ev)) {
    this->gui->handleEvent(*this->ev);
  }
  if (this->waiting) {
    this->quit = true;
  }
  this->updateSprite(dt);
}

void LoadingState::initTexture(const std::string &text) {

  Player::initFieldSize(12, 16);
  this->players.at(0) = std::make_unique<Player>(nullptr);
  this->players.at(1) = std::make_unique<Player>(nullptr);
  this->players.at(0)->shape.setPosition(0, 300);
  this->players.at(1)->shape.setPosition(0, 350);

  this->loadSprite.setTexture(this->textures->at("load_Screen"));
  this->loadSprite.setTextureRect(sf::IntRect(0, 0, 980, 926));
  this->loadSprite.setScale(0.09, 0.09);
  this->loadSprite.setOrigin(980 / 2, 926 / 2);
  this->loadSprite.setPosition(this->window->getSize().x / 2,
                               (this->window->getSize().y - this->loadSprite.getGlobalBounds().top) / 1.64);

  auto label = tgui::Label::create();
  label->setText(text);
  label->setTextSize(24);
  label->setPosition((this->window->getSize().x - label->getSize().x) / 2, "45%");
  this->gui->add(label);
  tgui::Button::Ptr button = tgui::Button::create();
  button->setText("quit");
  button->setPosition({(this->window->getSize().x - button->getSize().x) / 2, "85%"});
  button->connect("pressed", [this]() {
    for (int i = 1; i < this->states->size(); i++) {
      this->states->at(i)->exit();
    }
  });
  this->gui->add(button);

}
