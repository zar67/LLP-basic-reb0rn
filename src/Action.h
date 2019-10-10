//
// Created by Zoe on 10/10/2019.
//

#ifndef PROJECT_ACTION_H
#define PROJECT_ACTION_H

#include <string>

class Action
{
 public:
  Action() = default;
  ~Action() = default;

  void setup(int id,
             std::string action,
             int object_affected,
             int required_objects[3],
             int required_room,
             std::string output);

  int actionID();
  std::string actionVerb();
  int actionObject();
  int* objectsNeeded();
  int requiredRoom();
  std::string output();

 private:
  int ID;
  std::string verb;
  int object;
  int objects_needed[3];
  int room;
  std::string response;
};

#endif // PROJECT_ACTION_H
