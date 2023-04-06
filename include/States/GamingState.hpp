//
// Created by chelz on 5/13/20.
//

#ifndef MULTIPLAYER_MINESWEEPER_GAMINGSTATE_HPP
#define MULTIPLAYER_MINESWEEPER_GAMINGSTATE_HPP

#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>
#include <TGUI/TGUI.hpp>
#include "States/State.hpp"
#include <SFML/Network.hpp>
#include <thread>

class Field;

// forward declaration of incomplete type needs explizit dtor
struct FieldDeleter {
  void operator()(Field *p);
};

enum Type : unsigned int { LOCAL, SERVER, CLIENT, BUILD };

class GamingState : public State {
 private:
  int turn;

  int numPlayers;
  int mynum;
  // don't print name with winning players
  int looser;

  const Type type;
  std::unique_ptr<tgui::Gui> gui;
  bool updatedText = false;

  // render "you won"
  sf::Text endText;

  // flexible number of players
  std::vector<sf::Text> points;
  // everybody needs to listen to what the other ones have to say
  std::thread read;

  // host write current state to synchronize fields
  std::thread write;

  /*
   * host talks with everybody
   * star configuration (everybody talks to the host and the host passes along
   * the messages)
   *      C             C
   *        \         /
   *          \     /
   *             H    -  -  C
    */
  void readInBackgroundHost(int i);
  std::vector<std::thread> hostThreads;
  
  void initClientReadthread();
  void initHostReadthread();
  void readInBackgroundClient();


  // when game comes to its end calculate who won
  void updateText();

  void initText();
  void initVariables();
  void pollEvents(float dt);

  void checkGui();

  // easier to handle. Command pattern is overkill
  std::map<sf::Keyboard::Key, std::function<void()>> bindings;

  // move mynum-th player with keyboard
  void initBindings();
  // local multiplayer
  void initMultiBindings();
  // online game, the alternative keys will be used to move your Player
  void initSoloAlternative();

  // don't render field, if you didn't get one yet
  bool fieldinit = false;
  void writeField();

  // choose how to init Field depending on type
  void initGame(int x, int y, int numBombs); // merge with default value if x && y && numBombs
  void initGame(const std::string &filename);

  void initField(int x, int y, int bombs);
  void initField(const std::string &filename);

  void renderField(sf::RenderTarget &target);
  void renderEndText(sf::RenderTarget &target);
  void renderPoints(sf::RenderTarget &target);

  void updatePoints();
  void checkMouse();
  sf::Sprite mouseSprite;

  // button to go back to MainMenuState
  void playagain();
  tgui::Button::Ptr accept;

  std::unique_ptr<Field, FieldDeleter> field;

  float dt;

  // build level config
  // filename
  tgui::EditBox::Ptr name;
  void initBuildConfig();
  // press enter to save
  void saveField();

 public:
  // random game
  GamingState(State &oldState, Type type = LOCAL, int numPlayers = 0, int mynum = 0);
  // with parameters
  GamingState(State &oldState,
              int x,
              int y,
              int numBombs,
              Type type = LOCAL,
              int numPlayers = 0,
              int mynum = 0);
  // from file
  GamingState(State &oldState,
              const std::string &filename,
              Type type = LOCAL,
              int numPlayers = 0,
              int mynum = 0);
  ~GamingState();
  void update(float dt);
  void render();
};

#endif //MULTIPLAYER_MINESWEEPER_GAMINGSTATE_H
