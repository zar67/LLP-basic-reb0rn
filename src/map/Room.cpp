//
// Created by Zoe on 09/10/2019.
//

#include "Room.h"

void Room::setup(int id, std::string descriptor, bool n, bool e, bool s, bool w)
{
  ID = id;
  name = descriptor;
  directions.setup(n, e, s, w);
}

int Room::roomID()
{
  return ID;
}

std::string Room::roomName()
{
  return name;
}

Navigation Room::exits()
{
  return directions;
}
