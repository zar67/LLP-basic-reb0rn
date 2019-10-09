//
// Created by Zoe on 09/10/2019.
//

#include <Engine/FileIO.h>
#include <iostream>
#include <nlohmann/json.hpp>

#include "DataLoader.h"

void DataLoader::LoadRooms()
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

            Rooms[id].setup(id, name, n, e, s, w);
        }

        file.close();
    }
    else
    {
        std::cout << "File not found" << std::endl;
    }
}

Room* DataLoader::room(int index)
{
    return &Rooms[index];
}
