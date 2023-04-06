#include "States/MainMenuState.hpp"
#include "States/GamingState.hpp"
#include "States/ChooseLayoutState.hpp"
#include "States/HostState.hpp"
#include "States/ClientState.hpp"
#include "States/SettingsState.hpp"

#include <iostream>
MainMenuState::MainMenuState(sf::RenderWindow *window, std::vector<std::unique_ptr<State>> *states,
                             std::map<std::string, sf::Texture> *texture, sf::Event *eva)
    : stdView(window->getView()), stdWinSize(window->getSize()) {
  std::cout << "welcome to MainMenuState" << std::endl;
  this->window = window;
  this->states = states;
  this->textures = texture;
  this->ev = eva;
  this->initVariables();
  this->initButtons();
  this->conn = nullptr;
}

void MainMenuState::initButtons() {
  auto button = tgui::Button::create();
  button->setPosition({"30%", "37%"});
  button->setSize({"40%", "10%"});
  button->setText("Local Game");
  button->connect("pressed", [&]() {
    this->states->push_back(std::make_unique<ChooseLayoutState>(*this, LOCAL));
  });
  this->gui->add(button);

  button = tgui::Button::copy(button);
  button->setPosition({"30%", "51%"});
  button->setText("Create level");
  button->connect("pressed", [&]() {
    this->states->push_back(std::make_unique<ChooseLayoutState>(*this, BUILD));
  });
  this->gui->add(button);

  button = tgui::Button::copy(button);
  button->setPosition({"30%", "65%"});
  button->setText("Host Game");
  button->connect("pressed", [&]() {
    this->conn = new std::vector<std::unique_ptr<sf::TcpSocket>>();
    this->conn->push_back(std::make_unique<sf::TcpSocket>());
    this->states->push_back(std::make_unique<HostState>(*this));
  });
  this->gui->add(button);

  button = tgui::Button::copy(button);
  button->setPosition({"30%", "79%"});
  button->setText("Join Game");
  button->connect("pressed", [&]() {
    this->conn = new std::vector<std::unique_ptr<sf::TcpSocket>>();
    this->conn->push_back(std::make_unique<sf::TcpSocket>());
    this->states->push_back(std::make_unique<ClientState>(*this));
  });
  this->gui->add(button);

  button = tgui::Button::create();
  button->setPosition({"2%", "96%"});
  // button->setSize({"40%", "10%"});
  button->setText("EXIT");
  button->connect("pressed", [&]() {
    this->quit = true;
  });
  this->gui->add(button);

  auto settings = tgui::BitmapButton::create();
  auto img = tgui::Texture("../resources/textures/settings.png");
  settings->setImage(img);
  settings->setImageScaling(0.8);
  settings->setSize("6%", "4%");
  settings->setPosition({"92%", "95%"});
  settings->connect("pressed", [this]() {
    this->states->push_back(std::make_unique<SettingsState>(*this));
  });
  this->gui->add(settings);
}

void MainMenuState::update(float dt) {
  this->conn = nullptr;
  this->pollEvents();
}

void MainMenuState::render() {
  this->gui->draw();
}

void MainMenuState::pollEvents() {
  while (this->window->pollEvent(*this->ev)) {
    if (this->ev->type == sf::Event::Closed) {
      this->window->close();
    }
    this->gui->handleEvent(*this->ev);
  }
}

MainMenuState::~MainMenuState() {
}
void MainMenuState::initVariables() {
  this->quit = false;

  this->gui = std::make_unique<tgui::Gui>(*this->window);
  gui->setFont("../resources/DejaVuSansMono.ttf");
  sf::Texture texture;
  texture.loadFromFile("../resources/textures/loadscreen.png");
  this->textures->emplace("load_Screen", texture);

  // texture.loadFromFile("../resources/textures/grass.jpg");
  // this->textures->emplace("Field_closed", texture);
  texture.loadFromFile("../resources/textures/green1.png");
  this->textures->emplace("Field_closed1", texture);
  texture.loadFromFile("../resources/textures/green2.png");
  this->textures->emplace("Field_closed2", texture);

  texture.loadFromFile("../resources/textures/brown1.png");
  this->textures->emplace("Field_open1", texture);
  texture.loadFromFile("../resources/textures/brown2.png");
  this->textures->emplace("Field_open2", texture);

  // texture.loadFromFile("../resources/textures/mark.png");
  // texture.setSmooth(1);
  // this->textures->emplace("markshape", texture);

  texture.loadFromFile("../resources/textures/bomb.png");
  this->textures->emplace("bomb", texture);

  texture.loadFromFile("../resources/textures/explosion.png");
  this->textures->emplace("explosion", texture);
}
