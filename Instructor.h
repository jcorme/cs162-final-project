#ifndef ESCAPEFROMCS162_INSTRUCTOR_H
#define ESCAPEFROMCS162_INSTRUCTOR_H
/*********************************************************************
** Program Filename: Instructor.h
** Author: Jason Chen
** Date: 03/19/2018
** Description: Declares the Instructor class and its related members.
** Input: None
** Output: None
*********************************************************************/


#include "MazePerson.h"

class Instructor : public MazePerson {
  public:
    explicit Instructor(MazePosition pos): MazePerson(pos) {}

    // The instructor never moves.
    Option<PlayerAction>
    GetMove(std::vector<PlayerAction>) override { return None; }

    // Since the instructor never moves, these methods shouldn't do anything.
    void Occupy(OpenSpace*) override {}
    void Unoccupy(OpenSpace*) override {}
};


#endif //ESCAPEFROMCS162_INSTRUCTOR_H
