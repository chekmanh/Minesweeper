#ifndef MULTIPLAYER_MINESWEEPER_FIELD_CONTENT_HPP
#define MULTIPLAYER_MINESWEEPER_FIELD_CONTENT_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <TGUI/TGUI.hpp>
#include <thread>

class Field;

class Field_content {
 protected:
  // animation time
  static std::chrono::milliseconds timespan;

  // if animating ignore input
  bool animating = false;

  // flag texture and sprite
  void initMark();
  sf::Sprite markshape;

  // where to render Field_content
  void calcPosition();
  int xCoo;
  int yCoo;

  sf::Sprite shape;

  // tgui buttons
  tgui::Gui *gui;

  // play online
  static std::vector<std::unique_ptr<sf::TcpSocket>> *conn;

  // set brown texture
  void openSprite();
 public:
  // background sprite
  void initShape();

  // for build level
  void markUp();
  void markDown();

  bool isOpen = false; // if open ignore marked
  bool marked = false;
  bool getMarked();
  void setMarked();

  // recursive, without points or normal gamemode // take turns
  static int gameType; 

  // step on field and open it
  // open without changing the score
  bool open(Field &field);
  virtual bool open(Field &field, int &point, bool send = true) = 0;
  virtual void calcnumBombs(std::vector<std::vector<std::unique_ptr<Field_content>>> &field) {};
  virtual void render(sf::RenderTarget &target) {};

  virtual void animate() = 0;

  // one connection for them all
  static void initConn(std::vector<std::unique_ptr<sf::TcpSocket>> *conn);

  void setPosition(float x, float y);

  Field_content();
  Field_content(int x, int y);
  virtual ~Field_content();

};

class Bomb : public Field_content {
 private:
  sf::Sprite explosion;
  std::thread explode;

  sf::Sprite bomb;
 public:

  // explode
  void animate();
  static bool end;

  Bomb(int x, int y);
  ~Bomb();
  bool open(Field &field, int &point, bool send = true);
  void updateFieldContent();
  void calcnumBombs(std::vector<std::vector<std::unique_ptr<Field_content>>> &field);
  void render(sf::RenderTarget &target);
};

class Normal_Field : public Field_content {
 private:
  int numBombs = 0;
  sf::Text text;
  void initText();
 public:

  // noop
  void animate();

  static sf::Font font;
  Normal_Field(int x, int y);
  Normal_Field();
  ~Normal_Field();
  bool open(Field &field, int &points, bool send = true);
  void open();
  // how many bombs are surrounding me?
  void calcnumBombs(std::vector<std::vector<std::unique_ptr<Field_content>>> &field);
  void updateFieldContent();
  void render(sf::RenderTarget &target);
};

#endif
