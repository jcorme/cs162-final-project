/*********************************************************************
** Program Filename: IntrepidStudent.cpp
** Author: Jason Chen
** Date: 03/19/2018
** Description: Implements functions declared by the IntrepidStudent class
 * and in the IntrepidStudent header.
** Input: None
** Output: None
*********************************************************************/
#include "IntrepidStudent.h"
#include "MenuPrompt.h"

/*********************************************************************
** Function: GetMove
** Description: Prompts the user to choose an action from the list of valid
 * options.
** Parameters: valid_moves is a vector of all valid actions the user can make.
** Pre-Conditions: None
** Post-Conditions: The returned action, if it is not None, is valid, given
 * the player's current position.
*********************************************************************/
Option<PlayerAction>
IntrepidStudent::GetMove(std::vector<PlayerAction> valid_moves) {
  MenuPrompt<char, PlayerAction> prompt;
  prompt.AddOptions(valid_moves);
  return prompt();
}
