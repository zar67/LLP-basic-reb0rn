#pragma once
#include <Engine/OGLGame.h>
#include <string>

#include "GameConstants.h"
#include "../map/Room.h"
#include "../map/Object.h"
#include "../Action.h"

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

  void Play();

  void LoadRooms();
  void LoadObjects();
  void LoadActions();

  int key_callback_id = -1;   /**< Key Input Callback ID. */
  int mouse_callback_id = -1; /**< Mouse Input Callback ID. */

  int screen_open = 0;
  int menu_option = 0;

  Room rooms[DATA::ROOM_NUM];
  Object objects[DATA::OBJECT_NUM];
  Action actions[DATA::ACTION_NUM];

  int inventory[DATA::OBJECT_NUM];
  int current_room = 57;
  int score = 0;

};