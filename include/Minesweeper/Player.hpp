#ifndef MULTIPLAYER_MINESWEEPER_PLAYER_HPP
#define MULTIPLAYER_MINESWEEPER_PLAYER_HPP

#include <memory>
#include <array>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>

class Field;

class Player {
 private:
  // music in you ears
  static sf::SoundBuffer digBuffer;
  static sf::SoundBuffer magicBuffer;
  static sf::Sound sound;

  // trail behind moving character
  static std::array<sf::Texture, 3> trailTex;
  std::array<sf::Sprite, 4> trail;
  void updateTrail();
  // don't update trail to often
  int cnt = 0;

  // move 1/3 of TILE_SIZE
  const static float movementspeed;

  // mark field to open
  static sf::Texture ringtexture;

  std::vector<std::unique_ptr<sf::TcpSocket>> *conn;

  void initShape();
  void initTexture();
  // save resources
  void initStaticVariables();

  // return center of a Player instead of top left corner
  sf::Vector2f center();

  // move and open
  void update();

  // ignore input if animating
  bool animating = false;

  // Pokemon Sprite
  sf::Texture texture;
  sf::Sprite ring;

  // to calculate animation
  int orientation = 0; // down, left, up, right

  // where am I?
  int xtex = 0;
  int ytex = 0;

  // player 1 or 2 won
  int number;
  int numPlayers = 2;

 public:
  // determine color and sex of player
  static int playernum;

  static bool loading;
  sf::Sprite shape;
  int points = 0;
  int rotation = 0;

  static int fieldSizeX;
  static int fieldSizeY;
  static void initFieldSize(int fieldSizeX, int fieldSizeY);

  Player(std::vector<std::unique_ptr<sf::TcpSocket>> *conn, int numPlayers = 2);
  ~Player();
  // move from keystroke
  void move(int x, int y, float dt);
  // move from message from socket (don't send message, that you moved)
  void move(int x, int y);

  bool open(Field &field);
  void mark(Field &field);

  void render(sf::RenderTarget &target);
};

#endif 
