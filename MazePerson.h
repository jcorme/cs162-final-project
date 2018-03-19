#ifndef ESCAPEFROMCS162_MAZEPERSON_H
#define ESCAPEFROMCS162_MAZEPERSON_H
/*********************************************************************
** Program Filename: MazePerson.h
** Author: Jason Chen
** Date: 03/19/2018
** Description: Declares the MazePerson class and its related members. 
** Input: None
** Output: None
*********************************************************************/


#include "PlayerAction.h"
#include "MazePosition.h"
#include "OpenSpace.h"

class MazePerson {
  public:
    explicit MazePerson(MazePosition pos): position_(pos) {}

    // Just in case.
    virtual ~MazePerson() = default;

    virtual Option<PlayerAction>
    GetMove(std::vector<PlayerAction> valid_moves) = 0;

    // What to do when the person enters/leaves the given space.
    virtual void Occupy(OpenSpace* space) = 0;
    virtual void Unoccupy(OpenSpace* space) = 0;

    MazePosition position() const { return position_; }

    void set_position(MazePosition pos) { position_ = pos; }

  private:
    MazePosition position_;
};


#endif //ESCAPEFROMCS162_MAZEPERSON_H
