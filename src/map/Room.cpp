//
// Created by Zoe on 09/10/2019.
//

#include "Room.h"

void Room::setup(int id,
                 std::string descriptor,
                 bool north,
                 bool east,
                 bool south,
                 bool west,
                 int room_objects[5])
{
  ID = id;
  name = descriptor;
  directions.setup(north, east, south, west);

  for (int i = 0; i < 5; i++)
  {
    items[i] = room_objects[i];
  }
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

int* Room::roomObjects()
{
  return items;
}
