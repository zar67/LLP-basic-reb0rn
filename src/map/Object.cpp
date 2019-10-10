//
// Created by Zoe on 10/10/2019.
//

#include "Object.h"

void Object::setup(
  int id, std::string descriptor, std::string examine, bool c, bool h)
{
  ID = id;
  name = descriptor;
  description = examine;
  can_pick_up = c;
  hiding = h;
}

int Object::objectID()
{
  return ID;
}

std::string Object::objectName()
{
  return name;
}

std::string Object::examine()
{
  return description;
}

bool Object::collectible()
{
  return can_pick_up;
}

bool Object::hidden()
{
  return hiding;
}

void Object::hidden(bool h)
{
  hiding = h;
}
