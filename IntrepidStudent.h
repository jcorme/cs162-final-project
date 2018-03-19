#ifndef ESCAPEFROMCS162_INTREPIDSTUDENT_H
#define ESCAPEFROMCS162_INTREPIDSTUDENT_H
/*********************************************************************
** Program Filename: IntrepidStudent.h
** Author: Jason Chen
** Date: 03/19/2018
** Description: Declares the IntrepidStudent class and its related members. 
** Input: None
** Output: None
*********************************************************************/


#include "MazePerson.h"

class IntrepidStudent : public MazePerson {
  public:
    explicit IntrepidStudent(MazePosition pos): MazePerson(pos) {}

    Option<PlayerAction>
    GetMove(std::vector<PlayerAction> valid_moves) override;

    void Occupy(OpenSpace* space) override { space->set_has_student(true); }
    void Unoccupy(OpenSpace* space) override { space->set_has_student(false); }

    bool HasSkills() const { return prog_skills_ > 0; }

    void IncrementSkills() { ++prog_skills_; }
    void DecrementSkills(){
      // prog_skill_ is unsigned.
      if (prog_skills_ > 0)
        --prog_skills_;
    }

    unsigned prog_skills() const { return prog_skills_; }

  private:
    unsigned prog_skills_ = 0;
};


#endif //ESCAPEFROMCS162_INTREPIDSTUDENT_H
