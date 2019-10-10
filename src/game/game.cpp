#include <string>

#include <Engine/DebugPrinter.h>
#include <Engine/Input.h>
#include <Engine/InputEvents.h>
#include <Engine/Keys.h>
#include <Engine/Sprite.h>
#include <Engine/FileIO.h>

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "game.h"
/**
 *   @brief   Default Constructor.
 *   @details Consider setting the game's width and height
 *            and even seeding the random number generator.
 */
MyASGEGame::MyASGEGame()
{
  game_name = "ASGE Game";
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

void MyASGEGame::LoadRooms()
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
    auto file_data = nlohmann::json::parse(buffer.as_char());

    // Populate each room with it's information
    for (const auto& room : file_data.items())
    {
      int id = room.value()["ID"];
      std::string name = room.value()["Name"];
      bool n = room.value()["Exits"][0];
      bool e = room.value()["Exits"][1];
      bool s = room.value()["Exits"][2];
      bool w = room.value()["Exits"][3];
      int items[5] = {room.value()["Items"][0],
                      room.value()["Items"][1],
                      room.value()["Items"][2],
                      room.value()["Items"][3],
                      room.value()["Items"][4]};

      Rooms[id].setup(id, name, n, e, s, w, items);
    }

    file.close();
  }
  else
  {
    std::cout << "Rooms file not found" << std::endl;
  }
}

void MyASGEGame::LoadObjects()
{
  using File = ASGE::FILEIO::File;
  File file = File();

  // Open file
  if (file.open("/data/objects.json", ASGE::FILEIO::File::IOMode::READ))
  {
    // Get file data
    using Buffer = ASGE::FILEIO::IOBuffer;
    Buffer buffer = file.read();

    std::cout << buffer.as_char() << std::endl;

    // Read file data as JSON
    auto file_data = nlohmann::json::parse(buffer.as_char());

    // Populate each room with it's information
    for (const auto& object : file_data.items())
    {
      int id = object.value()["ID"];
      std::string name = object.value()["Name"];
      std::string description = object.value()["Description"];
      bool c = object.value()["Collectible"];
      bool h = object.value()["Hidden"];

      Objects[id-1].setup(id, name, description, c, h);
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

  toggleFPS();

  // input handling functions
  inputs->use_threads = false;

  key_callback_id =
    inputs->addCallbackFnc(ASGE::E_KEY, &MyASGEGame::keyHandler, this);

  mouse_callback_id = inputs->addCallbackFnc(
    ASGE::E_MOUSE_CLICK, &MyASGEGame::clickHandler, this);

  LoadRooms();
  LoadObjects();

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
  game_width = 320;
  game_height = 200;
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
  // auto dt_sec = game_time.delta.count() / 1000.0;;
  // make sure you use delta time in any movement calculations!

  if (!in_menu)
  {
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

  if (in_menu)
  {
  }
}
