#ifndef MULTIPLAYER_MINESWEEPER_FIELD_HPP
#define MULTIPLAYER_MINESWEEPER_FIELD_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <TGUI/Gui.hpp>
#include <array>
#include <tuple>
#include <thread>

// set to 0 to demonstrate sync thread
#define SYNC 1

// everything scales with this const, change if to small
inline const float TILE_SIZE = 50.f;

struct coordinates_t {
  int x;
  int y;
};

enum Status { RUNNING, ENDED, YOU_LOST, PLAYER_1_WON, PLAYER_2_WON };

class Player;
class Field_content;

class Field {
 private:
  // total number of players and my player number (0 is Host)
  int numPlayers;

  sf::Texture haloTex;
  sf::Sprite halo;

  bool quit = false;

  std::thread animBombs;
  std::unique_ptr<tgui::Gui> gui;
  std::vector<std::unique_ptr<sf::TcpSocket>> *conn;
  sf::RenderWindow *window;
  sf::Font font;

  void initPlayers();

  // move
  void updatePlayer();
  void renderPlayer(sf::RenderTarget &target);

  // change size and view accordingly
  void initWindow(int x, int y);

 public:
  int mynum;
  static int* turn;

  // for Synchronisation
  std::string currentState();
  void check(std::string text);

  static std::map<std::string, sf::Texture> *textures;
  // common denominator of all ctors
  Field(sf::RenderWindow *window,
        std::vector<std::unique_ptr<sf::TcpSocket>> *conn,
        int numPlayers,
        int mynum); 
  // server inits a Field
  Field(int x,
        int y,
        int numBombs,
        sf::RenderWindow *window,
        std::vector<std::unique_ptr<sf::TcpSocket>> *conn,
        int numPlayers,
        int mynum); 
  // server inits field from textfile
  Field(std::ifstream &filename,
        sf::RenderWindow *window,
        std::vector<std::unique_ptr<sf::TcpSocket>> *conn,
        int numPlayers,
        int mynum);
  // client receives field over TCP connection.
  Field(std::string const &code,
        sf::RenderWindow *window,
        std::vector<std::unique_ptr<sf::TcpSocket>> *conn,
        int numPlayers,
        int mynum);
  ~Field();

  void showBombs();

  std::vector<std::unique_ptr<Player>> players;

  Status status = RUNNING; // running
  Status getStatus();

  // 2D array representing the field
  std::vector<std::vector<std::unique_ptr<Field_content>>> field;

  // == 0 ? Game is finished
  int remaining;
  int width, height;

  void render(sf::RenderTarget &target);

  void markField(int x, int y, int z = 0);
  bool openField(int x, int y, int &points, bool send = true);

  // get X and Y from absolute position
  static coordinates_t calcXY(float x, float y, int xmax, int ymax);

  // only render one player and register mouse
  static bool build;
};

#endif
