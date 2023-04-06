#include "Minesweeper/Field.hpp"
#include "Minesweeper/Player.hpp"
#include "Minesweeper/Field_content.hpp"
#include <fstream>
#include <iostream>

bool Field::build = false;
int* Field::turn;

std::map<std::string, sf::Texture> *Field::textures = nullptr;

Field::Field(sf::RenderWindow *window, std::vector<std::unique_ptr<sf::TcpSocket>> *conn, int numPlayers, int mynum)
    : window(window), conn(conn), numPlayers(numPlayers), mynum(mynum) {
  Player::loading = false;
  this->font.loadFromFile("../resources/DejaVuSansMono.ttf");
  Field_content::initConn(conn);

  this->haloTex.loadFromFile("../resources/textures/halo.png");
  this->halo.setTexture(this->haloTex);
  this->halo.setTextureRect(sf::IntRect(0, 0, 100, 100));
  this->halo.setScale(TILE_SIZE / 100, TILE_SIZE / 100);
}

Field::Field(int x, int y, int numBombs, sf::RenderWindow *window, std::vector<std::unique_ptr<sf::TcpSocket>> *conn,
             int numPlayers, int mynum)
    : Field(window, conn, numPlayers, mynum) {
  this->width = x;
  this->height = y;
  this->remaining = x * y - numBombs;
  // try to optimize push_back operations
  this->field.reserve(x);
  for (int i = 0; i < x; i++) {
    this->field.push_back(std::vector<std::unique_ptr<Field_content>>());
    this->field.at(i).reserve(y);
    for (int j = 0; j < y; j++) {
      this->field.at(i).push_back(std::make_unique<Normal_Field>(i, j));
    }
  }
  for (int i = 0; i < numBombs;) {
    int x1 = rand() % x;
    int y1 = rand() % y;
    // set random bombs
    if (dynamic_cast<Normal_Field *>(this->field.at(x1).at(y1).get())) {
      this->field.at(x1).at(y1).reset(new Bomb(x1, y1));
      i++;
    }
  }
  std::ofstream file("../resources/fields/tempField.debug");
  std::stringstream ss;
  ss << "C ";
  if (file.is_open()) {
    ss << this->width << " " << this->height << " " << Field_content::gameType << " ";
    for (int i = 0; i < y; i++) {
      for (int j = 0; j < x; j++) {
        if (dynamic_cast<Bomb *>(this->field.at(j).at(i).get())) {
          ss << "1 ";
          file << "1 "; // top to bottom
        } else {
          ss << "0 ";
          file << "0 ";
        }
        this->field.at(j).at(i)->calcnumBombs(this->field);
      }
      file << std::endl;
    }
    std::cout << "init field" << std::endl;
    if (this->conn) {
      sf::Packet msg;
      msg << ss.str();
      for (auto const &i : *this->conn) {
        i->send(msg);
        std::cout << "send a field" << std::endl;
      }
    }
    file.close();
  }
  this->initPlayers();
  this->initWindow(this->width, this->height);
}

Field::Field(std::ifstream &file,
             sf::RenderWindow *window,
             std::vector<std::unique_ptr<sf::TcpSocket>> *conn,
             int numPlayers,
             int mynum)
    : Field(window, conn, numPlayers, mynum) {
  std::stringstream ss;
  std::string line;
  int x = 0;
  int y = 0;
  int numBombs = 0;
  this->height = 0;
  int type;
  // read file line by line
  if (std::getline(file, line)) {
    std::stringstream temp(line);
    while (temp >> type) {
      this->field.push_back(std::vector<std::unique_ptr<Field_content>>());
      if (type) {
        this->field.at(x).push_back(std::make_unique<Bomb>(x, y));
        ss << "1 ";
        numBombs++;
      } else {
        ss << "0 ";
        this->field.at(x).push_back(std::make_unique<Normal_Field>(x, y));
      }
      x++;
    }
    this->width = x;
    y++;
    x = 0;
  }
  // optimize push_back
  while (std::getline(file, line)) {
    ss << std::endl;
    std::stringstream temp(line);
    this->field.at(x).reserve(this->width);
    while (temp >> type) {
      if (type) {
        this->field.at(x).push_back(std::make_unique<Bomb>(x, y));
        ss << "1 ";
        numBombs++;
      } else {
        ss << "0 ";
        this->field.at(x).push_back(std::make_unique<Normal_Field>(x, y));
      }
      x++;
    }
    y++;
    x = 0;
  }
  file.close();
  this->height = y;
  this->remaining = this->width * this->height - numBombs;
  std::stringstream tosend;
  tosend << "C " + std::to_string(this->width) + " " + std::to_string(this->height) + " "
      + std::to_string(Field_content::gameType) + " " + ss.str();
  std::cout << "field from ifstream" << std::endl;
  if (this->conn) {
    sf::Packet msg;
    msg << tosend.str();
    for (auto const &i : *this->conn) {
      i->send(msg);
      std::cout << "send a field" << std::endl;
    }
  }
  for (auto &i : this->field) {
    for (auto &j : i) {
      j->calcnumBombs(this->field);
    }
  }
  this->initPlayers();
  this->initWindow(this->width, this->height);
}

Field::Field(std::string const &code,
             sf::RenderWindow *window,
             std::vector<std::unique_ptr<sf::TcpSocket>> *conn,
             int numPlayers,
             int mynum)
    : Field(window, conn, numPlayers, mynum) {
  std::cout << "Field received from server" << std::endl;
  int x, y, t;
  int type;
  int numBombs = 0;
  std::stringstream ss(code);
  char ul;
  ss >> ul;
  ss >> x >> y >> t;
  std::cout << "x: " << x << " y: " << y << " t: " << t << std::endl;
  Field_content::gameType = t;
  std::ofstream file("../resources/fields/RecievedField.debug"); // debug check
  if (!file.is_open()) {
    std::cout << "FIELD::UNABLE TO OPEN RECIEVEDFIELD" << std::endl;
  }
  this->field.reserve(x);
  for (int i = 0; i < x; i++) {
    this->field.push_back(std::vector<std::unique_ptr<Field_content>>());
    // optimize push_back
    this->field.at(i).reserve(y);
    ss >> type;
    if (type == 0) {
      file << "0 ";
      this->field.at(i).push_back(std::make_unique<Normal_Field>(i, 0));
    } else {
      file << "1 ";
      this->field.at(i).push_back(std::make_unique<Bomb>(i, 0));
      numBombs++;
    }
  }
  file << std::endl;
  for (int j = 1; j < y; j++) {
    for (int i = 0; i < x; i++) {
      ss >> type;
      if (type == 0) {
        file << "0 ";
        this->field.at(i).push_back(std::make_unique<Normal_Field>(i, j));
      } else {
        file << "1 ";
        this->field.at(i).push_back(std::make_unique<Bomb>(i, j));
        numBombs++;
      }
    }
    file << std::endl;
  }
  file.close();

  this->remaining = x * y - numBombs;
  this->width = x;
  this->height = y;

  for (auto &x : this->field) {
    for (auto &y : x) {
      y->calcnumBombs(this->field);
    }
  }
  this->initPlayers();
  this->initWindow(this->width, this->height);
}

Field::~Field() {
  this->quit = true;
  if (this->animBombs.joinable()) {
    this->animBombs.join();
  }
}

// synchronize field with Host
void Field::check(std::string text) {
  std::stringstream ss(text);
  char a;
  for (int i = 0; i < this->width; i++) {
    for (int j = 0; j < this->height; j++) {
      ss >> a;
      if (a == '1') { // offen
        if (!this->field.at(i).at(j)->isOpen) {
          static_cast<Normal_Field *>(this->field.at(i).at(j).get())->open();
        }
      } else if (a == '2') { // marked && zu
        if (!this->field.at(i).at(j)->marked) {
          this->field.at(i).at(j)->setMarked();
        }
      } else { // zu && !marked
        if (this->field.at(i).at(j)->isOpen) {
          this->field.at(i).at(j)->isOpen = false;
          this->field.at(i).at(j)->initShape();
        }
        if (this->field.at(i).at(j)->marked) {
          this->field.at(i).at(j)->setMarked();
        }
      }
    }
  }
}

// synchronize field
std::string Field::currentState() {
  std::stringstream ss;
  for (int i = 0; i < this->width; i++) {
    for (int j = 0; j < this->height; j++) {
      if (this->field.at(i).at(j)->isOpen) {
        ss << 1 << " ";
      } else if (this->field.at(i).at(j)->getMarked()) {
        ss << 2 << " ";
      } else {
        ss << 0 << " ";
      }
    }
    ss << std::endl;
  }
  return ss.str();
}

// animate all bombs in random order
void Field::showBombs() {
  Bomb::end = true;
  std::cout << "showing bobms" << std::endl;

  this->animBombs = std::thread([this]() {
    std::vector<int> vec(this->height * this->width);
    for (int i = 0; i < vec.size(); i++) {
      vec.at(i) = i;
    }
    std::random_shuffle(vec.begin(), vec.end());
    for (const auto &it : vec) {
      if (this->quit) {
        std::cout << "quit showbobms thread" << std::endl;
        return;
      }
      // std::cout << "animating" << std::endl;
      this->field.at(it % this->width).at(it / this->width)->animate();
    }
  });
}

// resize window
void Field::initWindow(int x, int y) {
  sf::FloatRect visibleArea(0, 0, x * TILE_SIZE, (y + 2) * TILE_SIZE);
  this->window->setSize(sf::Vector2u(x * TILE_SIZE, (y + 2) * TILE_SIZE));
  this->window->setView(sf::View(visibleArea));
}

void Field::renderPlayer(sf::RenderTarget &target) {
  if (Field_content::gameType == 3) {
    this->halo.setPosition(this->players.at(*Field::turn % this->numPlayers)->shape.getPosition());
    target.draw(halo);
  }
  for (const auto &i : this->players) {
    i->render(target);
  }
}
void Field::initPlayers() {
  Player::initFieldSize(this->width, this->height);
  Player::playernum = 0;
  for (int i = 0; i < this->numPlayers; i++) {
    this->players.push_back(std::make_unique<Player>(this->conn, this->numPlayers));
  }
}
void Field::markField(int x, int y, int z) {
#if SYNC
  if (z == 0 && this->conn) {
    sf::Packet msg;
    std::cout << "send R" << (float) x << (float) y << std::endl;
    msg << "R" << (float) x << (float) y;
    for (auto const &i : *this->conn) {
      i->send(msg);
    }
  }
#endif
  this->field.at(x).at(y).get()->setMarked();
}
bool Field::openField(int x, int y, int &points, bool send) {
  return this->field.at(x).at(y).get()->open(*this, points, send);
}
Status Field::getStatus() {
  return this->status;
}
void Field::render(sf::RenderTarget &target) {
  for (auto &x : this->field) {
    for (auto &y : x) {
      y.get()->render(target);
    }
  }
  this->renderPlayer(target);
}
coordinates_t Field::calcXY(float xCoo, float yCoo, int xmax, int ymax) {
  if (xCoo < 0 || xCoo >= xmax * TILE_SIZE
      || yCoo < 0 || yCoo >= ymax * TILE_SIZE) {
    return coordinates_t{-1, -1};
  }
  int xIndex = xCoo / TILE_SIZE;
  int yIndex = yCoo / TILE_SIZE;
  return coordinates_t{xIndex, yIndex};
}
