#ifndef ESCAPEFROMCS162_UTILS_H
#define ESCAPEFROMCS162_UTILS_H
/*********************************************************************
** Program Filename: Utils.h
** Author: Jason Chen
** Date: 03/19/2018
** Description: Declares utility functions used throughout the program.
** Input: None
** Output: None
*********************************************************************/


#include <algorithm>
#include <functional>
#include <memory>
#include <random>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include "Option.h"

// Used by the PromptUntilValid template to validate input.
template <class T>
using InputValidationFn = std::function<bool(const T&)>;

std::mt19937 MakeRngEngine();

// Avoids need for explicit hash specialization when using enum class
// types in an unordered_map, per https://stackoverflow.com/questions
// /18837857/cant-use-enum-class-as-unordered-map-key
struct EnumClassHash {
  template <class T>
  std::size_t operator()(T t) const {
    return static_cast<std::size_t>(t);
  }
};

/*********************************************************************
** Function: StreamGetT
** Description: Reads a line from the given input string and parses
 * it into the reference of type T, returning whether the input was
 * successfully parsed into a T.
** Parameters: in is a reference to an input stream; t is a reference
 * to an object of type T (the type the input is being parsed to)
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <class T>
bool StreamGetT(std::istream &is, T &t) {
  std::string line;
  std::getline(is, line);
  std::istringstream iss(line);
  iss >> t;
  char c;
  if (iss.fail() || iss.get(c))
    return false;
  return true;
}


/*********************************************************************
** Function: PromptUntilValid
** Description: Continues to prompt the user for input until is_valid
 * returns true.
** Parameters: prompt_msg is the message to show to the user on each
 * prompt; fail_msg is an optional message to show the user if their
 * input is invalid; is_valid is a function that returns true if the
 * given value is valid input.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
template <class T>
T PromptUntilValid(
    const std::string &prompt_msg,
    InputValidationFn<T> is_valid,
    const Option<std::string> &fail_msg = None) {
  T t;

  for (;;) {
    std::cout << prompt_msg;
    if (StreamGetT(std::cin, t)) {
      if (is_valid(t))
        return t;
    }

    if (fail_msg.IsSome()) {
      std::cout << fail_msg.CUnwrapRef() << std::endl;
    }
  }
}


#endif //ESCAPEFROMCS162_UTILS_H
