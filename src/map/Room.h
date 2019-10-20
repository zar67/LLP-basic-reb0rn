//
// Created by Zoe on 09/10/2019.
//

#ifndef PROJECT_ROOM_H
#define PROJECT_ROOM_H

#include <string>

class Room
{
 public:
  Room() = default;
  ~Room() = default;

  void setup(int id,
             const std::string* descriptor,
             bool north,
             bool east,
             bool south,
             bool west,
             int room_objects[5],
             bool dark);

  int roomID();
  std::string roomName();
  bool needsLight();
  bool North();
  bool East();
  bool South();
  bool West();

  void North(bool n_exit);
  void East(bool e_exit);
  void South(bool s_exit);
  void West(bool w_exit);

  int* roomObjects();

 private:
  int ID = 0;
  std::string name = "";
  bool north = false;
  bool east = false;
  bool south = false;
  bool west = false;
  int items[5];
  bool dark;
};

#endif // PROJECT_ROOM_H
