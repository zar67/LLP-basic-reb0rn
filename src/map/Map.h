//
// Created by Zoe on 22/10/2019.
//

#ifndef PROJECT_MAP_H
#define PROJECT_MAP_H

#include "../game/GameConstants.h"
#include "Object.h"
#include "Room.h"

class Map
{
 public:
  Map() = default;
  ~Map() = default;

  void reset();

  void loadRooms();
  void loadObjects();

  int checkRoom(int object);
  void removeObjectFromCurrentRoom(int index);

  void changeExits(int room, int dir, bool value);
  void revealObject(int index);

  std::string moveNorth();
  std::string moveEast();
  std::string moveSouth();
  std::string moveWest();
  std::string removeBats();
  std::string removeGhosts();
  std::string checkLight();
  std::string lightCandle();
  void unlightCandle();
  void magicRandomRoom();

  Room room(int i);
  Room currentRoom();
  Object object(int i);

  int treasure(int i);
  int candleAmount();
  bool candleLit();

 private:
  Room rooms[DATA::ROOM_NUM];
  Object objects[DATA::OBJECT_NUM];
  int treasures[DATA::TREASURE_NUM] = { -1 };

  int current_room = 57;

  int say_random_rooms[DATA::SAY_RANDOM_ROOM_NUM] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 14,
    15, 16, 17, 23, 24, 25, 31, 32, 33, 34, 39, 40, 41,
    42, 43, 47, 48, 49, 56, 57, 58, 59, 60, 61, 62, 63
  };

  int light_amount = 40;
  bool light_ignited = false;
};

#endif // PROJECT_MAP_H
