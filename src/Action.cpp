//
// Created by Zoe on 10/10/2019.
//

#include "Action.h"

void Action::setup(int id,
                   std::string action,
                   int* required_objects,
                   int required_room,
                   std::string output)
{
  ID = id;
  verb = action;

  for (int i = 0; i < 3; i++)
  {
    objects_needed[i] = required_objects[i];
  }

  room = required_room;
  response = output;
}

int Action::actionID()
{
  return ID;
}

std::string Action::actionVerb()
{
  return verb;
}

int* Action::objectsNeeded()
{
  return objects_needed;
}

int Action::requiredRoom()
{
  return room;
}

std::string Action::output()
{
  return response;
}
