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
  map.reset();

  score = 0;
  std::string empty_input = "";
  input_controller.input(&empty_input);
  num_objects_carrying = 0;
  for (int i = 0; i < DATA::OBJECT_NUM; i++)
  {
    inventory[i] = -1;
  }

  screen_open = DATA::GAME_SCREEN;

  in_end_state = false;
  game_over = false;
  menu_option = 0;

  current_action = -1;
  current_action_object = -1;

  axed_tree = false;
  up_tree = false;

  say_value = "";
  action_response = "The gate slams shut behind you.";
}

void MyASGEGame::loadWords()
{
  using File = ASGE::FILEIO::File;
  File file = File();

  // Open file
  if (file.open("/data/actions.json", ASGE::FILEIO::File::IOMode::READ))
  {
    // Get file data
    using Buffer = ASGE::FILEIO::IOBuffer;
    Buffer buffer = file.read();
    file.close();

    // Read file data as JSON
    auto file_data =
      nlohmann::json::parse(buffer.as_char(), buffer.as_char() + buffer.length);

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

void MyASGEGame::getAction()
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
        if (map.object(i).objectName() == object)
        {
          current_action_object = map.object(i).objectID() - 1;
          break;
        }
      }
    }
  }
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
      getAction();

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
  if (screen_open == DATA::GAME_SCREEN && !game_over && current_action != -1 &&
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
          action_response = map.moveNorth();
          break;
        }
        case (3):
        {
          action_response = map.moveEast();
          break;
        }
        case (4):
        {
          action_response = map.moveSouth();
          break;
        }
        case (5):
        {
          action_response = map.moveWest();
          break;
        }
        case (6):
        case (7):
        {
          addObjectToInventory();

          if (current_action_object + 1 == 15 &&
              map.currentRoom().roomID() == 47)
          {
            map.changeExits(47, 0, false);
            map.changeExits(47, 2, true);
          }
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
          if (map.room(28).South())
          {
            action_response = "You've already done this action.";
          }
          else
          {
            action_response = actions[11].output();
            map.changeExits(28, 2, true);
          }
          break;
        }
        case (12):
        {
          if (map.object(16).hidden())
          {
            action_response = actions[12].output();
            map.revealObject(16);
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
          if (map.room(31).West())
          {
            action_response = "You've already done this action.";
          }
          else if (map.currentRoom().roomID() == 30 ||
                   map.currentRoom().roomID() == 31)
          {
            map.changeExits(31, 3, true);
            map.changeExits(30, 1, true);
          }
          break;
        }
        case (17):
        {
          if (map.currentRoom().roomID() == 7)
          {
            action_response = "TIMBERRRRR!";
            axed_tree = true;
          }
          else if (map.currentRoom().roomID() == 43)
          {
            if (map.room(43).North())
            {
              action_response = "You've already done this action.";
            }
            else
            {
              action_response = "You broke the thin wall.\nA secret room to "
                                "the NORTH appears.";
              map.changeExits(43, 0, true);
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
          if (axed_tree)
          {
            action_response = "You cut the tree down, you can't climb it "
                              "now.";
          }
          else
          {
            if (up_tree)
            {
              action_response = "You climb down the tree.";
              up_tree = false;
            }
            else
            {
              if (checkInventory(3) == -1)
              {
                action_response = "You fall out of the tree! OUCH!";
              }
              else
              {
                action_response = actions[current_action].output();
                up_tree = true;
              }
            }
          }
          break;
        }
        case (19):
        {
          action_response = map.removeBats();
          break;
        }
        case (20):
        {
          action_response = map.removeGhosts();
          break;
        }
        case (21):
        {
          action_response = map.lightCandle();
          break;
        }
        case (22):
        {
          map.unlightCandle();
          break;
        }
        default:
          break;
      }
    }
    current_action = -1;
    current_action_object = -1;

    checkEndState();
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
    renderer->renderText("YOUR LOCATION: " + map.currentRoom().roomName(),
                         10,
                         150,
                         2,
                         ASGE::COLOURS::GRAY);

    std::string exits = "";
    exits += map.currentRoom().North() ? "N, " : "";
    exits += map.currentRoom().East() ? "E, " : "";
    exits += map.currentRoom().South() ? "S, " : "";
    exits += map.currentRoom().West() ? "W" : "";

    renderer->renderText("EXITS: " + exits, 10, 190, 2, ASGE::COLOURS::GRAY);

    std::string items_text = "";
    int* items = map.currentRoom().roomObjects();
    for (int i = 0; i < 5; i++)
    {
      if (items[i] != -1 && !map.object(items[i] - 1).hidden())
      {
        items_text += map.object(items[i] - 1).objectName() + ", ";
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
    renderer->renderText(
      "Score: " + std::to_string(score), 411, 240, 2, ASGE::COLOURS::GRAY);
    renderer->renderText(menu_option == 0 ? ">> PLAY AGAIN" : "   PLAY AGAIN",
                         372,
                         375,
                         2,
                         ASGE::COLOURS::GRAY);
    renderer->renderText(menu_option == 1 ? ">> MENU" : "   MENU",
                         372,
                         475,
                         2,
                         ASGE::COLOURS::GRAY);
    renderer->renderText(menu_option == 2 ? ">> QUIT" : "   QUIT",
                         372,
                         575,
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

void MyASGEGame::checkEndState()
{
  if (!in_end_state)
  {
    bool items_collected = true;
    for (int i = 0; i < DATA::TREASURE_NUM; i++)
    {
      if (checkInventory(map.treasure(i)) == -1)
      {
        items_collected = false;
        break;
      }
    }

    if (items_collected)
    {
      map.changeExits(41, 2, true);
      in_end_state = true;
    }
  }

  if (in_end_state)
  {
    if (map.currentRoom().roomID() == 57)
    {
      game_over = true;
      setScore();
      screen_open = DATA::GAME_OVER_SCREEN;
    }
    else
    {
      action_response += "\nYou have collected all the treasures!\nHead "
                         "back to the gate to see your score.";
    }
  }
}

void MyASGEGame::setScore()
{
  score = 0;
  for (int i = 0; i < DATA::OBJECT_NUM; i++)
  {
    if (inventory[i] != -1)
    {
      if (map.object(inventory[i]).treasure())
      {
        score += 10;
      }
      else
      {
        score += 1;
      }
    }
  }
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
      actions[current_action].requiredRoom() != map.currentRoom().roomID())
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

    if (i != 12 && i != 14)
    {
      action_response += actions[i].actionVerb() + ", ";
    }
  }
}

void MyASGEGame::showInventory()
{
  for (int i = 0; i < DATA::OBJECT_NUM; i++)
  {
    if (inventory[i] != -1)
    {
      if (i % 4 == 0 && i != 0)
      {
        action_response += "\n";
      }
      action_response += map.object(inventory[i]).objectName() + ", ";
    }
  }
}

void MyASGEGame::addObjectToInventory()
{
  int index = map.checkRoom(current_action_object + 1);
  if (index != -1 && map.object(current_action_object).collectible() &&
      !map.object(current_action_object).hidden())
  {
    map.removeObjectFromCurrentRoom(index);
    inventory[num_objects_carrying] = current_action_object;
    num_objects_carrying += 1;
    action_response =
      "You picked up " + map.object(current_action_object).objectName();
  }
  else if (map.object(current_action_object).collectible())
  {
    action_response = "There is no " +
                      map.object(current_action_object).objectName() +
                      " in this room";
  }
  else
  {
    action_response =
      "You cannot pickup " + map.object(current_action_object).objectName();
  }
}

void MyASGEGame::removeObjectFromInventory()
{
  bool space_in_room = false;
  int inventory_index = 0;
  for (int i = 0; i < 5; i++)
  {
    if (map.currentRoom().roomObjects()[i] == -1)
    {
      space_in_room = true;
      inventory_index = i;
      break;
    }
  }

  if (space_in_room)
  {
    int index = checkInventory(current_action_object);
    if (index == -1)
    {
      action_response =
        "You aren't carrying " + map.object(current_action_object).objectName();
    }
    else
    {
      map.currentRoom().roomObjects()[inventory_index] =
        current_action_object + 1;
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

      inventory[num_objects_carrying] = -1;
      num_objects_carrying -= 1;
      action_response =
        "You dropped " + map.object(current_action_object).objectName();
    }
  }
  else
  {
    action_response = "There is no space to put down\n" +
                      map.object(current_action_object).objectName() +
                      " try a different room";
  }
}

void MyASGEGame::examineObject()
{
  if ((map.checkRoom(current_action_object + 1) != -1 ||
       checkInventory(current_action_object) != -1) &&
      !map.object(current_action_object).hidden())
  {
    action_response = map.object(current_action_object).examine();
  }
  else
  {
    action_response =
      "There is no " + map.object(current_action_object).objectName() + " here";
  }

  if (current_action_object + 1 == 22 && map.object(17).hidden())
  {
    map.revealObject(17);
    action_response += "\nA key is revealed!";
  }
  else if (current_action_object + 1 == 20)
  {
    action_response = map.object(current_action_object).examine();
  }
}

void MyASGEGame::showScore()
{
  setScore();
  action_response = "Your score is: " + std::to_string(score);
}

void MyASGEGame::say()
{
  action_response = "You said '" + say_value + "'";

  if (say_value == "XZANFAR")
  {
    action_response += "\n*MAGIC OCCURS*";
    if (map.currentRoom().roomID() == 45)
    {
      action_response += "\nThe magical barrier falls";
      map.changeExits(45, 3, true);
    }
    else
    {
      map.magicRandomRoom();
    }
  }
}

bool MyASGEGame::checkFrozen()
{
  if (map.currentRoom().roomID() == 13 && map.checkRoom(23) != -1)
  {
    action_response = "The bats frighten you,\nyou're too scared to do "
                      "anything but run!";
    if (current_action == 5)
    {
      action_response = map.moveWest();
      action_response = "You flee.";
    }
    else if (current_action == 19)
    {
      action_response = map.removeBats();
    }
    return true;
  }
  if (map.currentRoom().roomID() == 52 && map.checkRoom(24) != -1)
  {
    action_response = "The ghosts frighten you,\nyou're too scared to do "
                      "anything but run!";
    if (current_action == 2)
    {
      action_response = map.moveNorth();
      action_response = "You flee.";
    }
    else if (current_action == 20)
    {
      action_response = map.removeGhosts();
    }
    return true;
  }

  if ((map.currentRoom().roomID() == 61 || map.currentRoom().roomID() == 62) &&
      checkInventory(14) != -1)
  {
    action_response = "The boat get's stuck,\nyou have to leave it behind.";
    if (current_action == 9 && current_action_object + 1 == 15)
    {
      removeObjectFromInventory();
      action_response = "You get out of the boat.";
      map.changeExits(47, 0, true);
      map.changeExits(47, 2, false);
    }
    return true;
  }
  return false;
}
