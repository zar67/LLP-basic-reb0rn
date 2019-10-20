//
// Created by Zoe on 10/10/2019.
//

#ifndef PROJECT_OBJECT_H
#define PROJECT_OBJECT_H

#include <string>

class Object
{
 public:
  Object() = default;
  ~Object() = default;

  void setup(int id,
             const std::string* descriptor,
             const std::string* examine,
             bool carry,
             bool hide,
             bool treasure);

  int objectID();
  std::string objectName();
  std::string examine();
  bool collectible();
  bool hidden();
  bool treasure();

  void hidden(bool hide);

 private:
  int ID;
  std::string name;
  std::string description;
  bool can_pick_up;
  bool hiding;
  bool valuable;
};

#endif // PROJECT_OBJECT_H
