#ifndef ESCAPEFROMCS162_MAZE_H
#define ESCAPEFROMCS162_MAZE_H
/*********************************************************************
** Program Filename: Maze.h
** Author: Jason Chen
** Date: 03/19/2018
** Description: Declares the Maze class and its related members. 
** Input: None
** Output: None
*********************************************************************/


#include <fstream>
#include <sstream>
#include "MazeLevel.h"
#include "OpenSpace.h"
#include "IntrepidStudent.h"
#include "TA.h"
#include "Instructor.h"

// The result of the student moving on a given turn.
enum class MoveResult {
  AcquiredSkill,
  CaughtByTA,
  FailedByInstructor,
  NoEvent,
  SatisfiedInstructor,
};

class Maze {
  friend std::ostream& operator<<(std::ostream& os, const Maze& maze);

  public:
    explicit Maze(std::ifstream& is);

    ~Maze();

    IntrepidStudent* student() { return student_; };

    MoveResult HandleOccupiedSpace(OpenSpace* space);
    MoveResult HandleCurrentPosition();
    void MovePeople();
    bool MovePerson(MazePerson* person, PlayerAction move);
    void ResetAllLevels();
    void ResetCurrentLevel();
    void ResetLevel(MazeLevel& level);

    MazeLevel& CurrentStudentLevel();
    Option<MazeLocation*> LocationAt(MazePosition pos);
    Option<TA*> TAOnLevel(MazeLevel& level);
    Option<OpenSpace*> SpaceAt(MazePosition pos);
    Option<TA*> TaAt(MazePosition pos);

    Option<std::vector<OpenSpace*>> SpacesAdjacentTo(MazePosition pos);
    Option<std::vector<OpenSpace*>> SpacesAdjacentToStudent();
    bool CanMoveInDirection(MazePosition pos, PlayerDirectionAction dir);
    std::vector<PlayerAction> ValidActionsAt(MazePosition pos);
    std::vector<PlayerAction> ValidMovementsAt(MazePosition pos);

    void PrintCurrentLevel();
    void PrintState();

  private:
    std::vector<MazeLevel> levels_;

    IntrepidStudent* student_;
    std::vector<std::vector<TA*>> tas_;
    Instructor* instructor_;

    void FreePeople();
    void PlaceTAs();
    std::vector<TA*> PlaceTAsAtLevel(MazeLevel& level);
    void PlaceSkills();
    void PlaceSkillsAtLevel(MazeLevel& level);

    // Type used to parse the first line of a maze data file.
    struct MazeInfo {
      int levels;
      int height;
      int width;
    };
    Option<MazeInfo> ReadMazeInfo(std::ifstream& is);
};

std::ostream& operator<<(std::ostream& os, const Maze& maze);


#endif //ESCAPEFROMCS162_MAZE_H
