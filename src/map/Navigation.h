//
// Created by Zoe on 09/10/2019.
//

#ifndef PROJECT_NAVIGATION_H
#define PROJECT_NAVIGATION_H

class Navigation
{
 public:
  Navigation() = default;
  ~Navigation() = default;

  void setup(bool n_exit, bool e_exit, bool s_exit, bool w_exit);

  bool North();
  bool East();
  bool South();
  bool West();

  void North(bool n_exit);
  void East(bool e_exit);
  void South(bool s_exit);
  void West(bool w_exit);

 private:
  bool north = false;
  bool east = false;
  bool south = false;
  bool west = false;
};

#endif // PROJECT_NAVIGATION_H
