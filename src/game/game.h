#pragma once
#include <Engine/OGLGame.h>
#include <string>

#include "../Action.h"
#include "../Input.h"
#include "../map/Object.h"
#include "../map/Room.h"
#include "GameConstants.h"

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
  void loadRooms();
  void loadObjects();

  int checkInventory(int ID);
  int checkRoom();
  int checkRoom(int object);
  void checkEndState();
  void setScore();
  bool validateInput();

  void showActions();
  void showInventory();

  void moveNorth();
  void moveEast();
  void moveSouth();
  void moveWest();

  void addObjectToInventory();
  void removeObjectFromInventory();

  void examineObject();
  void showScore();
  void changeExits(int room, int dir, bool value);
  void revealCandle();
  void say();
  bool checkFrozen();
  void removeBats();
  void removeGhosts();
  void checkLight();

  int key_callback_id = -1;   /**< Key Input Callback ID. */
  int mouse_callback_id = -1; /**< Mouse Input Callback ID. */

  bool in_end_state = false;
  bool game_over = false;
  int screen_open = 0;
  int menu_option = 0;

  Input input_controller = Input();

  Action actions[DATA::ACTION_NUM];
  Room rooms[DATA::ROOM_NUM];
  Object objects[DATA::OBJECT_NUM];

  int inventory[DATA::OBJECT_NUM] = { -1 };
  int num_objects_carrying = 0;
  int treasures[DATA::TREASURE_NUM] = { -1 };

  int current_room = 57;
  int current_action = -1;
  int current_action_object = -1;

  int score = 0;
  int light_amount = 40;

  int say_random_rooms[DATA::SAY_RANDOM_ROOM_NUM] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 14,
    15, 16, 17, 23, 24, 25, 31, 32, 33, 34, 39, 40, 41,
    42, 43, 47, 48, 49, 56, 57, 58, 59, 60, 61, 62, 63
  };

  bool axed_tree = false;
  bool light_ignited = false;

  std::string say_value = "";
  std::string action_response = "";
};