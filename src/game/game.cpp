#include <Engine/DebugPrinter.h>
#include <Engine/FileIO.h>
#include <Engine/Input.h>
#include <Engine/InputEvents.h>
#include <Engine/Keys.h>
#include <Engine/Sprite.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <time.h>

#include "game.h"
/**
 *   @brief   Default Constructor.
 *   @details Consider setting the game's width and height
 *            and even seeding the random number generator.
 */
MyASGEGame::MyASGEGame()
{
  srand(time(NULL));
  game_name = "Haunted House Adventure";
}

/**
 *   @brief   Destructor.
 *   @details Remove any non-managed memory and callbacks.
 */
MyASGEGame::~MyASGEGame()
{
  this->inputs->unregisterCallback(static_cast<unsigned int>(key_callback_id));

  this->inputs->unregisterCallback(
    static_cast<unsigned int>(mouse_callback_id));
}

void MyASGEGame::play()
{
  loadRooms();
  loadObjects();

  current_room = 57;
  score = 0;
  light_amount = 40;
  std::string empty_input = "";
  input_controller.input(&empty_input);
  num_objects_carrying = 0;

  for (int i = 0; i < DATA::OBJECT_NUM; i++)
  {
    inventory[i] = 0;
  }

  screen_open = DATA::GAME_SCREEN;
}

void MyASGEGame::loadWords()
{
  using File = ASGE::FILEIO::File;
  File file = File();

  // Open file
  if (file.open("/data/actions.txt", ASGE::FILEIO::File::IOMode::READ))
  {
    // Get file data
    using Buffer = ASGE::FILEIO::IOBuffer;
    Buffer buffer = file.read();
    std::string raw_data = buffer.as_char();
    file.close();

    // Format data
    std::string data = raw_data.substr(0, raw_data.find_last_of("]"));
    data += "]";

    // Read file data as JSON
    auto file_data = nlohmann::json::parse(raw_data);

    // Populate each action with it's information
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

    std::cout << "Loaded Actions" << std::endl;
  }
  else
  {
    std::cout << "Actions file not found" << std::endl;
  }
}

void MyASGEGame::loadRooms()
{
  using File = ASGE::FILEIO::File;
  File file = File();

  // Open file
  if (file.open("/data/rooms.txt", ASGE::FILEIO::File::IOMode::READ))
  {
    // Get file data
    using Buffer = ASGE::FILEIO::IOBuffer;
    Buffer buffer = file.read();
    std::string raw_data = buffer.as_char();
    file.close();

    // Format data
    std::string data = raw_data.substr(0, raw_data.find_last_of("]"));
    data += "]";

    // Read file data as JSON
    auto file_data = nlohmann::json::parse(data);

    // Populate each room with it's information
    for (const auto& room : file_data.items())
    {
      int id = room.value()["ID"];
      std::string name = room.value()["Name"];
      bool north = room.value()["Exits"][0];
      bool east = room.value()["Exits"][1];
      bool south = room.value()["Exits"][2];
      bool west = room.value()["Exits"][3];
      int items[5] = { room.value()["Items"][0],
                       room.value()["Items"][1],
                       room.value()["Items"][2],
                       room.value()["Items"][3],
                       room.value()["Items"][4] };
      bool dark = room.value()["Dark"];

      rooms[id].setup(id, &name, north, east, south, west, items, dark);
    }

    std::cout << "Loaded Rooms" << std::endl;
  }
  else
  {
    std::cout << "Rooms file not found" << std::endl;
  }
}

void MyASGEGame::loadObjects()
{
  using File = ASGE::FILEIO::File;
  File file = File();

  // Open file
  if (file.open("/data/objects.txt", ASGE::FILEIO::File::IOMode::READ))
  {
    // Get file data
    using Buffer = ASGE::FILEIO::IOBuffer;
    Buffer buffer = file.read();
    std::string raw_data = buffer.as_char();
    file.close();

    // Format String
    std::string data = raw_data.substr(0, raw_data.find("]"));
    data += "]";

    // Read file data as JSON
    auto file_data = nlohmann::json::parse(data);

    // Populate each object with it's information
    for (const auto& object : file_data.items())
    {
      int id = object.value()["ID"];
      std::string name = object.value()["Name"];
      std::string description = object.value()["Description"];
      bool carry = object.value()["Collectible"];
      bool hide = object.value()["Hidden"];

      objects[id - 1].setup(id, &name, &description, carry, hide);
    }

    std::cout << "Loaded Objects" << std::endl;
  }
  else
  {
    std::cout << "Objects file not found" << std::endl;
  }
}

/**
 *   @brief   Initialises the game.
 *   @details The game window is created and all assets required to
 *            run the game are loaded. The keyHandler and clickHandler
 *            callback should also be set in the initialise function.
 *   @return  True if the game initialised correctly.
 */
bool MyASGEGame::init()
{
  setupResolution();
  if (!initAPI())
  {
    return false;
  }

  renderer->setWindowedMode(ASGE::Renderer::WindowMode::WINDOWED);
  renderer->setClearColour(ASGE::COLOURS::BLACK);

  toggleFPS();

  // input handling functions
  inputs->use_threads = false;

  key_callback_id =
    inputs->addCallbackFnc(ASGE::E_KEY, &MyASGEGame::keyHandler, this);

  mouse_callback_id = inputs->addCallbackFnc(
    ASGE::E_MOUSE_CLICK, &MyASGEGame::clickHandler, this);

  loadWords();

  return true;
}

/**
 *   @brief   Sets the game window resolution
 *   @details This function is designed to create the window size, any
 *            aspect ratio scaling factors and safe zones to ensure the
 *            game frames when resolutions are changed in size.
 *   @return  void
 */
void MyASGEGame::setupResolution()
{
  // how will you calculate the game's resolution?
  // will it scale correctly in full screen? what AR will you use?
  // how will the game be framed in native 16:9 resolutions?
  // here are some arbitrary values for you to adjust as you see fit
  // https://www.gamasutra.com/blogs/KenanBolukbasi/20171002/306822/
  // Scaling_and_MultiResolution_in_2D_Games.php

  // Apple II had Hi-Res support of 320x200 with four colours!!
  game_width = 1024;
  game_height = 768;
}

/**
 *   @brief   Processes any key inputs
 *   @details This function is added as a callback to handle the game's
 *            keyboard input. For this game, calls to this function
 *            are thread safe, so you may alter the game's state as
 *            you see fit.
 *   @param   data The event data relating to key input.
 *   @see     KeyEvent
 *   @return  void
 */
void MyASGEGame::keyHandler(ASGE::SharedEventData data)
{
  auto key = static_cast<const ASGE::KeyEvent*>(data.get());

  if (key->key == ASGE::KEYS::KEY_ESCAPE)
  {
    signalExit();
  }
  else if (screen_open == DATA::MENU_SCREEN)
  {
    input_controller.menuOption(key->key, key->action, &menu_option, 2);

    if (key->key == ASGE::KEYS::KEY_ENTER &&
        key->action == ASGE::KEYS::KEY_RELEASED)
    {
      if (menu_option == 0)
      {
        play();
      }
      else
      {
        signalExit();
      }
    }
  }
  else if (screen_open == DATA::GAME_SCREEN)
  {
    input_controller.update(key->key, key->action);

    if (key->key == ASGE::KEYS::KEY_ENTER &&
        key->action == ASGE::KEYS::KEY_RELEASED)
    {
      std::istringstream iss(input_controller.input());
      std::string action;
      std::string object;
      iss >> action;
      iss >> object;

      for (int i = 0; i < DATA::ACTION_NUM; i++)
      {
        if (actions[i].actionVerb() == action)
        {
          current_action = actions[i].actionID();
          break;
        }
      }

      if (current_action == -1)
      {
        action_response = "This is not a valid command.";
      }
      if (current_action == 15)
      {
        current_action_object = 0;
        say_value = object;
      }
      else
      {
        if (object == "")
        {
          current_action_object = -1;
        }
        else
        {
          for (int i = 0; i < DATA::OBJECT_NUM; i++)
          {
            if (objects[i].objectName() == object)
            {
              current_action_object = objects[i].objectID() - 1;
              break;
            }
          }
        }
      }

      std::string empty_input = "";
      input_controller.input(&empty_input);
    }
  }
  else if (screen_open == DATA::GAME_OVER_SCREEN)
  {
    input_controller.menuOption(key->key, key->action, &menu_option, 3);

    if (key->key == ASGE::KEYS::KEY_ENTER &&
        key->action == ASGE::KEYS::KEY_RELEASED)
    {
      if (menu_option == 0)
      {
        play();
      }
      else if (menu_option == 1)
      {
        screen_open = DATA::MENU_SCREEN;
        menu_option = 0;
      }
      else
      {
        signalExit();
      }
    }
  }
}

/**
 *   @brief   Processes any click inputs
 *   @details This function is added as a callback to handle the game's
 *            mouse button input. For this game, calls to this function
 *            are thread safe, so you may alter the game's state as you
 *            see fit.
 *   @param   data The event data relating to key input.
 *   @see     ClickEvent
 *   @return  void
 */
void MyASGEGame::clickHandler(ASGE::SharedEventData data)
{
  auto click = static_cast<const ASGE::ClickEvent*>(data.get());

  double x_pos = click->xpos;
  double y_pos = click->ypos;

  ASGE::DebugPrinter{} << "x_pos: " << x_pos << std::endl;
  ASGE::DebugPrinter{} << "y_pos: " << y_pos << std::endl;
}

/**
 *   @brief   Updates the scene
 *   @details Prepares the renderer subsystem before drawing the
 *            current frame. Once the current frame is has finished
 *            the buffers are swapped accordingly and the image shown.
 *   @return  void
 */
void MyASGEGame::update(const ASGE::GameTime& game_time)
{
  if (screen_open == DATA::GAME_SCREEN && current_action != -1 &&
      validateInput())
  {
    action_response = actions[current_action].output();

    if (!checkFrozen())
    {
      switch (current_action)
      {
        case (0):
        {
          showActions();
          break;
        }
        case (1):
        {
          showInventory();
          break;
        }
        case (2):
        {
          moveNorth();
          break;
        }
        case (3):
        {
          moveEast();
          break;
        }
        case (4):
        {
          moveSouth();
          break;
        }
        case (5):
        {
          moveWest();
          break;
        }
        case (6):
        case (7):
        {
          addObjectToInventory();
          break;
        }
        case (8):
        {
          examineObject();
          break;
        }
        case (9):
        {
          removeObjectFromInventory();
          break;
        }
        case (10):
        {
          showScore();
          break;
        }
        case (11):
        {
          if (rooms[28].South())
          {
            action_response = "You've already done this action.";
          }
          else
          {
            action_response = actions[11].output();
            changeExits(28, 2);
          }
          break;
        }
        case (12):
        {
          if (objects[16].hidden())
          {
            action_response = actions[12].output();
            revealCandle();
          }
          else
          {
            action_response = "You've already done this action.";
          }
          break;
        }
        case (15):
        {
          say();
          break;
        }
        case (16):
        {
          if (rooms[31].West())
          {
            action_response = "You've already done this action.";
          }
          else
          {
            changeExits(31, 3);
            changeExits(30, 1);
          }
          break;
        }
        case (17):
        {
          if (current_room == 7)
          {
            action_response = "TIMBERRRRR!";
            axed_tree = true;
          }
          else if (current_room == 43)
          {
            if (rooms[43].North())
            {
              action_response = "You've already done this action.";
            }
            else
            {
              action_response = "You broke the thin wall.\nA secret room to "
                                "the "
                                "NORTH appears.";
              changeExits(43, 0);
            }
          }
          else
          {
            action_response = actions[current_action].output();
          }
          break;
        }
        case (18):
        {
          if (!axed_tree)
          {
            if (!climbed_tree)
            {
              action_response = actions[current_action].output();
              climbed_tree = true;
            }
            else
            {
              action_response = "You climb down the tree.";
            }
          }
          else
          {
            action_response = "You cut the tree down, you can't climb it now.";
          }
          break;
        }
        case (19):
        {
          removeBats();
          break;
        }
        case (20):
        {
          removeGhosts();
          break;
        }
        case (21):
        {
          if (light_amount > 0)
          {
            light_ignited = true;
          }
          else
          {
            action_response = "You're candle has burnt out, you can't light it "
                              "again.";
          }
          break;
        }
        case (22):
        {
          light_ignited = false;
          break;
        }
      }
    }
    current_action = -1;
    current_action_object = -1;
  }
}

/**
 *   @brief   Renders the scene
 *   @details Renders all the game objects to the current frame.
 *            Once the current frame is has finished the buffers are
 *            swapped accordingly and the image shown.
 *   @return  void
 */
void MyASGEGame::render(const ASGE::GameTime&)
{
  renderer->setFont(0);

  if (screen_open == DATA::MENU_SCREEN)
  {
    renderer->renderText("BASIC REB0RN", 317, 200, 3, ASGE::COLOURS::GRAY);
    renderer->renderText(menu_option == 0 ? ">> PLAY" : "   PLAY",
                         437,
                         350,
                         2,
                         ASGE::COLOURS::GRAY);
    renderer->renderText(menu_option == 1 ? ">> QUIT" : "   QUIT",
                         437,
                         450,
                         2,
                         ASGE::COLOURS::GRAY);
  }
  else if (screen_open == DATA::GAME_SCREEN)
  {
    renderer->renderText(
      "HAUNTED HOUSE ADVENTURE", 136, 80, 3, ASGE::COLOURS::GRAY);
    renderer->renderText("===============================================",
                         0,
                         110,
                         2,
                         ASGE::COLOURS::GRAY);
    renderer->renderText("YOUR LOCATION: " + rooms[current_room].roomName(),
                         10,
                         150,
                         2,
                         ASGE::COLOURS::GRAY);

    std::string exits = "";
    exits += rooms[current_room].North() ? "N, " : "";
    exits += rooms[current_room].East() ? "E, " : "";
    exits += rooms[current_room].South() ? "S, " : "";
    exits += rooms[current_room].West() ? "W" : "";

    renderer->renderText("EXITS: " + exits, 10, 190, 2, ASGE::COLOURS::GRAY);

    std::string items_text = "";
    int* items = rooms[current_room].roomObjects();
    for (int i = 0; i < 5; i++)
    {
      if (items[i] != -1 && !objects[items[i] - 1].hidden())
      {
        items_text += objects[items[i] - 1].objectName() + ", ";
      }
    }

    renderer->renderText(
      "ITEMS: " + items_text, 10, 230, 2, ASGE::COLOURS::GRAY);
    renderer->renderText("-----------------------------------------------",
                         0,
                         250,
                         2,
                         ASGE::COLOURS::GRAY);
    renderer->renderText(
      "WHAT WOULD YOU LIKE TO DO?", 10, 300, 2, ASGE::COLOURS::GRAY);
    renderer->renderText(
      "> " + input_controller.input(), 15, 350, 2, ASGE::COLOURS::GRAY);
    renderer->renderText("-----------------------------------------------",
                         0,
                         380,
                         2,
                         ASGE::COLOURS::GRAY);
    renderer->renderText(action_response, 10, 430, 2, ASGE::COLOURS::GRAY);
  }
  else if (screen_open == DATA::GAME_OVER_SCREEN)
  {
    renderer->renderText("GAME OVER", 377, 200, 3, ASGE::COLOURS::GRAY);
    renderer->renderText(menu_option == 0 ? ">> PLAY AGAIN" : "   PLAY AGAIN",
                         372,
                         350,
                         2,
                         ASGE::COLOURS::GRAY);
    renderer->renderText(menu_option == 1 ? ">> MENU" : "   MENU",
                         372,
                         450,
                         2,
                         ASGE::COLOURS::GRAY);
    renderer->renderText(menu_option == 2 ? ">> QUIT" : "   QUIT",
                         372,
                         550,
                         2,
                         ASGE::COLOURS::GRAY);
  }
}

int MyASGEGame::checkInventory(int ID)
{
  for (int i = 0; i < DATA::OBJECT_NUM; i++)
  {
    if (inventory[i] == ID)
    {
      return i;
    }
  }
  return -1;
}

int MyASGEGame::checkRoom()
{
  for (int i = 0; i < 5; i++)
  {
    if (rooms[current_room].roomObjects()[i] == current_action_object + 1)
    {
      return i;
    }
  }
  return -1;
}

int MyASGEGame::checkRoom(int object)
{
  for (int i = 0; i < 5; i++)
  {
    if (rooms[current_room].roomObjects()[i] == object)
    {
      return i;
    }
  }
  return -1;
}

bool MyASGEGame::validateInput()
{
  // Check has two words if needed
  if (actions[current_action].actionObject() != -1 &&
      current_action_object == -1)
  {
    action_response = "You need to say a valid object with\nthis action.";
    return false;
  }
  // Check correct object
  if (current_action == 11 || current_action == 13)
  {
    if (current_action_object + 1 == actions[current_action + 1].actionObject())
    {
      current_action += 1;
    }
  }
  else if (actions[current_action].actionObject() > 0 &&
           current_action_object + 1 != actions[current_action].actionObject())
  {
    action_response = "You can't do that.";
    return false;
  }

  // Check have objects
  if (actions[current_action].objectsNeeded()[0] != -1)
  {
    bool has_objects = true;
    for (int i = 0; i < 3; i++)
    {
      int obj = actions[current_action].objectsNeeded()[i];
      if (obj != -1 && checkInventory(obj - 1) == -1)
      {
        has_objects = false;
      }

      if (!has_objects)
      {
        action_response = "You don't have the required objects\nto "
                          "complete this action.";
        return false;
      }
    }
  }
  // Check correct room
  if (actions[current_action].requiredRoom() != -1 &&
      actions[current_action].requiredRoom() != current_room)
  {
    action_response = "You can't do this here.";
    return false;
  }

  return true;
}

void MyASGEGame::showActions()
{
  for (int i = 0; i < DATA::ACTION_NUM; i++)
  {
    if (i % 7 == 0 && i != 0)
    {
      action_response += "\n";
    }
    else
    {
      action_response += actions[i].actionVerb() + ", ";
    }
  }
}

void MyASGEGame::showInventory()
{
  for (int i = 0; i < DATA::OBJECT_NUM; i++)
  {
    if (inventory[i] != 0)
    {
      if (i % 7 == 0 && i != 0)
      {
        action_response += "\n";
      }
      else
      {
        action_response += objects[inventory[i]].objectName() + ", ";
      }
    }
  }
}

void MyASGEGame::moveNorth()
{
  if (rooms[current_room].North())
  {
    if (!rooms[current_room - 8].needsLight() ||
        (rooms[current_room - 8].needsLight() && light_ignited))
    {
      current_room -= 8;
      action_response = "You move NORTH";
      checkLight();
    }
    else
    {
      action_response = "You need a light to go NORTH";
    }
  }
  else
  {
    action_response = "You can't go that way!";
  }
}

void MyASGEGame::moveEast()
{
  if (rooms[current_room].East())
  {
    if (!rooms[current_room + 1].needsLight() ||
        (rooms[current_room + 1].needsLight() && light_ignited))
    {
      current_room += 1;
      action_response = "You move EAST";
      checkLight();
    }
    else
    {
      action_response = "You need a light to go EAST";
    }
  }
  else
  {
    action_response = "You can't go that way!";
  }
}

void MyASGEGame::moveSouth()
{
  if (rooms[current_room].South())
  {
    if (!rooms[current_room + 8].needsLight() ||
        (rooms[current_room + 8].needsLight() && light_ignited))
    {
      current_room += 8;
      action_response = "You move SOUTH";
      checkLight();
    }
    else
    {
      action_response = "You need a light to go SOUTH";
    }
  }
  else
  {
    action_response = "You can't go that way!";
  }
}

void MyASGEGame::moveWest()
{
  if (rooms[current_room].West())
  {
    if (!rooms[current_room - 1].needsLight() ||
        (rooms[current_room - 1].needsLight() && light_ignited))
    {
      current_room -= 1;
      action_response = "You move WEST";
      checkLight();
    }
    else
    {
      action_response = "You need a light to go WEST";
    }
  }
  else
  {
    if (current_room == 45)
    {
      action_response = "There's a magical barrier blocking the way.\nUnless "
                        "you know a magical spell,\nthere's no way out...";
    }
    else
    {
      action_response = "You can't go that way!";
    }
  }
}

void MyASGEGame::addObjectToInventory()
{
  int index = checkRoom();
  if (index != -1 && objects[current_action_object].collectible() &&
      !objects[current_action_object].hidden())
  {
    rooms[current_room].roomObjects()[index] = -1;
    inventory[num_objects_carrying] = current_action_object;
    num_objects_carrying += 1;
    action_response =
      "You picked up " + objects[current_action_object].objectName();
  }
  else if (!objects[current_action_object].collectible())
  {
    action_response =
      "You cannot pickup " + objects[current_action_object].objectName();
  }
  else
  {
    action_response = "There is no " +
                      objects[current_action_object].objectName() +
                      " in this room";
  }
}

void MyASGEGame::removeObjectFromInventory()
{
  bool space_in_room = false;
  for (int i = 0; i < 5; i++)
  {
    if (rooms[current_room].roomObjects()[i] == -1)
    {
      space_in_room = true;
      rooms[current_room].roomObjects()[i] = current_action_object + 1;
      break;
    }
  }

  if (space_in_room)
  {
    int index = checkInventory(current_action_object);
    if (index != -1)
    {
      for (int i = index; i < num_objects_carrying; i++)
      {
        if (i == DATA::OBJECT_NUM - 1)
        {
          inventory[1] = -1;
        }
        else
        {
          inventory[i] = inventory[i + 1];
        }
      }

      inventory[num_objects_carrying] = 0;
      num_objects_carrying -= 1;
      action_response =
        "You dropped " + objects[current_action_object].objectName();
    }
    else
    {
      action_response =
        "You aren't carrying " + objects[current_action_object].objectName();
    }
  }
  else
  {
    action_response = "There is no space to put down\n" +
                      objects[current_action_object].objectName() +
                      " try a different room";
  }
}

void MyASGEGame::examineObject()
{
  if ((checkRoom() != -1 || checkInventory(current_action_object) != -1) &&
      !objects[current_action_object].hidden())
  {
    action_response = objects[current_action_object].examine();
  }
  else
  {
    action_response =
      "There is no " + objects[current_action_object].objectName() + " here";
  }

  if (current_action_object + 1 == 22)
  {
    objects[17].hidden(false);
    action_response += "\nA key is revealed!";
  }
  else if (current_action_object + 1 == 20)
  {
    action_response = objects[current_action_object].examine();
  }
}

void MyASGEGame::showScore()
{
  action_response = "Your score is: " + std::to_string(score);
}

void MyASGEGame::changeExits(int room, int dir)
{
  switch (dir)
  {
    case 0:
      rooms[room].North(true);
      break;
    case 1:
      rooms[room].East(true);
      break;
    case 2:
      rooms[room].South(true);
      break;
    case 3:
      rooms[room].West(true);
      break;
  }
}

void MyASGEGame::revealCandle()
{
  objects[16].hidden(false);
  action_response += "\nA candle is revealed!";
}

void MyASGEGame::say()
{
  action_response = "You said '" + say_value + "'";

  if (say_value == "XZANFAR")
  {
    action_response += "\n*MAGIC OCCURS*";
    if (current_room == 45)
    {
      action_response += "\nThe magical barrier falls";
      changeExits(45, 3);
    }
    else
    {
      current_room = rand() % DATA::SAY_RANDOM_ROOM_NUM;
    }
  }
}

bool MyASGEGame::checkFrozen()
{
  if (current_room == 13 && checkRoom(23) != -1)
  {
    action_response = "The bats frighten you,\nyou're too scared to do "
                      "anything but run!";
    if (current_action == 5)
    {
      moveWest();
      action_response = "You flee.";
    }
    else if (current_action == 19)
    {
      removeBats();
      action_response = "You vanquish the bats!";
    }
    return true;
  }
  if (current_room == 52 && checkRoom(24) != -1)
  {
    action_response = "The ghosts frighten you,\nyou're too scared to do "
                      "anything but run!";
    if (current_action == 2)
    {
      moveNorth();
      action_response = "You flee.";
    }
    else if (current_action == 20)
    {
      removeGhosts();
      action_response = "You vanquish the ghosts!";
    }
    return true;
  }
  return false;
}

void MyASGEGame::removeBats()
{
  int index = checkRoom(23);

  if (index != -1)
  {
    rooms[current_room].roomObjects()[index] = -1;
  }
  else
  {
    action_response = "There are no bats in this room...";
  }
}

void MyASGEGame::removeGhosts()
{
  int index = checkRoom(24);

  if (index != -1)
  {
    rooms[current_room].roomObjects()[index] = -1;
  }
  else
  {
    action_response = "There are no ghosts in this room...";
  }
}

void MyASGEGame::checkLight()
{
  if (light_ignited)
  {
    light_amount -= 1;

    if (light_amount == 10)
    {
      action_response = "Your light is beginning to flicker out...";
    }
    else if (light_amount <= 0)
    {
      light_ignited = false;
      action_response = "Your light went out!";
    }
  }
}
