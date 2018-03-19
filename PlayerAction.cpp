/*********************************************************************
** Program Filename: PlayerAction.cpp
** Author: Jason Chen
** Date: 03/19/2018
** Description: Implements functions and members declared in the PlayerAction
 * header.
** Input: None
** Output: None
*********************************************************************/
#include "PlayerAction.h"

// Maps PlayerAction values to a string.
const ActionInputMap<char, PlayerAction>
    ActionInput<char, PlayerAction>::Inputs = {
    { PlayerAction::ClimbUp, std::make_pair('U', "Climb up the ladder to the next level.") },
    { PlayerAction::DemonstrateSkill, std::make_pair('P', "Demonstrate a programming skill.") },
    { PlayerAction::MoveUp, std::make_pair('W', "Move up.") },
    { PlayerAction::MoveDown, std::make_pair('S', "Move down.") },
    { PlayerAction::MoveLeft, std::make_pair('A', "Move left.") },
    { PlayerAction::MoveRight, std::make_pair('D', "Move right.") },
};

// Maps PlayerDirectionAction values to a string.
const ActionInputMap<char, PlayerDirectionAction>
    ActionInput<char, PlayerDirectionAction>::Inputs = {
  { PlayerDirectionAction::Up, std::make_pair('W', "Up.") },
  { PlayerDirectionAction::Down, std::make_pair('S', "Down.") },
  { PlayerDirectionAction::Left, std::make_pair('A', "Left.") },
  { PlayerDirectionAction::Right, std::make_pair('D', "Right.") },
};

Option<PlayerDirectionAction> PlayerActionToDirection(PlayerAction action) {
  switch (action) {
    case PlayerAction::MoveUp: return PlayerDirectionAction::Up;
    case PlayerAction::MoveDown: return PlayerDirectionAction::Down;
    case PlayerAction::MoveLeft: return PlayerDirectionAction::Left;
    case PlayerAction::MoveRight: return PlayerDirectionAction::Right;
    default: return None;
  }
}

PlayerAction PlayerDirectionToAction(PlayerDirectionAction dir) {
  switch (dir) {
    case PlayerDirectionAction::Up: return PlayerAction::MoveUp;
    case PlayerDirectionAction::Down: return PlayerAction::MoveDown;
    case PlayerDirectionAction::Left: return PlayerAction::MoveLeft;
    case PlayerDirectionAction::Right: return PlayerAction::MoveRight;
  }
}

/*********************************************************************
** Function: AllPlayerDirectionActions
** Description: Returns a vector of all possible PlayerAction values.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
std::vector<PlayerAction> AllPlayerActions() {
  return ActionInputMapKeys(ActionInput<char, PlayerAction>::Inputs);
}

/*********************************************************************
** Function: AllPlayerDirectionActions
** Description: Returns a vector of all possible PlayerDirectionAction
 * values.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
std::vector<PlayerDirectionAction> AllPlayerDirectionActions() {
  return ActionInputMapKeys(ActionInput<char, PlayerDirectionAction>::Inputs);
}

