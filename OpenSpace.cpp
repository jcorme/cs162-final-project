/*********************************************************************
** Program Filename: OpenSpace.cpp
** Author: Jason Chen
** Date: 03/19/2018
** Description: Implements functions declared by the OpenSpace class and
 * in the OpenSpace header.
** Input: None
** Output: None
*********************************************************************/
#include "OpenSpace.h"

/*********************************************************************
** Function: IsEmpty
** Description: Returns whether the space is empty, which is defined as having
 * no instructor, student, TA, ladder, or skill (primarily used for placing
 * things initially.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
bool OpenSpace::IsEmpty() const {
  return !has_instructor_ && !has_ladder_ && !has_student_ && !has_ta_
      && !has_skill_ && !is_beginning_;
}

/*********************************************************************
** Function: DisplayCharacter
** Description: Returns the display character for the space in its current
 * state.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
char OpenSpace::DisplayCharacter() const {
  if (has_student_) return '*';
  else if (has_ta_) return 'T';
  else if (has_skill_) return '$';
  else if (is_beginning_) return '@';
  else if (has_ladder_) return '^';
  else if (has_instructor_) return '%';
  else return ' ';
}