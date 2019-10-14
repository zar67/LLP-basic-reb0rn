//
// Created by Zoe on 10/10/2019.
//

#include "Input.h"

void Input::LoadWords()
{
  using File = ASGE::FILEIO::File;
  File file = File();

  // Open file
  if (file.open("/data/actions.json", ASGE::FILEIO::File::IOMode::READ))
  {
    // Get file data
    using Buffer = ASGE::FILEIO::IOBuffer;
    Buffer buffer = file.read();

    // Read file data as JSON
    auto file_data = nlohmann::json::parse(buffer.as_unsigned_char());

    // Populate each room with it's information
    for (const auto& action : file_data.items())
    {
      int id = action.value()["ID"];
      std::string verb = action.value()["Verb"];
      int second_word = action.value()["Object"];
      int required_objects[3] = { action.value()["Required Objects"][0],
                                  action.value()["Required Objects"][1],
                                  action.value()["Required Objects"][2] };
      int required_room = action.value()["Required Room"];
      std::string response = action.value()["Response"];

      actions[id].setup(
        id, verb, second_word, required_objects, required_room, response);
    }

    file.close();
  }
  else
  {
    std::cout << "Actions file not found" << std::endl;
  }
}

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

Action* Input::words(int index)
{
  return &actions[index];
}

std::string Input::input()
{
  return current_input;
}

void Input::input(const std::string input)
{
  current_input = input;
}
