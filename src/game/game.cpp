#include <Engine/DebugPrinter.h>
#include <Engine/FileIO.h>
#include <Engine/Input.h>
#include <Engine/InputEvents.h>
#include <Engine/Keys.h>
#include <Engine/Sprite.h>

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

#include "game.h"
/**
 *   @brief   Default Constructor.
 *   @details Consider setting the game's width and height
 *            and even seeding the random number generator.
 */
MyASGEGame::MyASGEGame()
{
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
  input_controller.input("");
  num_objects_carrying = 0;

  for (int i = 0; i < DATA::OBJECT_NUM; i++)
  {
    inventory[i] = 0;
  }

  screen_open = GAME_SCREEN;
}

void MyASGEGame::loadRooms()
{
  using File = ASGE::FILEIO::File;
  File file = File();

  // Open file
  if (file.open("/data/rooms.json", ASGE::FILEIO::File::IOMode::READ))
  {
    // Get file data
    using Buffer = ASGE::FILEIO::IOBuffer;
    Buffer buffer = file.read();

    // Read file data as JSON
    auto file_data = nlohmann::json::parse(buffer.as_unsigned_char());

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

      rooms[id].setup(id, name, north, east, south, west, items);
    }

    file.close();
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
  if (file.open("/data/objects.json", ASGE::FILEIO::File::IOMode::READ))
  {
    // Get file data
    using Buffer = ASGE::FILEIO::IOBuffer;
    Buffer buffer = file.read();

    // Read file data as JSON
    auto file_data = nlohmann::json::parse(buffer.as_unsigned_char());

    // Populate each room with it's information
    for (const auto& object : file_data.items())
    {
      int id = object.value()["ID"];
      std::string name = object.value()["Name"];
      std::string description = object.value()["Description"];
      bool carry = object.value()["Collectible"];
      bool hide = object.value()["Hidden"];

      objects[id - 1].setup(id, name, description, carry, hide);
    }

    file.close();
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

  input_controller.LoadWords();

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
  else if (screen_open == MENU_SCREEN)
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
  else if (screen_open == GAME_SCREEN)
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
        if (input_controller.words(i)->actionVerb() == action)
        {
          current_action = input_controller.words(i)->actionID();
          break;
        }
      }

      if (current_action == -1)
      {
        action_response = "This is not a valid command.";
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

      input_controller.input("");
    }
  }
  else if (screen_open == GAME_OVER_SCREEN)
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
        screen_open = MENU_SCREEN;
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
  if (screen_open == GAME_SCREEN && current_action != -1 && validateInput())
  {
    action_response = input_controller.words(current_action)->output();
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
        if (current_action_object + 1 == 19)
        {
          action_response = input_controller.words(11)->output();
          // OPEN DOOR
        }
        else if (current_action_object + 1 == 20)
        {
          action_response = input_controller.words(12)->output();
          revealCandle();
        }
        break;
      }
      case (15):
      {
        break;
      }
      case (16):
      {
        break;
      }
      case (17):
      {
        break;
      }
      case (18):
      {
        if (current_action_object + 1 == 18)
        {
          // CLIMB ROPE
        }
        else if (current_action_object + 1 == 19)
        {
          // Climb Tree
        }
        break;
      }
      case (20):
      {
        break;
      }
      case (21):
      {
        break;
      }
      case (23):
      {
        break;
      }
      case (24):
      {
        break;
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

  if (screen_open == MENU_SCREEN)
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
  else if (screen_open == GAME_SCREEN)
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
    exits += rooms[current_room].exits().North() ? "N, " : "";
    exits += rooms[current_room].exits().East() ? "E, " : "";
    exits += rooms[current_room].exits().South() ? "S, " : "";
    exits += rooms[current_room].exits().West() ? "W" : "";

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
  else if (screen_open == GAME_OVER_SCREEN)
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

bool MyASGEGame::validateInput()
{
  // Check has two words if needed
  if (input_controller.words(current_action)->actionObject() != -1 &&
      current_action_object == -1)
  {
    action_response = "You need two words for this action";
    return false;
  }
  // Check have objects
  else if (input_controller.words(current_action)->objectsNeeded()[0] != -1)
  {
    bool has_objects = true;
    for (int i = 0; i < 3; i++)
    {
      int obj = input_controller.words(current_action)->objectsNeeded()[i];
      if (obj != -1 && checkInventory(obj) != -1)
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
  else if (input_controller.words(current_action)->requiredRoom() != -1 &&
           input_controller.words(current_action)->requiredRoom() !=
             current_room)
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
      action_response += input_controller.words(i)->actionVerb() + ", ";
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
  if (rooms[current_room].exits().North())
  {
    current_room -= 8;
  }
  else
  {
    action_response == "You can't go that way!";
  }
}

void MyASGEGame::moveEast()
{
  if (rooms[current_room].exits().East())
  {
    current_room += 1;
  }
  else
  {
    action_response == "You can't go that way!";
  }
}

void MyASGEGame::moveSouth()
{
  if (rooms[current_room].exits().South())
  {
    current_room += 8;
  }
  else
  {
    action_response == "You can't go that way!";
  }
}

void MyASGEGame::moveWest()
{
  if (rooms[current_room].exits().West())
  {
    current_room -= 1;
  }
  else
  {
    action_response == "You can't go that way!";
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
  if (checkRoom() != -1 || checkInventory(current_action_object) != -1)
  {
    action_response = objects[current_action_object].examine();
  }

  if (current_action_object + 1 == 22)
  {
    objects[17].hidden(false);
    action_response += "\nA key is revealed!";
  }
  if (current_action_object + 1 == 20)
  {
    action_response = objects[20].examine();
    revealCandle();
  }
}

void MyASGEGame::showScore()
{
  action_response = "Your score is: " + std::to_string(score);
}

void MyASGEGame::changeExits() {}

void MyASGEGame::revealCandle()
{
  objects[16].hidden(false);
  action_response += "\nA candle is revealed!";
}

void MyASGEGame::say() {}

void MyASGEGame::removeEnemies() {}
