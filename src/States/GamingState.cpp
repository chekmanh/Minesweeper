#include "States/GamingState.hpp"
#include "States/LoadingState.hpp"
#include "States/MainMenuState.hpp"
#include "Minesweeper/Field.hpp"
#include "Minesweeper/Field_content.hpp"
#include "Minesweeper/Player.hpp"
#include <iostream>
#include <fstream>

GamingState::GamingState(State &oldState, const std::string &filename, Type type, int numPlayers, int mynum)
    : State(oldState), type(type), numPlayers(numPlayers), mynum(mynum), turn(0) {
  std::cout << "welcome to game state numPlayers: " << numPlayers << "mynum: " << mynum << "type: " << type<< std::endl;
  this->initGame(filename);
}

GamingState::GamingState(State &oldState, int x, int y, int numBombs, Type type, int numPlayers, int mynum)
    : State(oldState), type(type), numPlayers(numPlayers), mynum(mynum), turn(0) {
  std::cout << "welcome to game state numPlayers: " << numPlayers << "mynum: " << mynum << "type: " << type<< std::endl;
  this->initGame(x, y, numBombs);
}

GamingState::GamingState(State &oldState, Type type, int numPlayers, int mynum)
    : State(oldState), type(type), numPlayers(numPlayers), mynum(mynum), turn(0) {
  std::cout << "welcome to game state numPlayers: " << numPlayers << "mynum: " << mynum << "type: " << type<< std::endl;

  std::cout << "type: " << type << std::endl;
  int x = std::rand() % 10 + 5;
  int y = std::rand() % 10 + 5;
  int numBombs = (std::rand() % (int) (x * y * 0.6)) + (int) (x * y * 0.2);

  this->initGame(x, y, numBombs);
}

void GamingState::initBuildConfig() {
  sf::Texture tex;
  tex.loadFromFile("../resources/textures/bomb.png");
  (*this->textures)["markshape"] = tex;

  tex.loadFromFile("../resources/textures/bomb_hint.png");
  (*this->textures)["bomb_hint"] = tex;

  this->mouseSprite.setTexture(this->textures->at("bomb_hint"));
  this->mouseSprite.setTextureRect(sf::IntRect(0, 0, 100, 100));
  this->mouseSprite.setScale(TILE_SIZE / 100, TILE_SIZE / 100);

  this->gui->setView(this->window->getView());
  this->initSoloAlternative();
  this->bindings[sf::Keyboard::Enter] = [&]() {
    this->field->players[0]->mark(*this->field);
  };
  this->bindings[sf::Keyboard::Space] = [&]() {
    this->field->players[0]->mark(*this->field);
  };
  this->name = tgui::EditBox::create();
  this->name->setDefaultText("filename");
  if (this->window->getSize().x < this->name->getSize().x) {
    this->name->setSize(100, 20);
  }
  this->name->setPosition((this->window->getSize().x - this->name->getSize().x) / 2,
                          this->window->getSize().y - TILE_SIZE);
  this->name->connect("ReturnKeyPressed", &GamingState::saveField, this);
  this->gui->add(this->name);
}
void GamingState::saveField() {
  std::ofstream file("../resources/fields/" + this->name->getText() + ".mine");
  if (!file.is_open()) {
    std::cout << "error gamingstate unable to open file: " << this->name->getText().toAnsiString() << std::endl;
  }
  for (int i = 0; i < this->field->height; i++) {
    for (int j = 0; j < this->field->width; j++) {
      if (this->field->field.at(j).at(i)->getMarked()) {
        file << "1 ";
      } else {
        file << "0 ";
      }
    }
    file << std::endl;
  }
  std::cout << "done" << std::endl;
}

void GamingState::initGame(const std::string &filename) {
  Field::build = false;
  Field::turn = &this->turn;
  this->initVariables();

  switch (this->type) {
    case LOCAL: { // offline
      this->numPlayers = 2;
      this->initField(filename);
      this->initMultiBindings();
      break;
    }
    case SERVER: { // server
      std::cout << "play online as server" << std::endl;
      this->initHostReadthread();
      this->writeField();
      this->initSoloAlternative();
      this->initField(filename);
      break;
    }
    case CLIENT: { // client
      std::cout << "play online Waiting for server" << std::endl;
      this->initSoloAlternative();
      this->initClientReadthread();
      break;
    }
    case BUILD: // build level
    {
      this->numPlayers = 1;
      Field::build = true;
      this->initField(filename);
      this->initBuildConfig();
      break;
    }
    default:std::cout << "GAMINGSTATE::ERROR::INVALID::TYPE: " << type << std::endl;
      break;
  }
}
void GamingState::initGame(int x, int y, int numBombs) {
  Field::build = false;
  Field::turn = &this->turn;
  this->initVariables();

  switch (this->type) {
    case LOCAL: {
      this->numPlayers = 2;
      this->initField(x, y, numBombs);
      this->initMultiBindings();
      break;
    }
    case SERVER: {
      std::cout << "play online as server" << std::endl;
      this->initHostReadthread();
      this->writeField();
      this->initSoloAlternative();
      this->initField(x, y, numBombs);
      break;
    }
    case CLIENT: { // client
      std::cout << "play online Waiting for server" << std::endl;
      this->initSoloAlternative();
      this->initClientReadthread();
      break;
    }
    case BUILD: {
      this->numPlayers = 1;
      Field::build = true;
      this->initField(x, y, numBombs);
      this->initBuildConfig();
      break;
    }
    default:std::cout << "GAMINGSTATE::ERROR::INVALID::TYPE: " << type << std::endl;
      break;
  }
}

void GamingState::initClientReadthread() {
  this->read = std::thread(&GamingState::readInBackgroundClient, this);
}

void GamingState::initHostReadthread() {
  for (int i = 0; i < this->conn->size(); i++) {
    this->hostThreads.push_back(std::thread(&GamingState::readInBackgroundHost, this, i));
  }
}

void FieldDeleter::operator()(Field *p) {
  delete p;
}

void GamingState::initField(int x, int y, int bombs) {
  this->field = std::unique_ptr<Field, FieldDeleter>(new Field(x,
                                                               y,
                                                               bombs,
                                                               this->window,
                                                               this->conn,
                                                               this->numPlayers,
                                                               this->mynum), FieldDeleter());
  this->fieldinit = true;
  this->initText();
}

void GamingState::initField(const std::string &filename) {
  std::ifstream file("../resources/fields/" + filename + ".mine");
  if (!file.is_open()) {
    std::cout << "could not open file" << filename << std::endl;
  }
  this->field =
      std::unique_ptr<Field, FieldDeleter>(new Field(file, this->window, this->conn, this->numPlayers, this->mynum),
                                           FieldDeleter());
  this->fieldinit = true;
  this->initText();
}

void GamingState::initText() { // TODO change to tgui
  this->endText.setCharacterSize(this->window->getSize().x / 13);
  this->endText.setFont(Normal_Field::font);
  this->endText.setFillColor(sf::Color::Red);

  if (Field_content::gameType != 3) {
    this->points.reserve(this->numPlayers);
    for (int i = 0; i < this->numPlayers; i++) {
      this->points.push_back(sf::Text());
      this->points.at(i).setFont(Normal_Field::font);
      this->points.at(i).setCharacterSize(0.8 * TILE_SIZE);
      if (this->mynum == i) {
        this->points.at(i).setFillColor(sf::Color::White);
      } else {
        this->points.at(i).setFillColor(sf::Color::Black);
      }
      this->points.at(i).setString("0");
    }
    for (int i = 0; i < this->points.size(); i++) {
      this->points.at(i).setPosition((this->window->getSize().x / (this->numPlayers)) * (i + 0.2),
          this->window->getSize().y - 1.0 * TILE_SIZE);
    }
  }

}
void GamingState::initVariables() {
  this->initBindings();
  sf::Texture tex;
  tex.loadFromFile("../resources/textures/mark.png");
  (*this->textures)["markshape"] = tex;
  std::cout << "welcome to gaming State" << std::endl;
  Field::textures = this->textures;
  Normal_Field::font.loadFromFile("../resources/DejaVuSansMono.ttf");

  this->gui = std::make_unique<tgui::Gui>(*this->window);
}


void GamingState::checkGui() {
  while (this->window->pollEvent(*this->ev)) {
    this->gui->handleEvent(*this->ev);
  }
}
void GamingState::pollEvents(float dt) {
  this->dt = dt;
  while (this->window->pollEvent(*this->ev)) {
    if (this->gui->handleEvent(*this->ev)) {
      continue;
    }
    switch (this->ev->type) {
      case sf::Event::Closed:this->window->close();
        break;
      case sf::Event::KeyPressed: {
        auto it = this->bindings.find(this->ev->key.code);
        if (it != this->bindings.end()) {
          it->second();
        }
      }
        break;
      default:break;
    }
  }
}

void GamingState::renderField(sf::RenderTarget &target) {
  this->field->render(target);
  this->gui->draw();
}
void GamingState::renderEndText(sf::RenderTarget &target) {
  target.draw(this->endText);
}
void GamingState::renderPoints(sf::RenderTarget &target) {
  for (const auto &i : this->points) {
    target.draw(i);
  }
}
void GamingState::render() {
  if (!this->fieldinit) {
    // std::cout << "waiting for server" << std::endl;
    return; // wait for field to be build
  }
  this->window->clear(sf::Color(135, 185, 41));
  this->renderField(*this->window);
  if (this->type != BUILD) {
    if (Field_content::gameType != 3) {
    this->renderPoints(*this->window);
    } 
  } else {
    this->window->draw(this->mouseSprite);
  }
  if (this->field->getStatus() != RUNNING) {
    this->renderEndText(*this->window);
  }
}

// list all players with max score, unless you hit a bomb
void GamingState::updateText() {
  std::string text;
  // std::cout << "mynum: " << this->mynum << " looser: " << this->looser << std::endl;
  if (this->conn) {
    if(Field_content::gameType == 3) {
      text = "Player " + std::to_string(this->looser + 1) + " lost";
    } else {
    // std::cout << "online update" << std::endl;
    // if no one hit a bomb list all top players
    if (this->field->remaining == 0) {
      this->looser = -1;
    }
    std::vector<int> winners;
    int bestScore = -1;
    for (int i = 0; i < this->numPlayers; i++) {
      if (i == this->looser) {
        continue;
      }
      if (bestScore <= this->field->players.at(i)->points) {
        if (bestScore == this->field->players.at(i)->points) {
          winners.push_back(i + 1);
        } else {
          winners.clear();
          bestScore = this->field->players.at(i)->points;
          winners.push_back(i + 1);
        }
      }
    }
    text = "Player ";
    text += std::to_string(winners.at(0));
    for (int i = 1; i < winners.size(); i++) {
      text += "/" + std::to_string(winners.at(i));
    }
    text += " won";
    }
  } else { // offline
    if (this->field->remaining) {
      switch (this->field->status) {
        case PLAYER_1_WON:text = "Player 1 won";
          break;
        case PLAYER_2_WON:text = "Player 2 won";
          break;
        default: text = "draw";
      }
    } else {
      std::cout << "no field remaining" << std::endl;
      if (this->field->players.at(0)->points < this->field->players.at(1)->points) {
        text = "Player 2 won";
      } else if (this->field->players.at(0)->points > this->field->players.at(1)->points) {
        text = "Player 1 won";
      } else {
        text = "draw";
      }
    }
  }
  // don not render again
  this->field->status = YOU_LOST;
  this->endText.setString(text);
  this->endText.setPosition((this->window->getSize().x - this->endText.getGlobalBounds().width) / 2,
                            this->window->getSize().y - 1.9 * TILE_SIZE);
}

void GamingState::updatePoints() {
  if (Field_content::gameType != 3) {
    for (int i = 0; i < this->points.size(); i++) {
      this->points.at(i).setString(std::to_string(this->field->players[i]->points));
    }
  }
}

void GamingState::playagain() {
  this->gui->setView(this->window->getView());
  this->accept = tgui::Button::create();
  this->accept->setText("Back to Menu");
  this->accept->setPosition((this->window->getSize().x - this->accept->getSize().x) / 2,
                            this->window->getSize().y * 0.7);
  this->accept->connect("pressed", [this]() {
    std::cout << "exit" << std::endl;
    for (int i = 1; i < this->states->size(); i++) {
      this->states->at(i)->exit();
    }
  });
  this->gui->add(this->accept);
  std::cout << "play again ?" << std::endl;
}

void GamingState::update(float dt) {
  if (this->fieldinit) {
    if (this->field->getStatus() != RUNNING || this->field->remaining == 0) {
      if (!this->updatedText) {
        this->updatedText = true;
        this->updatePoints();
        this->updateText();
        this->playagain();
      }
      this->checkGui();
      return;
    } else {
      // only update if one wants to build a level
      if (Field::build) {
        this->checkMouse();
      }
      this->updatePoints();
      this->pollEvents(dt);
    }
    return; // wait for field to be build
  } else {
    this->states->push_back(std::make_unique<LoadingState>(*this, this->fieldinit, "waiting for field"));
  }
}

void GamingState::checkMouse() {
  sf::Vector2i globalPosition = sf::Mouse::getPosition(*this->window);
  coordinates_t coo = Field::calcXY(globalPosition.x, globalPosition.y, this->field->width, this->field->height);
  if (coo.x == -1) {
    this->window->setMouseCursorVisible(true);
    return;
  } else {
    this->window->setMouseCursorVisible(false);
  }
  if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
    // left mouse button is pressed: shoot
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    this->field->markField(coo.x, coo.y);
  } else {
    this->mouseSprite.setPosition(coo.x * TILE_SIZE, coo.y * TILE_SIZE);
  }
}

GamingState::~GamingState() {
  this->window->setMouseCursorVisible(true);
  if (this->type == SERVER) {
    if (this->write.joinable()) {
      std::cout << "killed write" << std::endl;
      this->write.join();
    }
    for (auto &i : this->hostThreads) {
      i.join();
    }
  }
  if (this->read.joinable()) {
    std::cout << "KILLED read" << std::endl;
    this->read.join();
  }
  this->window->setView(static_cast<MainMenuState *>(this->states->at(0).get())->stdView);
  this->window->setSize(static_cast<MainMenuState *>(this->states->at(0).get())->stdWinSize);
}

void GamingState::writeField() {
  this->write = std::thread([this]() {
    while (!this->fieldinit) {
      // save resources
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    while (!this->quit) {
      sf::Packet msg;
      msg << "B";
      for (const auto &i : this->field->players) {
        msg << (float) i->points;
      }
      msg << (float) this->field->remaining;
      msg << this->field->currentState();
      for (const auto &i : *this->conn) {
        i->send(msg);
      }
      std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    std::cout << "write thread killed" << std::endl;
  });
}

// only for client
void GamingState::readInBackgroundClient() {
  this->conn->at(0)->setBlocking(false);
  while (!this->quit) {
    sf::Packet msg;
    if (this->conn->at(0)->receive(msg) != sf::Socket::Done) {
      continue;
    }
    std::string type;
    float x, y, who;
    msg >> type;
    if (type.length() == 0) {
      continue;
    }
    switch (type.at(0)) {
      case 'M':msg >> who;
        msg >> x >> y;
        this->field->players[who]->move(x, y);
        break;
      case 'O':
        {
          msg >> who;
          msg >> x >> y;
          this->field->openField((int) x, (int) y, this->field->players[who]->points, false);
        }
        break;
      case 'R':msg >> x >> y;
        this->field->markField((int) x, (int) y, 1);
        break;
      case 'E':msg >> x;
        this->looser = (int) x;
        std::cout << "recv E looser is: " << this->looser << std::endl;
        if (field->status == RUNNING) {
          this->field->showBombs();
        }
        this->field->status = ENDED;
        break;
      case 'C':
        this->field = std::unique_ptr<Field, FieldDeleter>
            (new Field(type, this->window, this->conn, this->numPlayers, this->mynum), FieldDeleter());
        this->fieldinit = true;
        this->initText();
        break;
      case 'B': // sync
      {
        for(const auto& i : this->field->players) {
          msg >> x;
          i->points = (int) x;
        }
        msg >> x;
        this->field->remaining = x;
        std::string text;
        msg >> text;
        this->field->check(text);
        break;
      }
      case 'I': {
        msg >> x;
        if (x > *Field::turn) {
          *Field::turn = x;
        }
        std::cout << "client recieved = " << *Field::turn << std::endl;
        break;
      }
      default:std::cout << "ERROR INVALID TYPE BACKGROUND: " << type << std::endl;
        break;
    }
  }
  std::cout << "readthread just died" << std::endl;
}

void GamingState::readInBackgroundHost(int i) {
  this->conn->at(i)->setBlocking(false);
  int number = i + 1;
  while (!this->quit) {
    sf::Packet msg;
    if (this->conn->at(i)->receive(msg) != sf::Socket::Done) {
      continue;
    }
    std::string type;
    float x, y, z;
    msg >> type;
    if (type.length() == 0) {
      continue;
    }
    switch (type.at(0)) {
      case 'M': {
        // ignore first 0
        msg >> x >> x >> y;
        this->field->players[number]->move(x, y);
        sf::Packet temp;
        temp << "M" << (float) number << x << y;
        // std::cout << "send move: " << number << std::endl;
        for (int j = 0; j < this->conn->size(); j++) {
          if (i != j) {
            this->conn->at(j)->send(temp);
          }
        }
        break;
      }
      case 'O': {
        msg >> z >> x >> y;
        this->field->openField((int) x, (int) y, this->field->players[number]->points, false);
        sf::Packet temp;
        temp << "O" << (float) number << x << y;
        for (int j = 0; j < this->conn->size(); j++) {
          if (i != j) {
            this->conn->at(j)->send(temp);
          }
        }
        break;
      }
      case 'R': {
        msg >> x >> y;
        this->field->markField((int) x, (int) y, 1);
        sf::Packet temp;
        temp << "R" << x << y; // TODO to i send i?
        for (int j = 0; j < this->conn->size(); j++) {
          if (i != j) {
            this->conn->at(j)->send(temp);
          }
        }
        break;
      }
      case 'E': {
        sf::Packet temp;
        this->looser = number;
        temp << "E" << (float) number;
        std::cout << "E send looser = " << (float) number << " number" << std::endl;
        for (int j = 0; j < this->conn->size(); j++) {
          if (i != j) {
            this->conn->at(j)->send(temp);
          }
        }
        this->field->status = ENDED;
        this->field->showBombs();
        break;
      }
      case 'I': {
        std::cout << "recieved host " << *Field::turn << std::endl;
        msg >> x;
        if (x > *Field::turn) {
          *Field::turn = x;
        }
        sf::Packet msg;
        msg << "I" << (float) *Field::turn;
        for(const auto& it : *this->conn) {
          it->send(msg);
        }
        std::cout << "tf= " << *Field::turn << std::endl;
        break;
      }
      default:std::cout << "ERROR INVALID TYPE BACKGROUND: " << type << std::endl;
        break;
    }
  }
  std::cout << "Host read thread just died" << std::endl;
}

void GamingState::initBindings() {
  this->bindings.emplace(sf::Keyboard::H, [&]() {
    this->field->players[this->mynum]->move(-1, 0, this->dt);
  });
  this->bindings.emplace(sf::Keyboard::J, [&]() {
    this->field->players[this->mynum]->move(0, 1, this->dt);
  });
  this->bindings.emplace(sf::Keyboard::K, [&]() {
    this->field->players[this->mynum]->move(0, -1, this->dt);
  });
  this->bindings.emplace(sf::Keyboard::L, [&]() {
    this->field->players[this->mynum]->move(1, 0, this->dt);
  });
  this->bindings.emplace(sf::Keyboard::Enter, [&]() {
    if (!this->field->players[this->mynum]->open(*this->field)) {
      this->field->showBombs();
      this->looser = this->mynum;
      this->field->status = PLAYER_2_WON;
    }
  });
  this->bindings.emplace(sf::Keyboard::M, [&]() {
    this->field->players[this->mynum]->mark(*this->field);
  });
}

void GamingState::initSoloAlternative() {
  this->bindings.emplace(sf::Keyboard::A, [&]() {
    this->field->players[this->mynum]->move(-1, 0, this->dt);
  });
  this->bindings.emplace(sf::Keyboard::S, [&]() {
    this->field->players[this->mynum]->move(0, 1, this->dt);
  });
  this->bindings.emplace(sf::Keyboard::W, [&]() {
    this->field->players[this->mynum]->move(0, -1, this->dt);
  });
  this->bindings.emplace(sf::Keyboard::D, [&]() {
    this->field->players[this->mynum]->move(1, 0, this->dt);
  });
  this->bindings.emplace(sf::Keyboard::Space, [&]() {
    if (!this->field->players[this->mynum]->open(*this->field)) {
      this->field->status = ENDED;
      this->looser = this->mynum;
      this->field->showBombs();
    }
  });
  this->bindings.emplace(sf::Keyboard::V, [&]() {
    this->field->players[this->mynum]->mark(*this->field);
  });
}
void GamingState::initMultiBindings() {
  this->bindings.emplace(sf::Keyboard::A, [&]() {
    this->field->players[1]->move(-1, 0, this->dt);
  });
  this->bindings.emplace(sf::Keyboard::S, [&]() {
    this->field->players[1]->move(0, 1, this->dt);
  });
  this->bindings.emplace(sf::Keyboard::W, [&]() {
    this->field->players[1]->move(0, -1, this->dt);
  });
  this->bindings.emplace(sf::Keyboard::D, [&]() {
    this->field->players[1]->move(1, 0, this->dt);
  });
  this->bindings.emplace(sf::Keyboard::Space, [&]() {
    if (!this->field->players[1]->open(*this->field)) {
      std::cout << "lost" << std::endl;
      this->field->status = PLAYER_1_WON;
      this->field->showBombs();
    }
  });
  this->bindings.emplace(sf::Keyboard::V, [&]() {
    this->field->players[1]->mark(*this->field);
  });
}

