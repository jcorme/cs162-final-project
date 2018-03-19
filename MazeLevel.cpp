/*********************************************************************
** Program Filename: MazeLevel.cpp
** Author: Jason Chen
** Date: 03/19/2018
** Description: Implements functions declared by the MazeLevel class and in 
 * the MazeLevel header.
** Input: None
** Output: None
*********************************************************************/
#include <fstream>
#include "MazeLevel.h"
#include "OpenSpace.h"
#include "Wall.h"

/*********************************************************************
** Function: ConstructWhatString
** Description: Constructs a meaningful and readable error message for a
 * MazeLevel parsing exception.
** Parameters: level is the level being parsed; err is a short description of
 * the exception; row is the row of the level the error occurred on; col is the
 * column of the level the error occurred on.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
std::string MazeLevelParseError::ConstructWhatString(unsigned level,
    const std::string &err, Option<unsigned> row, Option<unsigned> col) {
  std::ostringstream oss;

  oss << "Error parsing ";
  if (row.IsSome()) oss << "row " << row.CUnwrapRef();
  if (col.IsSome()) oss << ", column " << col.Unwrap();
  if (row.IsSome()) oss << " of ";
  oss << "maze level " << (level + 1) << ": " << err << ".";

  return oss.str();
}

/*********************************************************************
** Function: MazeLevel
** Description: Constructor for the MazeLevel class.
** Parameters: is is the stream from which to read the maze data file; level
 * is the level being parsed; height is the height of the level; width is the
 * width of the level.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
MazeLevel::MazeLevel(std::ifstream& is, unsigned level, unsigned height,
    unsigned width): height_(height), width_(width) {
  locations_.reserve(height);

  try {
    // This throws whenever the given maze data file can't be parsed.
    // Considering the program can't run properly without a valid maze data
    // file, exceptions are the best option here.
    ParseLevelFromFile(is, level);
  } catch (...) {
    FreeLocations();
    throw;
  }
}

/*********************************************************************
** Function: ~MazeLevel
** Description: Destructor for the MazeLevel class.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
MazeLevel::~MazeLevel() {
  FreeLocations();

  height_ = 0;
  width_ = 0;
}

/*********************************************************************
** Function: Reset
** Description: Resets the entire level, removing all skills, students, and TAs.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void MazeLevel::Reset() {
  for (auto& row : locations_) {
    for (auto& loc : row) {
      if (!loc->occupiable()) continue;
      auto space = dynamic_cast<OpenSpace*>(loc);
      space->set_has_ta(false);
      space->set_has_skill(false);
      space->set_has_student(false);
    }
  }
}

/*********************************************************************
** Function: LocationAt
** Description: Returns the MazeLocation, if it exists, at the given position.
** Parameters: pos is the position from which to return the MazeLocation.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
Option<MazeLocation*> MazeLevel::LocationAt(MazePosition pos) {
  if (pos.row >= height_ || pos.col >= width_)
    return None;

  return locations_[pos.row][pos.col];
}

/*********************************************************************
** Function: RandomEmptySpaces
** Description: Returns the requested number of randomly-chosen empty spaces,
 * if they exist.
** Parameters: count is the number of spaces to return.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
Option<std::vector<OpenSpace*>> MazeLevel::RandomEmptySpaces(unsigned count) {
  return EmptySpacePositions().Map<std::vector<OpenSpace*>>(
      [&](std::vector<MazePosition> positions) {
          std::shuffle(positions.begin(), positions.end(), rng_engine_);
          std::vector<OpenSpace*> spaces;

          for (const auto& pos : positions) {
            if (spaces.size() >= count) break;

            auto space = dynamic_cast<OpenSpace*>(LocationAt(pos).Unwrap());
            spaces.push_back(space);
          }

          return spaces;
      }
  );
}

/*********************************************************************
** Function: EmptySpacePositions
** Description: Returns the positions of every empty space on the level.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
Option<std::vector<MazePosition>> MazeLevel::EmptySpacePositions() {
  std::vector<MazePosition> positions;

  for (const auto& row : locations_) {
    for (const auto& loc : row) {
      if (!loc->occupiable()) continue;
      auto space = dynamic_cast<OpenSpace*>(loc);
      if (space->IsEmpty()) positions.push_back(space->pos());
    }
  }

  if (positions.empty()) return None;
  return positions;
}

/*********************************************************************
** Function: ParseLevelFromFile
** Description: Reads from the given stream, parsing the data into a maze level.
** Parameters: is is the stream to read from; level is the level being parsed.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void MazeLevel::ParseLevelFromFile(std::ifstream &is, unsigned level) {
  bool has_ladder = false;

  for (unsigned i = 0; i != height_; ++i) {
    std::vector<MazeLocation *> row;
    row.reserve(width_);

    std::string row_str;

    if (!std::getline(is, row_str)) {
      throw MazeLevelParseError(level, "failed to read from stream", i + 1);
    }

    if (row_str.size() != width_) {
      throw MazeLevelParseError(level,
          "width of row not equal to width of maze", i + 1);
    }

    for (unsigned j = 0; j != width_; ++j) {
      MazePosition pos{level, i, j};

      switch (row_str[j]) {
        case ' ':
          row.push_back(new OpenSpace(pos));
          break;
        case '#':
          row.push_back(new Wall(pos));
          break;
        case '@': {
          if (start_location_ != nullptr) {
            throw MazeLevelParseError(level, "second beginning location found",
                                      i + 1, j + 1);
          }

          auto beg = new OpenSpace(pos);
          beg->set_is_beginning(true);
          start_location_ = beg;
          row.push_back(beg);

          break;
        }
        case '^': {
          if (has_ladder) {
            throw MazeLevelParseError(level, "second ladder found", i + 1,
                                      j + 1);
          }

          has_ladder = true;
          auto ladder = new OpenSpace(pos);
          ladder->set_has_ladder(true);
          row.push_back(ladder);

          break;
        }

        case '%': {
          if (instructor_location_ != nullptr) {
            throw MazeLevelParseError(level, "second instructor found", i + 1,
                                      j + 1);
          }

          auto instructor_loc = new OpenSpace(pos);
          instructor_location_ = instructor_loc;
          row.push_back(instructor_loc);

          break;
        }

        default:
          throw MazeLevelParseError(level, "unknown character: " +
              std::string(1, row_str[j]), i + 1, j + 1);
      }
    }

    locations_.push_back(row);
  }

  if (start_location_ == nullptr) {
    throw MazeLevelParseError(level, "no beginning location found");
  }

  if (!has_ladder && instructor_location_ == nullptr) {
    throw MazeLevelParseError(level, "no ladder or instructor found");
  } else if (has_ladder && instructor_location_ != nullptr) {
    throw MazeLevelParseError(level, "found both an instructor and a ladder");
  }
}

/*********************************************************************
** Function: FreeLocations
** Description: Helper function for the destructor; reduces code repetition.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void MazeLevel::FreeLocations() {
  for (auto& row : locations_) {
    for (MazeLocation* loc : row)
      delete loc;
    row.clear();
  }

  locations_.clear();
}

/*********************************************************************
** Function: operator<<
** Description: Overloads the insertion operator to print MazeLevel objects.
** Parameters: os is the output_stream; level is the MazeLevel to print.
** Pre-Conditions:  None
** Post-Conditions: None
*********************************************************************/
std::ostream& operator<<(std::ostream& os, const MazeLevel& level) {
  for (const auto& row : level.locations_) {
    for (MazeLocation* loc : row) {
      os << loc->DisplayCharacter();
    }

    os << '\n';
  }

  return os;
}
