//
// Created by Zoe on 09/10/2019.
//

#ifndef PROJECT_ROOM_H
#define PROJECT_ROOM_H

#include "Navigation.h"
#include <string>

class Room
{
 public:
  Room() = default;
  ~Room() = default;

  void setup(int id,
             std::string descriptor,
             bool n,
             bool e,
             bool s,
             bool w,
             int room_objects[5]);

  int roomID();
  std::string roomName();
  Navigation exits();

  int* roomObjects();

 private:
  int ID;
  std::string name;
  Navigation directions;
  int items[5];
};

#endif // PROJECT_ROOM_H
