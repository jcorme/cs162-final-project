/*********************************************************************
** Program Filename: Maze.cpp
** Author: Jason Chen
** Date: 03/19/2018
** Description: Implements functions declared by the Maze class and in the
 * Maze header.
** Input: None
** Output: None
*********************************************************************/
#include "Maze.h"

/*********************************************************************
** Function: Maze
** Description: Constructor for the Maze class.
** Parameters: is is the stream from which to read the maze data file.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
Maze::Maze(std::ifstream &is) {
  // Let the Unwrap throw if the info couldn't be read.
  MazeInfo info = ReadMazeInfo(is).Unwrap();

  // The bounds for width and height should definitely be higher, but this
  // simply checks that we have positive values.
  if (info.levels < 1 || info.width < 1 || info.height < 1) {
    throw std::runtime_error("Levels, height, and width must all be >= 1.");
  }

  // Avoids calling the (default) copy constructor of MazeLevel on each
  // iteration, which will cause a segfault from a double free. (Why implement
  // a copy constructor if we can just do this, right?)
  levels_.reserve(info.levels);
  for (unsigned i = 0; i != info.levels; ++i) {
    // MazeLevel constructor will throw if the maze data file is invalid.
    levels_.emplace_back(is, i, info.height, info.width);
  }

  // Is there an instructor on the final level?
  if (levels_[info.levels - 1].instructor_location() == nullptr) {
    throw std::runtime_error("Error parsing the maze: no instructor found on "
                             "final level.");
  } else {
    // Are there multiple instructors?
    for (unsigned i = 0; i != info.levels; ++i) {
      if (levels_[i].instructor_location() != nullptr && i < (info.levels - 1))
        throw std::runtime_error("Error parsing the maze: instructor found on "
                                 "a level other than the final one.");
    }
  }

  auto starting_loc = levels_[0].start_location();
  starting_loc->set_has_student(true);
  student_ = new IntrepidStudent(starting_loc->pos());

  auto instructor_loc = levels_[info.levels - 1].instructor_location();
  instructor_loc->set_has_instructor(true);
  instructor_ = new Instructor(instructor_loc->pos());

  try {
    PlaceTAs();
    PlaceSkills();
  } catch (...) {
    // Let's salvage all the memory we can if an exception is thrown here...
    FreePeople();
    throw;
  }
}

/*********************************************************************
** Function: ~Maze
** Description: Destructor for the Maze class.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
Maze::~Maze() {
  FreePeople();
}

/*********************************************************************
** Function: FreePeople
** Description: Helper function that acts as a destructor; avoids repeating
 * destructor code in multiple places.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void Maze::FreePeople() {
  delete student_;

  for (auto& row : tas_) {
    for (auto& ta : row) {
      delete ta;
    }
  }

  delete instructor_;
}

/*********************************************************************
** Function: HandleOccupiedSpace
** Description: Performs any necessary actions given the student's current
 * space; includes checking whether a TA is in the spot and whether the student
 * has picked up a skill.
** Parameters: space is the space at the student's current position.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
MoveResult Maze::HandleOccupiedSpace(OpenSpace* space) {
  if (space->has_ta()) {
    TA* ta = TaAt(space->pos()).Unwrap();
    if (!ta->IsAppeased()) {
      return MoveResult::CaughtByTA;
    }
  } else if (space->has_skill()) {
    student_->IncrementSkills();
    space->set_has_skill(false);
    return MoveResult::AcquiredSkill;
  }

  return MoveResult::NoEvent;
}

/*********************************************************************
** Function: HandleCurrentPosition
** Description: Same as HandleOccupiedSpace, but in addition to checking
 * the student's current position, also checks adjacent spaces.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
MoveResult Maze::HandleCurrentPosition() {
  MoveResult res = HandleOccupiedSpace(SpaceAt(student_->position()).Unwrap());
  if (res == MoveResult::CaughtByTA) return res;

  auto adjacent_spaces = SpacesAdjacentToStudent().Unwrap();
  for (auto& space : adjacent_spaces) {
    if (space->has_ta()) {
      TA *ta = TaAt(space->pos()).Unwrap();
      if (!ta->IsAppeased()) {
        return MoveResult::CaughtByTA;
      }
    } else if (space->has_instructor()) {
      if (student_->prog_skills() < 3) {
        return MoveResult::FailedByInstructor;
      } else {
        return MoveResult::SatisfiedInstructor;
      }
    }
  }

  return res;
}

/*********************************************************************
** Function: MovePeople
** Description: Prompts user to pick an action and performs that action; also
 * handles moving the TAs. This should be called each turn.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void Maze::MovePeople() {
  MazePosition s_pos = student_->position();
  PlayerAction s_move = student_->GetMove(ValidActionsAt(s_pos)).Unwrap();

  // Did the student demonstrate a skill?
  bool appease_tas = false;

  switch (s_move) {
    case PlayerAction::ClimbUp: {
      SpaceAt(s_pos).Unwrap()->set_has_student(false);
      auto start_loc = levels_[s_pos.level + 1].start_location();
      start_loc->set_has_student(true);
      student_->set_position(start_loc->pos());
      std::cout << "\nYou have climbed up to level " << (s_pos.level + 2)
                << ".\n";
      break;
    }
    case PlayerAction::DemonstrateSkill:
      student_->DecrementSkills();
      appease_tas = true;
      std::cout << "\nYou demonstrated a skill to the TAs; you now have "
                << student_->prog_skills() << " skills remaining.\n";
      break;
    default:
      MovePerson(student_, s_move);
      break;
  }

  for (auto& ta : tas_[student_->position().level]) {
    MazePosition ta_pos = ta->position();
    auto valid_moves = ValidMovementsAt(ta_pos);
    PlayerAction ta_move = ta->GetMove(valid_moves).Unwrap();
    MovePerson(ta, ta_move);
    if (appease_tas) ta->Appease();
  }
}

/*********************************************************************
** Function: MovePerson
** Description: Changes the position of a single person.
** Parameters: person is the person be moved; move is the direction.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
bool Maze::MovePerson(MazePerson *person, PlayerAction move) {
  switch (move) {
    case PlayerAction::MoveUp:
    case PlayerAction::MoveDown:
    case PlayerAction::MoveLeft:
    case PlayerAction::MoveRight:
      break;
    default:
      return false;
  }

  MazePosition pos = person->position();
  person->Unoccupy(SpaceAt(pos).Unwrap());
  pos.Translate(PlayerActionToDirection(move).Unwrap(), 1);
  person->Occupy(SpaceAt(pos).Unwrap());
  person->set_position(pos);
  return true;
}

/*********************************************************************
** Function: ResetAllLevels
** Description: Resets all maze levels to their original state, moves the
 * student back to the beginning of level 1, and replaces the TAs and skills.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void Maze::ResetAllLevels() {
  for (auto& level : levels_) {
    ResetLevel(level);
    // ResetLevel places the student at the beginning of the reset level.
    level.start_location()->set_has_student(false);
  }

  auto start_loc = levels_[0].start_location();
  start_loc->set_has_student(true);
  student_->set_position(start_loc->pos());
}

/*********************************************************************
** Function: ResetCurrentLevel
** Description: Resets the student's current level to its original state, moves
 * the student back to the beginning of the current level, and replaces the TAs
 * and skills.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void Maze::ResetCurrentLevel() {
  ResetLevel(CurrentStudentLevel());
}

/*********************************************************************
** Function: ResetLevel
** Description: Same as ResetCurrentLevel, but for the given level instead.
** Parameters: level is the level to reset.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void Maze::ResetLevel(MazeLevel& level) {
  level.Reset();

  auto start_loc = level.start_location();
  unsigned level_n = start_loc->pos().level;

  delete student_;
  for (auto& ta : tas_[level_n]) {
    delete ta;
  }

  start_loc->set_has_student(true);
  student_ = new IntrepidStudent(start_loc->pos());
  tas_[level_n] = PlaceTAsAtLevel(level);
  PlaceSkillsAtLevel(level);
}

/*********************************************************************
** Function: CurrentStudentLevel
** Description: Returns the current MazeLevel of the student.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
MazeLevel& Maze::CurrentStudentLevel() {
  return levels_[student_->position().level];
}

/*********************************************************************
** Function: LocationAt
** Description: Returns the MazeLocation, if it exists, at the given position.
** Parameters: pos is the position of the MazeLocation.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
Option<MazeLocation*> Maze::LocationAt(MazePosition pos) {
  if (pos.level >= levels_.size()) return None;
  MazeLevel& level = levels_[pos.level];
  return level.LocationAt(pos);
}

/*********************************************************************
** Function: TAOnLevel
** Description: Returns a TA on the given level.
** Parameters: level is the level to return a TA from.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
Option<TA*> Maze::TAOnLevel(MazeLevel& level) {
  auto level_n = level.start_location()->pos().level;
  if (tas_.size() > level_n) {
    if (!tas_.empty()) {
      return tas_[level_n][0];
    }
  }

  return None;
}

/*********************************************************************
** Function: SpaceAt
** Description: Returns the OpenSpace, if it exists, at the given position.
** Parameters: pos is the position of the OpenSpace.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
Option<OpenSpace*> Maze::SpaceAt(MazePosition pos) {
  return LocationAt(pos).AndThen<OpenSpace*>([&](MazeLocation* loc) {
      if (!loc->occupiable()) return Option<OpenSpace*>(None);
      return Option<OpenSpace*>(dynamic_cast<OpenSpace*>(loc));
  });
}

/*********************************************************************
** Function: TaAt
** Description: Returns the TA, if they exist, at the given space.
** Parameters: space is the space from which to return the TA.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
Option<TA*> Maze::TaAt(MazePosition pos) {
  return SpaceAt(pos).AndThen<TA*>([&](OpenSpace* space) {
      if (space->has_ta()) {
        MazePosition space_pos = space->pos();
        for (auto &ta : tas_[space_pos.level]) {
          if (ta->position() == space_pos) {
            return Option<TA*>(ta);
          }
        }
      }

      return Option<TA*>(None);
  });
}

/*********************************************************************
** Function: SpacesAdjacentTo
** Description: Returns all occupiable spaces directly adjacent to the given
 * position.
** Parameters: pos is the position from which to return adjacent spaces.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
Option<std::vector<OpenSpace*>> Maze::SpacesAdjacentTo(MazePosition pos) {
  return SpaceAt(pos).Map<std::vector<OpenSpace*>>([&](OpenSpace* space) {
      std::vector<OpenSpace*> spaces;

      for (const auto& dir : AllPlayerDirectionActions()) {
        if (CanMoveInDirection(space->pos(), dir)) {
          MazePosition pos_c = space->pos();
          pos_c.Translate(dir, 1);
          spaces.push_back(SpaceAt(pos_c).Unwrap());
        }
      }

      return spaces;
  });
}

/*********************************************************************
** Function: SpacesAdjacentToStudent
** Description: Same as SpacesAdjacentTo, for the student's current position.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
Option<std::vector<OpenSpace*>> Maze::SpacesAdjacentToStudent() {
  return SpacesAdjacentTo(student_->position());
}

/*********************************************************************
** Function: CanMoveInDirection
** Description: Returns whether it's legal to move one unit in the given
 * direction from the given position.
** Parameters: pos is the position to check; dir is the direction to move.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
bool Maze::CanMoveInDirection(MazePosition pos, PlayerDirectionAction dir) {
  return SpaceAt(pos).Map<bool>([&](OpenSpace* space) {
      MazePosition move_pos = space->pos();
      move_pos.Translate(dir, 1);
      return SpaceAt(move_pos).IsSome();
  }).UnwrapOr(false);
}

/*********************************************************************
** Function: ValidActionsAt
** Description: Returns all the moves a person can make at the given position.
** Parameters: pos is the position to check.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
std::vector<PlayerAction> Maze::ValidActionsAt(MazePosition pos) {
  std::vector<PlayerAction> valid_actions = ValidMovementsAt(pos);

  bool can_climb_ladder = SpaceAt(student_->position()).Map<bool>(
      [&](OpenSpace* space) {
          return space->has_ladder();
      }
  ).UnwrapOr(false);

  if (can_climb_ladder) valid_actions.push_back(PlayerAction::ClimbUp);
  if (student_->HasSkills())
    valid_actions.push_back(PlayerAction::DemonstrateSkill);

  return valid_actions;
}

/*********************************************************************
** Function: ValidMovementsAt
** Description: Returns every direction in which a person can move one unit
 * from the given position.
** Parameters: pos is the position to check.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
std::vector<PlayerAction> Maze::ValidMovementsAt(MazePosition pos) {
  std::vector<PlayerAction> movements;

  for (const auto& dir : AllPlayerDirectionActions()) {
    if (CanMoveInDirection(pos, dir)) {
      movements.push_back(PlayerDirectionToAction(dir));
    }
  }

  return movements;
}

/*********************************************************************
** Function: PrintCurrentLevel
** Description: Prints the map of the student's current level.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void Maze::PrintCurrentLevel() {
  std::cout << CurrentStudentLevel();
}

/*********************************************************************
** Function: PrintState
** Description: Prints the state of the maze.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void Maze::PrintState() {
  auto levels_left = levels_.size() - (student_->position().level + 1);
  std::cout << "# of Programming Skills: " << student_->prog_skills() << '\n'
            << "Current Position: " << student_->position() << '\n'
            << "Remaining Levels: " << levels_left << '\n'
            << "TAs Appeased: ";
  TA* ta = TAOnLevel(CurrentStudentLevel()).Unwrap();

  if (ta->IsAppeased()) {
    std::cout << "Yes; " << ta->appeased_turns() << " turns remaining\n\n";
  } else {
    std::cout << "No\n\n";
  }

  PrintCurrentLevel();
  std::cout << '\n';
}

/*********************************************************************
** Function: PlaceTAs
** Description: Randomly places two TAs on every level of the maze.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void Maze::PlaceTAs() {
  for (auto& level : levels_) {
    tas_.push_back(PlaceTAsAtLevel(level));
  }
}

/*********************************************************************
** Function: PlaceTAsAtLevel
** Description: Randomly places two TAs on the given level of the maze; this
 * function will throw if there are no empty spaces.
** Parameters: level is the level on which to place TAs.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
std::vector<TA*> Maze::PlaceTAsAtLevel(MazeLevel& level) {
  auto level_tas = level.RandomEmptySpaces(2).Map<std::vector<TA*>>(
      [&](std::vector<OpenSpace*> spaces) {
          std::vector<TA*> tas;

          for (auto& space : spaces) {
            tas.push_back(new TA(space->pos()));
            space->set_has_ta(true);
          }

          return tas;
      }
  );

  if (level_tas.IsNone()) {
    throw std::runtime_error(
        "Grid is not large enough to place TAs on one or more levels.");
  }

  return level_tas.Unwrap();
}

/*********************************************************************
** Function: PlaceSkills
** Description: Randomly places three skills on every level of the maze.
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void Maze::PlaceSkills() {
  for (auto& level : levels_) {
    PlaceSkillsAtLevel(level);
  }
}

/*********************************************************************
** Function: PlaceSkillsAtLevel
** Description: Randomly places three skills on the given level of the maze;
 * this function will throw if there are no empty spaces.
** Parameters: level is the level on which to place skills.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
void Maze::PlaceSkillsAtLevel(MazeLevel& level) {
  bool placed = level.RandomEmptySpaces(3).Map<bool>(
      [&](std::vector<OpenSpace*> spaces) {
          for (auto& space : spaces) {
            space->set_has_skill(true);
          }
          return true;
      }
  ).UnwrapOr(false);

  if (!placed) {
    throw std::runtime_error(
        "Grid is not large enough to place skills on one or more levels.");
  }
}

/*********************************************************************
** Function: ReadMazeInfo
** Description: Tries to parse the first line of the maze data file.
** Parameters: is is the stream from which to read the maze data file.
** Pre-Conditions: None
** Post-Conditions: None
*********************************************************************/
Option<Maze::MazeInfo> Maze::ReadMazeInfo(std::ifstream& is) {
  // This kind of convoluted process to parse the first line is necessary
  // for the MazeLevel constructor parser to work properly (reading directly
  // from `is` would result in an empty string when getline is called.
  std::string row_str;
  std::getline(is, row_str);
  std::istringstream iss(row_str);
  MazeInfo info;
  iss >> info.levels >> info.height >> info.width;
  if (!is) return None;
  return info;
}

/*********************************************************************
** Function: operator<<
** Description: Overloads the insertion operator to print Maze objects..
** Parameters: os is the output_stream; maze is the Maze to print.
** Pre-Conditions:  None
** Post-Conditions: None
*********************************************************************/
std::ostream& operator<<(std::ostream& os, const Maze& maze) {
  for (const auto& level : maze.levels_) {
    os << level << '\n';
  }

  return os;
}
