#ifndef ESCAPEFROMCS162_MAZELOCATION_H
#define ESCAPEFROMCS162_MAZELOCATION_H
/*********************************************************************
** Program Filename: MazeLocation.h
** Author: Jason Chen
** Date: 03/19/2018
** Description: Declares the MazeLocation class and its related members. 
** Input: None
** Output: None
*********************************************************************/


#include "MazePosition.h"

class MazeLocation {
  public:
    MazeLocation(MazePosition pos, bool occupiable): pos_(pos),
        occupiable_(occupiable) {}

    // Just in case.
    virtual ~MazeLocation() = default;

    // Abstract because the OpenSpace class changes its display character
    // depending on what's on/in the space at any given time.
    virtual char DisplayCharacter() const = 0;

    MazePosition pos() const { return pos_; }
    bool occupiable() const { return occupiable_; }

  private:
    MazePosition pos_;

    bool occupiable_;
};


#endif //ESCAPEFROMCS162_MAZELOCATION_H
