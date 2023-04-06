#include "States/HostState.hpp"
#include "States/GamingState.hpp"
#include "States/ChooseLayoutState.hpp"
#include "States/LoadingState.hpp"
#include <iostream>


HostState::HostState(State &oldstate)
    : State(oldstate) {
  std::cout << "welcome to HostState" << std::endl;
  this->gui = std::make_unique<tgui::Gui>(*this->window);
  this->initLabel();
}
HostState::~HostState() {
  delete this->conn;
  this->conn = nullptr;
  std::cout << "See you again in HostState" << std::endl;
}

// find your ip with ifconfig.me
std::string HostState::myip() {
  sf::Http http("http://ifconfig.me/");
  sf::Http::Request request;
  request.setUri("/ip");
  sf::Http::Response response = http.sendRequest(request);
  std::cout << response.getBody() << std::endl;
  // std::cout << response.getStatus()  << std::endl;
  return response.getBody();
}

void HostState::listenToConnection() {
  std::cout << "listen to connection" << std::endl;
  tgui::Theme theme{"../resources/themes/Mine.txt"};
  this->button->setRenderer(theme.getRenderer("button"));

  // bind the listener to a port

  this->waiting = false;
  this->states->push_back(std::make_unique<LoadingState>(*this, this->waiting,
                                                         "your ip is: " + this->myip()));
  std::thread t([&]() {
    sf::TcpListener listener;
    int players = this->numPlayers->getValue() + 1;
    if (listener.listen(this->slider->getValue()) != sf::Socket::Done) {
      std::cout << "listen failed on port " << this->slider->getValue() << std::endl;
      this->waiting = true;
      return;
    }
    std::cout << "listening for " << players - 1 << " players" << std::endl;
    for (int i = 0; i < players - 1; i++) {
      if (i) {
        this->conn->push_back(std::make_unique<sf::TcpSocket>());
      }

      std::cout << this->conn->size() << "und" << i << std::endl;
      if (listener.accept(*this->conn->at(i)) != sf::Socket::Done) {
        std::cout << "connecting failed" << std::endl;
        this->waiting = true;
        return;
      }
      sf::Packet msg;
      msg << (float) i + 1 << (float) players;
      this->conn->at(i)->send(msg);
      std::cout << "connected " << i << std::endl;
    }

    this->waiting = true;
    // GamingState::numPlayers = this->numPlayers->getValue();
    std::cout << "creating game" << std::endl;
    this->states->push_back(std::make_unique<ChooseLayoutState>(*this, SERVER, players));
  });
  t.detach();
}

void HostState::update(float dt) {
  while (this->window->pollEvent(*this->ev)) {
    this->gui->handleEvent(*this->ev);
    this->numlabel->setText(std::to_string((int) this->numPlayers->getValue() + 1) + " Players");
  }
}
void HostState::render() {
  // (*(this->states->begin()))->render(); //render background
  this->gui->draw();
}

void HostState::initLabel() {
  tgui::Label::Ptr label = tgui::Label::create();
  label->setText("Host a game");
  label->setTextSize(23);
  label->setPosition((this->window->getSize().x - label->getSize().x) / 2, "45%");
  this->gui->add(label);

  this->slider = tgui::Slider::create();
  this->slider->setSize({"30%", "12"});
  this->slider->setPosition({"35%", "85%"});
  this->slider->setMaximum(1238);
  this->slider->setMinimum(1234);
  this->gui->add(this->slider);

  auto slidelabel = tgui::Label::copy(label);
  slidelabel->setText("Port");
  slidelabel->setPosition({tgui::bindLeft(this->slider) + 60,
                           tgui::bindTop(this->slider) + 20});
  this->gui->add(slidelabel);

  this->numPlayers = tgui::Slider::create();
  this->numPlayers->setSize({"30%", "12"});
  this->numPlayers->setPosition({"35%", "70%"});
  this->numPlayers->setMinimum(1);
  this->numPlayers->setMaximum(9);
  this->gui->add(this->numPlayers);

  this->numlabel = tgui::Label::copy(label);
  this->numlabel->setText("2 Players");
  this->numlabel->setPosition({tgui::bindLeft(this->numPlayers) + 60,
                           tgui::bindTop(this->numPlayers) + 20});
  this->gui->add(numlabel);

  button = tgui::Button::create();
  this->button->setText("try to connect player");
  this->button->setPosition({(this->window->getSize().x - this->button->getSize().x) / 2, "60%"});
  this->button->connect("pressed", &HostState::listenToConnection, this);
  this->gui->add(this->button);
}
