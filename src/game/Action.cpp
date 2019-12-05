//
// Created by Zoe on 10/10/2019.
//

#include "Action.h"

void Action::setup(int id,
                   std::string action,
                   int second_word,
                   int* required_objects,
                   int required_room,
                   std::string output)
{
  ID = id;
  verb = action;
  object = second_word;

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

int Action::actionObject()
{
  return object;
}

int* Action::objectsNeeded()
{
  return &objects_needed[0];
}

int Action::requiredRoom()
{
  return room;
}

std::string Action::output()
{
  return response;
}
