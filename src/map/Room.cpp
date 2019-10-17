//
// Created by Zoe on 09/10/2019.
//

#include "Room.h"

void Room::setup(int id,
                 const std::string* descriptor,
                 bool north,
                 bool east,
                 bool south,
                 bool west,
                 int room_objects[5],
                 bool dark)
{
  ID = id;
  name = *descriptor;
  // directions.setup(north, east, south, west);
  this->north = north;
  this->east = east;
  this->south = south;
  this->west = west;
  this->dark = dark;

  for (int i = 0; i < 5; i++)
  {
    items[i] = room_objects[i];
  }
}

bool Room::North()
{
  return north;
}

bool Room::East()
{
  return east;
}

bool Room::South()
{
  return south;
}

bool Room::West()
{
  return west;
}

void Room::North(bool n_exit)
{
  north = n_exit;
}

void Room::East(bool e_exit)
{
  east = e_exit;
}

void Room::South(bool s_exit)
{
  south = s_exit;
}

void Room::West(bool w_exit)
{
  west = w_exit;
}

int Room::roomID()
{
  return ID;
}

std::string Room::roomName()
{
  return name;
}

bool Room::needsLight()
{
  return dark;
}

int* Room::roomObjects()
{
  return items;
}