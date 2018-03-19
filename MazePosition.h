#ifndef ESCAPEFROMCS162_MAZEPOSITION_H
#define ESCAPEFROMCS162_MAZEPOSITION_H
/*********************************************************************
** Program Filename: MazePosition.h
** Author: Jason Chen
** Date: 03/19/2018
** Description: Declares the MazePosition class and its related members. 
** Input: None
** Output: None
*********************************************************************/


#include <iostream>
#include "PlayerAction.h"

struct MazePosition {
  // All zero-indexed.
  unsigned level;
  unsigned row;
  unsigned col;

  void Translate(PlayerDirectionAction direction, unsigned amount);

  bool operator==(const MazePosition& rhs) const;
};

std::ostream& operator<<(std::ostream& os, const MazePosition& pos);


#endif //ESCAPEFROMCS162_MAZEPOSITION_H
