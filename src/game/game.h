#pragma once
#include <Engine/OGLGame.h>
#include <string>

#include "GameConstants.h"
#include "../map/Room.h"
#include "../map/Object.h"

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

  int key_callback_id = -1;   /**< Key Input Callback ID. */
  int mouse_callback_id = -1; /**< Mouse Input Callback ID. */
  bool in_menu = true;

  void LoadRooms();
  void LoadObjects();

  Room Rooms[DATA::ROOM_NUM];
  Object Objects[DATA::OBJECT_NUM];
};