#ifndef ESCAPEFROMCS162_WALL_H
#define ESCAPEFROMCS162_WALL_H
/*********************************************************************
** Program Filename: Wall.h
** Author: Jason Chen
** Date: 03/19/2018
** Description: Declares the Wall class and its related members. 
** Input: None
** Output: None
*********************************************************************/


#include "MazeLocation.h"

class Wall : public MazeLocation {
  public:
    explicit Wall(MazePosition pos): MazeLocation(pos, false) {}

    char DisplayCharacter() const override { return '#'; }
};


#endif //ESCAPEFROMCS162_WALL_H
