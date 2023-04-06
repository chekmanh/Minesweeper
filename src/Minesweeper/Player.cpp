#include "Minesweeper/Player.hpp"
#include "Minesweeper/Field.hpp"
#include "Minesweeper/Field_content.hpp"
#include <iostream>
#include <math.h>
int Player::playernum = 0;
sf::SoundBuffer Player::digBuffer;
sf::SoundBuffer Player::magicBuffer;
sf::Sound Player::sound;

void Player::initShape() {
  this->shape.setTexture(this->texture);
  this->shape.setTextureRect(sf::IntRect(this->xtex, this->ytex, 34, 34));
  this->shape.setScale(TILE_SIZE / 36, TILE_SIZE / 36);
  // this->shape.setPosition(0.11 * TILE_SIZE, 0);
  switch (this->playernum) {
    case 1:this->shape.setPosition(0.11 * TILE_SIZE, 0);
      break;
    case 2:this->shape.setPosition(0.11 * TILE_SIZE + (Player::fieldSizeX - 1) * TILE_SIZE, 0);
      break;
    case 3:
      this->shape.setPosition(0.11 * TILE_SIZE + (Player::fieldSizeX - 1) * TILE_SIZE,
                              (Player::fieldSizeY - 1) * TILE_SIZE);
      break;
    case 4:this->shape.setPosition(0.11 * TILE_SIZE, (Player::fieldSizeY - 1) * TILE_SIZE);
      break;
    default:std::cout << "weird" << std::endl;
      break;
  }

  this->ring.setTexture(Player::ringtexture);
  this->ring.setTextureRect(sf::IntRect(0, 0, 100, 100));
  this->ring.setScale(TILE_SIZE / 100, TILE_SIZE / 100);
  this->ring.setPosition(0, 0);
}
void Player::mark(Field &field) {
  if (this->animating) {
    return;
  }
  sf::Vector2f center = this->center();
  coordinates_t coo = Field::calcXY(center.x, center.y, field.width, field.height);
  if (coo.x == -1) {
    return;
  }
  Player::sound.setBuffer(Player::magicBuffer);
  Player::sound.play();
  std::thread anim([this, coo, &field]() {
    this->animating = true;
    int temp = 3 * 34;
    this->shape.setTextureRect(sf::IntRect(temp, this->ytex, 34, 34));
    std::chrono::milliseconds timespan(205); // or whatever
    std::this_thread::sleep_for(timespan);
    this->shape.setTextureRect(sf::IntRect(this->xtex, this->ytex, 34, 34));
    field.markField(coo.x, coo.y);
    this->animating = false;
  });

  anim.detach();
}
bool Player::open(Field &field) {
  std::cout << "number " << this->number << " numPlayers" << this->numPlayers << " field.mynum" << field.mynum  << " turn" << *Field::turn << std::endl;
  if (this->animating) {
    return true;
  }
  sf::Vector2f center = this->center();
  coordinates_t coo = Field::calcXY(center.x, center.y, field.width, field.height);
  if (coo.x == -1) {
    return true;
  }

  Player::sound.setBuffer(Player::digBuffer);
  Player::sound.play();
  std::thread anim([this, coo, &field]() {
    this->animating = true;
    int temp = 2 * 34;
    this->shape.setTextureRect(sf::IntRect(temp, this->ytex, 34, 34));
    std::chrono::milliseconds timespan(205); // or whatever
    std::this_thread::sleep_for(timespan);
    this->shape.setTextureRect(sf::IntRect(this->xtex, this->ytex, 34, 34));
    this->animating = false;
  });
  anim.detach();

  if (Field_content::gameType == 3) {
    if (this->conn) {
      if (((*Field::turn + this->numPlayers) % this->numPlayers) != field.mynum) {
        std::cout << "turn: " << *Field::turn << " mynum " << field.mynum << std::endl;
        return true;
      }
    } else {
      if (((*Field::turn + this->numPlayers) % this->numPlayers) != (this->number - 1)) {
        std::cout << "turn: " << *Field::turn << " mynum " << this->number << std::endl;
        return true;
      }
    }
  }

  // if (!field.openField(coo.x, coo.y, this->points)) {
  if (!field.field.at(coo.x).at(coo.y)->open(field, this->points)) {
    Player::digBuffer.loadFromFile("../resources/sounds/explosion.ogg");
    Player::sound.setBuffer(Player::digBuffer);
    Player::sound.setVolume(20);
    Player::sound.play();
    return false;
  }

  (*Field::turn)++;
  if (this->conn) {
    sf::Packet msg;
    msg << "I" << (float) *Field::turn;
    std::cout << "send player" << std::endl;
    for(const auto& it : *this->conn)
    {
      it->send(msg);
    }
  }  
  return true;
}

sf::Vector2f Player::center() {
  auto pos = this->shape.getPosition();
  float size = TILE_SIZE * 0.66;
  if (this->orientation == 0) {
    return {static_cast<float>(pos.x + size * 0.8), static_cast<float>(pos.y + size * 1.2)};
  } else if (this->orientation == 1) {
    return sf::Vector2f(pos.x + size * 0.8, pos.y + size);
  } else if (this->orientation == 2) {
    return sf::Vector2f(pos.x + size * 0.8, pos.y + size);
  }
  return sf::Vector2f(pos.x + size, pos.y + size);
}

void Player::move(int x, int y, float dt) { // move from keystroke
  if (this->animating) {
    return;
  }
  if (x == 1) {
    if (this->orientation == 3) {
      this->xtex = (this->xtex + 34) % 68;
      this->ytex = 3 * 34;
    } else {
      this->xtex = 0;
      this->ytex = 3 * 34;
      this->orientation = 3;
    }
  } else if (x == -1) {
    if (this->orientation == 1) {
      this->xtex = (this->xtex + 34) % 68;
      this->ytex = 2 * 34;
    } else {
      this->xtex = 0;
      this->ytex = 2 * 34;
      this->orientation = 1;
    }
  } else if (y == -1) {
    if (this->orientation == 2) {
      this->xtex = (this->xtex + 34) % 68;
      this->ytex = 1 * 34;
    } else {
      this->xtex = 0;
      this->ytex = 1 * 34;
      this->orientation = 2;
    }
  } else {
    if (this->orientation == 0) {
      this->xtex = (this->xtex + 34) % 68;
      this->ytex = 0;
    } else {
      this->xtex = 0;
      this->ytex = 0;
      this->orientation = 0;
    }
  }
  this->shape.setTextureRect(sf::IntRect(xtex, ytex, 34, 34));
  float moveX = x * Player::movementspeed;
  float moveY = y * Player::movementspeed;
  // float moveY = y * dt * 100;
  sf::Vector2 pos = this->shape.getPosition();

  // out pf bounds
  coordinates_t coo1 = Field::calcXY(pos.x + moveX + 0.68 * TILE_SIZE, pos.y + moveY + 0.88 * TILE_SIZE,
                                     Player::fieldSizeX, Player::fieldSizeY);
  coordinates_t coo2 = Field::calcXY(pos.x + moveX, pos.y + moveY,
                                     Player::fieldSizeX, Player::fieldSizeY);
  if (coo1.x == -1 || coo2.y == -1) {
    if (Player::loading) {
      if (this->number % 2) {
        this->shape.setPosition(0, (std::rand() % 350) + 250);
      } else {
        this->shape.setPosition(550, (std::rand() % 350) + 250);
      }
    }
    return;
  }
  this->shape.move(moveX, moveY);
  if (this->conn) {
    sf::Packet msg;
    float sx = this->shape.getPosition().x;
    float sy = this->shape.getPosition().y;
    msg << "M" << 0 << sx << sy;
    for (int i = 0; i < this->conn->size(); i++) {
      this->conn->at(i)->send(msg);
    }
  }
  this->updateTrail();
}

void Player::updateTrail() {
  if (this->cnt++ % 2) {
    return;
  }
  this->trail.at(0).setRotation(this->orientation * 90);
  this->trail.at(0).setScale(std::pow(-1, ((this->orientation == 0 || this->orientation == 1) ? 0 : 1)), 1);
  for (int i = 3; i > 0; i--) {
    this->trail.at(i).setPosition(this->trail.at(i - 1).getPosition());
    this->trail.at(i).setRotation(this->trail.at(i - 1).getRotation());
    this->trail.at(i).setScale(this->trail.at(i - 1).getScale());
  }

  sf::Vector2f center = this->center();
  this->trail.at(0).setPosition(center.x - TILE_SIZE * 0.1, center.y);
}

bool Player::loading = false;

void Player::move(int xAbs, int yAbs) { // move from server add animation
  auto pos = this->shape.getPosition();
  int x = 0;
  int y = 0;
  if (pos.x - xAbs < 0 && yAbs - pos.y == 0) {
    x = 1;
  } else if (pos.x - xAbs > 0 && yAbs - pos.y == 0) {
    x = -1;
  } else if (pos.x - xAbs == 0 && pos.y - yAbs > 0) {
    y = -1;
  } else if (pos.x - xAbs == 0 && pos.y - yAbs < 0) {
    y = 1;
  }
  if (this->animating) {
    return;
  }
  if (x == 1) {
    if (this->orientation == 3) {
      this->xtex = (this->xtex + 34) % 68;
      this->ytex = 3 * 34;
    } else {
      this->xtex = 0;
      this->ytex = 3 * 34;
      this->orientation = 3;
    }
  } else if (x == -1) {
    if (this->orientation == 1) {
      this->xtex = (this->xtex + 34) % 68;
      this->ytex = 2 * 34;
    } else {
      this->xtex = 0;
      this->ytex = 2 * 34;
      this->orientation = 1;
    }
  } else if (y == -1) {
    if (this->orientation == 2) {
      this->xtex = (this->xtex + 34) % 68;
      this->ytex = 1 * 34;
    } else {
      this->xtex = 0;
      this->ytex = 1 * 34;
      this->orientation = 2;
    }
  } else {
    if (this->orientation == 0) {
      this->xtex = (this->xtex + 34) % 68;
      this->ytex = 0;
    } else {
      this->xtex = 0;
      this->ytex = 0;
      this->orientation = 0;
    }
  }
  this->shape.setTextureRect(sf::IntRect(xtex, ytex, 34, 34));
  this->shape.setPosition(xAbs, yAbs);
  this->updateTrail();
}

void Player::update() {
  sf::Vector2f center = this->center();
  coordinates_t coo = Field::calcXY(center.x, center.y, Player::fieldSizeX, Player::fieldSizeY);
  if (coo.x == -1) {
    // std::cout << "Player weird" << std::endl;
  }
  this->ring.setPosition(coo.x * TILE_SIZE, coo.y * TILE_SIZE);
}

const float Player::movementspeed = TILE_SIZE / 3; // is pretty nice
void Player::render(sf::RenderTarget &target) {
  this->update();
  target.draw(this->shape);
  for (int i = 1; i < 4; i++) {
    target.draw(this->trail.at(i));
  }
  if (!Player::loading) {
    target.draw(this->ring);
  }
}
void Player::initFieldSize(int fieldSizeX, int fieldSizeY) {
  Player::fieldSizeX = fieldSizeX;
  Player::fieldSizeY = fieldSizeY;
}

void Player::initStaticVariables() {
    Player::digBuffer.loadFromFile("../resources/sounds/dig.ogg");
    Player::magicBuffer.loadFromFile("../resources/sounds/magic.ogg");
    Player::sound.setVolume(45);

    Player::ringtexture.loadFromFile("../resources/textures/ring.png");
    for (int i = 0; i < Player::trailTex.size(); i++) {
      Player::trailTex.at(i).loadFromFile("../resources/textures/foot" + std::to_string(i) + ".png");
    }
}

// each player gets a different skin
void Player::initTexture() {
  if (Player::playernum == 0) {
    this->initStaticVariables();
  }
  switch (Player::playernum++ % 4) {
    case 0:
      if (!this->texture.loadFromFile("../resources/textures/female_character.png")) {
        std::cout << "what?" << std::endl;
      }
      std::cout << "female 1" << std::endl;
      break;
    case 1:
      if (!this->texture.loadFromFile("../resources/textures/male_character.png")) {
        std::cout << "what?" << std::endl;
      }
      std::cout << "male 2" << std::endl;
      break;
    case 2:
      if (!this->texture.loadFromFile("../resources/textures/female_character_02.png")) {
        std::cout << "what?" << std::endl;
      }
      std::cout << "female 3" << std::endl;
      break;
    case 3:
      if (!this->texture.loadFromFile("../resources/textures/male_character_02.png")) {
        std::cout << "what?" << std::endl;
      }
      std::cout << "male 4" << std::endl;
      break;
    default:break;
  }
  this->number = Player::playernum;

  for (int i = 0; i < 4; i++) {
    if (i) {
      this->trail.at(i).setTexture(Player::trailTex.at(i - 1));
      this->trail.at(i).setTextureRect(sf::IntRect(0, 0, 50, 50));
    }
    this->trail.at(i).setPosition(this->shape.getPosition().x, this->shape.getPosition().y);
    this->trail.at(i).setOrigin(25, 25);
  }

}

std::array<sf::Texture, 3> Player::trailTex;
sf::Texture Player::ringtexture;

int Player::fieldSizeX = 0;
int Player::fieldSizeY = 0;

Player::Player(std::vector<std::unique_ptr<sf::TcpSocket>> *conn, int numPlayers)
    : conn(conn), numPlayers(numPlayers) {
  this->initTexture();
  this->initShape();
}

Player::~Player() {
  Player::sound.stop();
}
