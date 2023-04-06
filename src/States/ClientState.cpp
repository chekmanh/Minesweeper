#include <iostream>
#include <SFML/Network/IpAddress.hpp>
#include "States/ClientState.hpp"
#include "States/GamingState.hpp"

ClientState::ClientState(State &oldstate)
    : State(oldstate) {
  std::cout << "welcome to ClientState" << std::endl;
  this->gui = std::make_unique<tgui::Gui>(*this->window);
  this->initEditBox();
}

ClientState::~ClientState() {
  delete this->conn;
  this->conn = nullptr;
  std::cout << "See you again in ClientState" << std::endl;
}

void ClientState::update(float dt) {
  this->pollEvent();
}

void ClientState::render() {
  // (*(this->states->begin()))->render(); //render background
  this->gui->draw();
}

void ClientState::pollEvent() {
  while (this->window->pollEvent(*this->ev)) {
    this->gui->handleEvent(*this->ev);
  }
}

void ClientState::initEditBox() {
  this->editBox = tgui::EditBox::create();
  this->editBox->setDefaultText("127.0.0.1");
  this->editBox->setPosition({(this->window->getSize().x - editBox->getSize().x) / 2, "60%"});
  this->editBox->connect("ReturnKeyPressed", [&]() {
    if (!this->initConnection()) {
      // show error message
      std::thread t([this]() {
        this->editBox->setEnabled(false);
        tgui::Theme theme{"../resources/themes/Error.txt"};
        this->editBox->setRenderer(theme.getRenderer("editBox"));

        std::this_thread::sleep_for(std::chrono::milliseconds(600));
        theme = tgui::Theme{"../resources/themes/Mine.txt"};
        this->editBox->setRenderer(theme.getRenderer("editBox"));
        this->editBox->setEnabled(true);
      });
      t.detach();
    }
  });
  this->gui->add(this->editBox);

  tgui::Label::Ptr label = tgui::Label::create();
  label->setText("Join a game");
  label->setTextSize(23);
  label->setPosition((this->window->getSize().x - label->getSize().x) / 2, "45%");
  this->gui->add(label);

  this->portSlider = tgui::Slider::create();
  this->portSlider->setSize({"30%", "12"});
  this->portSlider->setPosition({"35%", "80%"});
  this->portSlider->setMaximum(1238);
  this->portSlider->setMinimum(1234);
  this->gui->add(this->portSlider);

  auto slidelabel = tgui::Label::copy(label);
  slidelabel->setText("Port");
  slidelabel->setPosition({tgui::bindLeft(this->portSlider) + 60,
                           tgui::bindTop(this->portSlider) + 20});
  this->gui->add(slidelabel);
}


// connect to host
bool ClientState::initConnection() {
  std::string temp = this->editBox->getText();
  // default to localhost
  // sf::IpAddress ip(temp.length() == 0 ? "127.0.0.1" : temp);
  sf::IpAddress ip(temp);
  if (this->conn == nullptr) {
    std::cout << "ClientState weird" << std::endl;
  }
  sf::Socket::Status status = this->conn->at(0)->connect(ip, this->portSlider->getValue());
  std::cout << "connecting" << std::endl;
  if (status == sf::Socket::Done) {
    std::cout << "connected to server" << std::endl;
    float mynum, numPlayers;
    while (1) {
      sf::Packet msg;
      this->conn->at(0)->receive(msg);
      msg >> mynum >> numPlayers;
      std::cout << "client recieved mynum: " << mynum << " numPlayers: " << numPlayers << std::endl;
      if ((int) mynum != 0 && (int) numPlayers != 0) {
        break;
      }
      continue;
    }
    this->states->push_back(std::make_unique<GamingState>(*this, CLIENT, numPlayers, mynum));

    return true;

  } else {
    std::cout << "connection failed" << std::endl;
    return false;
  }
}
