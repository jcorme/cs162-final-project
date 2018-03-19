#include "MazePosition.h"

/*********************************************************************
** Function: Translate
** Description: Moves the position by the given amount of the given
 * direction.
** Parameters: direction is the direction to translate in; amount is the
 * amount to translate.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void MazePosition::Translate(PlayerDirectionAction direction, unsigned amount) {
  switch (direction) {
    case PlayerDirectionAction::Up:
      row -= amount;
      break;
    case PlayerDirectionAction::Down:
      row += amount;
      break;
    case PlayerDirectionAction::Left:
      col -= amount;
      break;
    case PlayerDirectionAction::Right:
      col += amount;
      break;
  }
}

/*********************************************************************
** Function: operator==
** Description: Overloads equality operator.
** Parameters: rhs is the right hand operand to the operator.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
bool MazePosition::operator==(const MazePosition& rhs) const {
  return (level == rhs.level) && (row == rhs.row) && (col == rhs.col);
}

/*********************************************************************
** Function: operator<<
** Description: Overloads the insertion operator to print MazePosition
 * objects.
** Parameters: os is the stream to print to; pos is the MazePosition to
 * print.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
std::ostream& operator<<(std::ostream& os, const MazePosition& pos) {
  return os << '(' << pos.level << ", " << pos.row << ", " << pos.col << ')';
}
