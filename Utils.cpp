/*********************************************************************
** Program Filename: Utils.cpp
** Author: Jason Chen
** Date: 03/19/2018
** Description: Implements functions declared in the Utils header.
** Input: None
** Output: None
*********************************************************************/
#include <iterator>
#include <random>
#include "Utils.h"

/*********************************************************************
** Function: MakeRngEngine
** Description: Creates a 32-bit Mersenne Twister RNG.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
std::mt19937 MakeRngEngine() {
  std::mt19937::result_type seed_data[std::mt19937::state_size];
  std::random_device r;
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(r));
  std::seed_seq seed(std::begin(seed_data), std::end(seed_data));
  return std::mt19937(seed);
}
