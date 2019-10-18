//
// Created by Zoe on 10/10/2019.
//

#include "Input.h"

void Input::update(int key, int action)
{
  if (((key >= 65 && key <= 90) || key == ASGE::KEYS::KEY_SPACE) &&
      action == ASGE::KEYS::KEY_RELEASED)
  {
    current_input += char(key);
  }
  else if (key == ASGE::KEYS::KEY_BACKSPACE &&
           action == ASGE::KEYS::KEY_RELEASED && current_input.length() > 0)
  {
    current_input.resize(current_input.size() - 1);
  }
}

int Input::menuOption(int key, int action, int* menu_option, int num_options)
{
  if (key == ASGE::KEYS::KEY_UP && action == ASGE::KEYS::KEY_RELEASED)
  {
    *menu_option -= 1;

    if (*menu_option < 0)
    {
      *menu_option = 1;
    }
  }
  else if (key == ASGE::KEYS::KEY_DOWN && action == ASGE::KEYS::KEY_RELEASED)
  {
    *menu_option += 1;
    *menu_option %= num_options;
  }
}

std::string Input::input()
{
  return current_input;
}

void Input::input(const std::string* input)
{
  current_input = *input;
}
