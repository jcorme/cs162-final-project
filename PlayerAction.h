#ifndef ESCAPEFROMCS162_PLAYERACTION_H
#define ESCAPEFROMCS162_PLAYERACTION_H
/*********************************************************************
** Program Filename: PlayerAction.h
** Author: Jason Chen
** Date: 03/19/2018
** Description: Declares the ActionInput template and various action
 * enum types.
** Input: None
** Output: None
*********************************************************************/


#include <iostream>
#include "Utils.h"

// A pair of values that an action is mapped to; the first value is the action's
// input value (what user input should be mapped to the action); the second
// value is the readable representation of the action presented to the user.
template <typename I>
using KeyActionPair = std::pair<I, std::string>;

// Maps actions to their input values and string representations.
template <typename I, typename T>
using ActionInputMap = std::unordered_map<T, KeyActionPair<I>, EnumClassHash>;

// Template class declaration that all actions should explicitly specialize with
// a corresponding ActionInputMap member named Inputs (to use with MenuPrompt).
template <typename I, typename T>
struct ActionInput;

enum class PlayerAction {
    ClimbUp,
    DemonstrateSkill,
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
};

enum class PlayerDirectionAction {
    Up,
    Down,
    Left,
    Right,
};

// Macro to help easily specialize ActionInput for default actions.
#define ESC162_SPECIALIZE_ACTION_STRING(I, T) \
template <> struct ActionInput<I, T> { \
  static const ActionInputMap<I, T> Inputs; \
};

ESC162_SPECIALIZE_ACTION_STRING(char, PlayerAction);
ESC162_SPECIALIZE_ACTION_STRING(char, PlayerDirectionAction);

#undef ESC162_SPECIALIZE_ACTION_STRING

/*********************************************************************
** Function: ActionStringMapKeys
** Description: Returns a vector of all the keys in the given map.
** Parameters: map is the map to extract keys from.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <typename I, typename T>
std::vector<T> ActionInputMapKeys(const ActionInputMap<I, T> &map) {
  std::vector<T> keys;
  keys.reserve(map.size());
  for (const auto &m : map)
    keys.push_back(m.first);
  return keys;
}

Option<PlayerDirectionAction> PlayerActionToDirection(PlayerAction action);
PlayerAction PlayerDirectionToAction(PlayerDirectionAction dir);

std::vector<PlayerAction> AllPlayerActions();
std::vector<PlayerDirectionAction> AllPlayerDirectionActions();

std::istream& operator>>(std::istream& is, PlayerAction& action);


#endif //ESCAPEFROMCS162_PLAYERACTION_H
