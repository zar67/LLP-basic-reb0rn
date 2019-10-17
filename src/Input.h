//
// Created by Zoe on 10/10/2019.
//

#ifndef PROJECT_INPUT_H
#define PROJECT_INPUT_H

#include "Action.h"
#include "game/GameConstants.h"
#include <Engine/FileIO.h>
#include <Engine/Keys.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

class Input
{
 public:
  Input() = default;
  ~Input() = default;

  void LoadWords();
  void update(int key, int action);

  int menuOption(int key, int action, int* menu_option, int num_options);

  Action* words(int index);
  std::string input();
  void input(const std::string* input);

 private:
  Action actions[DATA::ACTION_NUM];
  std::string current_input = "";
};

#endif // PROJECT_INPUT_H
