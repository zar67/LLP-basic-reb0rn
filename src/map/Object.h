//
// Created by Zoe on 10/10/2019.
//

#ifndef PROJECT_OBJECT_H
#define PROJECT_OBJECT_H

#include <string>

class Object
{
public:
    Object() = default;
    ~Object() = default;

    void setup(int id, std::string descriptor, std::string examine, bool c, bool h);

    int objectID();
    std::string objectName();
    std::string examine();
    bool collectible();
    bool hidden();

    void hidden(bool h);

private:
    int ID;
    std::string name;
    std::string description;
    bool can_pick_up;
    bool hiding;
};


#endif //PROJECT_OBJECT_H