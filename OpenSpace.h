#ifndef ESCAPEFROMCS162_OPENSPACE_H
#define ESCAPEFROMCS162_OPENSPACE_H
/*********************************************************************
** Program Filename: OpenSpace.h
** Author: Jason Chen
** Date: 03/19/2018
** Description: Declares the OpenSpace class and its related members. 
** Input: None
** Output: None
*********************************************************************/


#include "MazeLocation.h"

class OpenSpace : public MazeLocation {
  public:
    explicit OpenSpace(MazePosition pos): MazeLocation(pos, true) {}

    bool IsEmpty() const;
    char DisplayCharacter() const override;

    bool is_beginning() const { return is_beginning_; }
    bool has_ladder() const { return has_ladder_; }
    bool has_instructor() const { return has_instructor_; }
    bool has_skill() const { return has_skill_; }
    bool has_student() const { return has_student_; }
    bool has_ta() const { return has_ta_; }

    void set_is_beginning(bool is_beginning) { is_beginning_ = is_beginning; }
    void set_has_ladder(bool has_ladder) { has_ladder_ = has_ladder; }
    void set_has_instructor(bool has_instructor) {
      has_instructor_ = has_instructor;
    }
    void set_has_skill(bool has_skill) { has_skill_ = has_skill; }
    void set_has_student(bool has_student) { has_student_ = has_student; }
    void set_has_ta(bool has_ta) { has_ta_ = has_ta; }

  private:
    bool is_beginning_ = false;
    bool has_ladder_ = false;
    bool has_instructor_ = false;
    bool has_skill_ = false;
    bool has_student_ = false;
    bool has_ta_ = false;
};


#endif //ESCAPEFROMCS162_OPENSPACE_H
