//
// Created by Zoe on 09/10/2019.
//

#ifndef PROJECT_DATALOADER_H
#define PROJECT_DATALOADER_H

#include <string>
#include "../game/GameConstants.h"
#include "Room.h"

class DataLoader
{
 public:
  DataLoader() = default;
  ~DataLoader() = default;

  void LoadRooms();
  Room* room(int index);

 private:
  Room Rooms[SETTINGS::ROOM_NUM];
  std::string Objects[SETTINGS::OBJECT_NUM];
  std::string Actions[SETTINGS::ACTION_NUM];
};

#endif // PROJECT_DATALOADER_H
