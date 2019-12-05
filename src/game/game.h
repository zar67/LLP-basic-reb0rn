#pragma once
#include <Engine/OGLGame.h>
#include <string>

#include "../map/Map.h"
#include "../map/Object.h"
#include "../map/Room.h"
#include "Action.h"
#include "GameConstants.h"
#include "Input.h"

/**
 *  An OpenGL Game based on ASGE.
 */
class MyASGEGame : public ASGE::OGLGame
{
 public:
  MyASGEGame();
  ~MyASGEGame() final;
  bool init() override;

 private:
  void keyHandler(ASGE::SharedEventData data);
  void clickHandler(ASGE::SharedEventData data);
  void setupResolution();

  void update(const ASGE::GameTime&) override;
  void render(const ASGE::GameTime&) override;

  void play();
  void loadWords();

  int checkInventory(int ID);
  void checkEndState();
  void setScore();
  bool validateInput();
  void getAction();

  void showActions();
  void showInventory();

  void addObjectToInventory();
  void removeObjectFromInventory();

  void examineObject();
  void showScore();
  void say();
  bool checkFrozen();

  int key_callback_id = -1;   /**< Key Input Callback ID. */
  int mouse_callback_id = -1; /**< Mouse Input Callback ID. */

  bool in_end_state = false;
  bool game_over = false;
  int screen_open = 0;
  int menu_option = 0;

  Map map = Map();
  Input input_controller = Input();

  // Action actions[DATA::ACTION_NUM];
  std::array<Action, DATA::ACTION_NUM> actions;

  //@feedback, a player has an inventory, not the game
  // int inventory[DATA::OBJECT_NUM] = { -1 };
  std::array<int, DATA::OBJECT_NUM> inventory = { -1 };
  int num_objects_carrying = 0;
  int current_action = -1;
  int current_action_object = -1;

  //@feedback, things like score could have been in a player class
  // or some sort of player game data struct, along with position, inventory
  // etc
  int score = 0;

  bool axed_tree = false;
  bool up_tree = false;

  std::string say_value = "";
  std::string action_response = "";
  void menuHandler(const ASGE::KeyEvent* key);
  void gameHandler(const ASGE::KeyEvent* key);
  void renderMenu() const;
};