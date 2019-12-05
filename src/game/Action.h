//
// Created by Zoe on 10/10/2019.
//

#ifndef PROJECT_ACTION_H
#define PROJECT_ACTION_H

#include <array>
#include <string>

class Action
{
 public:
  Action() = default;
  ~Action() = default;

  void setup(int id,
             std::string action,
             int second_word,
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
  int ID = 0;
  std::string verb = "";
  int object = 0;

  // int objects_needed[3];
  std::array<int, 3> objects_needed;

  int room;
  std::string response;
};

#endif // PROJECT_ACTION_H
