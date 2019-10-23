//
// Created by Zoe on 22/10/2019.
//

#include "Map.h"
#include <Engine/FileIO.h>
#include <iostream>
#include <nlohmann/json.hpp>

void Map::reset()
{
  loadRooms();
  loadObjects();

  current_room = 57;

  light_amount = 40;
  light_ignited = false;
}

void Map::loadRooms()
{
  using File = ASGE::FILEIO::File;
  File file = File();

  // Open file
  if (file.open("/data/rooms.json", ASGE::FILEIO::File::IOMode::READ))
  {
    // Get file data
    using Buffer = ASGE::FILEIO::IOBuffer;
    Buffer buffer = file.read();
    file.close();

    // Read file data as JSON
    auto file_data =
      nlohmann::json::parse(buffer.as_char(), buffer.as_char() + buffer.length);

    // Populate each room with it's information
    for (const auto& room : file_data.items())
    {
      int id = room.value()["ID"];
      std::string name = room.value()["Name"];
      bool north = room.value()["Exits"][0];
      bool east = room.value()["Exits"][1];
      bool south = room.value()["Exits"][2];
      bool west = room.value()["Exits"][3];
      int items[5] = { room.value()["Items"][0],
                       room.value()["Items"][1],
                       room.value()["Items"][2],
                       room.value()["Items"][3],
                       room.value()["Items"][4] };
      bool dark = room.value()["Dark"];

      rooms[id].setup(id, &name, north, east, south, west, items, dark);
    }

    std::cout << "Loaded Rooms" << std::endl;
  }
  else
  {
    std::cout << "Rooms file not found" << std::endl;
  }
}

void Map::loadObjects()
{
  using File = ASGE::FILEIO::File;
  File file = File();

  // Open file
  if (file.open("/data/objects.json", ASGE::FILEIO::File::IOMode::READ))
  {
    // Get file data
    using Buffer = ASGE::FILEIO::IOBuffer;
    Buffer buffer = file.read();
    file.close();

    // Read file data as JSON
    auto file_data =
      nlohmann::json::parse(buffer.as_char(), buffer.as_char() + buffer.length);

    // Populate each object with it's information
    int treasure_count = 0;
    for (const auto& object : file_data.items())
    {
      int id = object.value()["ID"];
      std::string name = object.value()["Name"];
      std::string description = object.value()["Description"];
      bool carry = object.value()["Collectible"];
      bool hide = object.value()["Hidden"];
      bool treasure = object.value()["Treasure"];

      if (treasure)
      {
        treasures[treasure_count] = id - 1;
        treasure_count += 1;
      }

      objects[id - 1].setup(id, &name, &description, carry, hide, treasure);
    }

    std::cout << "Loaded Objects" << std::endl;
  }
  else
  {
    std::cout << "Objects file not found" << std::endl;
  }
}

std::string Map::lightCandle()
{
  std::string response = "";
  if (light_amount > 0)
  {
    light_ignited = true;
    objects[6].hidden(false);
    response = "You light the candle.";
  }
  else
  {
    response = "You're candle has burnt out,\nyou can't light "
               "it again.";
  }
  return response;
}

void Map::unlightCandle()
{
  light_ignited = false;
  objects[6].hidden(true);
}

int Map::checkRoom(int object)
{
  for (int i = 0; i < 5; i++)
  {
    if (rooms[current_room].roomObjects()[i] == object)
    {
      return i;
    }
  }
  return -1;
}

void Map::removeObjectFromCurrentRoom(int index)
{
  rooms[current_room].roomObjects()[index] = -1;
}

void Map::changeExits(int room, int dir, bool value)
{
  switch (dir)
  {
    case 0:
      rooms[room].North(value);
      break;
    case 1:
      rooms[room].East(value);
      break;
    case 2:
      rooms[room].South(value);
      break;
    case 3:
      rooms[room].West(value);
      break;
    default:
      break;
  }
}

void Map::revealObject(int index)
{
  objects[index].hidden(false);
}

std::string Map::moveNorth()
{
  std::string response = "";
  if (rooms[current_room].North())
  {
    if (!rooms[current_room - 8].needsLight() ||
        (rooms[current_room - 8].needsLight() && light_ignited))
    {
      current_room -= 8;
      response = "You move NORTH";

      if (current_room == 41)
      {
        response = "The door slams shut behind you, and locks...";
      }

      response += checkLight();
    }
    else
    {
      response = "You need a light to go NORTH";
    }
  }
  else
  {
    response = "You can't go that way!";
  }

  return response;
}

std::string Map::moveEast()
{
  std::string response = "";
  if (rooms[current_room].East())
  {
    if (!rooms[current_room + 1].needsLight() ||
        (rooms[current_room + 1].needsLight() && light_ignited))
    {
      current_room += 1;
      response = "You move EAST";
      response += checkLight();
    }
    else
    {
      response = "You need a light to go EAST";
    }
  }
  else
  {
    response = "You can't go that way!";
  }

  return response;
}

std::string Map::moveSouth()
{
  std::string response = "";
  if (rooms[current_room].South())
  {
    if (!rooms[current_room + 8].needsLight() ||
        (rooms[current_room + 8].needsLight() && light_ignited))
    {
      current_room += 8;
      response = "You move SOUTH";
      response += checkLight();
    }
    else
    {
      response = "You need a light to go SOUTH";
    }
  }
  else
  {
    response = "You can't go that way!";
  }

  return response;
}

std::string Map::moveWest()
{
  std::string response = "";
  if (rooms[current_room].West())
  {
    if (!rooms[current_room - 1].needsLight() ||
        (rooms[current_room - 1].needsLight() && light_ignited))
    {
      current_room -= 1;
      response = "You move WEST";
      response += checkLight();
    }
    else
    {
      response = "You need a light to go WEST";
    }
  }
  else
  {
    if (current_room == 45)
    {
      response = "There's a magical barrier blocking the way.\nUnless "
                 "you know a magical spell,\nthere's no way out...";
    }
    else
    {
      response = "You can't go that way!";
    }
  }

  return response;
}

std::string Map::removeBats()
{
  std::string response = "";
  int index = checkRoom(23);

  if (index == -1)
  {
    response = "There are no bats in this room...";
  }
  else
  {
    rooms[current_room].roomObjects()[index] = -1;
    response = "You vanquish the bats!";
  }
  return response;
}

std::string Map::removeGhosts()
{
  std::string response = "";
  int index = checkRoom(24);

  if (index == -1)
  {
    response = "There are no ghosts in this room...";
  }
  else
  {
    rooms[current_room].roomObjects()[index] = -1;
    response = "You vanquish the ghosts!";
  }

  return response;
}

std::string Map::checkLight()
{
  std::string response = "";
  if (light_ignited)
  {
    light_amount -= 1;

    if (light_amount == 10)
    {
      response = "\nYour light is beginning to flicker out...";
    }
    else if (light_amount <= 0)
    {
      light_ignited = false;
      response = "\nYour light went out!";
    }
  }
  return response;
}

void Map::magicRandomRoom()
{
  current_room = say_random_rooms[rand() % DATA::SAY_RANDOM_ROOM_NUM];
}

Room Map::room(int i)
{
  return rooms[i];
}

Room Map::currentRoom()
{
  return rooms[current_room];
}

Object Map::object(int i)
{
  return objects[i];
}

int Map::treasure(int i)
{
  return treasures[i];
}

bool Map::candleLit()
{
  return light_ignited;
}
