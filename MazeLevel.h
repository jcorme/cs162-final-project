#ifndef ESCAPEFROMCS162_MAZELEVEL_H
#define ESCAPEFROMCS162_MAZELEVEL_H
/*********************************************************************
** Program Filename: MazeLevel.h
** Author: Jason Chen
** Date: 03/19/2018
** Description: Declares the MazeLevel class and its related members.
** Input: None
** Output: None
*********************************************************************/


#include "MazeLocation.h"
#include "OpenSpace.h"

// Used as a utility to easily create readable exceptions for errors that occur
// while parsing the data file.
// level, row, and col are all one-indexed.
class MazeLevelParseError : public std::runtime_error {
  public:
    MazeLevelParseError(unsigned level, const std::string& err,
        Option<unsigned int> row = None, Option<unsigned int> col = None):
        std::runtime_error(ConstructWhatString(level, err, row, col)) {}

  private:
    std::string ConstructWhatString(unsigned level, const std::string& err,
        Option<unsigned> row, Option<unsigned> col);
};

class MazeLevel {
  friend std::ostream& operator<<(std::ostream& os, const MazeLevel& level);

  public:
    MazeLevel(std::ifstream& is, unsigned level, unsigned height,
        unsigned width);

    ~MazeLevel();

    void Reset();

    Option<MazeLocation*> LocationAt(MazePosition pos);
    Option<std::vector<OpenSpace*>> RandomEmptySpaces(unsigned count);

    OpenSpace* start_location() { return start_location_; }
    OpenSpace* instructor_location() { return instructor_location_; }

  private:
    std::mt19937 rng_engine_ = MakeRngEngine();

    std::vector<std::vector<MazeLocation*>> locations_;

    OpenSpace* start_location_ = nullptr;
    OpenSpace* instructor_location_ = nullptr;

    unsigned height_;
    unsigned width_;

    Option<std::vector<MazePosition>> EmptySpacePositions();
    void ParseLevelFromFile(std::ifstream& is, unsigned level);
    void FreeLocations();
};

std::ostream& operator<<(std::ostream& os, const MazeLevel& level);


#endif //ESCAPEFROMCS162_MAZELEVEL_H
