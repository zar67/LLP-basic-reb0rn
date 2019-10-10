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

  Room *LoadRooms();
};

#endif // PROJECT_DATALOADER_H
