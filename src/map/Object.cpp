//
// Created by Zoe on 10/10/2019.
//

#include "Object.h"

void Object::setup(int id,
                   const std::string descriptor,
                   const std::string examine,
                   bool carry,
                   bool hide)
{
  ID = id;
  name = descriptor;
  description = examine;
  can_pick_up = carry;
  hiding = hide;
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

void Object::hidden(bool hide)
{
  hiding = hide;
}
