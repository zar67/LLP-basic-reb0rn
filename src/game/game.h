#pragma once
#include <Engine/OGLGame.h>
#include <string>

#include "../Action.h"
#include "../Input.h"
#include "../map/Object.h"
#include "../map/Room.h"
#include "GameConstants.h"

const int MENU_SCREEN = 0;
const int GAME_SCREEN = 1;
const int GAME_OVER_SCREEN = 2;

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
  void loadRooms();
  void loadObjects();

  int checkInventory(int ID);
  int checkRoom(int ID);
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
  void changeExits();
  void revealCandle();
  void say();
  void removeEnemies();
  // Light?

  int key_callback_id = -1;   /**< Key Input Callback ID. */
  int mouse_callback_id = -1; /**< Mouse Input Callback ID. */

  int screen_open = 0;
  int menu_option = 0;

  Input input_controller = Input();

  Room rooms[DATA::ROOM_NUM];
  Object objects[DATA::OBJECT_NUM];

  int inventory[DATA::OBJECT_NUM];
  int num_objects_carrying = 0;
  int current_room = 57;
  int current_action = -1;
  int current_action_object = -1;
  int score = 0;

  std::string action_response = "";
};