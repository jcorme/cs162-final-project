/*********************************************************************
** Program Filename: EscapeFromCS162.cpp
** Author: Jason Chen
** Date: 03/19/2018
** Description: Application file for the Escape from CS 162 game.
** Input: Path to maze data file.
** Output: None
*********************************************************************/
#include <fstream>
#include <iostream>
#include "Maze.h"

/*********************************************************************
** Function: PromptToContinue
** Description: Prompts the user to hit enter to continue.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void PromptToContinue() {
  std::cout << "\nHit enter to continue the game...";
  std::cin.ignore();
}

/*********************************************************************
** Function: InitGameLoop
** Description: Starts the game loop, running until the player passes CS 162.
** Parameters: maze is the game's maze.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void InitGameLoop(Maze& maze) {
  for (;;) {
    maze.PrintState();
    maze.MovePeople();
    MoveResult result = maze.HandleCurrentPosition();

    switch (result) {
      case MoveResult::AcquiredSkill:
        std::cout << "\nYou have acquired a skill! You now have "
                  << maze.student()->prog_skills() << " programming skills!\n";
        break;
      case MoveResult::CaughtByTA:
        std::cout << "\nYou have been caught by an unappeased TA! They sent you"
                  << " back to the start of your current level.\n";
        maze.ResetCurrentLevel();
        PromptToContinue();
        break;
      case MoveResult::FailedByInstructor:
        std::cout << "\nYou have been failed by the instructor! They sent you "
                  << "all the way back to the beginning.\n";
        maze.ResetAllLevels();
        PromptToContinue();
        break;
      case MoveResult::SatisfiedInstructor:
        std::cout << "\nCONGRATULATIONS! You have satisfied the instructor and "
                  << "passed CS 162!\n";
        return;
      case MoveResult::NoEvent:
        break;
    }

    std::cout << "\n\n\n==============================\n\n\n" << std::endl;
  }
};

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "You must provide a maze data file as an argument.\n";
    return -1;
  }

  std::ifstream is(argv[1]);
  if (!is) {
    std::cerr << "Unable to open stream to given maze data file.\n";
    return -1;
  }

  Maze maze(is);

  std::cout << "Welcome to Escape from CS 162!\n"
            << "Hit enter to start the game...";
  std::cin.ignore();
  std::cout << "\n\n\n";

  InitGameLoop(maze);

  std::cout << "Thanks for playing Escape from CS 162!\n";

  return 0;
}