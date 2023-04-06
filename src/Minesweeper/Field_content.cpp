#include "Minesweeper/Field_content.hpp"
#include "Minesweeper/Field.hpp"
#include <iostream>

std::vector<std::unique_ptr<sf::TcpSocket>> *Field_content::conn = nullptr;
Field_content::Field_content(int x, int y)
    : xCoo(x), yCoo(y), isOpen(false), marked(false) {
  this->initShape();
  this->initMark();
}

Field_content::Field_content() {}

Field_content::~Field_content() {}
Normal_Field::~Normal_Field() {}
Bomb::~Bomb() {
  if (this->explode.joinable()) {
    this->explode.join();
  }
}

bool Field_content::getMarked() {
  return this->marked;
}

void Field_content::initConn(std::vector<std::unique_ptr<sf::TcpSocket>> *conn) {
  Field_content::conn = conn;
}

Normal_Field::Normal_Field(int x, int y)
    : Field_content(x, y) {
}

Bomb::Bomb(int x, int y)
    : Field_content(x, y) {
  this->explosion.setTexture(Field::textures->at("explosion"));
  this->explosion.setTextureRect(sf::IntRect(0, 0, 256, 256));
  this->explosion.setScale(TILE_SIZE / 256, TILE_SIZE / 256);
}

void Field_content::initMark() {
  this->markshape.setTexture(Field::textures->at("markshape"));
  this->markshape.setTextureRect(sf::IntRect(0, 0, 100, 100));
  this->markshape.setOrigin(50, 50);
  this->markshape.setScale(TILE_SIZE / 100 / 9, TILE_SIZE / 100 / 9);
  this->markshape.rotate(180);
}

void Field_content::markDown() {
  this->animating = true;
  std::thread anim([&]() {
    for (int i = 0; i < 9; i++) {
      // nice marking animation
      this->markshape.rotate(20);
      this->markshape.scale(0.7833810369372723, 0.7833810369372723);
      std::this_thread::sleep_for(Field_content::timespan);
    }
    this->marked = false;
    this->animating = false;
  });
  anim.detach();
}

void Field_content::markUp() {
  this->animating = true;
  std::thread anim([&]() {
    int max = 18;
    for (int i = 0; i < max; i++) {
      this->markshape.rotate(10);
      this->markshape.scale(1.129830963909753, 1.129830963909753);
      std::this_thread::sleep_for(Field_content::timespan);
    }
    this->animating = false;
  });
  anim.detach();
}

void Field_content::initShape() {
  // get nice look of changing colors
  if ((this->xCoo + this->yCoo) % 2) {
    this->shape.setTexture(Field::textures->at("Field_closed1"));
  } else {
    this->shape.setTexture(Field::textures->at("Field_closed2"));
  }
  this->shape.setTextureRect(sf::IntRect(0, 0, 1024, 1024));
  this->shape.setScale(TILE_SIZE / 1024, TILE_SIZE / 1024);
}

void Normal_Field::initText() {
  this->text.setFont(Normal_Field::font);
  this->text.setString(std::to_string(this->numBombs));
  this->text.setCharacterSize(TILE_SIZE * 0.5);
  this->text.setFillColor(sf::Color::White);
  this->text.setStyle(sf::Text::Bold);
  sf::Vector2f pos = this->shape.getPosition();
  this->text.setPosition(pos.x + 0.35 * TILE_SIZE, pos.y + 0.25 * TILE_SIZE);
  // different colors for different numbers of neighbors
  switch (this->numBombs) {
    case 1:this->text.setFillColor(sf::Color::Red);
      break;
    case 2:this->text.setFillColor(sf::Color::Green);
      break;
    case 3:this->text.setFillColor(sf::Color::Blue);
      break;
    case 4:this->text.setFillColor(sf::Color::Black);
      break;
    case 5:this->text.setFillColor(sf::Color::Magenta);
      break;
    case 6:this->text.setFillColor(sf::Color::Yellow);
      break;
    case 7:this->text.setFillColor(sf::Color::Cyan);
      break;
    case 8:this->text.setFillColor(sf::Color::White);
      break;
    case 0:this->text.setString("");
      break;
    default:std::cout << "Field_content weird" << std::endl;
      break;
  }
}

// static font for all Field_contents
sf::Font Normal_Field::font;

void Field_content::setMarked() {
  if (this->animating) {
    return;
  }
  if (this->marked) {
    this->markDown();
  } else {
    this->marked = true;
    this->markUp();
  }
}

void Field_content::calcPosition() {
  this->shape.setPosition(this->xCoo * TILE_SIZE, this->yCoo * TILE_SIZE);
  this->markshape.setPosition((this->xCoo + 0.5) * TILE_SIZE, (this->yCoo + 0.5) * TILE_SIZE);
}

void Bomb::calcnumBombs(std::vector<std::vector<std::unique_ptr<Field_content>>> &field) {
  this->calcPosition();
  this->explosion.setPosition(this->xCoo * TILE_SIZE, this->yCoo * TILE_SIZE);
  if (Field::build) {
    this->marked = true;
    this->markshape.setScale(TILE_SIZE / 100, TILE_SIZE / 100);
  }
}

void Normal_Field::calcnumBombs(std::vector<std::vector<std::unique_ptr<Field_content>>> &field) {
  this->calcPosition();
  int xlen = field.size();
  int ylen = field.at(1).size();
  for (int i = ((this->xCoo == 0) ? 0 : this->xCoo - 1); i <= ((this->xCoo == xlen - 1) ? this->xCoo : this->xCoo + 1);
       i++) {
    for (int j = ((this->yCoo == 0) ? 0 : this->yCoo - 1);
         j <= ((this->yCoo == ylen - 1) ? this->yCoo : this->yCoo + 1); j++) {
      if (dynamic_cast<Bomb *>(field.at(i).at(j).get()))
        this->numBombs++;
    }
  }
  this->initText();
}

bool Field_content::open(Field &field) {
  int a = 0;
  this->open(field, a);
  return true;
}

int Field_content::gameType;

void Field_content::openSprite() {
  this->isOpen = true;
  if ((this->xCoo + this->yCoo) % 2) {
    this->shape.setTexture(Field::textures->at("Field_open1"));
  } else {
    this->shape.setTexture(Field::textures->at("Field_open2"));
  }
  this->shape.setTextureRect(sf::IntRect(0, 0, 100, 100));
  this->shape.setScale(TILE_SIZE / 100, TILE_SIZE / 100);
}
void Normal_Field::open() {
  this->openSprite();
}

bool Normal_Field::open(Field &field, int &points, bool send) {
  if (this->isOpen) {
    return true;
  } else if (this->marked) {
    return true;
  }
  points++;
  field.remaining--;
  this->openSprite();

#if SYNC
  if (Field_content::conn && send) {
    sf::Packet msg;
    msg << "O" << (float) 0 << (float) this->xCoo << (float) this->yCoo;
    for (auto const &i : *Field_content::conn) {
      i->send(msg);
    }
  }
#endif

  // recursive opening of fields
  if (this->numBombs == 0 && Field_content::gameType) {
    int xlen = field.field.size();
    int ylen = field.field.at(0).size();
    for (int i = ((this->xCoo == 0) ? 0 : this->xCoo - 1);
         i <= ((this->xCoo == xlen - 1) ? this->xCoo : this->xCoo + 1); i++) {
      for (int j = ((this->yCoo == 0) ? 0 : this->yCoo - 1);
           j <= ((this->yCoo == ylen - 1) ? this->yCoo : this->yCoo + 1); j++) {
        if (Field_content::gameType == 1 || Field_content::gameType == 3) {
          field.field.at(i).at(j)->open(field);
        } else {
          field.field.at(i).at(j)->open(field, points);
        }
      }
    }
  }
  return true;
}

// sleep after thread started, for better effect
std::chrono::milliseconds Field_content::timespan = std::chrono::milliseconds(14);

void Normal_Field::animate() {}

// sophisticated animation of an explosion
void Bomb::animate() {
  std::this_thread::sleep_for(Field_content::timespan * 20);

  if ((this->xCoo + this->yCoo) % 2) {
    this->shape.setTexture(Field::textures->at("Field_open1"));
  } else {
    this->shape.setTexture(Field::textures->at("Field_open2"));
  }
  this->shape.setTextureRect(sf::IntRect(0, 0, 100, 100));
  this->shape.setScale(TILE_SIZE / 100, TILE_SIZE / 100);
  this->bomb.setTexture(Field::textures->at("bomb"));
  this->bomb.setTextureRect(sf::IntRect(0, 0, 100, 100));
  this->bomb.setScale(TILE_SIZE / 100, TILE_SIZE / 100);
  this->bomb.setPosition(this->xCoo * TILE_SIZE, this->yCoo * TILE_SIZE);

  this->explode = std::thread([this]() {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        std::this_thread::sleep_for(Field_content::timespan);
        this->explosion.setTextureRect(sf::IntRect(j * 256, i * 256, 256, 256));
      }
    }
  });

}

bool Bomb::open(Field &field, int &points, bool send) {
  if (this->marked) {
    return true;
  }
  // Lost
#if SYNC
  if (Field_content::conn && send) {
    std::cout << "send Lost" << std::endl;
    sf::Packet msg;
    msg << "E" << (float) 0;
    for (auto const &i : *Field_content::conn) {
      i->send(msg);
    }
  }
#endif

  return false;
}

void Normal_Field::render(sf::RenderTarget &target) {
  target.draw(this->shape);
  if (this->isOpen) {
    target.draw(this->text);
    return;
  }
  if (this->marked) {
    target.draw(this->markshape);
  }
}

bool Bomb::end = false;

void Bomb::render(sf::RenderTarget &target) {
  target.draw(this->shape);
  if (Bomb::end) {
    // std::cout << "render" << std::endl;
    target.draw(this->explosion);
    target.draw(this->bomb);
  }
  if (this->marked) {
    target.draw(this->markshape);
  }
}
