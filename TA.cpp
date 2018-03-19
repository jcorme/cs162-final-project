/*********************************************************************
** Program Filename: TA.cpp
** Author: Jason Chen
** Date: 03/19/2018
** Description: Implements functions declared in the TA class and in the
 * TA header.
** Input: None
** Output: None
*********************************************************************/
#include "TA.h"

/*********************************************************************
** Function: GetMove
** Description: Randomly selects a move from the given list of valid moves.
** Parameters: valid_moves is a vector of all valid movements for the TA.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
Option<PlayerAction> TA::GetMove(std::vector<PlayerAction> valid_moves) {
  // Calling GetMove is considered a single turn, so it has the side effect of
  // decreasing the number of turns the TA is appeased.
  DecrementAppeasement();

  std::vector<PlayerDirectionAction> valid_dirs;
  for (const auto& m : valid_moves) {
    Option<PlayerDirectionAction> dir = PlayerActionToDirection(m);
    if (dir.IsSome())
      valid_dirs.push_back(dir.Unwrap());
  }

  std::uniform_int_distribution<unsigned long> uni(0, valid_dirs.size() - 1);
  return PlayerDirectionToAction(valid_dirs[uni(rng_engine_)]);
}
