//
// Created by Zoe on 09/10/2019.
//

#include "Navigation.h"

void Navigation::setup(bool n_exit, bool e_exit, bool s_exit, bool w_exit)
{
  north = n_exit;
  east = e_exit;
  south = s_exit;
  west = w_exit;
}

bool Navigation::North()
{
  return north;
}

bool Navigation::East()
{
  return east;
}

bool Navigation::South()
{
  return south;
}

bool Navigation::West()
{
  return west;
}

void Navigation::North(bool n_exit)
{
  north = n_exit;
}

void Navigation::East(bool e_exit)
{
  east = e_exit;
}

void Navigation::South(bool s_exit)
{
  south = s_exit;
}

void Navigation::West(bool w_exit)
{
  west = w_exit;
}
