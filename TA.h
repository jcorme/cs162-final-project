#ifndef ESCAPEFROMCS162_TA_H
#define ESCAPEFROMCS162_TA_H
/*********************************************************************
** Program Filename: TA.h
** Author: Jason Chen
** Date: 03/19/2018
** Description: Declares the TA class and its related members. 
** Input: None
** Output: None
*********************************************************************/


#include "MazePerson.h"

class TA : public MazePerson {
  public:
    explicit TA(MazePosition pos): MazePerson(pos) {}

    Option<PlayerAction>
    GetMove(std::vector<PlayerAction> valid_moves) override;

    void Occupy(OpenSpace* space) override { space->set_has_ta(true); }
    void Unoccupy(OpenSpace* space) override { space->set_has_ta(false); }

    void Appease() { appeased_turns_ += 10; }
    bool IsAppeased() { return appeased_turns_ > 1; }
    void DecrementAppeasement() { if (appeased_turns_ > 0) --appeased_turns_; }

    unsigned appeased_turns() const { return appeased_turns_; }

  private:
    std::mt19937 rng_engine_ = MakeRngEngine();

    unsigned appeased_turns_ = 0;
};


#endif //ESCAPEFROMCS162_TA_H
